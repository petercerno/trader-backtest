// Copyright © 2020 Peter Cerno. All rights reserved.

#include "traders/rebalancing_trader.h"

namespace trader {

void RebalancingTrader::Update(const OhlcTick& ohlc_tick,
                               const std::vector<float>& side_input_signals,
                               float base_balance, float quote_balance,
                               std::vector<Order>& orders) {
  const int timestamp_sec = ohlc_tick.timestamp_sec();
  const float price = ohlc_tick.close();
  assert(timestamp_sec > last_timestamp_sec_);
  assert(price > 0);
  assert(base_balance > 0 || quote_balance > 0);
  const float portfolio_value = base_balance * price + quote_balance;
  const float alpha = trader_config_.alpha();
  const float epsilon = trader_config_.epsilon();
  const float alpha_up = alpha * (1 + epsilon);
  const float alpha_down = alpha * (1 - epsilon);
  const float beta = base_balance * price / portfolio_value;
  if (beta > alpha_up) {
    const float market_sell_base_amount =
        ((1 - alpha) * portfolio_value - quote_balance) / price;
    orders.emplace_back();
    Order& order = orders.back();
    order.set_type(Order_Type_MARKET);
    order.set_side(Order_Side_SELL);
    order.set_base_amount(market_sell_base_amount);
  } else if (beta < alpha_down) {
    const float market_buy_base_amount =
        (quote_balance - (1 - alpha) * portfolio_value) / price;
    orders.emplace_back();
    Order& order = orders.back();
    order.set_type(Order_Type_MARKET);
    order.set_side(Order_Side_BUY);
    order.set_base_amount(market_buy_base_amount);
  } else if (base_balance > 1.0e-6f && quote_balance > 1.0e-6f) {
    if (alpha * (1 + epsilon) < 1) {
      const float sell_price =
          (alpha * (1 + epsilon) * quote_balance) / (1 - alpha * (1 + epsilon));
      if (sell_price > price && sell_price < 100.0f * price) {
        const float sell_base_amount = base_balance * epsilon / (1 + epsilon);
        orders.emplace_back();
        Order& sell_order = orders.back();
        sell_order.set_type(Order_Type_LIMIT);
        sell_order.set_side(Order_Side_SELL);
        sell_order.set_base_amount(sell_base_amount);
        sell_order.set_price(sell_price);
      }
    }
    const float buy_price =
        (alpha * (1 - epsilon) * quote_balance) / (1 - alpha * (1 - epsilon));
    if (buy_price < price && buy_price > price / 100.0f) {
      const float buy_base_amount = base_balance * epsilon / (1 - epsilon);
      orders.emplace_back();
      Order& buy_order = orders.back();
      buy_order.set_type(Order_Type_LIMIT);
      buy_order.set_side(Order_Side_BUY);
      buy_order.set_base_amount(buy_base_amount);
      buy_order.set_price(buy_price);
    }
  }
  last_base_balance_ = base_balance;
  last_quote_balance_ = quote_balance;
  last_timestamp_sec_ = timestamp_sec;
  last_close_ = price;
}

std::string RebalancingTrader::GetInternalState() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(0)  // nowrap
     << last_timestamp_sec_ << ","          // nowrap
     << std::setprecision(3)                // nowrap
     << last_base_balance_ << ","           // nowrap
     << last_quote_balance_ << ","          // nowrap
     << last_close_;                        // nowrap
  return ss.str();
}

std::string RebalancingTraderEmitter::GetName() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(3) << "rebalancing-trader["
     << trader_config_.alpha() << "|" << trader_config_.epsilon() << "]";
  return ss.str();
}

std::unique_ptr<Trader> RebalancingTraderEmitter::NewTrader() const {
  return std::unique_ptr<RebalancingTrader>(
      new RebalancingTrader(trader_config_));
}

std::vector<std::unique_ptr<TraderEmitter>>
RebalancingTraderEmitter::GetBatchOfTraders(
    const std::vector<float>& alphas, const std::vector<float>& epsilons) {
  std::vector<std::unique_ptr<TraderEmitter>> batch;
  for (const float alpha : alphas)
    for (const float epsilon : epsilons) {
      RebalancingTraderConfig trader_config;
      trader_config.set_alpha(alpha);
      trader_config.set_epsilon(epsilon);
      batch.emplace_back(new RebalancingTraderEmitter(trader_config));
    }
  return batch;
}

}  // namespace trader
