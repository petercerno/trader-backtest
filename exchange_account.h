// Copyright © 2017 Peter Cerno. All rights reserved.

#ifndef EXCHANGE_ACCOUNT_H
#define EXCHANGE_ACCOUNT_H

#include "trader_base.h"

namespace trader {

// Exchange account.
// For simplicity, we implement the following exchange algorithm: At the
// beginning of every OHLC tick, there is at most one active order (either
// market, stop, or limit order). The exchange either executes this active
// order, or cancels it. Thus, there are no active orders when the exchange
// passes this OHLC tick (and all available account balances) to the trader.
// The trader can then decide whether to place a new order or not.
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

 private:
  // Returns the transaction fee config corresponding to the given order.
  const ExchangeAccountConfig::TransactionFeeConfig& GetTransactionFeeConfig(
      const Order& order) const;
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

  // Exchange account configuration.
  ExchangeAccountConfig exchange_account_config_;
};

}  // namespace trader

#endif  // EXCHANGE_ACCOUNT_H
