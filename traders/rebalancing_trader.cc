// Copyright © 2020 Peter Cerno. All rights reserved.

#include "traders/rebalancing_trader.h"

namespace trader {

void RebalancingTrader::Update(const OhlcTick& ohlc_tick,
                               float security_balance, float cash_balance,
                               std::vector<Order>* orders) {
  assert(orders != nullptr);
  const int timestamp_sec = ohlc_tick.timestamp_sec();
  const float price = ohlc_tick.close();
  assert(timestamp_sec > last_timestamp_sec_);
  assert(price > 0);
  assert(security_balance > 0 || cash_balance > 0);
  const float alpha = trader_config_.alpha();
  const float upper_deviation = trader_config_.upper_deviation();
  const float lower_deviation = trader_config_.lower_deviation();
  if (security_balance * price >
      alpha * (1.0f + upper_deviation) * cash_balance) {
    orders->emplace_back();
    Order* order = &orders->back();
    order->set_type(Order_Type_MARKET);
    order->set_side(Order_Side_SELL);
    order->set_security_amount(security_balance / (1.0f + alpha) -
                               alpha / (1.0f + alpha) * cash_balance / price);
  } else if (security_balance * price <
             alpha * (1.0f - lower_deviation) * cash_balance) {
    orders->emplace_back();
    Order* order = &orders->back();
    order->set_type(Order_Type_MARKET);
    order->set_side(Order_Side_BUY);
    order->set_security_amount(alpha / (1.0f + alpha) * cash_balance / price -
                               security_balance / (1.0f + alpha));
  } else {
    orders->emplace_back();
    Order* sell_order = &orders->back();
    sell_order->set_type(Order_Type_LIMIT);
    sell_order->set_side(Order_Side_SELL);
    sell_order->set_security_amount(security_balance / (1.0f + alpha) *
                                    upper_deviation / (1.0f + upper_deviation));
    sell_order->set_price(alpha * (1.0f + upper_deviation) * cash_balance /
                          security_balance);
    orders->emplace_back();
    Order* buy_order = &orders->back();
    buy_order->set_type(Order_Type_LIMIT);
    buy_order->set_side(Order_Side_BUY);
    buy_order->set_security_amount(security_balance / (1.0f + alpha) *
                                   lower_deviation / (1.0f - lower_deviation));
    buy_order->set_price(alpha * (1.0f - lower_deviation) * cash_balance /
                         security_balance);
  }
  last_security_balance_ = security_balance;
  last_cash_balance_ = cash_balance;
  last_timestamp_sec_ = timestamp_sec;
  last_close_ = price;
}

void RebalancingTrader::LogInternalState(std::ostream* os) const {
  if (os == nullptr) {
    return;
  }
  *os << std::fixed << std::setprecision(0)  // nowrap
      << last_timestamp_sec_ << ","          // nowrap
      << std::setprecision(3)                // nowrap
      << last_security_balance_ << ","       // nowrap
      << last_cash_balance_ << ","           // nowrap
      << last_close_ << std::endl;           // nowrap
}

std::string RebalancingTraderFactory::GetTraderName() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(3) << "rebalancing-trader["
     << trader_config_.alpha() << "|" << trader_config_.upper_deviation() << "|"
     << trader_config_.lower_deviation() << "]";
  return ss.str();
}

std::unique_ptr<TraderInterface> RebalancingTraderFactory::NewTrader() const {
  return std::unique_ptr<RebalancingTrader>(
      new RebalancingTrader(trader_config_));
}

std::vector<std::unique_ptr<TraderFactoryInterface>>
RebalancingTraderFactory::GetBatchOfTraders(
    const std::vector<float>& alphas, const std::vector<float>& deviations) {
  std::vector<std::unique_ptr<TraderFactoryInterface>> batch;
  for (const float alpha : alphas)
    for (const float deviation : deviations) {
      RebalancingTraderConfig trader_config;
      trader_config.set_alpha(alpha);
      trader_config.set_upper_deviation(deviation);
      trader_config.set_lower_deviation(deviation);
      batch.emplace_back(new RebalancingTraderFactory(trader_config));
    }
  return batch;
}

}  // namespace trader
