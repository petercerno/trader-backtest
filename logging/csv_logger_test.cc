// Copyright © 2020 Peter Cerno. All rights reserved.

#include "logging/csv_logger.h"

#include "gtest/gtest.h"

namespace trader {
namespace {
void AddOhlcTick(float open, float high, float low, float close, float volume,
                 int timestamp_sec, OhlcHistory& ohlc_history) {
  ASSERT_LE(low, open);
  ASSERT_LE(low, high);
  ASSERT_LE(low, close);
  ASSERT_GE(high, open);
  ASSERT_GE(high, close);
  ohlc_history.emplace_back();
  OhlcTick& ohlc_tick = ohlc_history.back();
  ohlc_tick.set_open(open);
  ohlc_tick.set_high(high);
  ohlc_tick.set_low(low);
  ohlc_tick.set_close(close);
  ohlc_tick.set_volume(volume);
  ohlc_tick.set_timestamp_sec(timestamp_sec);
}

void PrepareExampleOhlcHistory(OhlcHistory& ohlc_history) {
  AddOhlcTick(100, 150, 80, 120, 1000, 1483228800, ohlc_history);  // 2017-01-01
  AddOhlcTick(120, 180, 100, 150, 500, 1483315200, ohlc_history);  // 2017-01-02
  AddOhlcTick(150, 250, 100, 140, 800, 1483401600, ohlc_history);  // 2017-01-03
}

void PrepareExampleAccount(Account& account) {
  account.base_balance = 2.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 50.0f;
  account.base_unit = 0.0001f;
  account.quote_unit = 0.01f;
  account.market_liquidity = 0.5f;
  account.max_volume_ratio = 0.9f;
}
}  // namespace

TEST(CsvLoggerTest, LogExchangeState) {
  OhlcHistory ohlc_history;
  PrepareExampleOhlcHistory(ohlc_history);

  Account account;
  PrepareExampleAccount(account);

  std::stringstream exchange_os;
  CsvLogger logger(&exchange_os, /* trader_os = */ nullptr);
  logger.LogExchangeState(ohlc_history[0], account);
  logger.LogExchangeState(ohlc_history[1], account);
  logger.LogExchangeState(ohlc_history[2], account);

  std::stringstream expected_exchange_os;
  expected_exchange_os  // nowrap
      << "1483228800,100.000,150.000,80.000,120.000,"
      << "1000.000,2.000,1000.000,50.000,,,,," << std::endl
      << "1483315200,120.000,180.000,100.000,150.000,"
      << "500.000,2.000,1000.000,50.000,,,,," << std::endl
      << "1483401600,150.000,250.000,100.000,140.000,"
      << "800.000,2.000,1000.000,50.000,,,,," << std::endl;

  EXPECT_EQ(expected_exchange_os.str(), exchange_os.str());
}

TEST(CsvLoggerTest, LogExchangeStateWithOrder) {
  OhlcHistory ohlc_history;
  PrepareExampleOhlcHistory(ohlc_history);

  Account account;
  PrepareExampleAccount(account);

  Order order;
  order.set_type(Order::Type::Order_Type_LIMIT);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_quote_amount(1.0f);
  order.set_price(500.0f);

  std::stringstream exchange_os;
  CsvLogger logger(&exchange_os, /* trader_os = */ nullptr);
  logger.LogExchangeState(ohlc_history[0], account);
  logger.LogExchangeState(ohlc_history[1], account, order);
  logger.LogExchangeState(ohlc_history[2], account);

  std::stringstream expected_exchange_os;
  expected_exchange_os  // nowrap
      << "1483228800,100.000,150.000,80.000,120.000,"
      << "1000.000,2.000,1000.000,50.000,,,,," << std::endl
      << "1483315200,120.000,180.000,100.000,150.000,"
      << "500.000,2.000,1000.000,50.000,LIMIT,SELL,,1.000,500.000" << std::endl
      << "1483401600,150.000,250.000,100.000,140.000,"
      << "800.000,2.000,1000.000,50.000,,,,," << std::endl;

  EXPECT_EQ(expected_exchange_os.str(), exchange_os.str());
}

TEST(CsvLoggerTest, LogTraderState) {
  std::stringstream trader_os;
  CsvLogger logger(/* exchange_os = */ nullptr, &trader_os);
  logger.LogTraderState("state_1");
  logger.LogTraderState("state_2");
  logger.LogTraderState("state_3");

  std::stringstream expected_trader_os;
  expected_trader_os  // nowrap
      << "state_1" << std::endl
      << "state_2" << std::endl
      << "state_3" << std::endl;

  EXPECT_EQ(expected_trader_os.str(), trader_os.str());
}

}  // namespace trader
