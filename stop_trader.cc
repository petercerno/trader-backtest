// Copyright © 2020 Peter Cerno. All rights reserved.

#include "stop_trader.h"

#include <iomanip>
#include <ostream>
#include <sstream>

namespace trader {

void StopTrader::Update(const OhlcTick& ohlc_tick, float security_balance,
                        float cash_balance, std::vector<Order>* orders) {
  assert(orders != nullptr);
  const int timestamp_sec = ohlc_tick.timestamp_sec();
  const float price = ohlc_tick.close();
  assert(timestamp_sec > last_timestamp_sec_);
  assert(price > 0);
  assert(security_balance > 0 || cash_balance > 0);
  const Mode mode = (security_balance * price >= cash_balance) ? Mode::IN_LONG
                                                               : Mode::IN_CASH;
  if (timestamp_sec >= last_timestamp_sec_ + max_allowed_gap_sec_ ||
      mode != mode_) {
    if (mode == Mode::IN_LONG) {
      stop_order_price_ = (1 - trader_config_.stop_order_margin()) * price;
    } else {
      assert(mode == Mode::IN_CASH);
      stop_order_price_ = (1 + trader_config_.stop_order_margin()) * price;
    }
  } else {
    assert(mode == mode_);
    UpdateStopOrderPrice(mode, timestamp_sec, price);
  }
  last_security_balance_ = security_balance;
  last_cash_balance_ = cash_balance;
  last_timestamp_sec_ = timestamp_sec;
  last_close_ = price;
  mode_ = mode;
  EmitStopOrder(price, orders);
}

void StopTrader::UpdateStopOrderPrice(Mode mode, int timestamp_sec,
                                      float price) {
  static constexpr int kSecondsPerDay = 24 * 60 * 60;
  const float sampling_rate_sec =
      std::min(kSecondsPerDay, timestamp_sec - last_timestamp_sec_);
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
    assert(mode == Mode::IN_CASH);
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

void StopTrader::EmitStopOrder(float price, std::vector<Order>* orders) const {
  orders->emplace_back();
  Order* order = &orders->back();
  order->set_type(Order_Type_STOP);
  if (mode_ == Mode::IN_LONG) {
    order->set_side(Order_Side_SELL);
    order->set_security_amount(last_security_balance_);
  } else {
    assert(mode_ == Mode::IN_CASH);
    order->set_side(Order_Side_BUY);
    order->set_cash_amount(last_cash_balance_);
  }
  order->set_price(stop_order_price_);
}

void StopTrader::LogInternalState(std::ostream* os) const {
  if (os == nullptr) {
    return;
  }
  std::string mode;
  if (mode_ == Mode::IN_LONG) {
    mode = "IN_LONG";
  } else {
    assert(mode_ == Mode::IN_CASH);
    mode = "IN_CASH";
  }
  *os << std::fixed << std::setprecision(0)  // nowrap
      << last_timestamp_sec_ << ","          // nowrap
      << std::setprecision(3)                // nowrap
      << last_security_balance_ << ","       // nowrap
      << last_cash_balance_ << ","           // nowrap
      << last_close_ << ","                  // nowrap
      << mode << ","                         // nowrap
      << stop_order_price_ << std::endl;     // nowrap
}

std::string StopTraderFactory::GetTraderName() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(3) << "stop-trader["
     << trader_config_.stop_order_margin() << "|"
     << trader_config_.stop_order_move_margin() << "|"
     << trader_config_.stop_order_increase_per_day() << "|"
     << trader_config_.stop_order_decrease_per_day() << "]";
  return ss.str();
}

std::unique_ptr<TraderInterface> StopTraderFactory::NewTrader() const {
  return std::unique_ptr<StopTrader>(new StopTrader(trader_config_));
}

std::vector<std::unique_ptr<TraderFactoryInterface>>
StopTraderFactory::GetBatchOfTraders(
    const std::vector<float>& stop_order_margins,
    const std::vector<float>& stop_order_move_margins,
    const std::vector<float>& stop_order_increases_per_day,
    const std::vector<float>& stop_order_decreases_per_day) {
  std::vector<std::unique_ptr<TraderFactoryInterface>> batch;
  for (const float stop_order_margin : stop_order_margins)
    for (const float stop_order_move_margin : stop_order_move_margins)
      for (const float stop_order_increase_per_day :
           stop_order_increases_per_day)
        for (const float stop_order_decrease_per_day :
             stop_order_decreases_per_day) {
          StopTraderConfig trader_config;
          trader_config.set_stop_order_margin(stop_order_margin);
          trader_config.set_stop_order_move_margin(stop_order_move_margin);
          trader_config.set_stop_order_increase_per_day(
              stop_order_increase_per_day);
          trader_config.set_stop_order_decrease_per_day(
              stop_order_decrease_per_day);
          batch.emplace_back(new StopTraderFactory(trader_config));
        }
  return batch;
}

}  // namespace trader
