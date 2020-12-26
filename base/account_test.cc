// Copyright © 2020 Peter Cerno. All rights reserved.

#include "base/account.h"

#include "gtest/gtest.h"

namespace trader {
namespace {
void SetupOhlcTick(OhlcTick& ohlc_tick) {
  ohlc_tick.set_open(10.0f);
  ohlc_tick.set_high(20.0f);
  ohlc_tick.set_low(2.0f);
  ohlc_tick.set_close(15.0f);
  ohlc_tick.set_volume(1234.56f);
}
}  // namespace

TEST(GetFeeTest, RelativeFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.0f, account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.0f, account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.23456789f, account.GetFee(fee_config, 12.3456789f));

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.24f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.35f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.46f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.3f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.4f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.5f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(1.0f, account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(0.1f, account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(0.123456789f, account.GetFee(fee_config, 12.3456789f));

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(0.13f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.24f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.35f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(0.2f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.3f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.4f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(1.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.0f, account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, FixedFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_fixed_fee(0.12345f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(0.12345f, account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(0.12345f, account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(0.12345f, account.GetFee(fee_config, 12.3456789f));

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(0.13f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(0.13f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(0.13f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(0.2f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(0.2f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(0.2f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(1.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.0f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(1.0f, account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, RelativeAndFixedFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(0.1f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.1f, account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.1f, account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.33456789f, account.GetFee(fee_config, 12.3456789f));

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.34f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.45f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.56f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.4f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.5f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.6f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);
  fee_config.set_fixed_fee(1.0f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.1f, account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.123456789f, account.GetFee(fee_config, 12.3456789f));

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.13f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.24f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.35f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.2f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.3f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.4f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(3.0f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(14.0f, account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, RelativeAndMinimumFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_minimum_fee(2.0f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.0f, account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.35f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.46f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.4f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.5f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);
  fee_config.set_minimum_fee(0.5f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(1.0f, account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(0.5f, account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(0.5f, account.GetFee(fee_config, 12.3456789f));

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(0.5f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.24f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.35f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(0.5f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.3f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.4f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(1.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.0f, account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, RelativeAndFixedAndMinimumFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(0.1f);
  fee_config.set_minimum_fee(2.0f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.1f, account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.45f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.56f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.5f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.6f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.5f, account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.5f, account.GetFee(fee_config, 12.3456789f));

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.5f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.24f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.35f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.5f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.3f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.4f, account.GetFee(fee_config, 1234.56789f));

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(3.0f, account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(14.0f, account.GetFee(fee_config, 1234.56789f));
}

TEST(GetPriceTest, MarketBuy) {
  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(10.0f, account.GetMarketBuyPrice(ohlc_tick));

  account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(20.0f, account.GetMarketBuyPrice(ohlc_tick));

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(15.0f, account.GetMarketBuyPrice(ohlc_tick));
}

TEST(GetPriceTest, MarketSell) {
  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(10.0f, account.GetMarketSellPrice(ohlc_tick));

  account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(2.0f, account.GetMarketSellPrice(ohlc_tick));

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(6.0f, account.GetMarketSellPrice(ohlc_tick));
}

TEST(GetPriceTest, StopBuy) {
  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(  // Target stop price below opening price
      10.0f, account.GetStopBuyPrice(ohlc_tick, /* price = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f,
                  account.GetStopBuyPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f,
                  account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));

  account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(  // Target stop price below opening price
      20.0f, account.GetStopBuyPrice(ohlc_tick, /* price = */ 5.0f));
  EXPECT_FLOAT_EQ(20.0f,
                  account.GetStopBuyPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f,
                  account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(  // Target stop price below opening price
      15.0f, account.GetStopBuyPrice(ohlc_tick, /* price = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f,
                  account.GetStopBuyPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(17.5f,
                  account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));
}

TEST(GetPriceTest, StopSell) {
  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(  // Target stop price above opening price
      10.0f, account.GetStopSellPrice(ohlc_tick, /* price = */ 15.0f));
  EXPECT_FLOAT_EQ(10.0f,
                  account.GetStopSellPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(5.0f,
                  account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));

  account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(  // Target stop price above opening price
      2.0f, account.GetStopSellPrice(ohlc_tick, /* price = */ 15.0f));
  EXPECT_FLOAT_EQ(2.0f,
                  account.GetStopSellPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(2.0f,
                  account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(  // Target stop price above opening price
      6.0f, account.GetStopSellPrice(ohlc_tick, /* price = */ 15.0f));
  EXPECT_FLOAT_EQ(6.0f,
                  account.GetStopSellPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(3.5f,
                  account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));
}

TEST(GetMaxBaseAmountTest, Basic) {
  static constexpr float FLOAT_MAX = std::numeric_limits<float>::max();

  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0;
  account.base_unit = 0;
  EXPECT_FLOAT_EQ(FLOAT_MAX, account.GetMaxBaseAmount(ohlc_tick));

  account.max_volume_ratio = 0.1f;
  EXPECT_FLOAT_EQ(123.456f, account.GetMaxBaseAmount(ohlc_tick));

  account.max_volume_ratio = 0.01f;
  EXPECT_FLOAT_EQ(12.3456f, account.GetMaxBaseAmount(ohlc_tick));

  account.max_volume_ratio = 0;
  account.base_unit = 0.1f;
  EXPECT_FLOAT_EQ(FLOAT_MAX, account.GetMaxBaseAmount(ohlc_tick));

  account.max_volume_ratio = 0.1f;
  EXPECT_FLOAT_EQ(123.4f, account.GetMaxBaseAmount(ohlc_tick));

  account.max_volume_ratio = 0.01f;
  EXPECT_FLOAT_EQ(12.3f, account.GetMaxBaseAmount(ohlc_tick));
}

TEST(BuyTest, BuyWithoutFeeAndInfinitePrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /* base_amount = */ 5.0f,
                              /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, account.base_balance);
  EXPECT_FLOAT_EQ(950.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /* base_amount = */ 10.0f,
                              /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 12.345f;
  account.quote_balance = 123.456f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /* base_amount = */ 12.345f,
                              /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.69f, account.base_balance);
  EXPECT_FLOAT_EQ(55.5585f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyBase(fee_config,
                               /* base_amount = */ 15.0f,
                               /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);
}

TEST(BuyTest, BuyWithoutFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /* base_amount = */ 5.0f,
                              /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, account.base_balance);
  EXPECT_FLOAT_EQ(950.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /* base_amount = */ 10.0f,
                              /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /* base_amount = */ 12.345f,
                              /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.6f, account.base_balance);
  EXPECT_FLOAT_EQ(55.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyBase(fee_config,
                               /* base_amount = */ 15.0f,
                               /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);
}

TEST(BuyTest, BuyWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /* base_amount = */ 5.0f,
                              /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 111.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /* base_amount = */ 10.0f,
                              /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(21.0f, account.total_fee);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /* base_amount = */ 12.345f,
                              /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.6f, account.base_balance);
  EXPECT_FLOAT_EQ(47.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(8.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.BuyBase(fee_config,
                               /* base_amount = */ 15.0f,
                               /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, account.total_fee);
}

TEST(BuyTest, BuyAtQuoteWithoutFeeAndInfinitePrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 50.0f,
                                 /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, account.base_balance);
  EXPECT_FLOAT_EQ(950.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 50.0f,
                                 /* price = */ 10.0f,
                                 /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(12.0f, account.base_balance);
  EXPECT_FLOAT_EQ(980.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 100.0f,
                                 /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 100.0f,
                                 /* price = */ 10.0f,
                                 /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, account.base_balance);
  EXPECT_FLOAT_EQ(50.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 12.345f;
  account.quote_balance = 123.456f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 67.8975f,
                                 /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.69f, account.base_balance);
  EXPECT_FLOAT_EQ(55.5585f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 12.345f;
  account.quote_balance = 123.456f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 67.8975f,
                                 /* price = */ 5.5f,
                                 /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(17.345f, account.base_balance);
  EXPECT_FLOAT_EQ(95.956f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /* quote_amount = */ 1000.0f,
                                  /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /* quote_amount = */ 1000.0f,
                                  /* price = */ 10.0f,
                                  /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);
}

TEST(BuyTest, BuyAtQuoteWithoutFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 50.0f,
                                 /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, account.base_balance);
  EXPECT_FLOAT_EQ(950.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 50.0f,
                                 /* price = */ 10.0f,
                                 /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(12.0f, account.base_balance);
  EXPECT_FLOAT_EQ(980.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 100.0f,
                                 /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 100.0f,
                                 /* price = */ 10.0f,
                                 /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, account.base_balance);
  EXPECT_FLOAT_EQ(50.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 67.8975f,
                                 /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.6f, account.base_balance);
  EXPECT_FLOAT_EQ(55.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 67.8975f,
                                 /* price = */ 5.5f,
                                 /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(17.3f, account.base_balance);
  EXPECT_FLOAT_EQ(95.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /* quote_amount = */ 1000.0f,
                                  /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /* quote_amount = */ 1000.0f,
                                  /* price = */ 10.0f,
                                  /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);
}

TEST(BuyTest, BuyAtQuoteWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 57.0f,
                                 /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 57.0f,
                                 /* price = */ 10.0f,
                                 /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(12.0f, account.base_balance);
  EXPECT_FLOAT_EQ(977.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(3.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 111.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 111.0f,
                                 /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(19.8f, account.base_balance);
  EXPECT_FLOAT_EQ(2.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(21.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 111.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 111.0f,
                                 /* price = */ 10.0f,
                                 /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, account.base_balance);
  EXPECT_FLOAT_EQ(55.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, account.total_fee);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 76.123f,
                                 /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.4f, account.base_balance);
  EXPECT_FLOAT_EQ(48.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(8.0f, account.total_fee);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /* quote_amount = */ 76.123f,
                                 /* price = */ 5.5f,
                                 /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(17.3f, account.base_balance);
  EXPECT_FLOAT_EQ(91.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(4.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /* quote_amount = */ 1000.0f,
                                  /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /* quote_amount = */ 1000.0f,
                                  /* price = */ 10.0f,
                                  /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, account.total_fee);
}

TEST(SellTest, SellWithoutFeeAndInfinitePrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /* base_amount = */ 5.0f,
                               /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /* base_amount = */ 10.0f,
                               /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 24.69f;
  account.quote_balance = 55.5585f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /* base_amount = */ 12.345f,
                               /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.345f, account.base_balance);
  EXPECT_FLOAT_EQ(123.456f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellBase(fee_config,
                                /* base_amount = */ 15.0f,
                                /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);
}

TEST(SellTest, SellWithoutFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /* base_amount = */ 5.0f,
                               /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /* base_amount = */ 10.0f,
                               /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 24.6f;
  account.quote_balance = 55.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /* base_amount = */ 12.345f,
                               /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.3f, account.base_balance);
  EXPECT_FLOAT_EQ(122.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellBase(fee_config,
                                /* base_amount = */ 15.0f,
                                /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);
}

TEST(SellTest, SellWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /* base_amount = */ 5.0f,
                               /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(994.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /* base_amount = */ 10.0f,
                               /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(89.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(21.0f, account.total_fee);

  account.base_balance = 24.6f;
  account.quote_balance = 47.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /* base_amount = */ 12.345f,
                               /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.3f, account.base_balance);
  EXPECT_FLOAT_EQ(106.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(8.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.SellBase(fee_config,
                                /* base_amount = */ 15.0f,
                                /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, account.total_fee);
}

TEST(SellTest, SellAtQuoteWithoutFeeAndInfinitePrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 50.0f,
                                  /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 50.0f,
                                  /* price = */ 10.0f,
                                  /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(13.0f, account.base_balance);
  EXPECT_FLOAT_EQ(970.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 100.0f,
                                  /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 15.0f;
  account.quote_balance = 50.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 100.0f,
                                  /* price = */ 10.0f,
                                  /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 24.69f;
  account.quote_balance = 55.5585f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 67.8975f,
                                  /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.345f, account.base_balance);
  EXPECT_FLOAT_EQ(123.456f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 17.345f;
  account.quote_balance = 95.956f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 67.8975f,
                                  /* price = */ 5.5f,
                                  /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(12.345f, account.base_balance);
  EXPECT_FLOAT_EQ(123.456f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /* quote_amount = */ 1000.0f,
                                   /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /* quote_amount = */ 1000.0f,
                                   /* price = */ 10.0f,
                                   /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);
}

TEST(SellTest, SellAtQuoteWithoutFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 50.0f,
                                  /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 12.0f;
  account.quote_balance = 980.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 50.0f,
                                  /* price = */ 10.0f,
                                  /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 100.0f,
                                  /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 15.0f;
  account.quote_balance = 50.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 100.0f,
                                  /* price = */ 10.0f,
                                  /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 24.6f;
  account.quote_balance = 55.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 67.8975f,
                                  /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.3f, account.base_balance);
  EXPECT_FLOAT_EQ(122.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 17.3f;
  account.quote_balance = 95.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 67.8975f,
                                  /* price = */ 5.5f,
                                  /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(12.3f, account.base_balance);
  EXPECT_FLOAT_EQ(122.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /* quote_amount = */ 1000.0f,
                                   /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /* quote_amount = */ 1000.0f,
                                   /* price = */ 10.0f,
                                   /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, account.total_fee);
}

TEST(SellTest, SellAtQuoteWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 50.0f,
                                  /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(9.4f, account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, account.total_fee);

  account.base_balance = 12.0f;
  account.quote_balance = 980.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 50.0f,
                                  /* price = */ 10.0f,
                                  /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(997.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(3.0f, account.total_fee);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 100.0f,
                                  /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(8.9f, account.base_balance);
  EXPECT_FLOAT_EQ(98.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(23.0f, account.total_fee);

  account.base_balance = 15.0f;
  account.quote_balance = 50.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 100.0f,
                                  /* price = */ 10.0f,
                                  /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(94.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, account.total_fee);

  account.base_balance = 24.6f;
  account.quote_balance = 55.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 67.8975f,
                                  /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(10.8f, account.base_balance);
  EXPECT_FLOAT_EQ(121.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(9.0f, account.total_fee);

  account.base_balance = 17.3f;
  account.quote_balance = 95.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /* quote_amount = */ 67.8975f,
                                  /* price = */ 5.5f,
                                  /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(12.3f, account.base_balance);
  EXPECT_FLOAT_EQ(118.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(4.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /* quote_amount = */ 1000.0f,
                                   /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, account.total_fee);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /* quote_amount = */ 1000.0f,
                                   /* price = */ 10.0f,
                                   /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, account.total_fee);
}

TEST(BuyTest, MarketBuyWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(15.0f, account.GetMarketBuyPrice(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  EXPECT_TRUE(account.MarketBuy(fee_config, ohlc_tick,
                                /* base_amount = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(834.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, account.total_fee);
}

TEST(BuyTest, MarketBuyAtQuoteWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(15.0f, account.GetMarketBuyPrice(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 169.0 units in quote currency.
  EXPECT_TRUE(account.MarketBuyAtQuote(fee_config, ohlc_tick,
                                       /* quote_amount = */ 169.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(834.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, account.total_fee);
}

TEST(SellTest, MarketSellWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(10.0f, account.GetMarketSellPrice(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  EXPECT_TRUE(account.MarketSell(fee_config, ohlc_tick,
                                 /* base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(994.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);
}

TEST(SellTest, MarketSellAtQuoteWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(10.0f, account.GetMarketSellPrice(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  EXPECT_TRUE(account.MarketSellAtQuote(fee_config, ohlc_tick,
                                        /* quote_amount = */ 50.0f));
  EXPECT_FLOAT_EQ(9.4f, account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, account.total_fee);
}

TEST(BuyTest, StopBuyWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(17.5f,
                  account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.StopBuy(fee_config, ohlc_tick,
                               /* base_amount = */ 10.0f,
                               /* stop_price = */ 25.0f));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.StopBuy(fee_config, ohlc_tick,
                              /* base_amount = */ 10.0f,
                              /* stop_price = */ 15.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(806.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(19.0f, account.total_fee);
}

TEST(BuyTest, StopBuyAtQuoteWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(17.5f,
                  account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 197.0 units in quote currency.
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.StopBuyAtQuote(fee_config, ohlc_tick,
                                      /* quote_amount = */ 197.0f,
                                      /* stop_price = */ 25.0f));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.StopBuyAtQuote(fee_config, ohlc_tick,
                                     /* quote_amount = */ 197.0f,
                                     /* stop_price = */ 15.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(806.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(19.0f, account.total_fee);
}

TEST(SellTest, StopSellWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(5.0f,
                  account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.StopSell(fee_config, ohlc_tick,
                                /* base_amount = */ 5.0f,
                                /* stop_price = */ 1.0f));
  // Stop price 5.0 is below the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.StopSell(fee_config, ohlc_tick,
                               /* base_amount = */ 5.0f,
                               /* stop_price = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(971.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(4.0f, account.total_fee);
}

TEST(SellTest, StopSellAtQuoteWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(5.0f,
                  account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.StopSellAtQuote(fee_config, ohlc_tick,
                                       /* quote_amount = */ 50.0f,
                                       /* stop_price = */ 1.0f));
  // Stop price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.StopSellAtQuote(fee_config, ohlc_tick,
                                      /* quote_amount = */ 50.0f,
                                      /* stop_price = */ 5.0f));
  EXPECT_FLOAT_EQ(3.8f, account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, account.total_fee);
}

TEST(BuyTest, LimitBuyWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0.1;
  EXPECT_FLOAT_EQ(123.456f, account.GetMaxBaseAmount(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitBuy(fee_config, ohlc_tick,
                                /* base_amount = */ 10.0f,
                                /* limit_price = */ 1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.LimitBuy(fee_config, ohlc_tick,
                               /* base_amount = */ 10.0f,
                               /* limit_price = */ 5.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);
}

TEST(BuyTest, LimitBuyWithFeeAndLimitedPrecisionExceedsMaxAmount) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0.001;
  EXPECT_FLOAT_EQ(1.23456f, account.GetMaxBaseAmount(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // However, we can buy at most 1.2 units of base (crypto) currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitBuy(fee_config, ohlc_tick,
                                /* base_amount = */ 10.0f,
                                /* limit_price = */ 1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.LimitBuy(fee_config, ohlc_tick,
                               /* base_amount = */ 10.0f,
                               /* limit_price = */ 5.0f));
  EXPECT_FLOAT_EQ(11.2f, account.base_balance);
  EXPECT_FLOAT_EQ(992.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(2.0f, account.total_fee);
}

TEST(BuyTest, LimitBuyAtQuoteWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0.1;
  EXPECT_FLOAT_EQ(123.456f, account.GetMaxBaseAmount(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 57.0 units in quote currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                       /* quote_amount = */ 57.0f,
                                       /* limit_price = */ 1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                      /* quote_amount = */ 57.0f,
                                      /* limit_price = */ 5.0f));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);
}

TEST(BuyTest, LimitBuyAtQuoteWithFeeAndLimitedPrecisionExceedsMaxAmount) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0.001;
  EXPECT_FLOAT_EQ(1.23456f, account.GetMaxBaseAmount(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 57.0 units in quote currency.
  // However, we can buy at most 1.2 units of base (crypto) currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                       /* quote_amount = */ 57.0f,
                                       /* limit_price = */ 1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                      /* quote_amount = */ 57.0f,
                                      /* limit_price = */ 5.0f));
  EXPECT_FLOAT_EQ(11.2f, account.base_balance);
  EXPECT_FLOAT_EQ(992.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(2.0f, account.total_fee);
}

TEST(SellTest, LimitSellWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0.1;
  EXPECT_FLOAT_EQ(123.456f, account.GetMaxBaseAmount(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitSell(fee_config, ohlc_tick,
                                 /* base_amount = */ 5.0f,
                                 /* limit_price = */ 25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.LimitSell(fee_config, ohlc_tick,
                                /* base_amount = */ 5.0f,
                                /* limit_price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(994.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);
}

TEST(SellTest, LimitSellWithFeeAndLimitedPrecisionExceedsMaxAmount) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0.001;
  EXPECT_FLOAT_EQ(1.23456f, account.GetMaxBaseAmount(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // However, we can sell at most 1.2 units of base (crypto) currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitSell(fee_config, ohlc_tick,
                                 /* base_amount = */ 5.0f,
                                 /* limit_price = */ 25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.LimitSell(fee_config, ohlc_tick,
                                /* base_amount = */ 5.0f,
                                /* limit_price = */ 10.0f));
  EXPECT_FLOAT_EQ(13.8f, account.base_balance);
  EXPECT_FLOAT_EQ(959.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(3.0f, account.total_fee);
}

TEST(SellTest, LimitSellAtQuoteWithFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0.1;
  EXPECT_FLOAT_EQ(123.456f, account.GetMaxBaseAmount(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitSellAtQuote(fee_config, ohlc_tick,
                                        /* quote_amount = */ 50.0f,
                                        /* limit_price = */ 25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.LimitSellAtQuote(fee_config, ohlc_tick,
                                       /* quote_amount = */ 50.0f,
                                       /* limit_price = */ 10.0f));
  EXPECT_FLOAT_EQ(9.4f, account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, account.total_fee);
}

TEST(SellTest, LimitSellAtQuoteWithFeeAndLimitedPrecisionExceedsMaxAmount) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0.001;
  EXPECT_FLOAT_EQ(1.23456f, account.GetMaxBaseAmount(ohlc_tick));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // However, we can sell at most 1.2 units of base (crypto) currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitSellAtQuote(fee_config, ohlc_tick,
                                        /* quote_amount = */ 50.0f,
                                        /* limit_price = */ 25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.LimitSellAtQuote(fee_config, ohlc_tick,
                                       /* quote_amount = */ 50.0f,
                                       /* limit_price = */ 10.0f));
  EXPECT_FLOAT_EQ(13.8f, account.base_balance);
  EXPECT_FLOAT_EQ(959.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(3.0f, account.total_fee);
}

TEST(ExecuteOrderTest, MarketBuyWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_market_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_MARKET);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_base_amount(10.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.market_liquidity = 0.5f;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(834.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, account.total_fee);
}

TEST(ExecuteOrderTest, MarketBuyAtQuoteWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_market_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_MARKET);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_quote_amount(169.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.market_liquidity = 0.5f;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 169.0 units in quote currency.
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(834.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, account.total_fee);
}

TEST(ExecuteOrderTest, MarketSellWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_market_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_MARKET);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_base_amount(5.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.market_liquidity = 1.0f;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(994.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);
}

TEST(ExecuteOrderTest, MarketSellAtQuoteWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_market_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_MARKET);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_quote_amount(50.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.market_liquidity = 1.0f;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(9.4f, account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, account.total_fee);
}

TEST(ExecuteOrderTest, StopBuyWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_stop_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_STOP);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_base_amount(10.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.market_liquidity = 0.5f;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  order.set_price(25.0f);
  ASSERT_FALSE(account.ExecuteOrder(account_config, order, ohlc_tick));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  order.set_price(15.0f);
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(806.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(19.0f, account.total_fee);
}

TEST(ExecuteOrderTest, StopBuyAtQuoteWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_stop_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_STOP);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_quote_amount(197.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.market_liquidity = 0.5f;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 197.0 units in quote currency.
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  order.set_price(25.0f);
  ASSERT_FALSE(account.ExecuteOrder(account_config, order, ohlc_tick));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  order.set_price(15.0f);
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(806.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(19.0f, account.total_fee);
}

TEST(ExecuteOrderTest, StopSellWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_stop_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_STOP);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_base_amount(5.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.market_liquidity = 1.0f;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  order.set_price(1.0f);
  ASSERT_FALSE(account.ExecuteOrder(account_config, order, ohlc_tick));
  // Stop price 5.0 is below the OHLC tick low price 2.0. Order is executed.
  order.set_price(5.0f);
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(971.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(4.0f, account.total_fee);
}

TEST(ExecuteOrderTest, StopSellAtQuoteWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_stop_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_STOP);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_quote_amount(50.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.market_liquidity = 1.0f;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  order.set_price(1.0f);
  ASSERT_FALSE(account.ExecuteOrder(account_config, order, ohlc_tick));
  // Stop price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  order.set_price(5.0f);
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(3.8f, account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, account.total_fee);
}

TEST(ExecuteOrderTest, LimitBuyWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_limit_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_LIMIT);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_base_amount(10.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.max_volume_ratio = 0.1;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  order.set_price(1.0f);
  ASSERT_FALSE(account.ExecuteOrder(account_config, order, ohlc_tick));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  order.set_price(5.0f);
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);
}

TEST(ExecuteOrderTest, LimitBuyAtQuoteWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_limit_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_LIMIT);
  order.set_side(Order::Side::Order_Side_BUY);
  order.set_quote_amount(57.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.max_volume_ratio = 0.1;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 57.0 units in quote currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  order.set_price(1.0f);
  ASSERT_FALSE(account.ExecuteOrder(account_config, order, ohlc_tick));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  order.set_price(5.0f);
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(20.0f, account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);
}

TEST(ExecuteOrderTest, LimitSellWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_limit_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_LIMIT);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_base_amount(5.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.max_volume_ratio = 0.1;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  order.set_price(25.0f);
  ASSERT_FALSE(account.ExecuteOrder(account_config, order, ohlc_tick));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  order.set_price(10.0f);
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(10.0f, account.base_balance);
  EXPECT_FLOAT_EQ(994.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, account.total_fee);
}

TEST(ExecuteOrderTest, LimitSellAtQuoteWithFeeAndLimitedPrecision) {
  AccountConfig account_config;
  FeeConfig* fee_config = account_config.mutable_limit_order_fee_config();
  fee_config->set_relative_fee(0.1f);
  fee_config->set_fixed_fee(1.0f);
  fee_config->set_minimum_fee(1.5f);

  Order order;
  order.set_type(Order::Type::Order_Type_LIMIT);
  order.set_side(Order::Side::Order_Side_SELL);
  order.set_quote_amount(50.0f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  Account account;
  account.max_volume_ratio = 0.1;
  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;
  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  order.set_price(25.0f);
  ASSERT_FALSE(account.ExecuteOrder(account_config, order, ohlc_tick));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  order.set_price(10.0f);
  ASSERT_TRUE(account.ExecuteOrder(account_config, order, ohlc_tick));
  EXPECT_FLOAT_EQ(9.4f, account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, account.total_fee);
}

}  // namespace trader
