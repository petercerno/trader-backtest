// Copyright © 2021 Peter Cerno. All rights reserved.

#ifndef TRADERS_STOP_TRADER_H
#define TRADERS_STOP_TRADER_H

#include "base/trader.h"
#include "traders/trader_config.pb.h"

namespace trader {

// Stop trader. Emits exactly one stop order per OHLC tick.
class StopTrader : public Trader {
 public:
  explicit StopTrader(const StopTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~StopTrader() {}

  void Update(const OhlcTick& ohlc_tick,
              const std::vector<float>& side_input_signals, float base_balance,
              float quote_balance, std::vector<Order>& orders) override;
  std::string GetInternalState() const override;

 private:
  // Enumeration of possible trader modes.
  enum class Mode {
    NONE,  // Undefined.
    LONG,  // Trader holds most of its assets in the base (crypto) currency.
    CASH   // Trader holds most of its assets in the quote currency.
  };

  StopTraderConfig trader_config_;
  // Last seen trader account balance.
  float last_base_balance_ = 0.0f;
  float last_quote_balance_ = 0.0f;
  // Last seen UNIX timestamp (in seconds).
  int64_t last_timestamp_sec_ = 0;
  // Last seen close price.
  float last_close_ = 0.0f;
  // Last trader mode.
  Mode mode_ = Mode::NONE;
  // Last base (crypto) currency price for the stop order.
  float stop_order_price_ = 0;

  // Maximum allowed timestamp gap (in seconds).
  // When we encounter such gap, we re-initialize the trader.
  int max_allowed_gap_sec_ = 1 * 60 * 60;  // 1 hour.

  // Updates the trader stop order price.
  void UpdateStopOrderPrice(Mode mode, int64_t timestamp_sec, float price);
  // Emits the stop order based on the (updated) internal trader state.
  void EmitStopOrder(float price, std::vector<Order>& orders) const;
};

// Emitter that emits StopTraders.
class StopTraderEmitter : public TraderEmitter {
 public:
  explicit StopTraderEmitter(const StopTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~StopTraderEmitter() {}

  std::string GetName() const override;
  std::unique_ptr<Trader> NewTrader() const override;

  static std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfTraders(
      const std::vector<float>& stop_order_margins,
      const std::vector<float>& stop_order_move_margins,
      const std::vector<float>& stop_order_increases_per_day,
      const std::vector<float>& stop_order_decreases_per_day);

 private:
  StopTraderConfig trader_config_;
};

}  // namespace trader

#endif  // TRADERS_STOP_TRADER_H
