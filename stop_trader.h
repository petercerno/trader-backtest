// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef STOP_TRADER_H
#define STOP_TRADER_H

#include "trader_base.h"

namespace trader {

// Stop trader. Emits exactly one stop order per OHLC tick.
class StopTrader : public TraderInterface {
 public:
  explicit StopTrader(const StopTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~StopTrader() {}

  void Update(const OhlcTick& ohlc_tick, float security_balance,
              float cash_balance, std::vector<Order>* orders) override;
  void LogInternalState(std::ostream* os) const override;

 private:
  // Enumeration of possible trader modes.
  enum class Mode {
    NONE,     // Undefined.
    IN_LONG,  // Trader holds most of its assets in security (crypto currency).
    IN_CASH   // Trader holds most of its assets in base currency.
  };

  StopTraderConfig trader_config_;
  // Last seen trader account balance.
  float last_security_balance_ = 0.0f;
  float last_cash_balance_ = 0.0f;
  // Last seen UNIX timestamp (in seconds).
  int last_timestamp_sec_ = 0;
  // Last seen close price.
  float last_close_ = 0.0f;
  // Last trader mode.
  Mode mode_ = Mode::NONE;
  // Last security (crypto currency) price for stop orders.
  float stop_order_price_ = 0;

  // Maximum allowed timestamp gap (in seconds).
  // When we encounter such gap, we re-initialize the trader.
  int max_allowed_gap_sec_ = 10 * 24 * 60 * 60;  // 10 days.

  // Updates the trader stop order price.
  void UpdateStopOrderPrice(Mode mode, int timestamp_sec, float price);
  // Emits the stop order based on the (updated) internal trader state.
  void EmitStopOrder(float price, std::vector<Order>* orders) const;
};

// Factory that emits StopTraders.
class StopTraderFactory : public TraderFactoryInterface {
 public:
  explicit StopTraderFactory(const StopTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~StopTraderFactory() {}

  std::string GetTraderName() const override;
  std::unique_ptr<TraderInterface> NewTrader() const override;

  static std::vector<std::unique_ptr<TraderFactoryInterface>> GetBatchOfTraders(
      const std::vector<float>& stop_order_margins,
      const std::vector<float>& stop_order_move_margins,
      const std::vector<float>& stop_order_increases_per_day,
      const std::vector<float>& stop_order_decreases_per_day);

 private:
  StopTraderConfig trader_config_;
};

}  // namespace trader

#endif  // STOP_TRADER_H
