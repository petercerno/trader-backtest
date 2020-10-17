// Copyright © 2020 Peter Cerno. All rights reserved.

#include "traders/rebalancing_trader.h"

namespace trader {

void RebalancingTrader::Update(const OhlcTick& ohlc_tick, float base_balance,
                               float quote_balance,
                               std::vector<Order>* orders) {
  assert(orders != nullptr);
  const int timestamp_sec = ohlc_tick.timestamp_sec();
  const float price = ohlc_tick.close();
  assert(timestamp_sec > last_timestamp_sec_);
  assert(price > 0);
  assert(base_balance > 0 || quote_balance > 0);
  const float portfolio_value = base_balance * price + quote_balance;
  const float alpha = trader_config_.alpha();
  const float beta = trader_config_.beta();
  float upper_deviation = trader_config_.upper_deviation();
  float lower_deviation = trader_config_.lower_deviation();
  const float market_sell_base_amount =
      base_balance / (1.0f + alpha) -
      alpha / (1.0f + alpha) * quote_balance / price;
  const float market_buy_base_amount =
      alpha / (1.0f + alpha) * quote_balance / price -
      base_balance / (1.0f + alpha);
  if (base_balance * price > alpha * (1.0f + upper_deviation) * quote_balance &&
      market_sell_base_amount * price >= beta * portfolio_value) {
    orders->emplace_back();
    Order* order = &orders->back();
    order->set_type(Order_Type_MARKET);
    order->set_side(Order_Side_SELL);
    order->set_base_amount(market_sell_base_amount);
  } else if (base_balance * price <
                 alpha * (1.0f - lower_deviation) * quote_balance &&
             market_buy_base_amount * price >= beta * portfolio_value) {
    orders->emplace_back();
    Order* order = &orders->back();
    order->set_type(Order_Type_MARKET);
    order->set_side(Order_Side_BUY);
    order->set_base_amount(market_buy_base_amount);
  } else {
    bool allow_limit_sell = true;
    if (1.0f - (1.0f + alpha) * beta < 0.2) {
      allow_limit_sell = false;
    } else {
      const float min_upper_deviation =
          (1.0f + (1.0f + 1.0f / alpha) * beta) /  // nowrap
              (1.0f - (1.0f + alpha) * beta) -
          1.0f;
      if (upper_deviation < min_upper_deviation) {
        upper_deviation = min_upper_deviation;
      }
    }
    bool allow_limit_buy = true;
    const float min_lower_deviation =
        1.0f - (1.0f - (1.0f + 1.0f / alpha) * beta) /  // nowrap
                   (1.0f + (1.0f + alpha) * beta);
    if (min_lower_deviation > 0.8) {
      allow_limit_buy = false;
    } else if (lower_deviation < min_lower_deviation) {
      lower_deviation = min_lower_deviation;
    }
    if (allow_limit_sell) {
      const float sell_base_amount = base_balance / (1.0f + alpha) *
                                     upper_deviation / (1.0f + upper_deviation);
      const float sell_price =
          alpha * (1.0f + upper_deviation) * quote_balance / base_balance;
      assert(sell_base_amount * sell_price >=
             0.99f * beta * (base_balance * sell_price + quote_balance));
      orders->emplace_back();
      Order* sell_order = &orders->back();
      sell_order->set_type(Order_Type_LIMIT);
      sell_order->set_side(Order_Side_SELL);
      sell_order->set_base_amount(sell_base_amount);
      sell_order->set_price(sell_price);
    }
    if (allow_limit_buy) {
      const float buy_base_amount = base_balance / (1.0f + alpha) *
                                    lower_deviation / (1.0f - lower_deviation);
      const float buy_price =
          alpha * (1.0f - lower_deviation) * quote_balance / base_balance;
      assert(buy_base_amount * buy_price >=
             0.99f * beta * (base_balance * buy_price + quote_balance));
      orders->emplace_back();
      Order* buy_order = &orders->back();
      buy_order->set_type(Order_Type_LIMIT);
      buy_order->set_side(Order_Side_BUY);
      buy_order->set_base_amount(buy_base_amount);
      buy_order->set_price(buy_price);
    }
  }
  last_base_balance_ = base_balance;
  last_quote_balance_ = quote_balance;
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
      << last_base_balance_ << ","           // nowrap
      << last_quote_balance_ << ","          // nowrap
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
      trader_config.set_beta(0.1f);
      trader_config.set_upper_deviation(deviation);
      trader_config.set_lower_deviation(deviation);
      batch.emplace_back(new RebalancingTraderFactory(trader_config));
    }
  return batch;
}

}  // namespace trader
