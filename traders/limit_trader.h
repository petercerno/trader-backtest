// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef TRADERS_LIMIT_TRADER_H
#define TRADERS_LIMIT_TRADER_H

#include "lib/trader_interface.h"
#include "traders/trader_config.pb.h"

namespace trader {

// Limit trader emits one limit buy and one limit sell order per OHLC tick.
class LimitTrader : public TraderInterface {
 public:
  explicit LimitTrader(const LimitTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~LimitTrader() {}

  void Update(const OhlcTick& ohlc_tick, float security_balance,
              float cash_balance, std::vector<Order>* orders) override;
  void LogInternalState(std::ostream* os) const override;

 private:
  LimitTraderConfig trader_config_;
  // Last seen trader account balance.
  float last_security_balance_ = 0.0f;
  float last_cash_balance_ = 0.0f;
  // Last seen UNIX timestamp (in seconds).
  int last_timestamp_sec_ = 0;
  // The latest UNIX timestamp (in seconds) when the trader was initialized.
  int init_timestamp_sec_ = 0;
  // Last seen close price.
  float last_close_ = 0.0f;
  // Last security (crypto currency) smoothed price.
  float smoothed_price_ = 0;

  // Minimum required warm-up period (in seconds) before emitting orders.
  int warmup_period_sec_ = 10 * 60;  // 10 min.
  // Maximum allowed timestamp gap (in seconds).
  // When we encounter such gap, we re-initialize and then warm-up the trader.
  int max_allowed_gap_sec_ = 10 * 24 * 60 * 60;  // 10 days.

  // Emits one limit buy and one limit sell order based on the smoothed price
  // and account balances.
  void EmitLimitOrders(std::vector<Order>* orders) const;
};

// Factory that emits LimitTraders.
class LimitTraderFactory : public TraderFactoryInterface {
 public:
  explicit LimitTraderFactory(const LimitTraderConfig& trader_config)
      : trader_config_(trader_config) {}
  virtual ~LimitTraderFactory() {}

  std::string GetTraderName() const override;
  std::unique_ptr<TraderInterface> NewTrader() const override;

  static std::vector<std::unique_ptr<TraderFactoryInterface>> GetBatchOfTraders(
      const std::vector<float>& alphas_per_hour,
      const std::vector<float>& limit_buy_margins,
      const std::vector<float>& limit_sell_margins);

 private:
  LimitTraderConfig trader_config_;
};

}  // namespace trader

#endif  // TRADERS_LIMIT_TRADER_H
