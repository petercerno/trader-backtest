// Copyright © 2017 Peter Cerno. All rights reserved.

#include "limit_trader.h"

#include <iomanip>
#include <sstream>

namespace trader {
namespace {
// Number of seconds per hour.
static const int kSecondsPerHour = 60 * 60;
}  // namespace

LimitTrader::LimitTrader(const LimitTraderConfig& trader_config)
    : trader_config_(trader_config) {}

void LimitTrader::Update(const OhlcTick& ohlc_tick, float security_balance,
                         float cash_balance, std::vector<Order>* orders) {
  assert(orders != nullptr);  // Output orders must be provided.
  const int timestamp_sec = ohlc_tick.timestamp_sec();
  const float price = ohlc_tick.close();
  assert(timestamp_sec > timestamp_sec_);  // Invalid OHLC tick timestamp.
  assert(price > 0);                       // Invalid OHLC tick close price.
  assert(security_balance > 0 || cash_balance > 0);  // Invalid balance(s).
  if (timestamp_sec >= timestamp_sec_ + max_age_sec_ || mode_ == Mode::NONE) {
    InitTrader(timestamp_sec, price);
  } else {
    UpdateSmoothedPrice(timestamp_sec, price);
  }
  timestamp_sec_ = timestamp_sec;
  security_balance_ = security_balance;
  cash_balance_ = cash_balance;
  mode_ = (security_balance * price >= cash_balance) ? Mode::IN_LONG
                                                     : Mode::IN_CASH;
  EmitLimitOrder(price, orders);
}

std::string LimitTrader::ToString() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(3) << "limit-trader ["
     << trader_config_.alpha_per_hour() << " "
     << trader_config_.limit_order_margin() << "]";
  return ss.str();
}

TraderInstance LimitTrader::NewInstance() const {
  return TraderInstance(new LimitTrader(trader_config_));
}

TraderBatch LimitTrader::GetBatch(
    const std::vector<float>& alphas_per_hour,
    const std::vector<float>& limit_order_margins) {
  TraderBatch batch;
  for (float alpha_per_hour : alphas_per_hour)
    for (float limit_order_margin : limit_order_margins) {
      LimitTraderConfig trader_config;
      trader_config.set_alpha_per_hour(alpha_per_hour);
      trader_config.set_limit_order_margin(limit_order_margin);
      batch.emplace_back(new LimitTrader(trader_config));
    }
  return batch;
}

void LimitTrader::InitTrader(int timestamp_sec, float price) {
  init_timestamp_sec_ = timestamp_sec;
  smoothed_price_ = price;
}

void LimitTrader::UpdateSmoothedPrice(int timestamp_sec, float price) {
  const float sampling_rate_sec =
      std::min(kSecondsPerHour, timestamp_sec - timestamp_sec_);
  float alpha_per_tick = std::exp(sampling_rate_sec / kSecondsPerHour *
                                  std::log(trader_config_.alpha_per_hour()));
  smoothed_price_ =
      alpha_per_tick * smoothed_price_ + (1.0f - alpha_per_tick) * price;
}

void LimitTrader::EmitLimitOrder(float price,
                                 std::vector<Order>* orders) const {
  if (timestamp_sec_ < init_timestamp_sec_ + min_age_sec_) {
    return;
  }
  float target_price = 0;
  orders->emplace_back();
  Order* order = &orders->back();
  order->set_type(Order_Type_LIMIT);
  if (mode_ == Mode::IN_LONG) {
    target_price =
        smoothed_price_ * (1.0f + trader_config_.limit_order_margin());
    if (price > target_price) {
      target_price = 1.001f * price;
    }
    order->set_side(Order_Side_SELL);
    order->set_security_amount(security_balance_);
  } else {
    assert(mode_ == Mode::IN_CASH);  // Invalid mode.
    target_price =
        smoothed_price_ * (1.0f - trader_config_.limit_order_margin());
    if (price < target_price) {
      target_price = 0.999f * price;
    }
    order->set_side(Order_Side_BUY);
    order->set_cash_amount(cash_balance_);
  }
  order->set_price(target_price);
}

}  // namespace trader
