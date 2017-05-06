// Copyright © 2017 Peter Cerno. All rights reserved.

#include "limit_trader_v2.h"

#include <iomanip>
#include <sstream>

namespace trader {
namespace {
// Number of seconds per hour.
static const int kSecondsPerHour = 60 * 60;
}  // namespace

LimitTraderV2::LimitTraderV2(const LimitTraderV2Config& trader_config)
    : trader_config_(trader_config) {}

void LimitTraderV2::Update(const OhlcTick& ohlc_tick, float security_balance,
                           float cash_balance, std::vector<Order>* orders) {
  assert(orders != nullptr);  // Output orders must be provided.
  const int timestamp_sec = ohlc_tick.timestamp_sec();
  const float price = ohlc_tick.close();
  assert(timestamp_sec > timestamp_sec_);  // Invalid OHLC tick timestamp.
  assert(price > 0);                       // Invalid OHLC tick close price.
  assert(security_balance > 0 || cash_balance > 0);  // Invalid balance(s).
  if (timestamp_sec >= timestamp_sec_ + max_age_sec_) {
    InitTrader(timestamp_sec, price);
  } else {
    UpdateSmoothedPrice(timestamp_sec, price);
  }
  timestamp_sec_ = timestamp_sec;
  EmitLimitOrders(price, security_balance, cash_balance, orders);
}

std::string LimitTraderV2::ToString() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(3) << "limit-trader-v2 ["
     << trader_config_.alpha_per_hour() << " "
     << trader_config_.limit_buy_margin() << " "
     << trader_config_.limit_sell_margin() << "]";
  return ss.str();
}

TraderInstance LimitTraderV2::NewInstance() const {
  return TraderInstance(new LimitTraderV2(trader_config_));
}

TraderBatch LimitTraderV2::GetBatch(
    const std::vector<float>& alphas_per_hour,
    const std::vector<float>& limit_buy_margins,
    const std::vector<float>& limit_sell_margins) {
  TraderBatch batch;
  for (float alpha_per_hour : alphas_per_hour)
    for (float limit_buy_margin : limit_buy_margins)
      for (float limit_sell_margin : limit_sell_margins) {
        LimitTraderV2Config trader_config;
        trader_config.set_alpha_per_hour(alpha_per_hour);
        trader_config.set_limit_buy_margin(limit_buy_margin);
        trader_config.set_limit_sell_margin(limit_sell_margin);
        batch.emplace_back(new LimitTraderV2(trader_config));
      }
  return batch;
}

void LimitTraderV2::InitTrader(int timestamp_sec, float price) {
  init_timestamp_sec_ = timestamp_sec;
  smoothed_price_ = price;
}

void LimitTraderV2::UpdateSmoothedPrice(int timestamp_sec, float price) {
  const float sampling_rate_sec =
      std::min(kSecondsPerHour, timestamp_sec - timestamp_sec_);
  float alpha_per_tick = std::exp(sampling_rate_sec / kSecondsPerHour *
                                  std::log(trader_config_.alpha_per_hour()));
  smoothed_price_ =
      alpha_per_tick * smoothed_price_ + (1.0f - alpha_per_tick) * price;
}

void LimitTraderV2::EmitLimitOrders(float price, float security_balance,
                                    float cash_balance,
                                    std::vector<Order>* orders) const {
  if (timestamp_sec_ < init_timestamp_sec_ + min_age_sec_) {
    return;
  }
  if (security_balance > 0) {
    // Limit sell order.
    orders->emplace_back();
    Order* sell_order = &orders->back();
    sell_order->set_type(Order_Type_LIMIT);
    sell_order->set_side(Order_Side_SELL);
    float sell_price =
        smoothed_price_ * (1.0f + trader_config_.limit_sell_margin());
    if (price > sell_price) {
      sell_price = 1.001f * price;
    }
    sell_order->set_security_amount(security_balance);
    sell_order->set_price(sell_price);
  }
  if (cash_balance > 0) {
    // Limit buy order.
    orders->emplace_back();
    Order* buy_order = &orders->back();
    buy_order->set_type(Order_Type_LIMIT);
    buy_order->set_side(Order_Side_BUY);
    float buy_price =
        smoothed_price_ * (1.0f - trader_config_.limit_buy_margin());
    if (price < buy_price) {
      buy_price = 0.999f * price;
    }
    buy_order->set_cash_amount(cash_balance);
    buy_order->set_price(buy_price);
  }
}

}  // namespace trader
