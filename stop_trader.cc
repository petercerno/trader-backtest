// Copyright © 2017 Peter Cerno. All rights reserved.

#include "stop_trader.h"

#include <iomanip>
#include <sstream>

namespace trader {
namespace {
// Number of seconds per day.
static const int kSecondsPerDay = 24 * 60 * 60;
}  // namespace

StopTrader::StopTrader(const StopTraderConfig& trader_config)
    : trader_config_(trader_config) {}

void StopTrader::Update(const OhlcTick& ohlc_tick, float security_balance,
                        float cash_balance, std::vector<Order>* orders) {
  assert(orders != nullptr);  // Output orders must be provided.
  const int timestamp_sec = ohlc_tick.timestamp_sec();
  const float price = ohlc_tick.close();
  assert(timestamp_sec > timestamp_sec_);  // Invalid OHLC tick timestamp.
  assert(price > 0);                       // Invalid OHLC tick close price.
  assert(security_balance > 0 || cash_balance > 0);  // Invalid balance(s).
  const Mode mode = (security_balance * price >= cash_balance) ? Mode::IN_LONG
                                                               : Mode::IN_CASH;
  if (timestamp_sec >= timestamp_sec_ + max_age_sec_ || mode != mode_) {
    InitStopOrderPrice(mode, price);
  } else if (timestamp_sec >= timestamp_sec_ + min_age_sec_) {
    assert(mode == mode_);
    UpdateStopOrderPrice(mode, timestamp_sec, price);
  }
  timestamp_sec_ = timestamp_sec;
  security_balance_ = security_balance;
  cash_balance_ = cash_balance;
  mode_ = mode;
  EmitStopOrder(orders);
}

std::string StopTrader::ToString() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(3) << "stop-trader ["
     << trader_config_.stop_order_margin() << " "
     << trader_config_.stop_order_move_margin() << " "
     << trader_config_.stop_order_increase_per_day() << " "
     << trader_config_.stop_order_decrease_per_day() << "]";
  return ss.str();
}

TraderInstance StopTrader::NewInstance() const {
  return TraderInstance(new StopTrader(trader_config_));
}

TraderBatch StopTrader::GetBatch(
    const std::vector<float>& stop_order_margins,
    const std::vector<float>& stop_order_move_margins,
    const std::vector<float>& stop_order_increases_per_day,
    const std::vector<float>& stop_order_decreases_per_day) {
  TraderBatch batch;
  for (float stop_order_margin : stop_order_margins)
    for (float stop_order_move_margin : stop_order_move_margins)
      for (float stop_order_increase_per_day : stop_order_increases_per_day)
        for (float stop_order_decrease_per_day : stop_order_decreases_per_day) {
          StopTraderConfig trader_config;
          trader_config.set_stop_order_margin(stop_order_margin);
          trader_config.set_stop_order_move_margin(stop_order_move_margin);
          trader_config.set_stop_order_increase_per_day(
              stop_order_increase_per_day);
          trader_config.set_stop_order_decrease_per_day(
              stop_order_decrease_per_day);
          batch.emplace_back(new StopTrader(trader_config));
        }
  return batch;
}

void StopTrader::InitStopOrderPrice(Mode mode, float price) {
  if (mode == Mode::IN_LONG) {
    stop_order_price_ = (1 - trader_config_.stop_order_margin()) * price;
  } else {
    assert(mode == Mode::IN_CASH);  // Invalid mode.
    stop_order_price_ = (1 + trader_config_.stop_order_margin()) * price;
  }
}

void StopTrader::UpdateStopOrderPrice(Mode mode, int timestamp_sec,
                                      float price) {
  const float sampling_rate_sec =
      std::min(kSecondsPerDay, timestamp_sec - timestamp_sec_);
  const float ticks_per_day = kSecondsPerDay / sampling_rate_sec;
  if (mode == Mode::IN_LONG) {
    const float stop_order_increase_threshold =
        (1 + trader_config_.stop_order_move_margin()) * stop_order_price_;
    if (price > stop_order_increase_threshold) {
      const float stop_order_increase_per_tick =
          std::exp(std::log(1 + trader_config_.stop_order_increase_per_day()) /
                   ticks_per_day) -
          1;
      stop_order_price_ = std::min(
          price, (1 + stop_order_increase_per_tick) * stop_order_price_);
    }
  } else {
    assert(mode == Mode::IN_CASH);  // Invalid mode.
    const float stop_order_decrease_threshold =
        (1 - trader_config_.stop_order_move_margin()) * stop_order_price_;
    if (price < stop_order_decrease_threshold) {
      const float stop_order_decrease_per_tick =
          1 -
          std::exp(std::log(1 - trader_config_.stop_order_decrease_per_day()) /
                   ticks_per_day);
      stop_order_price_ = std::max(
          price, (1 - stop_order_decrease_per_tick) * stop_order_price_);
    }
  }
}

void StopTrader::EmitStopOrder(std::vector<Order>* orders) const {
  orders->emplace_back();
  Order* order = &orders->back();
  order->set_type(Order_Type_STOP);
  order->set_price(stop_order_price_);
  if (mode_ == Mode::IN_LONG) {
    order->set_side(Order_Side_SELL);
    order->set_security_amount(security_balance_);
  } else {
    assert(mode_ == Mode::IN_CASH);  // Invalid mode.
    order->set_side(Order_Side_BUY);
    order->set_cash_amount(cash_balance_);
  }
}

}  // namespace trader
