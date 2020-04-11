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

namespace {
// Adds daily OHLC tick to the history.
void AddDailyOhlcTick(float open, float high, float low, float close,
                      OhlcHistory* ohlc_history) {
  int timestamp_sec = 1483228800;  // 2017-01-01
  if (!ohlc_history->empty()) {
    timestamp_sec = ohlc_history->back().timestamp_sec() + 24 * 60 * 60;
  }
  ohlc_history->emplace_back();
  OhlcTick* ohlc_tick = &ohlc_history->back();
  ohlc_tick->set_timestamp_sec(timestamp_sec);
  ohlc_tick->set_open(open);
  ohlc_tick->set_high(high);
  ohlc_tick->set_low(low);
  ohlc_tick->set_close(close);
  ohlc_tick->set_volume(1000.0f);
}

// Trader that sells the security (crypto currency) at 200 and buys at 50.
class TestTrader : public TraderInterface {
 public:
  TestTrader() {}
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
      order->set_type(Order::Type::Order_Type_LIMIT);
      order->set_side(Order::Side::Order_Side_SELL);
      order->set_security_amount(last_security_balance_);
      order->set_price(200.0f);
    } else {
      order->set_type(Order::Type::Order_Type_LIMIT);
      order->set_side(Order::Side::Order_Side_BUY);
      order->set_cash_amount(last_cash_balance_);
      order->set_price(50.0f);
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
      *os << "IN_LONG,LIMIT_SELL@200";
    } else {
      *os << "IN_CASH,LIMIT_BUY@50";
    }
    *os << std::endl;
  }

 private:
  float last_security_balance_ = 0.0f;
  float last_cash_balance_ = 0.0f;
  int last_timestamp_sec_ = 0;
  float last_close_ = 0.0f;
  bool is_long_ = false;
};
}  // namespace

TEST(ExecuteTraderTest, LimitBuyAndSell) {
  TraderAccountConfig trader_account_config;
  trader_account_config.set_start_security_balance(10.0f);
  trader_account_config.set_start_cash_balance(0.0f);
  trader_account_config.set_security_unit(0.1f);
  trader_account_config.set_cash_unit(1.0f);
  trader_account_config.set_market_liquidity(0.5f);
  trader_account_config.set_max_volume_ratio(0.1f);
  FeeConfig* fee_config =
      trader_account_config.mutable_limit_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  OhlcHistory ohlc_history;
  AddDailyOhlcTick(100, 150, 80, 120, &ohlc_history);
  AddDailyOhlcTick(120, 180, 100, 150, &ohlc_history);
  AddDailyOhlcTick(150, 250, 100, 140, &ohlc_history);
  AddDailyOhlcTick(140, 150, 80, 100, &ohlc_history);
  AddDailyOhlcTick(100, 120, 20, 50, &ohlc_history);

  TestTrader trader;
  std::stringstream exchange_os;
  std::stringstream trader_os;

  TraderExecutionResult result =
      ExecuteTrader(trader_account_config, ohlc_history.begin(),
                    ohlc_history.end(), &trader, &exchange_os, &trader_os);

  EXPECT_FLOAT_EQ(10.0f, result.start_security_balance());
  EXPECT_FLOAT_EQ(0.0f, result.start_cash_balance());
  EXPECT_FLOAT_EQ(32.3f, result.end_security_balance());
  EXPECT_FLOAT_EQ(21.0f, result.end_cash_balance());
  EXPECT_FLOAT_EQ(120.0f, result.start_price());
  EXPECT_FLOAT_EQ(50.0f, result.end_price());
  EXPECT_FLOAT_EQ(1200.0f, result.start_value());
  EXPECT_FLOAT_EQ(1636.0f, result.end_value());
  EXPECT_EQ(2, result.total_executed_orders());
  EXPECT_FLOAT_EQ(364.0f, result.total_fee());

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

}  // namespace trader
