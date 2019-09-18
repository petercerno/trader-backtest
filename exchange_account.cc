// Copyright © 2017 Peter Cerno. All rights reserved.

#include "exchange_account.h"

#include <limits>

namespace trader {
namespace {
// Returns the amount floored to the given unit.
float GetFlooredAmount(float amount, float unit) {
  if (unit <= 0) {
    return amount;
  }
  return unit * std::floor(amount / unit);
}

// Returns the amount ceiled to the given unit.
float GetCeiledAmount(float amount, float unit) {
  if (unit <= 0) {
    return amount;
  }
  return unit * std::ceil(amount / unit);
}

// Returns the amount rounded to the given unit.
float GetRoundedAmount(float amount, float unit) {
  if (unit <= 0) {
    return amount;
  }
  return unit * std::round(amount / unit);
}
}  // namespace

ExchangeAccount::ExchangeAccount(
    const ExchangeAccountConfig& exchange_account_config)
    : exchange_account_config_(exchange_account_config) {}

ExchangeAccountStates ExchangeAccount::Execute(const OhlcHistory& ohlc_history,
                                               long start_timestamp_sec,
                                               long end_timestamp_sec,
                                               bool keep_intermediate_states,
                                               TraderInterface* trader) const {
  constexpr size_t kEmittedOrdersReserve = 4;
  std::vector<Order> orders;
  orders.reserve(kEmittedOrdersReserve);
  const auto ohlc_history_subset =
      HistorySubset(ohlc_history, start_timestamp_sec, end_timestamp_sec);
  const auto size =
      std::distance(ohlc_history_subset.first, ohlc_history_subset.second);
  if (size == 0) {
    return {};
  }
  ExchangeAccountStates exchange_account_states;
  exchange_account_states.reserve(keep_intermediate_states ? size : 2);
  float security_balance = exchange_account_config_.start_security_balance();
  float cash_balance = exchange_account_config_.start_cash_balance();
  for (auto ohlc_tick_it = ohlc_history_subset.first;
       ohlc_tick_it != ohlc_history_subset.second; ++ohlc_tick_it) {
    const OhlcTick& ohlc_tick = *ohlc_tick_it;
    const ExchangeAccountState* previous_state =
        exchange_account_states.empty() ? nullptr
                                        : &exchange_account_states.back();
    ExchangeAccountState state;
    *state.mutable_ohlc_tick() = ohlc_tick;
    // Executed order statistics.
    state.set_executed_orders(0);
    state.set_traded_security_amount(0);
    state.set_transaction_fee(0);
    // Store the current account balances into the state.
    // The state account balances will be updated for every executed order.
    state.set_security_balance(security_balance);
    state.set_cash_balance(cash_balance);
    for (const Order& order : orders) {
      assert(IsValidOrder(order));  // Invalid order.
      *state.add_order() = order;
      if (IsExecutableOrder(order, ohlc_tick)) {
        // Execute the order at the ohlc_tick.
        const float price = GetTradedPrice(order, ohlc_tick);
        const float max_security_amount =
            GetMaxSecurityAmount(order, ohlc_tick);
        if (max_security_amount > 0) {
          switch (order.side()) {
            case Order::BUY:
              ExecuteBuyOrder(order, price, max_security_amount, &state);
              break;
            case Order::SELL:
              ExecuteSellOrder(order, price, max_security_amount, &state);
              break;
            default:
              assert(false);  // Invalid order side.
              break;
          }
        }
      }
    }
    // Fetch the updated account balances from the state.
    security_balance = state.security_balance();
    cash_balance = state.cash_balance();
    state.set_value(security_balance * ohlc_tick.close() + cash_balance);
    UpdateAccumulatedFields(&state, previous_state);
    if (!keep_intermediate_states && exchange_account_states.size() > 1) {
      // Pop the last state, which would otherwise become intermediate.
      exchange_account_states.pop_back();
    }
    exchange_account_states.push_back(std::move(state));
    // Update the trader and get new order(s).
    orders.clear();
    trader->Update(ohlc_tick, security_balance, cash_balance, &orders);
  }
  return exchange_account_states;
}

bool ExchangeAccount::IsValidOrder(const Order& order) {
  return
      // Both order type and order side must be defined.
      order.has_type() && order.has_side() &&
      (  // Positive price is required for non-market orders.
          order.type() == Order::MARKET ||
          (order.has_price() && order.price() > 0)) &&
      (  // Buy order must specify positive cash amount.
          (order.side() == Order::BUY && order.has_cash_amount() &&
           order.cash_amount() > 0) ||
          // Sell order must specify positive security (crypto currency) amount.
          (order.side() == Order::SELL && order.has_security_amount() &&
           order.security_amount() > 0));
}

bool ExchangeAccount::IsExecutableOrder(const Order& order,
                                        const OhlcTick& ohlc_tick) {
  if (IsValidOrder(order)) {
    switch (order.type()) {
      case Order::MARKET:
        // Market order can be always executed.
        return true;
      case Order::LIMIT:
        if (order.side() == Order::BUY) {
          // Buy limit order can be executed when the actual price drops
          // below the limit order price.
          return ohlc_tick.low() <= order.price();
        } else {
          assert(order.side() == Order::SELL);
          // Sell limit order can be executed when the actual price exceeds
          // the limit order price.
          return ohlc_tick.high() >= order.price();
        }
      case Order::STOP:
        if (order.side() == Order::BUY) {
          // Buy stop order can be executed when the actual price exceeds
          // the stop order price.
          return ohlc_tick.high() >= order.price();
        } else {
          assert(order.side() == Order::SELL);
          // Sell stop order can be executed when the actual price drops
          // below the stop order price.
          return ohlc_tick.low() <= order.price();
        }
      default:
        assert(false);  // Invalid order type.
    }
  }
  return false;
}

const ExchangeAccountConfig::TransactionFeeConfig&
ExchangeAccount::GetTransactionFeeConfig(const Order& order) const {
  switch (order.type()) {
    case Order::MARKET:
      return exchange_account_config_.market_order_fee_config();
    case Order::LIMIT:
      return exchange_account_config_.limit_order_fee_config();
    case Order::STOP:
      return exchange_account_config_.stop_order_fee_config();
    default:
      assert(false);  // Invalid order type.
  }
}

float ExchangeAccount::GetTransactionFee(const Order& order,
                                         float cash_amount) const {
  const auto& transaction_fee_config = GetTransactionFeeConfig(order);
  return GetCeiledAmount(
      std::max(transaction_fee_config.minimum_fee(),
               transaction_fee_config.fixed_fee() +
                   cash_amount * transaction_fee_config.relative_fee()),
      exchange_account_config_.cash_balance_unit());
}

float ExchangeAccount::GetTradedPrice(const Order& order,
                                      const OhlcTick& ohlc_tick) const {
  // The worst possible price (for market and stop orders).
  float pessimistic_price = 0;
  // Order price adjusted w.r.t. the open price (used for stop orders).
  float adjusted_order_price = 0;
  switch (order.side()) {
    case Order::BUY:
      pessimistic_price = ohlc_tick.high();
      adjusted_order_price = std::max(order.price(), ohlc_tick.open());
      break;
    case Order::SELL:
      pessimistic_price = ohlc_tick.low();
      adjusted_order_price = std::min(order.price(), ohlc_tick.open());
      break;
    default:
      assert(false);  // Invalid order side.
  }
  switch (order.type()) {
    case Order::MARKET:
      return (1.0f - exchange_account_config_.market_order_accuracy()) *
                 pessimistic_price +
             exchange_account_config_.market_order_accuracy() *
                 ohlc_tick.open();
    case Order::LIMIT:
      return order.price();
    case Order::STOP:
      return (1.0f - exchange_account_config_.stop_order_accuracy()) *
                 pessimistic_price +
             exchange_account_config_.stop_order_accuracy() *
                 adjusted_order_price;
    default:
      assert(false);  // Invalid order type.
  }
}

float ExchangeAccount::GetMaxSecurityAmount(const Order& order,
                                            const OhlcTick& ohlc_tick) const {
  float max_security_amount = std::numeric_limits<float>::max();
  // Market and stop orders can be always fully executed.
  if (order.type() == Order::LIMIT &&
      exchange_account_config_.limit_order_fill_volume_ratio() > 0) {
    max_security_amount = GetFlooredAmount(
        exchange_account_config_.limit_order_fill_volume_ratio() *
            ohlc_tick.volume(),
        exchange_account_config_.security_balance_unit());
  }
  return max_security_amount;
}

void ExchangeAccount::ExecuteBuyOrder(const Order& order, float price,
                                      float max_security_amount,
                                      ExchangeAccountState* state) const {
  assert(order.side() == Order::BUY);  // Invalid order side.
  assert(price > 0);                   // price must be positive.
  assert(max_security_amount > 0);     // max_security_amount must be positive.
  // Maximum possible cash amount that can be used to buy the security.
  const float max_cash_amount =
      GetFlooredAmount(std::min(order.cash_amount(), state->cash_balance()),
                       exchange_account_config_.cash_balance_unit());
  // Transaction fee corresponding to the maximum possible cash amount.
  const float max_transaction_fee = GetTransactionFee(order, max_cash_amount);
  // The actual security (crypto currency) amount to be bought.
  const float traded_security_amount =
      GetFlooredAmount(std::min((max_cash_amount - max_transaction_fee) / price,
                                max_security_amount),
                       exchange_account_config_.security_balance_unit());
  if (traded_security_amount <
      exchange_account_config_.security_balance_unit()) {
    // Insufficient cash funds.
    return;
  }
  // Note that traded_security_amount * price is bounded from above by
  // max_cash_amount - max_transaction_fee, which has correct units.
  // Therefore, also the ceiled amount traded_cash_amount is at most
  // max_cash_amount - max_transaction_fee.
  const float traded_cash_amount =
      GetCeiledAmount(traded_security_amount * price,
                      exchange_account_config_.cash_balance_unit());
  // Note that transaction_fee is at most max_transaction_fee.
  const float transaction_fee = GetTransactionFee(order, traded_cash_amount);
  // Update the executed order statistics.
  state->set_executed_orders(state->executed_orders() + 1);
  state->set_traded_security_amount(
      GetRoundedAmount(state->traded_security_amount() + traded_security_amount,
                       exchange_account_config_.security_balance_unit()));
  state->set_transaction_fee(state->transaction_fee() + transaction_fee);
  // Update the state balances.
  state->set_security_balance(
      GetRoundedAmount(state->security_balance() + traded_security_amount,
                       exchange_account_config_.security_balance_unit()));
  // traded_cash_amount + transaction_fee <=
  // max_cash_amount - max_transaction_fee + transaction_fee <=
  // max_cash_amount <= state->cash_balance().
  state->set_cash_balance(GetRoundedAmount(
      state->cash_balance() - traded_cash_amount - transaction_fee,
      exchange_account_config_.cash_balance_unit()));
}

void ExchangeAccount::ExecuteSellOrder(const Order& order, float price,
                                       float max_security_amount,
                                       ExchangeAccountState* state) const {
  assert(order.side() == Order::SELL);  // Invalid order side.
  assert(price > 0);                    // price must be positive.
  assert(max_security_amount > 0);      // max_security_amount must be positive.
  // The actual security (crypto currency) amount to be sold.
  const float traded_security_amount = GetFlooredAmount(
      std::min(std::min(order.security_amount(), state->security_balance()),
               max_security_amount),
      exchange_account_config_.security_balance_unit());
  // Obtained cash amount from selling the security (crypto currency).
  const float traded_cash_amount =
      GetFlooredAmount(traded_security_amount * price,
                       exchange_account_config_.cash_balance_unit());
  // Transaction fee.
  const float transaction_fee = GetTransactionFee(order, traded_cash_amount);
  if (traded_cash_amount - transaction_fee <
      exchange_account_config_.cash_balance_unit()) {
    // Insufficient security (crypto currency) funds.
    return;
  }
  // Update the executed order statistics.
  state->set_executed_orders(state->executed_orders() + 1);
  state->set_traded_security_amount(
      GetRoundedAmount(state->traded_security_amount() + traded_security_amount,
                       exchange_account_config_.security_balance_unit()));
  state->set_transaction_fee(state->transaction_fee() + transaction_fee);
  // Update the state balances.
  state->set_security_balance(
      GetRoundedAmount(state->security_balance() - traded_security_amount,
                       exchange_account_config_.security_balance_unit()));
  state->set_cash_balance(GetRoundedAmount(
      state->cash_balance() + traded_cash_amount - transaction_fee,
      exchange_account_config_.cash_balance_unit()));
}

void ExchangeAccount::UpdateAccumulatedFields(
    ExchangeAccountState* state,
    const ExchangeAccountState* previous_state) const {
  assert(state != nullptr);  // State must be provided.
  if (previous_state != nullptr) {
    state->set_accumulated_executed_orders(
        previous_state->accumulated_executed_orders() +
        state->executed_orders());
    state->set_accumulated_security_amount(
        GetRoundedAmount(previous_state->accumulated_security_amount() +
                             state->traded_security_amount(),
                         exchange_account_config_.security_balance_unit()));
    state->set_accumulated_fee(GetRoundedAmount(
        previous_state->accumulated_fee() + state->transaction_fee(),
        exchange_account_config_.cash_balance_unit()));
  } else {
    state->set_accumulated_executed_orders(state->executed_orders());
    state->set_accumulated_security_amount(state->traded_security_amount());
    state->set_accumulated_fee(state->transaction_fee());
  }
}

}  // namespace trader
