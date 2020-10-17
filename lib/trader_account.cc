// Copyright © 2020 Peter Cerno. All rights reserved.

#include "lib/trader_account.h"

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
                            float quote_amount) const {
  return Ceil(std::max(fee_config.minimum_fee(),
                       fee_config.fixed_fee() +
                           quote_amount * fee_config.relative_fee()),
              quote_unit);
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

float TraderAccount::GetMaxBaseAmount(const OhlcTick& ohlc_tick) const {
  return max_volume_ratio > 0
             ? Floor(max_volume_ratio * ohlc_tick.volume(), base_unit)
             : std::numeric_limits<float>::max();
}

bool TraderAccount::BuyBase(const FeeConfig& fee_config, float base_amount,
                            float price) {
  assert(price > 0);
  assert(base_amount >= 0);
  base_amount = Round(base_amount, base_unit);
  if (base_amount < base_unit) {
    return false;
  }
  const float quote_amount = Ceil(base_amount * price, quote_unit);
  const float quote_fee = GetFee(fee_config, quote_amount);
  const float total_quote_amount = quote_amount + quote_fee;
  if (total_quote_amount > quote_balance) {
    return false;
  }
  base_balance = Round(base_balance + base_amount, base_unit);
  quote_balance = Round(quote_balance - total_quote_amount, quote_unit);
  total_fee = Round(total_fee + quote_fee, quote_unit);
  return true;
}

bool TraderAccount::BuyAtQuote(const FeeConfig& fee_config, float quote_amount,
                               float price, float max_base_amount) {
  assert(price > 0);
  assert(quote_amount >= 0);
  quote_amount = Round(quote_amount, quote_unit);
  if (quote_amount < quote_unit || quote_amount > quote_balance) {
    return false;
  }
  const float quote_fee = GetFee(fee_config, quote_amount);
  if (quote_amount <= quote_fee) {
    return false;
  }
  const float base_amount = Floor(
      std::min((quote_amount - quote_fee) / price, max_base_amount), base_unit);
  if (base_amount < base_unit) {
    return false;
  }
  return BuyBase(fee_config, base_amount, price);
}

bool TraderAccount::SellBase(const FeeConfig& fee_config, float base_amount,
                             float price) {
  assert(price > 0);
  assert(base_amount >= 0);
  base_amount = Round(base_amount, base_unit);
  if (base_amount < base_unit || base_amount > base_balance) {
    return false;
  }
  const float quote_amount = Floor(base_amount * price, quote_unit);
  const float quote_fee = GetFee(fee_config, quote_amount);
  const float total_quote_amount = quote_amount - quote_fee;
  if (total_quote_amount < quote_unit) {
    return false;
  }
  base_balance = Round(base_balance - base_amount, base_unit);
  quote_balance = Round(quote_balance + total_quote_amount, quote_unit);
  total_fee = Round(total_fee + quote_fee, quote_unit);
  return true;
}

bool TraderAccount::SellAtQuote(const FeeConfig& fee_config, float quote_amount,
                                float price, float max_base_amount) {
  assert(price > 0);
  assert(quote_amount >= 0);
  quote_amount = Round(quote_amount, quote_unit);
  if (quote_amount < quote_unit) {
    return false;
  }
  const float quote_fee = GetFee(fee_config, quote_amount);
  const float base_amount = Floor(
      std::min((quote_amount + quote_fee) / price, max_base_amount), base_unit);
  if (base_amount < base_unit) {
    return false;
  }
  // Note: When we sell base_amount of base currency, we receive at most:
  //   (quote_amount + quote_fee) - GetFee(quote_amount + quote_fee)
  // Since GetFee(quote_amount) <= GetFee(quote_amount + quote_fee),
  // We receive at most quote_amount of quote currency.
  return SellBase(fee_config, base_amount, price);
}

bool TraderAccount::MarketBuy(const FeeConfig& fee_config,
                              const OhlcTick& ohlc_tick, float base_amount) {
  const float price = GetMarketBuyPrice(ohlc_tick);
  return BuyBase(fee_config, base_amount, price);
}

bool TraderAccount::MarketBuyAtQuote(const FeeConfig& fee_config,
                                     const OhlcTick& ohlc_tick,
                                     float quote_amount) {
  const float price = GetMarketBuyPrice(ohlc_tick);
  return BuyAtQuote(fee_config, quote_amount, price);
}

bool TraderAccount::MarketSell(const FeeConfig& fee_config,
                               const OhlcTick& ohlc_tick, float base_amount) {
  const float price = GetMarketSellPrice(ohlc_tick);
  return SellBase(fee_config, base_amount, price);
}

bool TraderAccount::MarketSellAtQuote(const FeeConfig& fee_config,
                                      const OhlcTick& ohlc_tick,
                                      float quote_amount) {
  const float price = GetMarketSellPrice(ohlc_tick);
  return SellAtQuote(fee_config, quote_amount, price);
}

bool TraderAccount::StopBuy(const FeeConfig& fee_config,
                            const OhlcTick& ohlc_tick, float base_amount,
                            float stop_price) {
  assert(stop_price > 0);
  assert(base_amount >= 0);
  // Stop buy order can be executed only if the actual price jumps above
  // the stop order price.
  if (ohlc_tick.high() < stop_price) {
    return false;
  }
  const float price = GetStopBuyPrice(ohlc_tick, stop_price);
  return BuyBase(fee_config, base_amount, price);
}

bool TraderAccount::StopBuyAtQuote(const FeeConfig& fee_config,
                                   const OhlcTick& ohlc_tick,
                                   float quote_amount, float stop_price) {
  assert(stop_price > 0);
  assert(quote_amount >= 0);
  // Stop buy order can be executed only if the actual price jumps above
  // the stop order price.
  if (ohlc_tick.high() < stop_price) {
    return false;
  }
  const float price = GetStopBuyPrice(ohlc_tick, stop_price);
  return BuyAtQuote(fee_config, quote_amount, price);
}

bool TraderAccount::StopSell(const FeeConfig& fee_config,
                             const OhlcTick& ohlc_tick, float base_amount,
                             float stop_price) {
  assert(stop_price > 0);
  assert(base_amount >= 0);
  // Stop sell order can be executed only if the actual price drops below
  // the stop order price.
  if (ohlc_tick.low() > stop_price) {
    return false;
  }
  const float price = GetStopSellPrice(ohlc_tick, stop_price);
  return SellBase(fee_config, base_amount, price);
}

bool TraderAccount::StopSellAtQuote(const FeeConfig& fee_config,
                                    const OhlcTick& ohlc_tick,
                                    float quote_amount, float stop_price) {
  assert(stop_price > 0);
  assert(quote_amount >= 0);
  // Stop sell order can be executed only if the actual price drops below
  // the stop order price.
  if (ohlc_tick.low() > stop_price) {
    return false;
  }
  const float price = GetStopSellPrice(ohlc_tick, stop_price);
  return SellAtQuote(fee_config, quote_amount, price);
}

bool TraderAccount::LimitBuy(const FeeConfig& fee_config,
                             const OhlcTick& ohlc_tick, float base_amount,
                             float limit_price) {
  assert(limit_price > 0);
  assert(base_amount >= 0);
  // Limit buy order can be executed only if the actual price drops below
  // the limit order price.
  if (ohlc_tick.low() > limit_price) {
    return false;
  }
  base_amount = std::min(base_amount, GetMaxBaseAmount(ohlc_tick));
  return BuyBase(fee_config, base_amount, limit_price);
}

bool TraderAccount::LimitBuyAtQuote(const FeeConfig& fee_config,
                                    const OhlcTick& ohlc_tick,
                                    float quote_amount, float limit_price) {
  assert(limit_price > 0);
  assert(quote_amount >= 0);
  // Limit buy order can be executed only if the actual price drops below
  // the limit order price.
  if (ohlc_tick.low() > limit_price) {
    return false;
  }
  const float max_base_amount = GetMaxBaseAmount(ohlc_tick);
  return BuyAtQuote(fee_config, quote_amount, limit_price, max_base_amount);
}

bool TraderAccount::LimitSell(const FeeConfig& fee_config,
                              const OhlcTick& ohlc_tick, float base_amount,
                              float limit_price) {
  assert(limit_price > 0);
  assert(base_amount >= 0);
  // Limit sell order can be executed only if the actual price jumps above
  // the limit order price.
  if (ohlc_tick.high() < limit_price) {
    return false;
  }
  base_amount = std::min(base_amount, GetMaxBaseAmount(ohlc_tick));
  return SellBase(fee_config, base_amount, limit_price);
}

bool TraderAccount::LimitSellAtQuote(const FeeConfig& fee_config,
                                     const OhlcTick& ohlc_tick,
                                     float quote_amount, float limit_price) {
  assert(limit_price > 0);
  assert(quote_amount >= 0);
  // Limit sell order can be executed only if the actual price jumps above
  // the limit order price.
  if (ohlc_tick.high() < limit_price) {
    return false;
  }
  const float max_base_amount = GetMaxBaseAmount(ohlc_tick);
  return SellAtQuote(fee_config, quote_amount, limit_price, max_base_amount);
}

}  // namespace trader
