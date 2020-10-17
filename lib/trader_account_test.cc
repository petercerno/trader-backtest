// Copyright © 2020 Peter Cerno. All rights reserved.

#include "lib/trader_account.h"

#include <limits>

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

TEST(GetFeeTest, RelativeFee) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);

  trader_account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.23456789f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.24f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.35f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.46f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.3f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.4f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.5f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, trader_account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);

  trader_account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(0.1f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(0.123456789f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(0.13f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.24f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.35f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(0.2f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.3f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.4f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, FixedFee) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_fixed_fee(0.12345f);

  trader_account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(0.12345f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(0.12345f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(0.12345f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(0.13f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(0.13f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(0.13f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(0.2f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(0.2f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(0.2f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, RelativeAndFixedFee) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(0.1f);

  trader_account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.1f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.1f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.33456789f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.34f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.45f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.56f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.4f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.5f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.6f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, trader_account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);
  fee_config.set_fixed_fee(1.0f);

  trader_account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.1f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.123456789f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.13f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.24f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.35f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.2f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.3f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.4f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(3.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(14.0f, trader_account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, RelativeAndMinimumFee) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_minimum_fee(2.0f);

  trader_account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.35f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.46f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.4f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.5f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, trader_account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);
  fee_config.set_minimum_fee(0.5f);

  trader_account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(0.5f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(0.5f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(0.5f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.24f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.35f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(0.5f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.3f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.4f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, RelativeAndFixedAndMinimumFee) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(0.1f);
  fee_config.set_minimum_fee(2.0f);

  trader_account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.1f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.45f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.56f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.5f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.6f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, trader_account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.quote_unit = 0.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.5f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.5f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.quote_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.5f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.24f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.35f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.5f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.3f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.4f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.quote_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(3.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(14.0f, trader_account.GetFee(fee_config, 1234.56789f));
}

TEST(GetPriceTest, MarketBuy) {
  TraderAccount trader_account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(10.0f, trader_account.GetMarketBuyPrice(ohlc_tick));

  trader_account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(20.0f, trader_account.GetMarketBuyPrice(ohlc_tick));

  trader_account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(15.0f, trader_account.GetMarketBuyPrice(ohlc_tick));
}

TEST(GetPriceTest, MarketSell) {
  TraderAccount trader_account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(10.0f, trader_account.GetMarketSellPrice(ohlc_tick));

  trader_account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetMarketSellPrice(ohlc_tick));

  trader_account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(6.0f, trader_account.GetMarketSellPrice(ohlc_tick));
}

TEST(GetPriceTest, StopBuy) {
  TraderAccount trader_account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(  // Target stop price below opening price
      10.0f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 5.0f));
  EXPECT_FLOAT_EQ(
      10.0f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(
      15.0f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));

  trader_account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(  // Target stop price below opening price
      20.0f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 5.0f));
  EXPECT_FLOAT_EQ(
      20.0f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(
      20.0f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));

  trader_account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(  // Target stop price below opening price
      15.0f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 5.0f));
  EXPECT_FLOAT_EQ(
      15.0f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(
      17.5f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));
}

TEST(GetPriceTest, StopSell) {
  TraderAccount trader_account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(  // Target stop price above opening price
      10.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 15.0f));
  EXPECT_FLOAT_EQ(
      10.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(
      5.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));

  trader_account.market_liquidity = 0.0f;
  EXPECT_FLOAT_EQ(  // Target stop price above opening price
      2.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 15.0f));
  EXPECT_FLOAT_EQ(
      2.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(
      2.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));

  trader_account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(  // Target stop price above opening price
      6.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 15.0f));
  EXPECT_FLOAT_EQ(
      6.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(
      3.5f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));
}

TEST(GetMaxBaseAmountTest, Basic) {
  static constexpr float FLOAT_MAX = std::numeric_limits<float>::max();

  TraderAccount trader_account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0;
  trader_account.base_unit = 0;
  EXPECT_FLOAT_EQ(FLOAT_MAX, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0.1f;
  EXPECT_FLOAT_EQ(123.456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0.01f;
  EXPECT_FLOAT_EQ(12.3456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0;
  trader_account.base_unit = 0.1f;
  EXPECT_FLOAT_EQ(FLOAT_MAX, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0.1f;
  EXPECT_FLOAT_EQ(123.4f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0.01f;
  EXPECT_FLOAT_EQ(12.3f, trader_account.GetMaxBaseAmount(ohlc_tick));
}

TEST(BuyTest, BuyWithoutFeeAndInfinitePrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyBase(fee_config,
                                     /* base_amount = */ 5.0f,
                                     /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(950.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyBase(fee_config,
                                     /* base_amount = */ 10.0f,
                                     /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 12.345f;
  trader_account.quote_balance = 123.456f;
  EXPECT_TRUE(trader_account.BuyBase(fee_config,
                                     /* base_amount = */ 12.345f,
                                     /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.69f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(55.5585f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyBase(fee_config,
                                      /* base_amount = */ 15.0f,
                                      /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyWithoutFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyBase(fee_config,
                                     /* base_amount = */ 5.0f,
                                     /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(950.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyBase(fee_config,
                                     /* base_amount = */ 10.0f,
                                     /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 12.3f;
  trader_account.quote_balance = 123.0f;
  EXPECT_TRUE(trader_account.BuyBase(fee_config,
                                     /* base_amount = */ 12.345f,
                                     /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.6f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(55.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyBase(fee_config,
                                      /* base_amount = */ 15.0f,
                                      /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyBase(fee_config,
                                     /* base_amount = */ 5.0f,
                                     /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 111.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.BuyBase(fee_config,
                                     /* base_amount = */ 10.0f,
                                     /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(21.0f, trader_account.total_fee);

  trader_account.base_balance = 12.3f;
  trader_account.quote_balance = 123.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyBase(fee_config,
                                     /* base_amount = */ 12.345f,
                                     /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.6f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(47.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(8.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.BuyBase(fee_config,
                                      /* base_amount = */ 15.0f,
                                      /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyAtQuoteWithoutFeeAndInfinitePrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 50.0f,
                                        /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(950.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 50.0f,
                                        /* price = */ 10.0f,
                                        /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(12.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(980.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 100.0f,
                                        /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 100.0f,
                                        /* price = */ 10.0f,
                                        /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(50.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 12.345f;
  trader_account.quote_balance = 123.456f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 67.8975f,
                                        /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.69f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(55.5585f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 12.345f;
  trader_account.quote_balance = 123.456f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 67.8975f,
                                        /* price = */ 5.5f,
                                        /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(17.345f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(95.956f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyAtQuote(fee_config,
                                         /* quote_amount = */ 1000.0f,
                                         /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyAtQuote(fee_config,
                                         /* quote_amount = */ 1000.0f,
                                         /* price = */ 10.0f,
                                         /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyAtQuoteWithoutFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 50.0f,
                                        /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(950.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 50.0f,
                                        /* price = */ 10.0f,
                                        /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(12.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(980.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 100.0f,
                                        /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 100.0f,
                                        /* price = */ 10.0f,
                                        /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(50.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 12.3f;
  trader_account.quote_balance = 123.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 67.8975f,
                                        /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.6f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(55.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 12.3f;
  trader_account.quote_balance = 123.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 67.8975f,
                                        /* price = */ 5.5f,
                                        /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(17.3f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(95.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyAtQuote(fee_config,
                                         /* quote_amount = */ 1000.0f,
                                         /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyAtQuote(fee_config,
                                         /* quote_amount = */ 1000.0f,
                                         /* price = */ 10.0f,
                                         /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyAtQuoteWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 57.0f,
                                        /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 57.0f,
                                        /* price = */ 10.0f,
                                        /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(12.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(977.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(3.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 111.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 111.0f,
                                        /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(19.8f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(2.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(21.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 111.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 111.0f,
                                        /* price = */ 10.0f,
                                        /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(55.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, trader_account.total_fee);

  trader_account.base_balance = 12.3f;
  trader_account.quote_balance = 123.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 76.123f,
                                        /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(24.4f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(48.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(8.0f, trader_account.total_fee);

  trader_account.base_balance = 12.3f;
  trader_account.quote_balance = 123.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyAtQuote(fee_config,
                                        /* quote_amount = */ 76.123f,
                                        /* price = */ 5.5f,
                                        /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(17.3f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(91.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(4.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.BuyAtQuote(fee_config,
                                         /* quote_amount = */ 1000.0f,
                                         /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.BuyAtQuote(fee_config,
                                         /* quote_amount = */ 1000.0f,
                                         /* price = */ 10.0f,
                                         /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);
}

TEST(SellTest, SellWithoutFeeAndInfinitePrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  EXPECT_TRUE(trader_account.SellBase(fee_config,
                                      /* base_amount = */ 5.0f,
                                      /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 20.0f;
  trader_account.quote_balance = 0.0f;
  EXPECT_TRUE(trader_account.SellBase(fee_config,
                                      /* base_amount = */ 10.0f,
                                      /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 24.69f;
  trader_account.quote_balance = 55.5585f;
  EXPECT_TRUE(trader_account.SellBase(fee_config,
                                      /* base_amount = */ 12.345f,
                                      /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.345f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(123.456f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellBase(fee_config,
                                       /* base_amount = */ 15.0f,
                                       /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(SellTest, SellWithoutFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  EXPECT_TRUE(trader_account.SellBase(fee_config,
                                      /* base_amount = */ 5.0f,
                                      /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 20.0f;
  trader_account.quote_balance = 0.0f;
  EXPECT_TRUE(trader_account.SellBase(fee_config,
                                      /* base_amount = */ 10.0f,
                                      /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 24.6f;
  trader_account.quote_balance = 55.0f;
  EXPECT_TRUE(trader_account.SellBase(fee_config,
                                      /* base_amount = */ 12.345f,
                                      /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(122.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellBase(fee_config,
                                       /* base_amount = */ 15.0f,
                                       /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(SellTest, SellWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellBase(fee_config,
                                      /* base_amount = */ 5.0f,
                                      /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(994.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);

  trader_account.base_balance = 20.0f;
  trader_account.quote_balance = 0.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.SellBase(fee_config,
                                      /* base_amount = */ 10.0f,
                                      /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(89.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(21.0f, trader_account.total_fee);

  trader_account.base_balance = 24.6f;
  trader_account.quote_balance = 47.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellBase(fee_config,
                                      /* base_amount = */ 12.345f,
                                      /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(106.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(8.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.SellBase(fee_config,
                                       /* base_amount = */ 15.0f,
                                       /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);
}

TEST(SellTest, SellAtQuoteWithoutFeeAndInfinitePrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 50.0f,
                                         /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 50.0f,
                                         /* price = */ 10.0f,
                                         /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(970.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 20.0f;
  trader_account.quote_balance = 0.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 100.0f,
                                         /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 50.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 100.0f,
                                         /* price = */ 10.0f,
                                         /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 24.69f;
  trader_account.quote_balance = 55.5585f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 67.8975f,
                                         /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.345f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(123.456f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 17.345f;
  trader_account.quote_balance = 95.956f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 67.8975f,
                                         /* price = */ 5.5f,
                                         /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(12.345f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(123.456f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellAtQuote(fee_config,
                                          /* quote_amount = */ 1000.0f,
                                          /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellAtQuote(fee_config,
                                          /* quote_amount = */ 1000.0f,
                                          /* price = */ 10.0f,
                                          /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(SellTest, SellAtQuoteWithoutFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 50.0f,
                                         /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 12.0f;
  trader_account.quote_balance = 980.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 50.0f,
                                         /* price = */ 10.0f,
                                         /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 20.0f;
  trader_account.quote_balance = 0.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 100.0f,
                                         /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 50.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 100.0f,
                                         /* price = */ 10.0f,
                                         /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 24.6f;
  trader_account.quote_balance = 55.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 67.8975f,
                                         /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(122.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 17.3f;
  trader_account.quote_balance = 95.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 67.8975f,
                                         /* price = */ 5.5f,
                                         /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(122.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellAtQuote(fee_config,
                                          /* quote_amount = */ 1000.0f,
                                          /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellAtQuote(fee_config,
                                          /* quote_amount = */ 1000.0f,
                                          /* price = */ 10.0f,
                                          /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(SellTest, SellAtQuoteWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 50.0f,
                                         /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(9.4f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, trader_account.total_fee);

  trader_account.base_balance = 12.0f;
  trader_account.quote_balance = 980.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 50.0f,
                                         /* price = */ 10.0f,
                                         /* max_base_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(997.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(3.0f, trader_account.total_fee);

  trader_account.base_balance = 20.0f;
  trader_account.quote_balance = 0.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 100.0f,
                                         /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(8.9f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(98.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(23.0f, trader_account.total_fee);

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 50.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 100.0f,
                                         /* price = */ 10.0f,
                                         /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(94.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, trader_account.total_fee);

  trader_account.base_balance = 24.6f;
  trader_account.quote_balance = 55.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 67.8975f,
                                         /* price = */ 5.5f));
  EXPECT_FLOAT_EQ(10.8f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(121.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(9.0f, trader_account.total_fee);

  trader_account.base_balance = 17.3f;
  trader_account.quote_balance = 95.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellAtQuote(fee_config,
                                         /* quote_amount = */ 67.8975f,
                                         /* price = */ 5.5f,
                                         /* max_base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(118.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(4.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.SellAtQuote(fee_config,
                                          /* quote_amount = */ 1000.0f,
                                          /* price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.SellAtQuote(fee_config,
                                          /* quote_amount = */ 1000.0f,
                                          /* price = */ 10.0f,
                                          /* max_base_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);
}

TEST(BuyTest, MarketBuyWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(15.0f, trader_account.GetMarketBuyPrice(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  EXPECT_TRUE(trader_account.MarketBuy(fee_config, ohlc_tick,
                                       /* base_amount = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(834.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, trader_account.total_fee);
}

TEST(BuyTest, MarketBuyAtQuoteWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(15.0f, trader_account.GetMarketBuyPrice(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy base currency with up to 169.0 units in quote currency.
  EXPECT_TRUE(trader_account.MarketBuyAtQuote(fee_config, ohlc_tick,
                                              /* quote_amount = */ 169.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(834.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(16.0f, trader_account.total_fee);
}

TEST(SellTest, MarketSellWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(10.0f, trader_account.GetMarketSellPrice(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  EXPECT_TRUE(trader_account.MarketSell(fee_config, ohlc_tick,
                                        /* base_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(994.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);
}

TEST(SellTest, MarketSellAtQuoteWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(10.0f, trader_account.GetMarketSellPrice(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  EXPECT_TRUE(trader_account.MarketSellAtQuote(fee_config, ohlc_tick,
                                               /* quote_amount = */ 50.0f));
  EXPECT_FLOAT_EQ(9.4f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, trader_account.total_fee);
}

TEST(BuyTest, StopBuyWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(
      17.5f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.StopBuy(fee_config, ohlc_tick,
                                      /* base_amount = */ 10.0f,
                                      /* stop_price = */ 25.0f));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(trader_account.StopBuy(fee_config, ohlc_tick,
                                     /* base_amount = */ 10.0f,
                                     /* stop_price = */ 15.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(806.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(19.0f, trader_account.total_fee);
}

TEST(BuyTest, StopBuyAtQuoteWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 0.5f;
  EXPECT_FLOAT_EQ(
      17.5f, trader_account.GetStopBuyPrice(ohlc_tick, /* price = */ 15.0f));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy base currency with up to 197.0 units in quote currency.
  // Stop price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.StopBuyAtQuote(fee_config, ohlc_tick,
                                             /* quote_amount = */ 197.0f,
                                             /* stop_price = */ 25.0f));
  // Stop price 15.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(trader_account.StopBuyAtQuote(fee_config, ohlc_tick,
                                            /* quote_amount = */ 197.0f,
                                            /* stop_price = */ 15.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(806.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(19.0f, trader_account.total_fee);
}

TEST(SellTest, StopSellWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(
      5.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.StopSell(fee_config, ohlc_tick,
                                       /* base_amount = */ 5.0f,
                                       /* stop_price = */ 1.0f));
  // Stop price 5.0 is below the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(trader_account.StopSell(fee_config, ohlc_tick,
                                      /* base_amount = */ 5.0f,
                                      /* stop_price = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(971.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(4.0f, trader_account.total_fee);
}

TEST(SellTest, StopSellAtQuoteWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.market_liquidity = 1.0f;
  EXPECT_FLOAT_EQ(
      5.0f, trader_account.GetStopSellPrice(ohlc_tick, /* price = */ 5.0f));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // Stop price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.StopSellAtQuote(fee_config, ohlc_tick,
                                              /* quote_amount = */ 50.0f,
                                              /* stop_price = */ 1.0f));
  // Stop price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(trader_account.StopSellAtQuote(fee_config, ohlc_tick,
                                             /* quote_amount = */ 50.0f,
                                             /* stop_price = */ 5.0f));
  EXPECT_FLOAT_EQ(3.8f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, trader_account.total_fee);
}

TEST(BuyTest, LimitBuyWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0.1;
  EXPECT_FLOAT_EQ(123.456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.LimitBuy(fee_config, ohlc_tick,
                                       /* base_amount = */ 10.0f,
                                       /* limit_price = */ 1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(trader_account.LimitBuy(fee_config, ohlc_tick,
                                      /* base_amount = */ 10.0f,
                                      /* limit_price = */ 5.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);
}

TEST(BuyTest, LimitBuyWithFeeAndLimitedPrecisionExceedsMaxAmount) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0.001;
  EXPECT_FLOAT_EQ(1.23456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy 10.0 units of base (crypto) currency.
  // However, we can buy at most 1.2 units of base (crypto) currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.LimitBuy(fee_config, ohlc_tick,
                                       /* base_amount = */ 10.0f,
                                       /* limit_price = */ 1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(trader_account.LimitBuy(fee_config, ohlc_tick,
                                      /* base_amount = */ 10.0f,
                                      /* limit_price = */ 5.0f));
  EXPECT_FLOAT_EQ(11.2f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(992.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(2.0f, trader_account.total_fee);
}

TEST(BuyTest, LimitBuyAtQuoteWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0.1;
  EXPECT_FLOAT_EQ(123.456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy base currency with up to 57.0 units in quote currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                              /* quote_amount = */ 57.0f,
                                              /* limit_price = */ 1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(trader_account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                             /* quote_amount = */ 57.0f,
                                             /* limit_price = */ 5.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(944.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);
}

TEST(BuyTest, LimitBuyAtQuoteWithFeeAndLimitedPrecisionExceedsMaxAmount) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0.001;
  EXPECT_FLOAT_EQ(1.23456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 10.0f;
  trader_account.quote_balance = 1000.0f;
  trader_account.total_fee = 0.0f;

  // We want to buy base currency with up to 57.0 units in quote currency.
  // However, we can buy at most 1.2 units of base (crypto) currency.
  // Limit price 1.0 is below the OHLC tick low price 2.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                              /* quote_amount = */ 57.0f,
                                              /* limit_price = */ 1.0f));
  // Limit price 5.0 is above the OHLC tick low price 2.0. Order is executed.
  EXPECT_TRUE(trader_account.LimitBuyAtQuote(fee_config, ohlc_tick,
                                             /* quote_amount = */ 57.0f,
                                             /* limit_price = */ 5.0f));
  EXPECT_FLOAT_EQ(11.2f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(992.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(2.0f, trader_account.total_fee);
}

TEST(SellTest, LimitSellWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0.1;
  EXPECT_FLOAT_EQ(123.456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.LimitSell(fee_config, ohlc_tick,
                                        /* base_amount = */ 5.0f,
                                        /* limit_price = */ 25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(trader_account.LimitSell(fee_config, ohlc_tick,
                                       /* base_amount = */ 5.0f,
                                       /* limit_price = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(994.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);
}

TEST(SellTest, LimitSellWithFeeAndLimitedPrecisionExceedsMaxAmount) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0.001;
  EXPECT_FLOAT_EQ(1.23456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell 5.0 units of base (crypto) currency.
  // However, we can sell at most 1.2 units of base (crypto) currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.LimitSell(fee_config, ohlc_tick,
                                        /* base_amount = */ 5.0f,
                                        /* limit_price = */ 25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(trader_account.LimitSell(fee_config, ohlc_tick,
                                       /* base_amount = */ 5.0f,
                                       /* limit_price = */ 10.0f));
  EXPECT_FLOAT_EQ(13.8f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(959.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(3.0f, trader_account.total_fee);
}

TEST(SellTest, LimitSellAtQuoteWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0.1;
  EXPECT_FLOAT_EQ(123.456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.LimitSellAtQuote(fee_config, ohlc_tick,
                                               /* quote_amount = */ 50.0f,
                                               /* limit_price = */ 25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(trader_account.LimitSellAtQuote(fee_config, ohlc_tick,
                                              /* quote_amount = */ 50.0f,
                                              /* limit_price = */ 10.0f));
  EXPECT_FLOAT_EQ(9.4f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(999.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(7.0f, trader_account.total_fee);
}

TEST(SellTest, LimitSellAtQuoteWithFeeAndLimitedPrecisionExceedsMaxAmount) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0.001;
  EXPECT_FLOAT_EQ(1.23456f, trader_account.GetMaxBaseAmount(ohlc_tick));

  trader_account.base_unit = 0.1f;
  trader_account.quote_unit = 1.0f;

  trader_account.base_balance = 15.0f;
  trader_account.quote_balance = 950.0f;
  trader_account.total_fee = 0.0f;

  // We want to sell base currency to get up to 50.0 units in quote currency.
  // However, we can sell at most 1.2 units of base (crypto) currency.
  // Limit price 25.0 is above the OHLC tick high price 20.0.
  // Therefore, the order cannot be executed.
  EXPECT_FALSE(trader_account.LimitSellAtQuote(fee_config, ohlc_tick,
                                               /* quote_amount = */ 50.0f,
                                               /* limit_price = */ 25.0f));
  // Limit price 10.0 is below the OHLC tick high price 20.0. Order is executed.
  EXPECT_TRUE(trader_account.LimitSellAtQuote(fee_config, ohlc_tick,
                                              /* quote_amount = */ 50.0f,
                                              /* limit_price = */ 10.0f));
  EXPECT_FLOAT_EQ(13.8f, trader_account.base_balance);
  EXPECT_FLOAT_EQ(959.0f, trader_account.quote_balance);
  EXPECT_FLOAT_EQ(3.0f, trader_account.total_fee);
}

}  // namespace trader
