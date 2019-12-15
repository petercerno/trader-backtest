// Copyright © 2017 Peter Cerno. All rights reserved.

#ifndef EXCHANGE_ACCOUNT_H
#define EXCHANGE_ACCOUNT_H

#include "trader_base.h"

#define FRIEND_TEST(test_case_name, test_name) \
  friend class test_case_name##_##test_name##_Test

namespace trader {

// Exchange account. For simplicity, we implement the following exchange:
// At the beginning of every OHLC tick, there might be zero or more active
// orders submitted by the trader (in the previous step). For every order,
// the exchange either executes this order, or cancels it. Thus, there are no
// orders after the exchange processes this OHLC tick. The exchange then sends
// this (processed) OHLC tick and all available account balances to the trader.
// The trader then decides whether to place new order(s) or not, and the whole
// process repeats with the next OHLC tick. Note that the trader (when placing
// new orders) has no information about the next OHLC tick against which its
// order(s) will be executed by the exchange.
class ExchangeAccount {
 public:
  explicit ExchangeAccount(
      const ExchangeAccountConfig& exchange_account_config);
  virtual ~ExchangeAccount() {}

  // Executes the trader over the ohlc_history (or its part).
  // Returns the resulting exchange states over the executed history.
  // If keep_intermediate_states is true, all states are emitted. Otherwise,
  // only the first and the last state are emitted.
  // Note that start_timestamp_sec (end_timestamp_sec) is ignored if zero.
  ExchangeAccountStates Execute(const OhlcHistory& ohlc_history,
                                long start_timestamp_sec,
                                long end_timestamp_sec,
                                bool keep_intermediate_states,
                                TraderInterface* trader) const;

 protected:
  // Returns true if the order is valid.
  static bool IsValidOrder(const Order& order);
  // Returns true if the order can be executed at the ohlc_tick.
  static bool IsExecutableOrder(const Order& order, const OhlcTick& ohlc_tick);
  // Returns the transaction fee config corresponding to the given order.
  const FeeConfig& GetTransactionFeeConfig(const Order& order) const;
  // Returns the transaction fee (in base currency) corresponding to the given
  // order and the transferred cash_amount. It is a non-decreasing function
  // of the cash_amount.
  float GetTransactionFee(const Order& order, float cash_amount) const;
  // Returns the actual traded price of the order w.r.t. the given ohlc_tick.
  // We assume that the order is executable at the given ohlc_tick.
  float GetTradedPrice(const Order& order, const OhlcTick& ohlc_tick) const;
  // Returns the maximum security amount that can be traded w.r.t. the given
  // ohlc_tick. Note that market and stop orders can be always fully executed.
  float GetMaxSecurityAmount(const Order& order,
                             const OhlcTick& ohlc_tick) const;
  // Executes the buy order at the given price and updates the corresponding
  // exchange account state.
  // max_security_amount specifies the maximum security amount that can be
  // traded. (Usally used for limit orders).
  void ExecuteBuyOrder(const Order& order, float price,
                       float max_security_amount,
                       ExchangeAccountState* state) const;
  // Executes the sell order at the given price and updates the corresponding
  // exchange account state.
  // max_security_amount specifies the maximum security amount that can be
  // traded. (Usally used for limit orders).
  void ExecuteSellOrder(const Order& order, float price,
                        float max_security_amount,
                        ExchangeAccountState* state) const;
  // Updates the accumulated fields in the state based on the previous state.
  void UpdateAccumulatedFields(
      ExchangeAccountState* state,
      const ExchangeAccountState* previous_state) const;

  FRIEND_TEST(ExchangeAccountTest, IsValidOrderBasicTest);
  FRIEND_TEST(ExchangeAccountTest, IsExecutableOrderBasicTest);

 private:
  // Exchange account configuration.
  ExchangeAccountConfig exchange_account_config_;
};

}  // namespace trader

#endif  // EXCHANGE_ACCOUNT_H
