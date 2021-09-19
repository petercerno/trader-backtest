// Copyright © 2021 Peter Cerno. All rights reserved.

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

TEST(InitAccountTest, Basic) {
  Account account;
  EXPECT_FLOAT_EQ(account.base_balance, 0.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 0.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
  EXPECT_FLOAT_EQ(account.base_unit, 0.0f);
  EXPECT_FLOAT_EQ(account.quote_unit, 0.0f);
  EXPECT_FLOAT_EQ(account.market_liquidity, 1.0f);
  EXPECT_FLOAT_EQ(account.max_volume_ratio, 0.0f);

  AccountConfig account_config;
  account_config.set_start_base_balance(2.0f);
  account_config.set_start_quote_balance(1000.0f);
  account_config.set_base_unit(0.0001f);
  account_config.set_quote_unit(0.01f);
  account_config.set_market_liquidity(0.5f);
  account_config.set_max_volume_ratio(0.9f);

  account.InitAccount(account_config);
  EXPECT_FLOAT_EQ(account.base_balance, 2.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 1000.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
  EXPECT_FLOAT_EQ(account.base_unit, 0.0001f);
  EXPECT_FLOAT_EQ(account.quote_unit, 0.01f);
  EXPECT_FLOAT_EQ(account.market_liquidity, 0.5f);
  EXPECT_FLOAT_EQ(account.max_volume_ratio, 0.9f);
}

TEST(GetFeeTest, RelativeFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 100.0f), 10.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 10.0f), 1.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.23456789f);

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.24f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 12.35f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 123.46f);

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.3f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 12.4f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 123.5f);

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 13.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 124.0f);

  fee_config.set_relative_fee(0.01f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 100.0f), 1.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 10.0f), 0.1f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 0.123456789f);

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 0.13f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 1.24f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 12.35f);

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 0.2f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 1.3f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 12.4f);

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 13.0f);
}

TEST(GetFeeTest, FixedFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_fixed_fee(0.12345f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 100.0f), 0.12345f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 10.0f), 0.12345f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 0.12345f);

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 0.13f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 0.13f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 0.13f);

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 0.2f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 0.2f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 0.2f);

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 1.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 1.0f);
}

TEST(GetFeeTest, RelativeAndFixedFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(0.1f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 100.0f), 10.1f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 10.0f), 1.1f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.33456789f);

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.34f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 12.45f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 123.56f);

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.4f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 12.5f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 123.6f);

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 13.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 124.0f);

  fee_config.set_relative_fee(0.01f);
  fee_config.set_fixed_fee(1.0f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 100.0f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 10.0f), 1.1f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.123456789f);

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.13f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 2.24f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 13.35f);

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.2f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 2.3f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 13.4f);

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 3.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 14.0f);
}

TEST(GetFeeTest, RelativeAndMinimumFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_minimum_fee(2.0f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 100.0f), 10.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 10.0f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 12.35f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 123.46f);

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 12.4f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 123.5f);

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 13.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 124.0f);

  fee_config.set_relative_fee(0.01f);
  fee_config.set_minimum_fee(0.5f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 100.0f), 1.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 10.0f), 0.5f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 0.5f);

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 0.5f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 1.24f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 12.35f);

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 0.5f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 1.3f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 12.4f);

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 13.0f);
}

TEST(GetFeeTest, RelativeAndFixedAndMinimumFee) {
  Account account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(0.1f);
  fee_config.set_minimum_fee(2.0f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 100.0f), 10.1f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 10.0f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 12.45f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 123.56f);

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 12.5f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 123.6f);

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 13.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 124.0f);

  fee_config.set_relative_fee(0.01f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 100.0f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 10.0f), 1.5f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.5f);

  account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.5f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 2.24f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 13.35f);

  account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 1.5f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 2.3f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 13.4f);

  account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 12.3456789f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 123.456789f), 3.0f);
  EXPECT_FLOAT_EQ(account.GetFee(fee_config, 1234.56789f), 14.0f);
}

TEST(GetPriceTest, MarketBuy) {
  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(account.GetMarketBuyPrice(ohlc_tick), 10.0f);

  account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(account.GetMarketBuyPrice(ohlc_tick), 20.0f);

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(account.GetMarketBuyPrice(ohlc_tick), 15.0f);
}

TEST(GetPriceTest, MarketSell) {
  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(account.GetMarketSellPrice(ohlc_tick), 10.0f);

  account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(account.GetMarketSellPrice(ohlc_tick), 2.0f);

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(account.GetMarketSellPrice(ohlc_tick), 6.0f);
}

TEST(GetPriceTest, StopBuy) {
  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(  // Target stop price below opening price
      account.GetStopBuyPrice(ohlc_tick, /*price=*/5.0f), 10.0f);
  EXPECT_FLOAT_EQ(account.GetStopBuyPrice(ohlc_tick, /*price=*/10.0f), 10.0f);
  EXPECT_FLOAT_EQ(account.GetStopBuyPrice(ohlc_tick, /*price=*/15.0f), 15.0f);

  account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(  // Target stop price below opening price
      account.GetStopBuyPrice(ohlc_tick, /*price=*/5.0f), 20.0f);
  EXPECT_FLOAT_EQ(account.GetStopBuyPrice(ohlc_tick, /*price=*/10.0f), 20.0f);
  EXPECT_FLOAT_EQ(account.GetStopBuyPrice(ohlc_tick, /*price=*/15.0f), 20.0f);

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(  // Target stop price below opening price
      account.GetStopBuyPrice(ohlc_tick, /*price=*/5.0f), 15.0f);
  EXPECT_FLOAT_EQ(account.GetStopBuyPrice(ohlc_tick, /*price=*/10.0f), 15.0f);
  EXPECT_FLOAT_EQ(account.GetStopBuyPrice(ohlc_tick, /*price=*/15.0f), 17.5f);
}

TEST(GetPriceTest, StopSell) {
  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(  // Target stop price above opening price
      account.GetStopSellPrice(ohlc_tick, /*price=*/15.0f), 10.0f);
  EXPECT_FLOAT_EQ(account.GetStopSellPrice(ohlc_tick, /*price=*/10.0f), 10.0f);
  EXPECT_FLOAT_EQ(account.GetStopSellPrice(ohlc_tick, /*price=*/5.0f), 5.0f);

  account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(  // Target stop price above opening price
      account.GetStopSellPrice(ohlc_tick, /*price=*/15.0f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetStopSellPrice(ohlc_tick, /*price=*/10.0f), 2.0f);
  EXPECT_FLOAT_EQ(account.GetStopSellPrice(ohlc_tick, /*price=*/5.0f), 2.0f);

  account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(  // Target stop price above opening price
      account.GetStopSellPrice(ohlc_tick, /*price=*/15.0f), 6.0f);
  EXPECT_FLOAT_EQ(account.GetStopSellPrice(ohlc_tick, /*price=*/10.0f), 6.0f);
  EXPECT_FLOAT_EQ(account.GetStopSellPrice(ohlc_tick, /*price=*/5.0f), 3.5f);
}

TEST(GetMaxBaseAmountTest, Basic) {
  static constexpr float FLOAT_MAX = std::numeric_limits<float>::max();

  Account account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  account.max_volume_ratio = 0;
  account.base_unit = 0;
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), FLOAT_MAX);

  account.max_volume_ratio = 0.1f;
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 123.456f);

  account.max_volume_ratio = 0.01f;
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 12.3456f);

  account.max_volume_ratio = 0;
  account.base_unit = 0.1f;
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), FLOAT_MAX);

  account.max_volume_ratio = 0.1f;
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 123.4f);

  account.max_volume_ratio = 0.01f;
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 12.3f);
}

TEST(BuyTest, BuyWithoutFeeAndInfinitePrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /*base_amount=*/5.0f,
                              /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 15.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 950.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /*base_amount=*/10.0f,
                              /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 0.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 12.345f;
  account.quote_balance = 123.456f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /*base_amount=*/12.345f,
                              /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 24.69f);
  EXPECT_FLOAT_EQ(account.quote_balance, 55.5585f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyBase(fee_config,
                               /*base_amount=*/15.0f,
                               /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
}

TEST(BuyTest, BuyWithoutFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /*base_amount=*/5.0f,
                              /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 15.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 950.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /*base_amount=*/10.0f,
                              /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 0.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /*base_amount=*/12.345f,
                              /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 24.6f);
  EXPECT_FLOAT_EQ(account.quote_balance, 55.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyBase(fee_config,
                               /*base_amount=*/15.0f,
                               /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
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
                              /*base_amount=*/5.0f,
                              /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 15.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 944.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 111.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /*base_amount=*/10.0f,
                              /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 0.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 21.0f);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyBase(fee_config,
                              /*base_amount=*/12.345f,
                              /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 24.6f);
  EXPECT_FLOAT_EQ(account.quote_balance, 47.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 8.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.BuyBase(fee_config,
                               /*base_amount=*/15.0f,
                               /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 10.0f);
}

TEST(BuyTest, BuyAtQuoteWithoutFeeAndInfinitePrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/50.0f,
                                 /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 15.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 950.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/50.0f,
                                 /*price=*/10.0f,
                                 /*max_base_amount=*/2.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 980.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/100.0f,
                                 /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 0.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/100.0f,
                                 /*price=*/10.0f,
                                 /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 15.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 50.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 12.345f;
  account.quote_balance = 123.456f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/67.8975f,
                                 /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 24.69f);
  EXPECT_FLOAT_EQ(account.quote_balance, 55.5585f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 12.345f;
  account.quote_balance = 123.456f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/67.8975f,
                                 /*price=*/5.5f,
                                 /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 17.345f);
  EXPECT_FLOAT_EQ(account.quote_balance, 95.956f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /*quote_amount=*/1000.0f,
                                  /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /*quote_amount=*/1000.0f,
                                  /*price=*/10.0f,
                                  /*max_base_amount=*/20.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
}

TEST(BuyTest, BuyAtQuoteWithoutFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/50.0f,
                                 /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 15.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 950.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/50.0f,
                                 /*price=*/10.0f,
                                 /*max_base_amount=*/2.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 980.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/100.0f,
                                 /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 0.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/100.0f,
                                 /*price=*/10.0f,
                                 /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 15.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 50.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/67.8975f,
                                 /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 24.6f);
  EXPECT_FLOAT_EQ(account.quote_balance, 55.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/67.8975f,
                                 /*price=*/5.5f,
                                 /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 17.3f);
  EXPECT_FLOAT_EQ(account.quote_balance, 95.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /*quote_amount=*/1000.0f,
                                  /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /*quote_amount=*/1000.0f,
                                  /*price=*/10.0f,
                                  /*max_base_amount=*/20.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
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
                                 /*quote_amount=*/57.0f,
                                 /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 15.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 944.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/57.0f,
                                 /*price=*/10.0f,
                                 /*max_base_amount=*/2.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 977.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 3.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 111.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/111.0f,
                                 /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 19.8f);
  EXPECT_FLOAT_EQ(account.quote_balance, 2.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 21.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 111.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/111.0f,
                                 /*price=*/10.0f,
                                 /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 15.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 55.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 16.0f);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/76.123f,
                                 /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 24.4f);
  EXPECT_FLOAT_EQ(account.quote_balance, 48.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 8.0f);

  account.base_balance = 12.3f;
  account.quote_balance = 123.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.BuyAtQuote(fee_config,
                                 /*quote_amount=*/76.123f,
                                 /*price=*/5.5f,
                                 /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 17.3f);
  EXPECT_FLOAT_EQ(account.quote_balance, 91.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 4.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /*quote_amount=*/1000.0f,
                                  /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 10.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.BuyAtQuote(fee_config,
                                  /*quote_amount=*/1000.0f,
                                  /*price=*/10.0f,
                                  /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 10.0f);
}

TEST(SellTest, SellWithoutFeeAndInfinitePrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /*base_amount=*/5.0f,
                               /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 1000.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /*base_amount=*/10.0f,
                               /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 24.69f;
  account.quote_balance = 55.5585f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /*base_amount=*/12.345f,
                               /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.345f);
  EXPECT_FLOAT_EQ(account.quote_balance, 123.456f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellBase(fee_config,
                                /*base_amount=*/15.0f,
                                /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
}

TEST(SellTest, SellWithoutFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /*base_amount=*/5.0f,
                               /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 1000.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /*base_amount=*/10.0f,
                               /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 24.6f;
  account.quote_balance = 55.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /*base_amount=*/12.345f,
                               /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.3f);
  EXPECT_FLOAT_EQ(account.quote_balance, 122.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellBase(fee_config,
                                /*base_amount=*/15.0f,
                                /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
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
                               /*base_amount=*/5.0f,
                               /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 994.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /*base_amount=*/10.0f,
                               /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 89.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 21.0f);

  account.base_balance = 24.6f;
  account.quote_balance = 47.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellBase(fee_config,
                               /*base_amount=*/12.345f,
                               /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.3f);
  EXPECT_FLOAT_EQ(account.quote_balance, 106.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 8.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.SellBase(fee_config,
                                /*base_amount=*/15.0f,
                                /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 10.0f);
}

TEST(SellTest, SellAtQuoteWithoutFeeAndInfinitePrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/50.0f,
                                  /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 1000.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/50.0f,
                                  /*price=*/10.0f,
                                  /*max_base_amount=*/2.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 13.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 970.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/100.0f,
                                  /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 15.0f;
  account.quote_balance = 50.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/100.0f,
                                  /*price=*/10.0f,
                                  /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 24.69f;
  account.quote_balance = 55.5585f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/67.8975f,
                                  /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.345f);
  EXPECT_FLOAT_EQ(account.quote_balance, 123.456f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 17.345f;
  account.quote_balance = 95.956f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/67.8975f,
                                  /*price=*/5.5f,
                                  /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.345f);
  EXPECT_FLOAT_EQ(account.quote_balance, 123.456f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /*quote_amount=*/1000.0f,
                                   /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /*quote_amount=*/1000.0f,
                                   /*price=*/10.0f,
                                   /*max_base_amount=*/20.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
}

TEST(SellTest, SellAtQuoteWithoutFeeAndLimitedPrecision) {
  Account account;
  FeeConfig fee_config;

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/50.0f,
                                  /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 1000.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 12.0f;
  account.quote_balance = 980.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/50.0f,
                                  /*price=*/10.0f,
                                  /*max_base_amount=*/2.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 1000.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/100.0f,
                                  /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 15.0f;
  account.quote_balance = 50.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/100.0f,
                                  /*price=*/10.0f,
                                  /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 24.6f;
  account.quote_balance = 55.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/67.8975f,
                                  /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.3f);
  EXPECT_FLOAT_EQ(account.quote_balance, 122.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 17.3f;
  account.quote_balance = 95.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/67.8975f,
                                  /*price=*/5.5f,
                                  /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.3f);
  EXPECT_FLOAT_EQ(account.quote_balance, 122.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /*quote_amount=*/1000.0f,
                                   /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /*quote_amount=*/1000.0f,
                                   /*price=*/10.0f,
                                   /*max_base_amount=*/20.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 0.0f);
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
                                  /*quote_amount=*/50.0f,
                                  /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 9.4f);
  EXPECT_FLOAT_EQ(account.quote_balance, 999.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 7.0f);

  account.base_balance = 12.0f;
  account.quote_balance = 980.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/50.0f,
                                  /*price=*/10.0f,
                                  /*max_base_amount=*/2.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 997.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 3.0f);

  account.base_balance = 20.0f;
  account.quote_balance = 0.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/100.0f,
                                  /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 8.9f);
  EXPECT_FLOAT_EQ(account.quote_balance, 98.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 23.0f);

  account.base_balance = 15.0f;
  account.quote_balance = 50.0f;
  account.total_fee = 10.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/100.0f,
                                  /*price=*/10.0f,
                                  /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 94.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 16.0f);

  account.base_balance = 24.6f;
  account.quote_balance = 55.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/67.8975f,
                                  /*price=*/5.5f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.8f);
  EXPECT_FLOAT_EQ(account.quote_balance, 121.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 9.0f);

  account.base_balance = 17.3f;
  account.quote_balance = 95.0f;
  account.total_fee = 0.0f;
  EXPECT_TRUE(account.SellAtQuote(fee_config,
                                  /*quote_amount=*/67.8975f,
                                  /*price=*/5.5f,
                                  /*max_base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 12.3f);
  EXPECT_FLOAT_EQ(account.quote_balance, 118.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 4.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /*quote_amount=*/1000.0f,
                                   /*price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 10.0f);

  account.base_balance = 10.0f;
  account.quote_balance = 100.0f;
  account.total_fee = 10.0f;
  EXPECT_FALSE(account.SellAtQuote(fee_config,
                                   /*quote_amount=*/1000.0f,
                                   /*price=*/10.0f,
                                   /*max_base_amount=*/20.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 100.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 10.0f);
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
  EXPECT_FLOAT_EQ(account.GetMarketBuyPrice(ohlc_tick), 15.0f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  EXPECT_TRUE(account.MarketBuy(fee_config, ohlc_tick,
                                /*base_amount=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 834.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 16.0f);
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
  EXPECT_FLOAT_EQ(account.GetMarketBuyPrice(ohlc_tick), 15.0f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 169.0 units in quote currency.
  EXPECT_TRUE(account.MarketBuyAtQuote(fee_config, ohlc_tick,
                                       /*quote_amount=*/169.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 834.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 16.0f);
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
  EXPECT_FLOAT_EQ(account.GetMarketSellPrice(ohlc_tick), 10.0f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  EXPECT_TRUE(account.MarketSell(fee_config, ohlc_tick,
                                 /*base_amount=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 994.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);
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
  EXPECT_FLOAT_EQ(account.GetMarketSellPrice(ohlc_tick), 10.0f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  EXPECT_TRUE(account.MarketSellAtQuote(fee_config, ohlc_tick,
                                        /*quote_amount=*/50.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 9.4f);
  EXPECT_FLOAT_EQ(account.quote_balance, 999.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 7.0f);
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
  EXPECT_FLOAT_EQ(17.5f, account.GetStopBuyPrice(ohlc_tick, /*price=*/15.0f));

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.StopBuy(fee_config, ohlc_tick,
                               /*base_amount=*/10.0f,
                               /*stop_price=*/25.0f));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.StopBuy(fee_config, ohlc_tick,
                              /*base_amount=*/10.0f,
                              /*stop_price=*/15.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 806.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 19.0f);
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
  EXPECT_FLOAT_EQ(account.GetStopBuyPrice(ohlc_tick, /*price=*/15.0f), 17.5f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 197.0 units in quote currency.
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.StopBuyAtQuote(fee_config, ohlc_tick,
                                      /*quote_amount=*/197.0f,
                                      /*stop_price=*/25.0f));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.StopBuyAtQuote(fee_config, ohlc_tick,
                                     /*quote_amount=*/197.0f,
                                     /*stop_price=*/15.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 806.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 19.0f);
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
  EXPECT_FLOAT_EQ(account.GetStopSellPrice(ohlc_tick, /*price=*/5.0f), 5.0f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.StopSell(fee_config, ohlc_tick,
                                /*base_amount=*/5.0f,
                                /*stop_price=*/1.0f));
  // Stop price 5.0 is below the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.StopSell(fee_config, ohlc_tick,
                               /*base_amount=*/5.0f,
                               /*stop_price=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 971.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 4.0f);
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
  EXPECT_FLOAT_EQ(account.GetStopSellPrice(ohlc_tick, /*price=*/5.0f), 5.0f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.StopSellAtQuote(fee_config, ohlc_tick,
                                       /*quote_amount=*/50.0f,
                                       /*stop_price=*/1.0f));
  // Stop price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.StopSellAtQuote(fee_config, ohlc_tick,
                                      /*quote_amount=*/50.0f,
                                      /*stop_price=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 3.8f);
  EXPECT_FLOAT_EQ(account.quote_balance, 999.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 7.0f);
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
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 123.456f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitBuy(fee_config, ohlc_tick,
                                /*base_amount=*/10.0f,
                                /*limit_price=*/1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.LimitBuy(fee_config, ohlc_tick,
                               /*base_amount=*/10.0f,
                               /*limit_price=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 944.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);
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
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 1.23456f);

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
                                /*base_amount=*/10.0f,
                                /*limit_price=*/1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.LimitBuy(fee_config, ohlc_tick,
                               /*base_amount=*/10.0f,
                               /*limit_price=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 11.2f);
  EXPECT_FLOAT_EQ(account.quote_balance, 992.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 2.0f);
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
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 123.456f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 10.0f;
  account.quote_balance = 1000.0f;
  account.total_fee = 0.0f;

  // We want to buy base currency with up to 57.0 units in quote currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                       /*quote_amount=*/57.0f,
                                       /*limit_price=*/1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                      /*quote_amount=*/57.0f,
                                      /*limit_price=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 944.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);
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
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 1.23456f);

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
                                       /*quote_amount=*/57.0f,
                                       /*limit_price=*/1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                      /*quote_amount=*/57.0f,
                                      /*limit_price=*/5.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 11.2f);
  EXPECT_FLOAT_EQ(account.quote_balance, 992.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 2.0f);
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
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 123.456f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitSell(fee_config, ohlc_tick,
                                 /*base_amount=*/5.0f,
                                 /*limit_price=*/25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.LimitSell(fee_config, ohlc_tick,
                                /*base_amount=*/5.0f,
                                /*limit_price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 994.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);
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
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 1.23456f);

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
                                 /*base_amount=*/5.0f,
                                 /*limit_price=*/25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.LimitSell(fee_config, ohlc_tick,
                                /*base_amount=*/5.0f,
                                /*limit_price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 13.8f);
  EXPECT_FLOAT_EQ(account.quote_balance, 959.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 3.0f);
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
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 123.456f);

  account.base_unit = 0.1f;
  account.quote_unit = 1.0f;

  account.base_balance = 15.0f;
  account.quote_balance = 950.0f;
  account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(account.LimitSellAtQuote(fee_config, ohlc_tick,
                                        /*quote_amount=*/50.0f,
                                        /*limit_price=*/25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.LimitSellAtQuote(fee_config, ohlc_tick,
                                       /*quote_amount=*/50.0f,
                                       /*limit_price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 9.4f);
  EXPECT_FLOAT_EQ(account.quote_balance, 999.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 7.0f);
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
  EXPECT_FLOAT_EQ(account.GetMaxBaseAmount(ohlc_tick), 1.23456f);

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
                                        /*quote_amount=*/50.0f,
                                        /*limit_price=*/25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(account.LimitSellAtQuote(fee_config, ohlc_tick,
                                       /*quote_amount=*/50.0f,
                                       /*limit_price=*/10.0f));
  EXPECT_FLOAT_EQ(account.base_balance, 13.8f);
  EXPECT_FLOAT_EQ(account.quote_balance, 959.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 3.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 834.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 16.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 834.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 16.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 994.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 9.4f);
  EXPECT_FLOAT_EQ(account.quote_balance, 999.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 7.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 806.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 19.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 806.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 19.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 971.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 4.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 3.8f);
  EXPECT_FLOAT_EQ(account.quote_balance, 999.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 7.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 944.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 20.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 944.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 10.0f);
  EXPECT_FLOAT_EQ(account.quote_balance, 994.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 6.0f);
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
  EXPECT_FLOAT_EQ(account.base_balance, 9.4f);
  EXPECT_FLOAT_EQ(account.quote_balance, 999.0f);
  EXPECT_FLOAT_EQ(account.total_fee, 7.0f);
}

}  // namespace trader
