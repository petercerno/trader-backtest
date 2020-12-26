// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef TRADERS_REBALANCING_TRADER_H
#define TRADERS_REBALANCING_TRADER_H

#include "base/trader.h"
#include "traders/trader_config.pb.h"

namespace trader {

// RebalancingTrader keeps the base (crypto) currency value to quote value
// ratio constant.
class RebalancingTrader : public Trader {
 public:
  explicit RebalancingTrader(const RebalancingTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~RebalancingTrader() {}

  void Update(const OhlcTick& ohlc_tick, float base_balance,
              float quote_balance, std::vector<Order>& orders) override;
  std::string GetInternalState() const override;

 private:
  RebalancingTraderConfig trader_config_;
  // Last seen trader account balance.
  float last_base_balance_ = 0.0f;
  float last_quote_balance_ = 0.0f;
  // Last seen UNIX timestamp (in seconds).
  int last_timestamp_sec_ = 0;
  // Last seen close price.
  float last_close_ = 0.0f;
};

// Emitter that emits RebalancingTraders.
class RebalancingTraderEmitter : public TraderEmitter {
 public:
  explicit RebalancingTraderEmitter(
      const RebalancingTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~RebalancingTraderEmitter() {}

  std::string GetName() const override;
  std::unique_ptr<Trader> NewTrader() const override;

  static std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfTraders(
      const std::vector<float>& alphas, const std::vector<float>& deviations);

 private:
  RebalancingTraderConfig trader_config_;
};

}  // namespace trader

#endif  // TRADERS_REBALANCING_TRADER_H
