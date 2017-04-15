// Copyright © 2017 Peter Cerno. All rights reserved.

#ifndef LIMIT_TRADER_H
#define LIMIT_TRADER_H

#include "trader_base.h"

namespace trader {

// Limit trader. Emits exactly one limit order per OHLC tick.
class LimitTrader : public TraderInterface {
 public:
  explicit LimitTrader(const LimitTraderConfig& trader_config);
  virtual ~LimitTrader() {}

  // Updates the (internal) trader state and emits a new limit order.
  virtual void Update(const OhlcTick& ohlc_tick, float security_balance,
                      float cash_balance, std::vector<Order>* orders);

  // Returns a string representation of the trader (and its configuration).
  virtual std::string ToString() const;

  // Returns a new (freshly initialized) instance of the same limit trader.
  virtual TraderInstance NewInstance() const;

  // Returns a batch of limit traders (for all combinations of parameters).
  static TraderBatch GetBatch(const std::vector<float>& alphas_per_hour,
                              const std::vector<float>& limit_order_margins);

 private:
  // Enumeration of possible trader modes.
  enum class Mode {
    NONE,     // Undefined.
    IN_LONG,  // Trader holds all its assets in security (crypto currency).
    IN_CASH   // Trader holds all its assets in base currency.
  };

  // Trader configuration.
  LimitTraderConfig trader_config_;
  // Last UNIX timestamp (in seconds) when the trader was updated.
  int timestamp_sec_ = 0;
  // Last security (crypto currency) balance.
  float security_balance_ = 0;
  // Last cash balance.
  float cash_balance_ = 0;
  // Last trader mode.
  Mode mode_ = Mode::NONE;
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
  // Emits the limit order based on the (updated) internal trader state.
  void EmitLimitOrder(float price, std::vector<Order>* orders) const;
};

}  // namespace trader

#endif  // LIMIT_TRADER_H
