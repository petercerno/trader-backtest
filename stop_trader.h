// Copyright © 2017 Peter Cerno. All rights reserved.

#ifndef STOP_TRADER_H
#define STOP_TRADER_H

#include "trader_base.h"

namespace trader {

// Stop trader. Emits exactly one stop order per OHLC tick.
class StopTrader : public TraderInterface {
 public:
  explicit StopTrader(const StopTraderConfig& trader_config);
  virtual ~StopTrader() {}

  // Updates the (internal) trader state and emits a new stop order.
  virtual void Update(const OhlcTick& ohlc_tick, float security_balance,
                      float cash_balance, std::vector<Order>* orders);

  // Returns a string representation of the trader (and its configuration).
  virtual std::string ToString() const;

  // Returns a new (freshly initialized) instance of the same stop trader.
  virtual TraderInstance NewInstance() const;

  // Returns a batch of stop traders (for all combinations of parameters).
  static TraderBatch GetBatch(
      const std::vector<float>& stop_order_margins,
      const std::vector<float>& stop_order_move_margins,
      const std::vector<float>& stop_order_increases_per_day,
      const std::vector<float>& stop_order_decreases_per_day);

 private:
  // Enumeration of possible trader modes.
  enum class Mode {
    NONE,     // Undefined.
    IN_LONG,  // Trader holds all its assets in security (crypto currency).
    IN_CASH   // Trader holds all its assets in base currency.
  };

  // Trader configuration.
  StopTraderConfig trader_config_;
  // Last UNIX timestamp (in seconds) when the trader was updated.
  int timestamp_sec_ = 0;
  // Last security (crypto currency) balance.
  float security_balance_ = 0;
  // Last cash balance.
  float cash_balance_ = 0;
  // Last trader mode.
  Mode mode_ = Mode::NONE;
  // Last security (crypto currency) price for stop orders.
  float stop_order_price_ = 0;

  // Minimum required timestamp age (in seconds) for adjusting the stop price.
  int min_age_sec_ = 30;  // 30 sec.
  // Maximum allowed timestamp age (in seconds).
  int max_age_sec_ = 10 * 24 * 60 * 60;  // 10 days.

  // Initializes the trader stop order price.
  void InitStopOrderPrice(Mode mode, float price);
  // Updates the trader stop order price.
  void UpdateStopOrderPrice(Mode mode, int timestamp_sec, float price);
  // Emits the stop order based on the (updated) internal trader state.
  void EmitStopOrder(float price, std::vector<Order>* orders) const;
};

}  // namespace trader

#endif  // STOP_TRADER_H
