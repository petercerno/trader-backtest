// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef TRADERS_LIMIT_TRADER_H
#define TRADERS_LIMIT_TRADER_H

#include "base/trader.h"
#include "traders/trader_config.pb.h"

namespace trader {

// Limit trader emits one limit buy and one limit sell order per OHLC tick.
class LimitTrader : public Trader {
 public:
  explicit LimitTrader(const LimitTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~LimitTrader() {}

  void Update(const OhlcTick& ohlc_tick, float base_balance,
              float quote_balance, std::vector<Order>& orders) override;
  std::string GetInternalState() const override;

 private:
  LimitTraderConfig trader_config_;
  // Last seen trader account balance.
  float last_base_balance_ = 0.0f;
  float last_quote_balance_ = 0.0f;
  // Last seen UNIX timestamp (in seconds).
  int last_timestamp_sec_ = 0;
  // The latest UNIX timestamp (in seconds) when the trader was initialized.
  int init_timestamp_sec_ = 0;
  // Last seen close price.
  float last_close_ = 0.0f;
  // Last base (crypto) currency smoothed price.
  float smoothed_price_ = 0;

  // Minimum required warm-up period (in seconds) before emitting orders.
  int warmup_period_sec_ = 10 * 60;  // 10 min.
  // Maximum allowed timestamp gap (in seconds).
  // When we encounter such gap, we re-initialize and then warm-up the trader.
  int max_allowed_gap_sec_ = 10 * 24 * 60 * 60;  // 10 days.

  // Emits one limit buy and one limit sell order based on the smoothed price
  // and account balances.
  void EmitLimitOrders(std::vector<Order>& orders) const;
};

// Emitter that emits LimitTraders.
class LimitTraderEmitter : public TraderEmitter {
 public:
  explicit LimitTraderEmitter(const LimitTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~LimitTraderEmitter() {}

  std::string GetName() const override;
  std::unique_ptr<Trader> NewTrader() const override;

  static std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfTraders(
      const std::vector<float>& alphas_per_hour,
      const std::vector<float>& limit_buy_margins,
      const std::vector<float>& limit_sell_margins);

 private:
  LimitTraderConfig trader_config_;
};

}  // namespace trader

#endif  // TRADERS_LIMIT_TRADER_H
