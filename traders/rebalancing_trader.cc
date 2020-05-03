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
  const float portfolio_value = security_balance * price + cash_balance;
  const float alpha = trader_config_.alpha();
  const float beta = trader_config_.beta();
  float upper_deviation = trader_config_.upper_deviation();
  float lower_deviation = trader_config_.lower_deviation();
  const float market_sell_security_amount =
      security_balance / (1.0f + alpha) -
      alpha / (1.0f + alpha) * cash_balance / price;
  const float market_buy_security_amount =
      alpha / (1.0f + alpha) * cash_balance / price -
      security_balance / (1.0f + alpha);
  if (security_balance * price >
          alpha * (1.0f + upper_deviation) * cash_balance &&
      market_sell_security_amount * price >= beta * portfolio_value) {
    orders->emplace_back();
    Order* order = &orders->back();
    order->set_type(Order_Type_MARKET);
    order->set_side(Order_Side_SELL);
    order->set_security_amount(market_sell_security_amount);
  } else if (security_balance * price <
                 alpha * (1.0f - lower_deviation) * cash_balance &&
             market_buy_security_amount * price >= beta * portfolio_value) {
    orders->emplace_back();
    Order* order = &orders->back();
    order->set_type(Order_Type_MARKET);
    order->set_side(Order_Side_BUY);
    order->set_security_amount(market_buy_security_amount);
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
      const float sell_security_amount = security_balance / (1.0f + alpha) *
                                         upper_deviation /
                                         (1.0f + upper_deviation);
      const float sell_price =
          alpha * (1.0f + upper_deviation) * cash_balance / security_balance;
      assert(sell_security_amount * sell_price >=
             0.99f * beta * (security_balance * sell_price + cash_balance));
      orders->emplace_back();
      Order* sell_order = &orders->back();
      sell_order->set_type(Order_Type_LIMIT);
      sell_order->set_side(Order_Side_SELL);
      sell_order->set_security_amount(sell_security_amount);
      sell_order->set_price(sell_price);
    }
    if (allow_limit_buy) {
      const float buy_security_amount = security_balance / (1.0f + alpha) *
                                        lower_deviation /
                                        (1.0f - lower_deviation);
      const float buy_price =
          alpha * (1.0f - lower_deviation) * cash_balance / security_balance;
      assert(buy_security_amount * buy_price >=
             0.99f * beta * (security_balance * buy_price + cash_balance));
      orders->emplace_back();
      Order* buy_order = &orders->back();
      buy_order->set_type(Order_Type_LIMIT);
      buy_order->set_side(Order_Side_BUY);
      buy_order->set_security_amount(buy_security_amount);
      buy_order->set_price(buy_price);
    }
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
      trader_config.set_beta(0.1f);
      trader_config.set_upper_deviation(deviation);
      trader_config.set_lower_deviation(deviation);
      batch.emplace_back(new RebalancingTraderFactory(trader_config));
    }
  return batch;
}

}  // namespace trader
