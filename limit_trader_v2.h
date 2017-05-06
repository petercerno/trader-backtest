// Copyright © 2017 Peter Cerno. All rights reserved.

#ifndef LIMIT_TRADER_V2_H
#define LIMIT_TRADER_V2_H

#include "trader_base.h"

namespace trader {

// Limit trader. Emits one limit buy and one limit sell order per OHLC tick.
class LimitTraderV2 : public TraderInterface {
 public:
  explicit LimitTraderV2(const LimitTraderV2Config& trader_config);
  virtual ~LimitTraderV2() {}

  // Updates the (internal) trader state and emits a new limit order.
  virtual void Update(const OhlcTick& ohlc_tick, float security_balance,
                      float cash_balance, std::vector<Order>* orders);

  // Returns a string representation of the trader (and its configuration).
  virtual std::string ToString() const;

  // Returns a new (freshly initialized) instance of the same limit trader.
  virtual TraderInstance NewInstance() const;

  // Returns a batch of limit traders (for all combinations of parameters).
  static TraderBatch GetBatch(const std::vector<float>& alphas_per_hour,
                              const std::vector<float>& limit_buy_margins,
                              const std::vector<float>& limit_sell_margins);

 private:
  // Trader configuration.
  LimitTraderV2Config trader_config_;
  // Last UNIX timestamp (in seconds) when the trader was updated.
  int timestamp_sec_ = 0;
  // UNIX timestamp (in seconds) when the trader was initialized.
  int init_timestamp_sec_ = 0;
  // Last security (crypto currency) smoothed price.
  float smoothed_price_ = 0;

  // Minimum required timestamp age (in seconds) for emitting the first order.
  int min_age_sec_ = 10 * 60;  // 10 min.
  // Maximum allowed timestamp age (in seconds).
  int max_age_sec_ = 10 * 24 * 60 * 60;  // 10 days.

  // Initializes the trader.
  void InitTrader(int timestamp_sec, float price);
  // Updates the trader's smoothed price.
  void UpdateSmoothedPrice(int timestamp_sec, float price);
  // Emits one limit buy and one limit sell order based on the actual price
  // and account balances.
  void EmitLimitOrders(float price, float security_balance, float cash_balance,
                       std::vector<Order>* orders) const;
};

}  // namespace trader

#endif  // LIMIT_TRADER_V2_H
