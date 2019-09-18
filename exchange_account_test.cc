// Copyright © 2017 Peter Cerno. All rights reserved.

#include "exchange_account.h"

#include "gtest/gtest.h"

namespace trader {

class ExchangeAccountTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    config_.set_start_security_balance(10);
    config_.set_start_cash_balance(5000);
    config_.set_security_balance_unit(0.01f);
    config_.set_cash_balance_unit(0.01f);
    config_.mutable_market_order_fee_config()->set_relative_fee(0);
    config_.mutable_market_order_fee_config()->set_fixed_fee(0);
    config_.mutable_market_order_fee_config()->set_minimum_fee(0);
    config_.mutable_limit_order_fee_config()->set_relative_fee(0);
    config_.mutable_limit_order_fee_config()->set_fixed_fee(0);
    config_.mutable_limit_order_fee_config()->set_minimum_fee(0);
    config_.mutable_stop_order_fee_config()->set_relative_fee(0);
    config_.mutable_stop_order_fee_config()->set_fixed_fee(0);
    config_.mutable_stop_order_fee_config()->set_minimum_fee(0);
    config_.set_market_order_accuracy(1.0f);
    config_.set_stop_order_accuracy(1.0f);
    config_.set_limit_order_fill_volume_ratio(1.0f);
  }

 private:
  ExchangeAccountConfig config_;
};

TEST_F(ExchangeAccountTest, IsValidOrderBasicTest) {
  Order invalid_order;
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(invalid_order));
  invalid_order.set_type(Order::MARKET);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(invalid_order));
  invalid_order.set_type(Order::LIMIT);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(invalid_order));
  invalid_order.set_type(Order::STOP);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(invalid_order));
  invalid_order.clear_type();
  invalid_order.set_side(Order::BUY);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(invalid_order));
  invalid_order.set_side(Order::SELL);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(invalid_order));

  Order buy_order;
  buy_order.set_side(Order::BUY);
  buy_order.set_type(Order::MARKET);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(buy_order));
  buy_order.set_cash_amount(1000.0f);
  EXPECT_TRUE(ExchangeAccount::IsValidOrder(buy_order));
  buy_order.set_type(Order::LIMIT);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(buy_order));
  buy_order.set_type(Order::STOP);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(buy_order));
  buy_order.set_type(Order::LIMIT);
  buy_order.set_price(100.0f);
  EXPECT_TRUE(ExchangeAccount::IsValidOrder(buy_order));
  buy_order.set_type(Order::STOP);
  EXPECT_TRUE(ExchangeAccount::IsValidOrder(buy_order));

  Order sell_order;
  sell_order.set_side(Order::SELL);
  sell_order.set_type(Order::MARKET);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(sell_order));
  sell_order.set_security_amount(10.0f);
  EXPECT_TRUE(ExchangeAccount::IsValidOrder(sell_order));
  sell_order.set_type(Order::LIMIT);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(sell_order));
  sell_order.set_type(Order::STOP);
  EXPECT_FALSE(ExchangeAccount::IsValidOrder(sell_order));
  sell_order.set_type(Order::LIMIT);
  sell_order.set_price(100.0f);
  EXPECT_TRUE(ExchangeAccount::IsValidOrder(sell_order));
  sell_order.set_type(Order::STOP);
  EXPECT_TRUE(ExchangeAccount::IsValidOrder(sell_order));
}

TEST_F(ExchangeAccountTest, IsExecutableOrderBasicTest) {
  OhlcTick ohlc_tick;
  ohlc_tick.set_timestamp_sec(1483228800);
  ohlc_tick.set_open(100.0f);
  ohlc_tick.set_high(200.0f);
  ohlc_tick.set_low(50.0f);
  ohlc_tick.set_close(150.0f);
  ohlc_tick.set_volume(10.0f);

  Order market_buy_order;
  market_buy_order.set_type(Order::MARKET);
  market_buy_order.set_side(Order::BUY);
  market_buy_order.set_cash_amount(1000.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(market_buy_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(market_buy_order, ohlc_tick));

  Order market_sell_order;
  market_sell_order.set_type(Order::MARKET);
  market_sell_order.set_side(Order::SELL);
  market_sell_order.set_security_amount(10.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(market_sell_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(market_sell_order, ohlc_tick));

  Order limit_buy_order;
  limit_buy_order.set_type(Order::LIMIT);
  limit_buy_order.set_side(Order::BUY);
  limit_buy_order.set_cash_amount(1000.0f);
  limit_buy_order.set_price(10.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(limit_buy_order));
  EXPECT_FALSE(ExchangeAccount::IsExecutableOrder(limit_buy_order, ohlc_tick));
  limit_buy_order.set_price(50.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(limit_buy_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(limit_buy_order, ohlc_tick));
  limit_buy_order.set_price(100.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(limit_buy_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(limit_buy_order, ohlc_tick));
  limit_buy_order.set_price(500.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(limit_buy_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(limit_buy_order, ohlc_tick));

  Order limit_sell_order;
  limit_sell_order.set_type(Order::LIMIT);
  limit_sell_order.set_side(Order::SELL);
  limit_sell_order.set_security_amount(10.0f);
  limit_sell_order.set_price(500.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(limit_sell_order));
  EXPECT_FALSE(ExchangeAccount::IsExecutableOrder(limit_sell_order, ohlc_tick));
  limit_sell_order.set_price(200.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(limit_sell_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(limit_sell_order, ohlc_tick));
  limit_sell_order.set_price(100.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(limit_sell_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(limit_sell_order, ohlc_tick));
  limit_sell_order.set_price(10.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(limit_sell_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(limit_sell_order, ohlc_tick));

  Order stop_buy_order;
  stop_buy_order.set_type(Order::STOP);
  stop_buy_order.set_side(Order::BUY);
  stop_buy_order.set_cash_amount(1000.0f);
  stop_buy_order.set_price(500.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(stop_buy_order));
  EXPECT_FALSE(ExchangeAccount::IsExecutableOrder(stop_buy_order, ohlc_tick));
  stop_buy_order.set_price(200.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(stop_buy_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(stop_buy_order, ohlc_tick));
  stop_buy_order.set_price(100.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(stop_buy_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(stop_buy_order, ohlc_tick));
  stop_buy_order.set_price(10.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(stop_buy_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(stop_buy_order, ohlc_tick));

  Order stop_sell_order;
  stop_sell_order.set_type(Order::STOP);
  stop_sell_order.set_side(Order::SELL);
  stop_sell_order.set_security_amount(10.0f);
  stop_sell_order.set_price(10.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(stop_sell_order));
  EXPECT_FALSE(ExchangeAccount::IsExecutableOrder(stop_sell_order, ohlc_tick));
  stop_sell_order.set_price(50.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(stop_sell_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(stop_sell_order, ohlc_tick));
  stop_sell_order.set_price(100.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(stop_buy_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(stop_buy_order, ohlc_tick));
  stop_sell_order.set_price(500.0f);
  ASSERT_TRUE(ExchangeAccount::IsValidOrder(stop_buy_order));
  EXPECT_TRUE(ExchangeAccount::IsExecutableOrder(stop_buy_order, ohlc_tick));
}

}  // namespace trader
