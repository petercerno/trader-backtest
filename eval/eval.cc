// Copyright © 2021 Peter Cerno. All rights reserved.

#include "eval/eval.h"

#include "indicators/volatility.h"
#include "util/time.h"

namespace trader {
namespace {
// Returns the average of values returned by the selector.
template <typename C, typename F>
float GetAverage(const C& container, F selector) {
  if (container.empty()) {
    return 0;
  }
  float sum = 0;
  for (const auto& element : container) {
    sum += selector(element);
  }
  return sum / container.size();
}

// Returns the geometric average of values returned by the selector.
template <typename C, typename F>
float GetGeometricAverage(const C& container, F selector) {
  if (container.empty()) {
    return 0;
  }
  double mul = 1;
  for (const auto& element : container) {
    mul *= selector(element);
  }
  assert(mul >= 0);
  return static_cast<float>(std::pow(mul, 1.0 / container.size()));
}
}  // namespace

ExecutionResult ExecuteTrader(const AccountConfig& account_config,
                              OhlcHistory::const_iterator ohlc_history_begin,
                              OhlcHistory::const_iterator ohlc_history_end,
                              const SideInput* side_input, bool fast_eval,
                              Trader& trader, Logger* logger) {
  ExecutionResult result;
  if (ohlc_history_begin == ohlc_history_end) {
    return {};
  }
  Account account;
  account.InitAccount(account_config);
  std::vector<float> side_input_signals;
  if (side_input != nullptr) {
    // The last signal is the age (in seconds) of the side input signals.
    side_input_signals.reserve(side_input->GetNumberOfSignals() + 1);
  }
  int prev_side_input_index = -1;
  std::vector<Order> orders;
  constexpr size_t kEmittedOrdersReserve = 8;
  orders.reserve(kEmittedOrdersReserve);
  int total_executed_orders = 0;
  Volatility base_volatility(/*window_size=*/0,
                             /*period_size_sec=*/kSecondsPerDay);
  Volatility trader_volatility(/*window_size=*/0,
                               /*period_size_sec=*/kSecondsPerDay);
  for (auto ohlc_tick_it = ohlc_history_begin; ohlc_tick_it != ohlc_history_end;
       ++ohlc_tick_it) {
    const OhlcTick& ohlc_tick = *ohlc_tick_it;
    if (side_input != nullptr) {
      const int side_input_index = side_input->GetSideInputIndex(
          ohlc_tick.timestamp_sec(), prev_side_input_index);
      if (side_input_index >= 0) {
        const int64_t side_input_timestamp_sec =
            side_input->GetSideInputTimestamp(side_input_index);
        side_input_signals.clear();
        side_input->GetSideInputSignals(side_input_index, side_input_signals);
        // Adding the age (in seconds) of the side input signals.
        side_input_signals.push_back(ohlc_tick.timestamp_sec() -
                                     side_input_timestamp_sec);
        prev_side_input_index = side_input_index;
      }
    }
    // Log the current OHLC tick T[i] and the trader account.
    // Note: We do not explicitly log the side_input_signals as those can be
    // easily logged through trader internal state.
    if (logger != nullptr) {
      logger->LogExchangeState(ohlc_tick, account);
    }
    // The trader was updated on the previous OHLC tick T[i-1] and emitted
    // "orders". There are no other active orders on the exchange.
    // Execute (or cancel) "orders" on the current OHLC tick T[i].
    for (const Order& order : orders) {
      const bool success =
          account.ExecuteOrder(account_config, order, ohlc_tick);
      if (success) {
        ++total_executed_orders;
        // Log only the executed orders and their impact on the trader account.
        if (logger != nullptr) {
          logger->LogExchangeState(ohlc_tick, account, order);
        }
      }
    }
    if (ohlc_tick.volume() == 0) {
      // Zero volume OHLC tick indicates a gap in a price history. Such gap
      // could have been caused by an unresponsive exchange (or its API).
      // Therefore, we do not update the trader and simply keep the previously
      // emitted orders.
      continue;
    }
    // Update the trader internal state on the current OHLC tick T[i].
    // Emit a new set of "orders" for the next OHLC tick T[i+1].
    orders.clear();
    trader.Update(ohlc_tick, side_input_signals, account.base_balance,
                  account.quote_balance, orders);
    if (logger != nullptr) {
      logger->LogTraderState(trader.GetInternalState());
    }
    if (!fast_eval) {
      base_volatility.Update(ohlc_tick, /*base_balance=*/1.0f,
                             /*quote_balance=*/0.0f);
      trader_volatility.Update(ohlc_tick, account.base_balance,
                               account.quote_balance);
    }
  }
  result.set_start_base_balance(account_config.start_base_balance());
  result.set_start_quote_balance(account_config.start_quote_balance());
  result.set_end_base_balance(account.base_balance);
  result.set_end_quote_balance(account.quote_balance);
  result.set_start_price(ohlc_history_begin->close());
  result.set_end_price((--ohlc_history_end)->close());
  result.set_start_value(result.start_quote_balance() +
                         result.start_price() * result.start_base_balance());
  result.set_end_value(result.end_quote_balance() +
                       result.end_price() * result.end_base_balance());
  result.set_total_executed_orders(total_executed_orders);
  result.set_total_fee(account.total_fee);
  if (!fast_eval) {
    result.set_base_volatility(base_volatility.GetVolatility() *
                               std::sqrt(365));
    result.set_trader_volatility(trader_volatility.GetVolatility() *
                                 std::sqrt(365));
  }
  return result;
}

EvaluationResult EvaluateTrader(const AccountConfig& account_config,
                                const EvaluationConfig& eval_config,
                                const OhlcHistory& ohlc_history,
                                const SideInput* side_input,
                                const TraderEmitter& trader_emitter,
                                Logger* logger) {
  EvaluationResult eval_result;
  *eval_result.mutable_account_config() = account_config;
  *eval_result.mutable_eval_config() = eval_config;
  eval_result.set_name(trader_emitter.GetName());
  for (int month_offset = 0;; ++month_offset) {
    const int64_t start_eval_timestamp_sec = AddMonthsToTimestampSec(
        eval_config.start_timestamp_sec(), month_offset);
    const int64_t end_eval_timestamp_sec =
        eval_config.evaluation_period_months() > 0
            ? AddMonthsToTimestampSec(start_eval_timestamp_sec,
                                      eval_config.evaluation_period_months())
            : eval_config.end_timestamp_sec();
    if (end_eval_timestamp_sec > eval_config.end_timestamp_sec()) {
      break;
    }
    const auto ohlc_history_subset = HistorySubset(
        ohlc_history, start_eval_timestamp_sec, end_eval_timestamp_sec);
    if (ohlc_history_subset.first == ohlc_history_subset.second) {
      continue;
    }
    std::unique_ptr<Trader> trader = trader_emitter.NewTrader();
    ExecutionResult result = ExecuteTrader(
        account_config, ohlc_history_subset.first, ohlc_history_subset.second,
        side_input, eval_config.fast_eval(), *trader, logger);
    EvaluationResult::Period* period = eval_result.add_period();
    period->set_start_timestamp_sec(start_eval_timestamp_sec);
    period->set_end_timestamp_sec(end_eval_timestamp_sec);
    *period->mutable_result() = result;
    assert(result.start_value() > 0);
    period->set_final_gain(result.end_value() / result.start_value());
    assert(result.start_price() > 0 && result.end_price() > 0);
    period->set_base_final_gain(result.end_price() / result.start_price());
    if (eval_config.evaluation_period_months() == 0) {
      break;
    }
  }
  eval_result.set_score(GetGeometricAverage(
      eval_result.period(), [](const EvaluationResult::Period& period) {
        return period.final_gain() / period.base_final_gain();
      }));
  eval_result.set_avg_gain(GetAverage(
      eval_result.period(), [](const EvaluationResult::Period& period) {
        return period.final_gain();
      }));
  eval_result.set_avg_base_gain(GetAverage(
      eval_result.period(), [](const EvaluationResult::Period& period) {
        return period.base_final_gain();
      }));
  eval_result.set_avg_total_executed_orders(GetAverage(
      eval_result.period(), [](const EvaluationResult::Period& period) {
        return period.result().total_executed_orders();
      }));
  eval_result.set_avg_total_fee(GetAverage(
      eval_result.period(), [](const EvaluationResult::Period& period) {
        return period.result().total_fee();
      }));
  return eval_result;
}

std::vector<EvaluationResult> EvaluateBatchOfTraders(
    const AccountConfig& account_config, const EvaluationConfig& eval_config,
    const OhlcHistory& ohlc_history, const SideInput* side_input,
    const std::vector<std::unique_ptr<TraderEmitter>>& trader_emitters) {
  std::vector<EvaluationResult> eval_results;
  std::vector<std::future<EvaluationResult>> eval_result_futures;
  for (const auto& trader_emitter_ptr : trader_emitters) {
    const TraderEmitter& trader_emitter = *trader_emitter_ptr;
    eval_result_futures.emplace_back(
        std::async([&account_config, &eval_config, &ohlc_history, side_input,
                    &trader_emitter]() {
          return EvaluateTrader(account_config, eval_config, ohlc_history,
                                side_input, trader_emitter,
                                /*logger=*/nullptr);
        }));
  }
  for (auto& eval_result_future : eval_result_futures) {
    eval_results.emplace_back(eval_result_future.get());
  }
  return eval_results;
}

}  // namespace trader
