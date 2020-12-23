// Copyright © 2020 Peter Cerno. All rights reserved.

#include "traders/limit_trader.h"

namespace trader {

void LimitTrader::Update(const OhlcTick& ohlc_tick, float base_balance,
                         float quote_balance, std::vector<Order>& orders) {
  const int timestamp_sec = ohlc_tick.timestamp_sec();
  const float price = ohlc_tick.close();
  assert(timestamp_sec > last_timestamp_sec_);
  assert(price > 0);
  assert(base_balance > 0 || quote_balance > 0);
  if (timestamp_sec >= last_timestamp_sec_ + max_allowed_gap_sec_) {
    init_timestamp_sec_ = timestamp_sec;
    smoothed_price_ = price;
  } else {
    const float sampling_rate_sec =
        std::min(kSecondsPerHour, timestamp_sec - last_timestamp_sec_);
    const float alpha_per_tick =
        std::exp(sampling_rate_sec / kSecondsPerHour *
                 std::log(trader_config_.alpha_per_hour()));
    smoothed_price_ =
        alpha_per_tick * smoothed_price_ + (1.0f - alpha_per_tick) * price;
  }
  last_base_balance_ = base_balance;
  last_quote_balance_ = quote_balance;
  last_timestamp_sec_ = timestamp_sec;
  last_close_ = price;
  EmitLimitOrders(orders);
}

void LimitTrader::EmitLimitOrders(std::vector<Order>& orders) const {
  if (last_timestamp_sec_ < init_timestamp_sec_ + warmup_period_sec_) {
    return;
  }
  if (last_base_balance_ > 0) {
    orders.emplace_back();
    Order& sell_order = orders.back();
    sell_order.set_type(Order_Type_LIMIT);
    sell_order.set_side(Order_Side_SELL);
    float sell_price =
        smoothed_price_ * (1.0f + trader_config_.limit_sell_margin());
    if (last_close_ > sell_price) {
      sell_price = 1.001f * last_close_;
    }
    sell_order.set_base_amount(last_base_balance_);
    sell_order.set_price(sell_price);
  }
  if (last_quote_balance_ > 0) {
    orders.emplace_back();
    Order& buy_order = orders.back();
    buy_order.set_type(Order_Type_LIMIT);
    buy_order.set_side(Order_Side_BUY);
    float buy_price =
        smoothed_price_ * (1.0f - trader_config_.limit_buy_margin());
    if (last_close_ < buy_price) {
      buy_price = 0.999f * last_close_;
    }
    buy_order.set_quote_amount(last_quote_balance_);
    buy_order.set_price(buy_price);
  }
}

std::string LimitTrader::GetInternalState() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(0)  // nowrap
     << last_timestamp_sec_ << ","          // nowrap
     << std::setprecision(3)                // nowrap
     << last_base_balance_ << ","           // nowrap
     << last_quote_balance_ << ","          // nowrap
     << last_close_ << ","                  // nowrap
     << smoothed_price_;                    // nowrap
  return ss.str();
}

std::string LimitTraderFactory::GetTraderName() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(3) << "limit-trader["
     << trader_config_.alpha_per_hour() << "|"
     << trader_config_.limit_buy_margin() << "|"
     << trader_config_.limit_sell_margin() << "]";
  return ss.str();
}

std::unique_ptr<TraderInterface> LimitTraderFactory::NewTrader() const {
  return std::unique_ptr<LimitTrader>(new LimitTrader(trader_config_));
}

std::vector<std::unique_ptr<TraderFactoryInterface>>
LimitTraderFactory::GetBatchOfTraders(
    const std::vector<float>& alphas_per_hour,
    const std::vector<float>& limit_buy_margins,
    const std::vector<float>& limit_sell_margins) {
  std::vector<std::unique_ptr<TraderFactoryInterface>> batch;
  for (const float alpha_per_hour : alphas_per_hour)
    for (const float limit_buy_margin : limit_buy_margins)
      for (const float limit_sell_margin : limit_sell_margins) {
        LimitTraderConfig trader_config;
        trader_config.set_alpha_per_hour(alpha_per_hour);
        trader_config.set_limit_buy_margin(limit_buy_margin);
        trader_config.set_limit_sell_margin(limit_sell_margin);
        batch.emplace_back(new LimitTraderFactory(trader_config));
      }
  return batch;
}

}  // namespace trader
