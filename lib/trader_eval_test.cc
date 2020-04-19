// Copyright © 2020 Peter Cerno. All rights reserved.

#include "lib/trader_eval.h"

#include <google/protobuf/text_format.h>
#include <google/protobuf/util/message_differencer.h>

#include "gtest/gtest.h"
#include "util/util_time.h"

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

namespace {
// Compares two protobuf messages and outputs a diff if they differ.
void ExpectProtoEq(const google::protobuf::Message& expected_message,
                   const google::protobuf::Message& message,
                   bool full_scope = true) {
  EXPECT_EQ(expected_message.GetTypeName(), message.GetTypeName());
  std::string diff_report;
  google::protobuf::util::MessageDifferencer differencer;
  differencer.set_scope(
      full_scope ? google::protobuf::util::MessageDifferencer::FULL
                 : google::protobuf::util::MessageDifferencer::PARTIAL);
  differencer.ReportDifferencesToString(&diff_report);
  EXPECT_TRUE(differencer.Compare(expected_message, message)) << diff_report;
}

// Adds OHLC tick to the history. Does not set the timestamp_sec.
void AddOhlcTick(float open, float high, float low, float close,
                 OhlcHistory* ohlc_history) {
  ASSERT_LE(low, open);
  ASSERT_LE(low, high);
  ASSERT_LE(low, close);
  ASSERT_GE(high, open);
  ASSERT_GE(high, close);
  ohlc_history->emplace_back();
  OhlcTick* ohlc_tick = &ohlc_history->back();
  ohlc_tick->set_open(open);
  ohlc_tick->set_high(high);
  ohlc_tick->set_low(low);
  ohlc_tick->set_close(close);
  ohlc_tick->set_volume(1000.0f);
}

// Adds daily OHLC tick to the history.
void AddDailyOhlcTick(float open, float high, float low, float close,
                      OhlcHistory* ohlc_history) {
  int timestamp_sec = 1483228800;  // 2017-01-01
  if (!ohlc_history->empty()) {
    ASSERT_FLOAT_EQ(ohlc_history->back().close(), open);
    timestamp_sec = ohlc_history->back().timestamp_sec() + 24 * 60 * 60;
  }
  AddOhlcTick(open, high, low, close, ohlc_history);
  ohlc_history->back().set_timestamp_sec(timestamp_sec);
}

// Adds monthly OHLC tick to the history.
void AddMonthlyOhlcTick(float open, float high, float low, float close,
                        OhlcHistory* ohlc_history) {
  int timestamp_sec = 1483228800;  // 2017-01-01
  if (!ohlc_history->empty()) {
    ASSERT_FLOAT_EQ(ohlc_history->back().close(), open);
    timestamp_sec =
        AddMonthsToTimestampSec(ohlc_history->back().timestamp_sec(), 1);
  }
  AddOhlcTick(open, high, low, close, ohlc_history);
  ohlc_history->back().set_timestamp_sec(timestamp_sec);
}

void SetupDailyOhlcHistory(OhlcHistory* ohlc_history) {
  AddDailyOhlcTick(100, 150, 80, 120, ohlc_history);   // 2017-01-01
  AddDailyOhlcTick(120, 180, 100, 150, ohlc_history);  // 2017-01-02
  AddDailyOhlcTick(150, 250, 100, 140, ohlc_history);  // 2017-01-03
  AddDailyOhlcTick(140, 150, 80, 100, ohlc_history);   // 2017-01-04
  AddDailyOhlcTick(100, 120, 20, 50, ohlc_history);    // 2017-01-05
}

void SetupMonthlyOhlcHistory(OhlcHistory* ohlc_history) {
  AddMonthlyOhlcTick(100, 150, 80, 120, ohlc_history);   // 2017-01-01
  AddMonthlyOhlcTick(120, 180, 100, 150, ohlc_history);  // 2017-02-01
  AddMonthlyOhlcTick(150, 250, 100, 140, ohlc_history);  // 2017-03-01
  AddMonthlyOhlcTick(140, 150, 80, 100, ohlc_history);   // 2017-04-01
  AddMonthlyOhlcTick(100, 120, 20, 50, ohlc_history);    // 2017-05-01
  AddMonthlyOhlcTick(50, 100, 40, 80, ohlc_history);     // 2017-06-01
  AddMonthlyOhlcTick(80, 180, 50, 150, ohlc_history);    // 2017-07-01
  AddMonthlyOhlcTick(150, 250, 120, 240, ohlc_history);  // 2017-08-01
  AddMonthlyOhlcTick(240, 450, 220, 400, ohlc_history);  // 2017-09-01
  AddMonthlyOhlcTick(400, 450, 250, 300, ohlc_history);  // 2017-10-01
  AddMonthlyOhlcTick(300, 700, 220, 650, ohlc_history);  // 2017-11-01
  AddMonthlyOhlcTick(650, 800, 600, 750, ohlc_history);  // 2017-12-01
}

// Trader that buys and sells the security (crypto currency) at fixed prices.
class TestTrader : public TraderInterface {
 public:
  TestTrader(float buy_price, float sell_price)
      : buy_price_(buy_price), sell_price_(sell_price) {}
  virtual ~TestTrader() {}

  void Update(const OhlcTick& ohlc_tick, float security_balance,
              float cash_balance, std::vector<Order>* orders) override {
    assert(orders != nullptr);
    last_security_balance_ = security_balance;
    last_cash_balance_ = cash_balance;
    last_timestamp_sec_ = ohlc_tick.timestamp_sec();
    last_close_ = ohlc_tick.close();
    is_long_ = last_close_ * last_security_balance_ > last_cash_balance_;
    orders->emplace_back();
    Order* order = &orders->back();
    if (is_long_) {
      order->set_type(Order_Type_LIMIT);
      order->set_side(Order_Side_SELL);
      order->set_security_amount(last_security_balance_);
      order->set_price(sell_price_);
    } else {
      order->set_type(Order_Type_LIMIT);
      order->set_side(Order_Side_BUY);
      order->set_cash_amount(last_cash_balance_);
      order->set_price(buy_price_);
    }
  }

  void LogInternalState(std::ostream* os) const override {
    if (os == nullptr) {
      return;
    }
    *os << std::fixed << std::setprecision(0)  // nowrap
        << last_timestamp_sec_ << ","          // nowrap
        << std::setprecision(3)                // nowrap
        << last_security_balance_ << ","       // nowrap
        << last_cash_balance_ << ","           // nowrap
        << last_close_ << ",";                 // nowrap
    if (is_long_) {
      *os << "IN_LONG,LIMIT_SELL@"  // nowrap
          << std::setprecision(0) << sell_price_;
    } else {
      *os << "IN_CASH,LIMIT_BUY@"  // nowrap
          << std::setprecision(0) << buy_price_;
    }
    *os << std::endl;
  }

 private:
  // Price at which we want to buy the security (crypto currency).
  float buy_price_ = 0.0f;
  // Price at which we want to sell the security (crypto currency).
  float sell_price_ = 0.0f;
  // Last seen trader account balance.
  float last_security_balance_ = 0.0f;
  float last_cash_balance_ = 0.0f;
  // Last seen UNIX timestamp (in seconds).
  int last_timestamp_sec_ = 0;
  // Last seen close price.
  float last_close_ = 0.0f;
  // True iff most of the account value is in security (crypto currency).
  bool is_long_ = false;
};

// Factory that emits TestTrader as defined above.
class TestTraderFactory : public TraderFactoryInterface {
 public:
  TestTraderFactory(float buy_price, float sell_price)
      : buy_price_(buy_price), sell_price_(sell_price) {}
  virtual ~TestTraderFactory() {}

  std::string GetTraderName() const override {
    std::stringstream trader_name_os;
    trader_name_os << std::fixed << std::setprecision(0) << "test-trader"
                   << "[" << buy_price_ << "|" << sell_price_ << "]";
    return trader_name_os.str();
  }

  std::unique_ptr<TraderInterface> NewTrader() const override {
    // Note: std::make_unique is C++14 feature.
    return std::unique_ptr<TestTrader>(new TestTrader(buy_price_, sell_price_));
  }

 private:
  // Price at which we want to buy the security (crypto currency).
  float buy_price_ = 0.0f;
  // Price at which we want to sell the security (crypto currency).
  float sell_price_ = 0.0f;
};
}  // namespace

TEST(ExecuteTraderTest, LimitBuyAndSell) {
  TraderAccountConfig trader_account_config;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        start_security_balance: 10
        start_cash_balance: 0
        security_unit: 0.1
        cash_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1)",
      &trader_account_config));

  OhlcHistory ohlc_history;
  SetupDailyOhlcHistory(&ohlc_history);

  TestTrader trader(/* buy_price = */ 50, /* sell_price = */ 200);

  std::stringstream exchange_os;
  std::stringstream trader_os;

  TraderExecutionResult result =
      ExecuteTrader(trader_account_config, ohlc_history.begin(),
                    ohlc_history.end(), &trader, &exchange_os, &trader_os);

  TraderExecutionResult expected_result;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        start_security_balance: 10
        start_cash_balance: 0
        end_security_balance: 32.3
        end_cash_balance: 21
        start_price: 120
        end_price: 50
        start_value: 1200
        end_value: 1636
        total_executed_orders: 2
        total_fee: 364   
        )",
      &expected_result));
  ExpectProtoEq(expected_result, result);

  std::stringstream expected_exchange_os;
  std::stringstream expected_trader_os;

  expected_exchange_os  // nowrap
      << "1483228800,100.000,150.000,80.000,120.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1483315200,120.000,180.000,100.000,150.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1483401600,150.000,250.000,100.000,140.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1483401600,150.000,250.000,100.000,140.000,"
      << "1000.000,0.000,1799.000,201.000,LIMIT,SELL,200.000,10.000,"
      << std::endl
      << "1483488000,140.000,150.000,80.000,100.000,"
      << "1000.000,0.000,1799.000,201.000,,,,," << std::endl
      << "1483574400,100.000,120.000,20.000,50.000,"
      << "1000.000,0.000,1799.000,201.000,,,,," << std::endl
      << "1483574400,100.000,120.000,20.000,50.000,"
      << "1000.000,32.300,21.000,364.000,LIMIT,BUY,50.000,,1799.000"
      << std::endl;

  expected_trader_os  // nowrap
      << "1483228800,10.000,0.000,120.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1483315200,10.000,0.000,150.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1483401600,0.000,1799.000,140.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1483488000,0.000,1799.000,100.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1483574400,32.300,21.000,50.000,IN_LONG,LIMIT_SELL@200" << std::endl;

  EXPECT_EQ(expected_exchange_os.str(), exchange_os.str());
  EXPECT_EQ(expected_trader_os.str(), trader_os.str());
}

TEST(EvaluateTraderTest, LimitBuyAndSellOnePeriod) {
  TraderAccountConfig trader_account_config;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        start_security_balance: 10
        start_cash_balance: 0
        security_unit: 0.1
        cash_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1
        )",
      &trader_account_config));

  OhlcHistory ohlc_history;
  SetupMonthlyOhlcHistory(&ohlc_history);

  TraderEvaluationConfig trader_eval_config;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        start_timestamp_sec: 1483228800
        end_timestamp_sec: 1514764800
        evaluation_period_months: 0
        )",
      &trader_eval_config));

  TestTraderFactory trader_factory(/* buy_price = */ 50,
                                   /* sell_price = */ 200);
  EXPECT_EQ("test-trader[50|200]", trader_factory.GetTraderName());

  std::stringstream exchange_os;
  std::stringstream trader_os;

  TraderEvaluationResult result =
      EvaluateTrader(trader_account_config, trader_eval_config, ohlc_history,
                     trader_factory, &exchange_os, &trader_os);

  TraderEvaluationResult expected_result;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        trader_account_config {
            start_security_balance: 10
            start_cash_balance: 0
            security_unit: 0.1
            cash_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        trader_eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 0
        }
        trader_name: "test-trader[50|200]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            result {
                start_security_balance: 10
                start_cash_balance: 0
                end_security_balance: 0
                end_cash_balance: 5834
                start_price: 120
                end_price: 750
                start_value: 1200
                end_value: 5834
                total_executed_orders: 3
                total_fee: 1011
            }
            trader_final_gain: 4.86166668
            base_final_gain: 6.25
        }
        score: 0.777866662
        avg_trader_gain: 4.86166668
        avg_base_gain: 6.25
        avg_total_executed_orders: 3
        avg_total_fee: 1011
        )",
      &expected_result));
  ExpectProtoEq(expected_result, result);

  std::stringstream expected_exchange_os;
  std::stringstream expected_trader_os;

  expected_exchange_os  // nowrap
      << "1483228800,100.000,150.000,80.000,120.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1485907200,120.000,180.000,100.000,150.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1488326400,150.000,250.000,100.000,140.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1488326400,150.000,250.000,100.000,140.000,"
      << "1000.000,0.000,1799.000,201.000,LIMIT,SELL,200.000,10.000,"
      << std::endl
      << "1491004800,140.000,150.000,80.000,100.000,"
      << "1000.000,0.000,1799.000,201.000,,,,," << std::endl
      << "1493596800,100.000,120.000,20.000,50.000,"
      << "1000.000,0.000,1799.000,201.000,,,,," << std::endl
      << "1493596800,100.000,120.000,20.000,50.000,"
      << "1000.000,32.300,21.000,364.000,LIMIT,BUY,50.000,,1799.000"
      << std::endl
      << "1496275200,50.000,100.000,40.000,80.000,"
      << "1000.000,32.300,21.000,364.000,,,,," << std::endl
      << "1498867200,80.000,180.000,50.000,150.000,"
      << "1000.000,32.300,21.000,364.000,,,,," << std::endl
      << "1501545600,150.000,250.000,120.000,240.000,"
      << "1000.000,32.300,21.000,364.000,,,,," << std::endl
      << "1501545600,150.000,250.000,120.000,240.000,"
      << "1000.000,0.000,5834.000,1011.000,LIMIT,SELL,200.000,32.300,"
      << std::endl
      << "1504224000,240.000,450.000,220.000,400.000,"
      << "1000.000,0.000,5834.000,1011.000,,,,," << std::endl
      << "1506816000,400.000,450.000,250.000,300.000,"
      << "1000.000,0.000,5834.000,1011.000,,,,," << std::endl
      << "1509494400,300.000,700.000,220.000,650.000,"
      << "1000.000,0.000,5834.000,1011.000,,,,," << std::endl
      << "1512086400,650.000,800.000,600.000,750.000,"
      << "1000.000,0.000,5834.000,1011.000,,,,," << std::endl;

  expected_trader_os  // nowrap
      << "1483228800,10.000,0.000,120.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1485907200,10.000,0.000,150.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1488326400,0.000,1799.000,140.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1491004800,0.000,1799.000,100.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1493596800,32.300,21.000,50.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1496275200,32.300,21.000,80.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1498867200,32.300,21.000,150.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1501545600,0.000,5834.000,240.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1504224000,0.000,5834.000,400.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1506816000,0.000,5834.000,300.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1509494400,0.000,5834.000,650.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1512086400,0.000,5834.000,750.000,IN_CASH,LIMIT_BUY@50" << std::endl;

  EXPECT_EQ(expected_exchange_os.str(), exchange_os.str());
  EXPECT_EQ(expected_trader_os.str(), trader_os.str());
}

TEST(EvaluateTraderTest, LimitBuyAndSellMultiple6MonthPeriods) {
  TraderAccountConfig trader_account_config;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        start_security_balance: 10
        start_cash_balance: 0
        security_unit: 0.1
        cash_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1
        )",
      &trader_account_config));

  OhlcHistory ohlc_history;
  SetupMonthlyOhlcHistory(&ohlc_history);

  TraderEvaluationConfig trader_eval_config;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        start_timestamp_sec: 1483228800
        end_timestamp_sec: 1514764800
        evaluation_period_months: 6
        )",
      &trader_eval_config));

  TestTraderFactory trader_factory(/* buy_price = */ 50,
                                   /* sell_price = */ 200);
  EXPECT_EQ("test-trader[50|200]", trader_factory.GetTraderName());

  TraderEvaluationResult result =
      EvaluateTrader(trader_account_config, trader_eval_config, ohlc_history,
                     trader_factory, /* exchange_os = */ nullptr,
                     /* trader_os = */ nullptr);

  TraderEvaluationResult expected_result;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        trader_account_config {
            start_security_balance: 10
            start_cash_balance: 0
            security_unit: 0.1
            cash_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        trader_eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 6
        }
        trader_name: "test-trader[50|200]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1498867200
            result {
                start_security_balance: 10
                start_cash_balance: 0
                end_security_balance: 32.3
                end_cash_balance: 21
                start_price: 120
                end_price: 80
                start_value: 1200
                end_value: 2605
                total_executed_orders: 2
                total_fee: 364
            }
            trader_final_gain: 2.17083335
            base_final_gain: 0.666666687
        }
        period {
            start_timestamp_sec: 1485907200
            end_timestamp_sec: 1501545600
            result {
                start_security_balance: 10
                start_cash_balance: 0
                end_security_balance: 32.3
                end_cash_balance: 21
                start_price: 150
                end_price: 150
                start_value: 1500
                end_value: 4866
                total_executed_orders: 2
                total_fee: 364
            }
            trader_final_gain: 3.244
            base_final_gain: 1
        }
        period {
            start_timestamp_sec: 1488326400
            end_timestamp_sec: 1504224000
            result {
                start_security_balance: 10
                start_cash_balance: 0
                end_security_balance: 0
                end_cash_balance: 1799
                start_price: 140
                end_price: 240
                start_value: 1400
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
            }
            trader_final_gain: 1.285
            base_final_gain: 1.71428573
        }
        period {
            start_timestamp_sec: 1491004800
            end_timestamp_sec: 1506816000
            result {
                start_security_balance: 10
                start_cash_balance: 0
                end_security_balance: 0
                end_cash_balance: 1799
                start_price: 100
                end_price: 400
                start_value: 1000
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
            }
            trader_final_gain: 1.799
            base_final_gain: 4
        }
        period {
            start_timestamp_sec: 1493596800
            end_timestamp_sec: 1509494400
            result {
                start_security_balance: 10
                start_cash_balance: 0
                end_security_balance: 0
                end_cash_balance: 1799
                start_price: 50
                end_price: 300
                start_value: 500
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
            }
            trader_final_gain: 3.598
            base_final_gain: 6
        }
        period {
            start_timestamp_sec: 1496275200
            end_timestamp_sec: 1512086400
            result {
                start_security_balance: 10
                start_cash_balance: 0
                end_security_balance: 0
                end_cash_balance: 1799
                start_price: 80
                end_price: 650
                start_value: 800
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
            }
            trader_final_gain: 2.24875
            base_final_gain: 8.125
        }
        period {
            start_timestamp_sec: 1498867200
            end_timestamp_sec: 1514764800
            result {
                start_security_balance: 10
                start_cash_balance: 0
                end_security_balance: 0
                end_cash_balance: 1799
                start_price: 150
                end_price: 750
                start_value: 1500
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
            }
            trader_final_gain: 1.19933331
            base_final_gain: 5
        }
        score: 0.75648129
        avg_trader_gain: 2.22070217
        avg_base_gain: 3.78656459
        avg_total_executed_orders: 1.28571427
        avg_total_fee: 247.571426
        )",
      &expected_result));
  ExpectProtoEq(expected_result, result);
}

TEST(EvaluateBatchOfTradersTest, LimitBuyAndSellMultiple6MonthPeriods) {
  TraderAccountConfig trader_account_config;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        start_security_balance: 10
        start_cash_balance: 0
        security_unit: 0.1
        cash_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1
        )",
      &trader_account_config));

  OhlcHistory ohlc_history;
  SetupMonthlyOhlcHistory(&ohlc_history);

  TraderEvaluationConfig trader_eval_config;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        start_timestamp_sec: 1483228800
        end_timestamp_sec: 1514764800
        evaluation_period_months: 6
        )",
      &trader_eval_config));

  std::vector<std::unique_ptr<TraderFactoryInterface>> trader_factories;
  trader_factories.emplace_back(new TestTraderFactory(/* buy_price = */ 50,
                                                      /* sell_price = */ 200));
  trader_factories.emplace_back(new TestTraderFactory(/* buy_price = */ 40,
                                                      /* sell_price = */ 250));
  trader_factories.emplace_back(new TestTraderFactory(/* buy_price = */ 30,
                                                      /* sell_price = */ 500));

  std::vector<TraderEvaluationResult> trader_eval_results =
      EvaluateBatchOfTraders(trader_account_config, trader_eval_config,
                             ohlc_history, trader_factories);

  ASSERT_EQ(3, trader_eval_results.size());
  TraderEvaluationResult expected_result[3];

  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        trader_account_config {
            start_security_balance: 10
            start_cash_balance: 0
            security_unit: 0.1
            cash_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        trader_eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 6
        }
        trader_name: "test-trader[50|200]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1498867200
            trader_final_gain: 2.17083335
            base_final_gain: 0.666666687
        }
        period {
            start_timestamp_sec: 1485907200
            end_timestamp_sec: 1501545600
            trader_final_gain: 3.244
            base_final_gain: 1
        }
        period {
            start_timestamp_sec: 1488326400
            end_timestamp_sec: 1504224000
            trader_final_gain: 1.285
            base_final_gain: 1.71428573
        }
        period {
            start_timestamp_sec: 1491004800
            end_timestamp_sec: 1506816000
            trader_final_gain: 1.799
            base_final_gain: 4
        }
        period {
            start_timestamp_sec: 1493596800
            end_timestamp_sec: 1509494400
            trader_final_gain: 3.598
            base_final_gain: 6
        }
        period {
            start_timestamp_sec: 1496275200
            end_timestamp_sec: 1512086400
            trader_final_gain: 2.24875
            base_final_gain: 8.125
        }
        period {
            start_timestamp_sec: 1498867200
            end_timestamp_sec: 1514764800
            trader_final_gain: 1.19933331
            base_final_gain: 5
        }
        score: 0.75648129
        avg_trader_gain: 2.22070217
        avg_base_gain: 3.78656459
        avg_total_executed_orders: 1.28571427
        avg_total_fee: 247.571426
        )",
      &expected_result[0]));
  ExpectProtoEq(expected_result[0], trader_eval_results[0],
                /* full_scope = */ false);

  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        trader_account_config {
            start_security_balance: 10
            start_cash_balance: 0
            security_unit: 0.1
            cash_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        trader_eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 6
        }
        trader_name: "test-trader[40|250]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1498867200
            trader_final_gain: 3.38833332
            base_final_gain: 0.666666687
        }
        period {
            start_timestamp_sec: 1485907200
            end_timestamp_sec: 1501545600
            trader_final_gain: 5.06733322
            base_final_gain: 1
        }
        period {
            start_timestamp_sec: 1488326400
            end_timestamp_sec: 1504224000
            trader_final_gain: 1.60642862
            base_final_gain: 1.71428573
        }
        period {
            start_timestamp_sec: 1491004800
            end_timestamp_sec: 1506816000
            trader_final_gain: 2.249
            base_final_gain: 4
        }
        period {
            start_timestamp_sec: 1493596800
            end_timestamp_sec: 1509494400
            trader_final_gain: 4.498
            base_final_gain: 6
        }
        period {
            start_timestamp_sec: 1496275200
            end_timestamp_sec: 1512086400
            trader_final_gain: 2.81125
            base_final_gain: 8.125
        }
        period {
            start_timestamp_sec: 1498867200
            end_timestamp_sec: 1514764800
            trader_final_gain: 1.49933338
            base_final_gain: 5
        }
        score: 1.00773919
        avg_trader_gain: 3.01709747
        avg_base_gain: 3.78656459
        avg_total_executed_orders: 1.28571427
        avg_total_fee: 309
        )",
      &expected_result[1]));
  ExpectProtoEq(expected_result[1], trader_eval_results[1],
                /* full_scope = */ false);

  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"(
        trader_account_config {
            start_security_balance: 10
            start_cash_balance: 0
            security_unit: 0.1
            cash_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        trader_eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 6
        }
        trader_name: "test-trader[30|500]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1498867200
            trader_final_gain: 0.666666687
            base_final_gain: 0.666666687
        }
        period {
            start_timestamp_sec: 1485907200
            end_timestamp_sec: 1501545600
            trader_final_gain: 1
            base_final_gain: 1
        }
        period {
            start_timestamp_sec: 1488326400
            end_timestamp_sec: 1504224000
            trader_final_gain: 1.71428573
            base_final_gain: 1.71428573
        }
        period {
            start_timestamp_sec: 1491004800
            end_timestamp_sec: 1506816000
            trader_final_gain: 4
            base_final_gain: 4
        }
        period {
            start_timestamp_sec: 1493596800
            end_timestamp_sec: 1509494400
            trader_final_gain: 6
            base_final_gain: 6
        }
        period {
            start_timestamp_sec: 1496275200
            end_timestamp_sec: 1512086400
            trader_final_gain: 5.62375
            base_final_gain: 8.125
        }
        period {
            start_timestamp_sec: 1498867200
            end_timestamp_sec: 1514764800
            trader_final_gain: 2.99933338
            base_final_gain: 5
        }
        score: 0.881993413
        avg_trader_gain: 3.14343381
        avg_base_gain: 3.78656459
        avg_total_executed_orders: 0.285714298
        avg_total_fee: 143.142853
        )",
      &expected_result[2]));
  ExpectProtoEq(expected_result[2], trader_eval_results[2],
                /* full_scope = */ false);
}

}  // namespace trader
