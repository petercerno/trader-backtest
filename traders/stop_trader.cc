// Copyright © 2021 Peter Cerno. All rights reserved.

#include "traders/stop_trader.h"

#include "absl/memory/memory.h"
#include "absl/strings/str_format.h"

namespace trader {

void StopTrader::Update(const OhlcTick& ohlc_tick,
                        const std::vector<float>& side_input_signals,
                        float base_balance, float quote_balance,
                        std::vector<Order>& orders) {
  const int64_t timestamp_sec = ohlc_tick.timestamp_sec();
  const float price = ohlc_tick.close();
  assert(timestamp_sec > last_timestamp_sec_);
  assert(price > 0);
  assert(base_balance > 0 || quote_balance > 0);
  const Mode mode =
      (base_balance * price >= quote_balance) ? Mode::LONG : Mode::CASH;
  if (timestamp_sec >= last_timestamp_sec_ + max_allowed_gap_sec_ ||
      mode != mode_) {
    if (mode == Mode::LONG) {
      stop_order_price_ = (1 - trader_config_.stop_order_margin()) * price;
    } else {
      assert(mode == Mode::CASH);
      stop_order_price_ = (1 + trader_config_.stop_order_margin()) * price;
    }
  } else {
    assert(mode == mode_);
    UpdateStopOrderPrice(mode, timestamp_sec, price);
  }
  last_base_balance_ = base_balance;
  last_quote_balance_ = quote_balance;
  last_timestamp_sec_ = timestamp_sec;
  last_close_ = price;
  mode_ = mode;
  EmitStopOrder(price, orders);
}

void StopTrader::UpdateStopOrderPrice(Mode mode, int64_t timestamp_sec,
                                      float price) {
  const float sampling_rate_sec = std::min(static_cast<int64_t>(kSecondsPerDay),
                                           timestamp_sec - last_timestamp_sec_);
  const float ticks_per_day = kSecondsPerDay / sampling_rate_sec;
  if (mode == Mode::LONG) {
    const float stop_order_increase_threshold =
        (1 - trader_config_.stop_order_move_margin()) * price;
    if (stop_order_price_ <= stop_order_increase_threshold) {
      const float stop_order_increase_per_tick =
          std::exp(std::log(1 + trader_config_.stop_order_increase_per_day()) /
                   ticks_per_day) -
          1;
      stop_order_price_ = std::max(
          stop_order_price_,
          std::min(stop_order_increase_threshold,
                   (1 + stop_order_increase_per_tick) * stop_order_price_));
    }
  } else {
    assert(mode == Mode::CASH);
    const float stop_order_decrease_threshold =
        (1 + trader_config_.stop_order_move_margin()) * price;
    if (stop_order_price_ >= stop_order_decrease_threshold) {
      const float stop_order_decrease_per_tick =
          1 -
          std::exp(std::log(1 - trader_config_.stop_order_decrease_per_day()) /
                   ticks_per_day);
      stop_order_price_ = std::min(
          stop_order_price_,
          std::max(stop_order_decrease_threshold,
                   (1 - stop_order_decrease_per_tick) * stop_order_price_));
    }
  }
}

void StopTrader::EmitStopOrder(float price, std::vector<Order>& orders) const {
  orders.emplace_back();
  Order& order = orders.back();
  order.set_type(Order_Type_STOP);
  if (mode_ == Mode::LONG) {
    order.set_side(Order_Side_SELL);
    order.set_base_amount(last_base_balance_);
  } else {
    assert(mode_ == Mode::CASH);
    order.set_side(Order_Side_BUY);
    order.set_quote_amount(last_quote_balance_);
  }
  order.set_price(stop_order_price_);
}

std::string StopTrader::GetInternalState() const {
  return absl::StrFormat("%d,%.3f,%.3f,%.3f,%s,%.3f", last_timestamp_sec_,
                         last_base_balance_, last_quote_balance_, last_close_,
                         mode_ == Mode::LONG ? "LONG" : "CASH",
                         stop_order_price_);
}

std::string StopTraderEmitter::GetName() const {
  return absl::StrFormat("stop-trader[%.3f|%.3f|%.3f|%.3f]",
                         trader_config_.stop_order_margin(),
                         trader_config_.stop_order_move_margin(),
                         trader_config_.stop_order_increase_per_day(),
                         trader_config_.stop_order_decrease_per_day());
}

std::unique_ptr<Trader> StopTraderEmitter::NewTrader() const {
  return absl::make_unique<StopTrader>(trader_config_);
}

std::vector<std::unique_ptr<TraderEmitter>>
StopTraderEmitter::GetBatchOfTraders(
    const std::vector<float>& stop_order_margins,
    const std::vector<float>& stop_order_move_margins,
    const std::vector<float>& stop_order_increases_per_day,
    const std::vector<float>& stop_order_decreases_per_day) {
  std::vector<std::unique_ptr<TraderEmitter>> batch;
  batch.reserve(stop_order_margins.size() * stop_order_move_margins.size() *
                stop_order_increases_per_day.size() *
                stop_order_decreases_per_day.size());
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
          batch.emplace_back(new StopTraderEmitter(trader_config));
        }
  return batch;
}

}  // namespace trader
