//  Copyright © 2020 Peter Cerno. All rights reserved.

#include "trader_eval.h"

#include "util_time.h"

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
                       TraderAccount* trader_account) {
  trader_account->security_balance =
      trader_account_config.start_security_balance();
  trader_account->cash_balance = trader_account_config.start_cash_balance();
  trader_account->total_fee = 0;
  trader_account->security_unit = trader_account_config.security_unit();
  trader_account->cash_unit = trader_account_config.cash_unit();
  trader_account->market_liquidity = trader_account_config.market_liquidity();
  trader_account->max_volume_ratio = trader_account_config.max_volume_ratio();
}

// Checks if the "order" is valid.
bool IsValidOrder(const Order& order) {
  return
      // Both order type and order side must be defined.
      order.has_type() && order.has_side() &&
      (  // Positive price is required for non-market orders.
          order.type() == Order::MARKET ||
          (order.has_price() && order.price() > 0)) &&
      (  // Every order must specify a positive security amount or cash amount.
          (order.oneof_amount_case() == Order::kSecurityAmount &&
           order.has_security_amount() && order.security_amount() > 0) ||
          (order.oneof_amount_case() == Order::kCashAmount &&
           order.has_cash_amount() && order.cash_amount() > 0));
}

// Logs the "ohlc_tick" to the output stream "os".
void LogOhlcTick(const OhlcTick& ohlc_tick, std::ostream* os) {
  *os << std::fixed << std::setprecision(0)  // nowrap
      << ohlc_tick.timestamp_sec() << ","    // nowrap
      << std::setprecision(3)                // nowrap
      << ohlc_tick.open() << ","             // nowrap
      << ohlc_tick.high() << ","             // nowrap
      << ohlc_tick.low() << ","              // nowrap
      << ohlc_tick.close() << ","            // nowrap
      << ohlc_tick.volume();
}

// Logs the "trader_account" balances to the output stream "os".
void LogTraderAccount(const TraderAccount& trader_account, std::ostream* os) {
  *os << std::fixed << std::setprecision(3)      // nowrap
      << trader_account.security_balance << ","  // nowrap
      << trader_account.cash_balance << ","      // nowrap
      << trader_account.total_fee;
}

// Logs the "order" to the output stream "os".
void LogOrder(const Order& order, std::ostream* os) {
  *os << Order::Type_Name(order.type()) << ","  // nowrap
      << Order::Side_Name(order.side()) << ","  // nowrap
      << std::fixed << std::setprecision(3);
  if (order.has_price() && order.price() > 0) {
    *os << order.price();
  }
  *os << ",";
  if (order.oneof_amount_case() == Order::kSecurityAmount) {
    *os << order.security_amount();
  }
  *os << ",";
  if (order.oneof_amount_case() == Order::kCashAmount) {
    *os << order.cash_amount();
  }
}

// Logs empty order to the output stream "os".
void LogEmptyOrder(std::ostream* os) { *os << ",,,,"; }

// Logs the "ohlc_tick" and "trader_account" to the output stream "os".
void LogExchangeState(const OhlcTick& ohlc_tick,
                      const TraderAccount& trader_account, std::ostream* os) {
  if (os == nullptr) {
    return;
  }
  LogOhlcTick(ohlc_tick, os);
  *os << ",";
  LogTraderAccount(trader_account, os);
  *os << ",";
  LogEmptyOrder(os);
  *os << std::endl;
}

// Logs the "ohlc_tick", "trader_account", "order" to the output stream "os".
void LogExchangeState(const OhlcTick& ohlc_tick,
                      const TraderAccount& trader_account, const Order& order,
                      std::ostream* os) {
  if (os == nullptr) {
    return;
  }
  LogOhlcTick(ohlc_tick, os);
  *os << ",";
  LogTraderAccount(trader_account, os);
  *os << ",";
  LogOrder(order, os);
  *os << std::endl;
}
}  // namespace

bool ExecuteOrder(const TraderAccountConfig& trader_account_config,
                  const Order& order, const OhlcTick& ohlc_tick,
                  TraderAccount* trader_account) {
  assert(IsValidOrder(order));
  switch (order.type()) {
    case Order::MARKET:
      if (order.side() == Order::BUY) {
        if (order.oneof_amount_case() == Order::kSecurityAmount) {
          return trader_account->MarketBuy(
              trader_account_config.market_order_fee_config(), ohlc_tick,
              order.security_amount());
        } else {
          assert(order.oneof_amount_case() == Order::kCashAmount);
          return trader_account->MarketBuyAtCash(
              trader_account_config.market_order_fee_config(), ohlc_tick,
              order.cash_amount());
        }
      } else {
        assert(order.side() == Order::SELL);
        if (order.oneof_amount_case() == Order::kSecurityAmount) {
          return trader_account->MarketSell(
              trader_account_config.market_order_fee_config(), ohlc_tick,
              order.security_amount());
        } else {
          assert(order.oneof_amount_case() == Order::kCashAmount);
          return trader_account->MarketSellAtCash(
              trader_account_config.market_order_fee_config(), ohlc_tick,
              order.cash_amount());
        }
      }
    case Order::STOP:
      if (order.side() == Order::BUY) {
        if (order.oneof_amount_case() == Order::kSecurityAmount) {
          return trader_account->StopBuy(
              trader_account_config.stop_order_fee_config(), ohlc_tick,
              order.price(), order.security_amount());
        } else {
          assert(order.oneof_amount_case() == Order::kCashAmount);
          return trader_account->StopBuyAtCash(
              trader_account_config.stop_order_fee_config(), ohlc_tick,
              order.price(), order.cash_amount());
        }
      } else {
        assert(order.side() == Order::SELL);
        if (order.oneof_amount_case() == Order::kSecurityAmount) {
          return trader_account->StopSell(
              trader_account_config.stop_order_fee_config(), ohlc_tick,
              order.price(), order.security_amount());
        } else {
          assert(order.oneof_amount_case() == Order::kCashAmount);
          return trader_account->StopSellAtCash(
              trader_account_config.stop_order_fee_config(), ohlc_tick,
              order.price(), order.cash_amount());
        }
      }
    case Order::LIMIT:
      if (order.side() == Order::BUY) {
        if (order.oneof_amount_case() == Order::kSecurityAmount) {
          return trader_account->LimitBuy(
              trader_account_config.limit_order_fee_config(), ohlc_tick,
              order.price(), order.security_amount());
        } else {
          assert(order.oneof_amount_case() == Order::kCashAmount);
          return trader_account->LimitBuyAtCash(
              trader_account_config.limit_order_fee_config(), ohlc_tick,
              order.price(), order.cash_amount());
        }
      } else {
        assert(order.side() == Order::SELL);
        if (order.oneof_amount_case() == Order::kSecurityAmount) {
          return trader_account->LimitSell(
              trader_account_config.limit_order_fee_config(), ohlc_tick,
              order.price(), order.security_amount());
        } else {
          assert(order.oneof_amount_case() == Order::kCashAmount);
          return trader_account->LimitSellAtCash(
              trader_account_config.limit_order_fee_config(), ohlc_tick,
              order.price(), order.cash_amount());
        }
      }
    default:
      assert(false);  // Invalid order type.
  }
}

TraderExecutionResult ExecuteTrader(
    const TraderAccountConfig& trader_account_config,
    OhlcHistory::const_iterator ohlc_history_begin,
    OhlcHistory::const_iterator ohlc_history_end, TraderInterface* trader,
    std::ostream* exchange_os, std::ostream* trader_os) {
  TraderExecutionResult result;
  if (ohlc_history_begin == ohlc_history_end) {
    return {};
  }
  TraderAccount trader_account;
  InitTraderAccount(trader_account_config, &trader_account);
  std::vector<Order> trader_orders;
  constexpr size_t kEmittedOrdersReserve = 8;
  trader_orders.reserve(kEmittedOrdersReserve);
  int total_executed_orders = 0;
  for (auto ohlc_tick_it = ohlc_history_begin; ohlc_tick_it != ohlc_history_end;
       ++ohlc_tick_it) {
    const OhlcTick& ohlc_tick = *ohlc_tick_it;
    // Log the current OHLC tick T[i] and the trader account.
    LogExchangeState(ohlc_tick, trader_account, exchange_os);
    // The trader was updated on the previous OHLC tick T[i-1] and emitted
    // "trader_orders". There are no other active orders on the exchange.
    // Execute (or cancel) "trader_orders" on the current OHLC tick T[i].
    for (const Order& order : trader_orders) {
      const bool success = ExecuteOrder(trader_account_config, order, ohlc_tick,
                                        &trader_account);
      if (success) {
        ++total_executed_orders;
        // Log only the executed orders and their impact on the trader account.
        LogExchangeState(ohlc_tick, trader_account, order, exchange_os);
      }
    }
    // Update the trader internal state on the current OHLC tick T[i].
    // Emit a new set of "trader_orders" for the next OHLC tick T[i+1].
    trader_orders.clear();
    trader->Update(ohlc_tick, trader_account.security_balance,
                   trader_account.cash_balance, &trader_orders);
    trader->LogInternalState(trader_os);
  }
  result.set_start_security_balance(
      trader_account_config.start_security_balance());
  result.set_start_cash_balance(trader_account_config.start_cash_balance());
  result.set_end_security_balance(trader_account.security_balance);
  result.set_end_cash_balance(trader_account.cash_balance);
  result.set_start_price(ohlc_history_begin->close());
  result.set_end_price((--ohlc_history_end)->close());
  result.set_start_value(result.start_cash_balance() +
                         result.start_price() *
                             result.start_security_balance());
  result.set_end_value(result.end_cash_balance() +
                       result.end_price() * result.end_security_balance());
  result.set_total_executed_orders(total_executed_orders);
  result.set_total_fee(trader_account.total_fee);
  return result;
}

TraderEvaluationResult EvaluateTrader(
    const TraderAccountConfig& trader_account_config,
    const TraderEvaluationConfig& trader_eval_config,
    const OhlcHistory& ohlc_history,
    const TraderFactoryInterface& trader_factory, std::ostream* exchange_os,
    std::ostream* trader_os) {
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
    TraderExecutionResult result = ExecuteTrader(
        trader_account_config, ohlc_history_subset.first,
        ohlc_history_subset.second, trader.get(), exchange_os, trader_os);
    TraderEvaluationResult::Period* period = trader_eval_result.add_period();
    period->set_start_timestamp_sec(start_eval_timestamp_sec);
    period->set_end_timestamp_sec(end_eval_timestamp_sec);
    *period->mutable_result() = result;
    assert(result.start_value() > 0);
    period->set_trader_final_gain(result.end_value() / result.start_value());
    assert(result.start_price() > 0 && result.end_price() > 0);
    period->set_base_final_gain(result.end_price() / result.start_price());
    if (trader_eval_config.evaluation_period_months() == 0) {
      break;
    }
  }
  trader_eval_result.set_score(GetGeometricAverage(
      trader_eval_result.period(),
      [](const TraderEvaluationResult::Period& period) {
        return period.trader_final_gain() / period.base_final_gain();
      }));
  trader_eval_result.set_avg_trader_gain(
      GetAverage(trader_eval_result.period(),
                 [](const TraderEvaluationResult::Period& period) {
                   return period.trader_final_gain();
                 }));
  trader_eval_result.set_avg_base_gain(
      GetAverage(trader_eval_result.period(),
                 [](const TraderEvaluationResult::Period& period) {
                   return period.base_final_gain();
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
                                /* exchange_os = */ nullptr,
                                /* trader_os = */ nullptr);
        }));
  }
  for (auto& trader_eval_result_future : trader_eval_result_futures) {
    trader_eval_results.emplace_back(trader_eval_result_future.get());
  }
  return trader_eval_results;
}

}  // namespace trader
