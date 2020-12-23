// Copyright © 2020 Peter Cerno. All rights reserved.

#include "lib/trader_eval.h"

#include "util/util_time.h"

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

// Initializes the trader account based on the trader account configuration.
void InitTraderAccount(const TraderAccountConfig& trader_account_config,
                       TraderAccount& trader_account) {
  trader_account.base_balance = trader_account_config.start_base_balance();
  trader_account.quote_balance = trader_account_config.start_quote_balance();
  trader_account.total_fee = 0;
  trader_account.base_unit = trader_account_config.base_unit();
  trader_account.quote_unit = trader_account_config.quote_unit();
  trader_account.market_liquidity = trader_account_config.market_liquidity();
  trader_account.max_volume_ratio = trader_account_config.max_volume_ratio();
}
}  // namespace

TraderExecutionResult ExecuteTrader(
    const TraderAccountConfig& trader_account_config,
    OhlcHistory::const_iterator ohlc_history_begin,
    OhlcHistory::const_iterator ohlc_history_end, TraderInterface& trader,
    LoggerInterface* logger) {
  TraderExecutionResult result;
  if (ohlc_history_begin == ohlc_history_end) {
    return {};
  }
  TraderAccount trader_account;
  InitTraderAccount(trader_account_config, trader_account);
  std::vector<Order> trader_orders;
  constexpr size_t kEmittedOrdersReserve = 8;
  trader_orders.reserve(kEmittedOrdersReserve);
  int total_executed_orders = 0;
  for (auto ohlc_tick_it = ohlc_history_begin; ohlc_tick_it != ohlc_history_end;
       ++ohlc_tick_it) {
    const OhlcTick& ohlc_tick = *ohlc_tick_it;
    // Log the current OHLC tick T[i] and the trader account.
    if (logger != nullptr) {
      logger->LogExchangeState(ohlc_tick, trader_account);
    }
    // The trader was updated on the previous OHLC tick T[i-1] and emitted
    // "trader_orders". There are no other active orders on the exchange.
    // Execute (or cancel) "trader_orders" on the current OHLC tick T[i].
    for (const Order& order : trader_orders) {
      const bool success =
          trader_account.ExecuteOrder(trader_account_config, order, ohlc_tick);
      if (success) {
        ++total_executed_orders;
        // Log only the executed orders and their impact on the trader account.
        if (logger != nullptr) {
          logger->LogExchangeState(ohlc_tick, trader_account, order);
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
    // Emit a new set of "trader_orders" for the next OHLC tick T[i+1].
    trader_orders.clear();
    trader.Update(ohlc_tick, trader_account.base_balance,
                  trader_account.quote_balance, trader_orders);
    if (logger != nullptr) {
      logger->LogTraderState(trader.GetInternalState());
    }
  }
  result.set_start_base_balance(trader_account_config.start_base_balance());
  result.set_start_quote_balance(trader_account_config.start_quote_balance());
  result.set_end_base_balance(trader_account.base_balance);
  result.set_end_quote_balance(trader_account.quote_balance);
  result.set_start_price(ohlc_history_begin->close());
  result.set_end_price((--ohlc_history_end)->close());
  result.set_start_value(result.start_quote_balance() +
                         result.start_price() * result.start_base_balance());
  result.set_end_value(result.end_quote_balance() +
                       result.end_price() * result.end_base_balance());
  result.set_total_executed_orders(total_executed_orders);
  result.set_total_fee(trader_account.total_fee);
  return result;
}

TraderEvaluationResult EvaluateTrader(
    const TraderAccountConfig& trader_account_config,
    const TraderEvaluationConfig& trader_eval_config,
    const OhlcHistory& ohlc_history,
    const TraderFactoryInterface& trader_factory, LoggerInterface* logger) {
  TraderEvaluationResult trader_eval_result;
  *trader_eval_result.mutable_trader_account_config() = trader_account_config;
  *trader_eval_result.mutable_trader_eval_config() = trader_eval_config;
  trader_eval_result.set_trader_name(trader_factory.GetTraderName());
  for (int month_offset = 0;; ++month_offset) {
    const int start_eval_timestamp_sec =
        static_cast<int>(AddMonthsToTimestampSec(
            trader_eval_config.start_timestamp_sec(), month_offset));
    const int end_eval_timestamp_sec =
        trader_eval_config.evaluation_period_months() > 0
            ? static_cast<int>(AddMonthsToTimestampSec(
                  start_eval_timestamp_sec,
                  trader_eval_config.evaluation_period_months()))
            : trader_eval_config.end_timestamp_sec();
    if (end_eval_timestamp_sec > trader_eval_config.end_timestamp_sec()) {
      break;
    }
    const auto ohlc_history_subset = HistorySubset(
        ohlc_history, start_eval_timestamp_sec, end_eval_timestamp_sec);
    if (ohlc_history_subset.first == ohlc_history_subset.second) {
      continue;
    }
    std::unique_ptr<TraderInterface> trader = trader_factory.NewTrader();
    TraderExecutionResult result =
        ExecuteTrader(trader_account_config, ohlc_history_subset.first,
                      ohlc_history_subset.second, *trader, logger);
    TraderEvaluationResult::Period* period = trader_eval_result.add_period();
    period->set_start_timestamp_sec(start_eval_timestamp_sec);
    period->set_end_timestamp_sec(end_eval_timestamp_sec);
    *period->mutable_result() = result;
    assert(result.start_value() > 0);
    period->set_trader_final_gain(result.end_value() / result.start_value());
    assert(result.start_price() > 0 && result.end_price() > 0);
    period->set_baseline_final_gain(result.end_price() / result.start_price());
    if (trader_eval_config.evaluation_period_months() == 0) {
      break;
    }
  }
  trader_eval_result.set_score(GetGeometricAverage(
      trader_eval_result.period(),
      [](const TraderEvaluationResult::Period& period) {
        return period.trader_final_gain() / period.baseline_final_gain();
      }));
  trader_eval_result.set_avg_trader_gain(
      GetAverage(trader_eval_result.period(),
                 [](const TraderEvaluationResult::Period& period) {
                   return period.trader_final_gain();
                 }));
  trader_eval_result.set_avg_baseline_gain(
      GetAverage(trader_eval_result.period(),
                 [](const TraderEvaluationResult::Period& period) {
                   return period.baseline_final_gain();
                 }));
  trader_eval_result.set_avg_total_executed_orders(
      GetAverage(trader_eval_result.period(),
                 [](const TraderEvaluationResult::Period& period) {
                   return period.result().total_executed_orders();
                 }));
  trader_eval_result.set_avg_total_fee(
      GetAverage(trader_eval_result.period(),
                 [](const TraderEvaluationResult::Period& period) {
                   return period.result().total_fee();
                 }));
  return trader_eval_result;
}

std::vector<TraderEvaluationResult> EvaluateBatchOfTraders(
    const TraderAccountConfig& trader_account_config,
    const TraderEvaluationConfig& trader_eval_config,
    const OhlcHistory& ohlc_history,
    const std::vector<std::unique_ptr<TraderFactoryInterface>>&
        trader_factories) {
  std::vector<TraderEvaluationResult> trader_eval_results;
  std::vector<std::future<TraderEvaluationResult>> trader_eval_result_futures;
  for (const auto& trader_factory_ptr : trader_factories) {
    const TraderFactoryInterface& trader_factory = *trader_factory_ptr.get();
    trader_eval_result_futures.emplace_back(
        std::async([&trader_account_config, &trader_eval_config, &ohlc_history,
                    &trader_factory]() {
          return EvaluateTrader(trader_account_config, trader_eval_config,
                                ohlc_history, trader_factory,
                                /* logger = */ nullptr);
        }));
  }
  for (auto& trader_eval_result_future : trader_eval_result_futures) {
    trader_eval_results.emplace_back(trader_eval_result_future.get());
  }
  return trader_eval_results;
}

}  // namespace trader
