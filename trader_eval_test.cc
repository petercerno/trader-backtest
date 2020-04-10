// Copyright © 2020 Peter Cerno. All rights reserved.

#include "trader_eval.h"

#include "gtest/gtest.h"

namespace trader {
namespace {
void SetupOhlcTick(OhlcTick* ohlc_tick) {
  ohlc_tick->set_open(10.0f);
  ohlc_tick->set_high(20.0f);
  ohlc_tick->set_low(2.0f);
  ohlc_tick->set_close(15.0f);
  ohlc_tick->set_volume(1234.56f);
}
}  // namespace

TEST(ExecuteOrderTest, MarketBuyWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config =
      trader_account_config.mutable_market_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_MARKET);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_security_amount(10.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.market_liquidity = 0.5f;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy 10.0 units of security (crypto currency).
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(834.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(16.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, MarketBuyAtCashWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config =
      trader_account_config.mutable_market_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_MARKET);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_cash_amount(169.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.market_liquidity = 0.5f;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy security (crypto currency) with up to 169.0 units in cash.
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(834.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(16.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, MarketSellWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config =
      trader_account_config.mutable_market_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_MARKET);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_security_amount(5.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.market_liquidity = 1.0f;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell 5.0 units of security (crypto currency).
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(994.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, MarketSellAtCashWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config =
      trader_account_config.mutable_market_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_MARKET);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_cash_amount(50.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.market_liquidity = 1.0f;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell security (crypto currency) to get up to 50.0 units in cash.
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(9.4f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(999.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(7.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, StopBuyWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config = trader_account_config.mutable_stop_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_STOP);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_security_amount(10.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.market_liquidity = 0.5f;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy 10.0 units of security (crypto currency).
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  order.set_price(25.0f);
  ASSERT_FALSE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  order.set_price(15.0f);
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(806.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(19.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, StopBuyAtCashWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config = trader_account_config.mutable_stop_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_STOP);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_cash_amount(197.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.market_liquidity = 0.5f;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy security (crypto currency) with up to 197.0 units in cash.
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  order.set_price(25.0f);
  ASSERT_FALSE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  order.set_price(15.0f);
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(806.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(19.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, StopSellWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config = trader_account_config.mutable_stop_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_STOP);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_security_amount(5.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.market_liquidity = 1.0f;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell 5.0 units of security (crypto currency).
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  order.set_price(1.0f);
  ASSERT_FALSE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  // Stop price 5.0 is below the OHLC tick low price 2.0. Order is executed.
  order.set_price(5.0f);
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(971.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(4.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, StopSellAtCashWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config = trader_account_config.mutable_stop_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_STOP);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_cash_amount(50.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.market_liquidity = 1.0f;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell security (crypto currency) to get up to 50.0 units in cash.
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  order.set_price(1.0f);
  ASSERT_FALSE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  // Stop price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  order.set_price(5.0f);
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(3.8f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(999.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(7.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, LimitBuyWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config =
      trader_account_config.mutable_limit_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_LIMIT);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_security_amount(10.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.max_volume_ratio = 0.1;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy 10.0 units of security (crypto currency).
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  order.set_price(1.0f);
  ASSERT_FALSE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  order.set_price(5.0f);
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(944.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, LimitBuyAtCashWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config =
      trader_account_config.mutable_limit_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_LIMIT);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_cash_amount(57.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.max_volume_ratio = 0.1;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy security (crypto currency) with up to 57.0 units in cash.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  order.set_price(1.0f);
  ASSERT_FALSE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  order.set_price(5.0f);
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(944.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, LimitSellWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config =
      trader_account_config.mutable_limit_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_LIMIT);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_security_amount(5.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.max_volume_ratio = 0.1;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell 5.0 units of security (crypto currency).
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  order.set_price(25.0f);
  ASSERT_FALSE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  order.set_price(10.0f);
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(994.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);
}

TEST(ExecuteOrderTest, LimitSellAtCashWithFeeAndLimitedPrecision) {
  TraderAccountConfig trader_account_config;
  FeeConfig* fee_config =
      trader_account_config.mutable_limit_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_LIMIT);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_cash_amount(50.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  TraderAccount trader_account;
  trader_account.max_volume_ratio = 0.1;
  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;
  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell security (crypto currency) to get up to 50.0 units in cash.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  order.set_price(25.0f);
  ASSERT_FALSE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  order.set_price(10.0f);
  ASSERT_TRUE(
      ExecuteOrder(trader_account_config, order, ohlc_tick, &trader_account));
  EXPECT_FLOAT_EQ(9.4f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(999.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(7.0f, trader_account.total_fee);
}

}  // namespace trader
