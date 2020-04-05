// Copyright © 2020 Peter Cerno. All rights reserved.

#include "trader_account.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace trader {
namespace {
float Floor(float amount, float unit) {
  return unit > 0 ? unit * std::floor(amount / unit) : amount;
}

float Ceil(float amount, float unit) {
  return unit > 0 ? unit * std::ceil(amount / unit) : amount;
}

float Round(float amount, float unit) {
  return unit > 0 ? unit * std::round(amount / unit) : amount;
}
}  // namespace

float TraderAccount::GetFee(const FeeConfig& fee_config,
                            float cash_amount) const {
  return Ceil(std::max(fee_config.minimum_fee(),
                       fee_config.fixed_fee() +
                           cash_amount * fee_config.relative_fee()),
              cash_unit);
}

float TraderAccount::GetMarketBuyPrice(const OhlcTick& ohlc_tick) const {
  return market_liquidity * ohlc_tick.open() +
         (1.0f - market_liquidity) * ohlc_tick.high();
}

float TraderAccount::GetMarketSellPrice(const OhlcTick& ohlc_tick) const {
  return market_liquidity * ohlc_tick.open() +
         (1.0f - market_liquidity) * ohlc_tick.low();
}

float TraderAccount::GetStopBuyPrice(const OhlcTick& ohlc_tick,
                                     float price) const {
  return market_liquidity * std::max(price, ohlc_tick.open()) +
         (1.0f - market_liquidity) * ohlc_tick.high();
}

float TraderAccount::GetStopSellPrice(const OhlcTick& ohlc_tick,
                                      float price) const {
  return market_liquidity * std::min(price, ohlc_tick.open()) +
         (1.0f - market_liquidity) * ohlc_tick.low();
}

float TraderAccount::GetMaxSecurityAmount(const OhlcTick& ohlc_tick) const {
  return max_volume_ratio > 0
             ? Floor(max_volume_ratio * ohlc_tick.volume(), security_unit)
             : std::numeric_limits<float>::max();
}

bool TraderAccount::Buy(const FeeConfig& fee_config, float price,
                        float security_amount) {
  assert(price > 0);
  assert(security_amount >= 0);
  security_amount = Round(security_amount, security_unit);
  if (security_amount < security_unit) {
    return false;
  }
  const float cash_amount = Ceil(security_amount * price, cash_unit);
  const float cash_fee = GetFee(fee_config, cash_amount);
  const float total_cash_amount = cash_amount + cash_fee;
  if (total_cash_amount > cash_balance) {
    return false;
  }
  security_balance = Round(security_balance + security_amount, security_unit);
  cash_balance = Round(cash_balance - total_cash_amount, cash_unit);
  total_fee = Round(total_fee + cash_fee, cash_unit);
  return true;
}

bool TraderAccount::BuyAtCash(const FeeConfig& fee_config, float price,
                              float cash_amount, float max_security_amount) {
  assert(price > 0);
  assert(cash_amount >= 0);
  cash_amount = Round(cash_amount, cash_unit);
  if (cash_amount < cash_unit || cash_amount > cash_balance) {
    return false;
  }
  const float cash_fee = GetFee(fee_config, cash_amount);
  if (cash_amount <= cash_fee) {
    return false;
  }
  const float security_amount =
      Floor(std::min((cash_amount - cash_fee) / price, max_security_amount),
            security_unit);
  if (security_amount < security_unit) {
    return false;
  }
  return Buy(fee_config, price, security_amount);
}

bool TraderAccount::Sell(const FeeConfig& fee_config, float price,
                         float security_amount) {
  assert(price > 0);
  assert(security_amount >= 0);
  security_amount = Round(security_amount, security_unit);
  if (security_amount < security_unit || security_amount > security_balance) {
    return false;
  }
  const float cash_amount = Floor(security_amount * price, cash_unit);
  const float cash_fee = GetFee(fee_config, cash_amount);
  const float total_cash_amount = cash_amount - cash_fee;
  if (total_cash_amount < cash_unit) {
    return false;
  }
  security_balance = Round(security_balance - security_amount, security_unit);
  cash_balance = Round(cash_balance + total_cash_amount, cash_unit);
  total_fee = Round(total_fee + cash_fee, cash_unit);
  return true;
}

bool TraderAccount::SellAtCash(const FeeConfig& fee_config, float price,
                               float cash_amount, float max_security_amount) {
  assert(price > 0);
  assert(cash_amount >= 0);
  cash_amount = Round(cash_amount, cash_unit);
  if (cash_amount < cash_unit) {
    return false;
  }
  const float cash_fee = GetFee(fee_config, cash_amount);
  const float security_amount =
      Floor(std::min((cash_amount + cash_fee) / price, max_security_amount),
            security_unit);
  if (security_amount < security_unit) {
    return false;
  }
  // Note: when we sell security_amount of security, we receive at most:
  //   (cash_amount + cash_fee) - GetFee(cash_amount + cash_fee) cash.
  // Since GetFee(cash_amount) <= GetFee(cash_amount + cash_fee),
  // We receive at most cash_amount cash.
  return Sell(fee_config, price, security_amount);
}

bool TraderAccount::MarketBuy(const FeeConfig& fee_config,
                              const OhlcTick& ohlc_tick,
                              float security_amount) {
  const float price = GetMarketBuyPrice(ohlc_tick);
  return Buy(fee_config, price, security_amount);
}

bool TraderAccount::MarketBuyAtCash(const FeeConfig& fee_config,
                                    const OhlcTick& ohlc_tick,
                                    float cash_amount) {
  const float price = GetMarketBuyPrice(ohlc_tick);
  return BuyAtCash(fee_config, price, cash_amount);
}

bool TraderAccount::MarketSell(const FeeConfig& fee_config,
                               const OhlcTick& ohlc_tick,
                               float security_amount) {
  const float price = GetMarketSellPrice(ohlc_tick);
  return Sell(fee_config, price, security_amount);
}

bool TraderAccount::MarketSellAtCash(const FeeConfig& fee_config,
                                     const OhlcTick& ohlc_tick,
                                     float cash_amount) {
  const float price = GetMarketSellPrice(ohlc_tick);
  return SellAtCash(fee_config, price, cash_amount);
}

bool TraderAccount::StopBuy(const FeeConfig& fee_config,
                            const OhlcTick& ohlc_tick, float stop_price,
                            float security_amount) {
  assert(stop_price > 0);
  assert(security_amount >= 0);
  // Stop buy order can be executed only if the actual price jumps above
  // the stop order price.
  if (ohlc_tick.high() < stop_price) {
    return false;
  }
  const float price = GetStopBuyPrice(ohlc_tick, stop_price);
  return Buy(fee_config, price, security_amount);
}

bool TraderAccount::StopBuyAtCash(const FeeConfig& fee_config,
                                  const OhlcTick& ohlc_tick, float stop_price,
                                  float cash_amount) {
  assert(stop_price > 0);
  assert(cash_amount >= 0);
  // Stop buy order can be executed only if the actual price jumps above
  // the stop order price.
  if (ohlc_tick.high() < stop_price) {
    return false;
  }
  const float price = GetStopBuyPrice(ohlc_tick, stop_price);
  return BuyAtCash(fee_config, price, cash_amount);
}

bool TraderAccount::StopSell(const FeeConfig& fee_config,
                             const OhlcTick& ohlc_tick, float stop_price,
                             float security_amount) {
  assert(stop_price > 0);
  assert(security_amount >= 0);
  // Stop sell order can be executed only if the actual price drops below
  // the stop order price.
  if (ohlc_tick.low() > stop_price) {
    return false;
  }
  const float price = GetStopSellPrice(ohlc_tick, stop_price);
  return Sell(fee_config, price, security_amount);
}

bool TraderAccount::StopSellAtCash(const FeeConfig& fee_config,
                                   const OhlcTick& ohlc_tick, float stop_price,
                                   float cash_amount) {
  assert(stop_price > 0);
  assert(cash_amount >= 0);
  // Stop sell order can be executed only if the actual price drops below
  // the stop order price.
  if (ohlc_tick.low() > stop_price) {
    return false;
  }
  const float price = GetStopSellPrice(ohlc_tick, stop_price);
  return SellAtCash(fee_config, price, cash_amount);
}

bool TraderAccount::LimitBuy(const FeeConfig& fee_config,
                             const OhlcTick& ohlc_tick, float limit_price,
                             float security_amount) {
  assert(limit_price > 0);
  assert(security_amount >= 0);
  // Limit buy order can be executed only if the actual price drops below
  // the limit order price.
  if (ohlc_tick.low() > limit_price) {
    return false;
  }
  security_amount = std::min(security_amount, GetMaxSecurityAmount(ohlc_tick));
  return Buy(fee_config, limit_price, security_amount);
}

bool TraderAccount::LimitBuyAtCash(const FeeConfig& fee_config,
                                   const OhlcTick& ohlc_tick, float limit_price,
                                   float cash_amount) {
  assert(limit_price > 0);
  assert(cash_amount >= 0);
  // Limit buy order can be executed only if the actual price drops below
  // the limit order price.
  if (ohlc_tick.low() > limit_price) {
    return false;
  }
  const float max_security_amount = GetMaxSecurityAmount(ohlc_tick);
  return BuyAtCash(fee_config, limit_price, cash_amount, max_security_amount);
}

bool TraderAccount::LimitSell(const FeeConfig& fee_config,
                              const OhlcTick& ohlc_tick, float limit_price,
                              float security_amount) {
  assert(limit_price > 0);
  assert(security_amount >= 0);
  // Limit sell order can be executed only if the actual price jumps above
  // the limit order price.
  if (ohlc_tick.high() < limit_price) {
    return false;
  }
  security_amount = std::min(security_amount, GetMaxSecurityAmount(ohlc_tick));
  return Sell(fee_config, limit_price, security_amount);
}

bool TraderAccount::LimitSellAtCash(const FeeConfig& fee_config,
                                    const OhlcTick& ohlc_tick,
                                    float limit_price, float cash_amount) {
  assert(limit_price > 0);
  assert(cash_amount >= 0);
  // Limit sell order can be executed only if the actual price jumps above
  // the limit order price.
  if (ohlc_tick.high() < limit_price) {
    return false;
  }
  const float max_security_amount = GetMaxSecurityAmount(ohlc_tick);
  return SellAtCash(fee_config, limit_price, cash_amount, max_security_amount);
}

}  // namespace trader
