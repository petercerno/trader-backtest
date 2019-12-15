// Copyright © 2019 Peter Cerno. All rights reserved.

#include "trader_account.h"

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

  trader_account.cash_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.23456789f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.cash_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.24f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.35f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.46f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.3f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.4f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.5f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, trader_account.GetFee(fee_config, 1234.56789f));

  // 1% Relative Fee
  fee_config.set_relative_fee(0.01f);

  trader_account.cash_unit = 0.0f;
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(0.1f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(0.123456789f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.cash_unit = 0.01f;
  EXPECT_FLOAT_EQ(0.13f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.24f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.35f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 0.1f;
  EXPECT_FLOAT_EQ(0.2f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.3f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.4f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 1.0f;
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, FixedFee) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_fixed_fee(0.12345f);

  trader_account.cash_unit = 0.0f;
  EXPECT_FLOAT_EQ(0.12345f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(0.12345f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(0.12345f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.cash_unit = 0.01f;
  EXPECT_FLOAT_EQ(0.13f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(0.13f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(0.13f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 0.1f;
  EXPECT_FLOAT_EQ(0.2f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(0.2f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(0.2f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 1.0f;
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, RelativeAndFixedFee) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(0.1f);

  trader_account.cash_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.1f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.1f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.33456789f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.cash_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.34f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.45f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.56f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.4f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.5f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.6f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, trader_account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);
  fee_config.set_fixed_fee(1.0f);

  trader_account.cash_unit = 0.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.1f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.123456789f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.cash_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.13f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.24f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.35f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.2f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.3f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.4f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(3.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(14.0f, trader_account.GetFee(fee_config, 1234.56789f));
}

TEST(GetFeeTest, RelativeAndMinimumFee) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_minimum_fee(2.0f);

  trader_account.cash_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.cash_unit = 0.01f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.35f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.46f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 0.1f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.4f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.5f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, trader_account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);
  fee_config.set_minimum_fee(0.5f);

  trader_account.cash_unit = 0.0f;
  EXPECT_FLOAT_EQ(1.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(0.5f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(0.5f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.cash_unit = 0.01f;
  EXPECT_FLOAT_EQ(0.5f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.24f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.35f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 0.1f;
  EXPECT_FLOAT_EQ(0.5f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(1.3f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(12.4f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 1.0f;
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

  trader_account.cash_unit = 0.0f;
  EXPECT_FLOAT_EQ(10.1f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.cash_unit = 0.01f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.45f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.56f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 0.1f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(12.5f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(123.6f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 1.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(124.0f, trader_account.GetFee(fee_config, 1234.56789f));

  fee_config.set_relative_fee(0.01f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.cash_unit = 0.0f;
  EXPECT_FLOAT_EQ(2.0f, trader_account.GetFee(fee_config, 100.0f));
  EXPECT_FLOAT_EQ(1.5f, trader_account.GetFee(fee_config, 10.0f));
  EXPECT_FLOAT_EQ(1.5f, trader_account.GetFee(fee_config, 12.3456789f));

  trader_account.cash_unit = 0.01f;
  EXPECT_FLOAT_EQ(1.5f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.24f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.35f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 0.1f;
  EXPECT_FLOAT_EQ(1.5f, trader_account.GetFee(fee_config, 12.3456789f));
  EXPECT_FLOAT_EQ(2.3f, trader_account.GetFee(fee_config, 123.456789f));
  EXPECT_FLOAT_EQ(13.4f, trader_account.GetFee(fee_config, 1234.56789f));

  trader_account.cash_unit = 1.0f;
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

TEST(GetMaxSecurityAmountTest, Basic) {
  static constexpr float FLOAT_MAX = std::numeric_limits<float>::max();

  TraderAccount trader_account;
  OhlcTick ohlc_tick;
  SetupOhlcTick(&ohlc_tick);  // O = 10, H = 20, L = 2, C = 15, V = 1234.56

  trader_account.max_volume_ratio = 0;
  trader_account.security_unit = 0;
  EXPECT_FLOAT_EQ(FLOAT_MAX, trader_account.GetMaxSecurityAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0.1f;
  EXPECT_FLOAT_EQ(123.456f, trader_account.GetMaxSecurityAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0.01f;
  EXPECT_FLOAT_EQ(12.3456f, trader_account.GetMaxSecurityAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0;
  trader_account.security_unit = 0.1f;
  EXPECT_FLOAT_EQ(FLOAT_MAX, trader_account.GetMaxSecurityAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0.1f;
  EXPECT_FLOAT_EQ(123.4f, trader_account.GetMaxSecurityAmount(ohlc_tick));

  trader_account.max_volume_ratio = 0.01f;
  EXPECT_FLOAT_EQ(12.3f, trader_account.GetMaxSecurityAmount(ohlc_tick));
}

TEST(BuyTest, BuyWithoutFeeAndInfinitePrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  EXPECT_TRUE(trader_account.Buy(fee_config, /* price = */ 10.0f,
                                 /* security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(950.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_TRUE(trader_account.Buy(fee_config, /* price = */ 10.0f,
                                 /* security_amount = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 12.345f;
  trader_account.cash_balance = 123.456f;
  EXPECT_TRUE(trader_account.Buy(fee_config, /* price = */ 5.5f,
                                 /* security_amount = */ 12.345f));
  EXPECT_FLOAT_EQ(24.69f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(55.5585f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.Buy(fee_config, /* price = */ 10.0f,
                                  /* security_amount = */ 15.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyWithoutFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  EXPECT_TRUE(trader_account.Buy(fee_config, /* price = */ 10.0f,
                                 /* security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(950.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_TRUE(trader_account.Buy(fee_config, /* price = */ 10.0f,
                                 /* security_amount = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 12.3f;
  trader_account.cash_balance = 123.0f;
  EXPECT_TRUE(trader_account.Buy(fee_config, /* price = */ 5.5f,
                                 /* security_amount = */ 12.345f));
  EXPECT_FLOAT_EQ(24.6f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(55.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.Buy(fee_config, /* price = */ 10.0f,
                                  /* security_amount = */ 15.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.Buy(fee_config, /* price = */ 10.0f,
                                 /* security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(944.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 111.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.Buy(fee_config, /* price = */ 10.0f,
                                 /* security_amount = */ 10.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(21.0f, trader_account.total_fee);

  trader_account.security_balance = 12.3f;
  trader_account.cash_balance = 123.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.Buy(fee_config, /* price = */ 5.5f,
                                 /* security_amount = */ 12.345f));
  EXPECT_FLOAT_EQ(24.6f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(47.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(8.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.Buy(fee_config, /* price = */ 10.0f,
                                  /* security_amount = */ 15.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyAtCashWithoutFeeAndInfinitePrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 50.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(950.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 50.0f,
                                       /* max_security_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(12.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(980.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 100.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 100.0f,
                                       /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(50.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 12.345f;
  trader_account.cash_balance = 123.456f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 5.5f,
                                       /* cash_amount = */ 67.8975f));
  EXPECT_FLOAT_EQ(24.69f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(55.5585f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 12.345f;
  trader_account.cash_balance = 123.456f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 5.5f,
                                       /* cash_amount = */ 67.8975f,
                                       /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(17.345f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(95.956f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 1000.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 1000.0f,
                                        /* max_security_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyAtCashWithoutFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 50.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(950.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 50.0f,
                                       /* max_security_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(12.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(980.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 100.0f));
  EXPECT_FLOAT_EQ(20.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 100.0f,
                                       /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(50.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 12.3f;
  trader_account.cash_balance = 123.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 5.5f,
                                       /* cash_amount = */ 67.8975f));
  EXPECT_FLOAT_EQ(24.6f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(55.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 12.3f;
  trader_account.cash_balance = 123.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 5.5f,
                                       /* cash_amount = */ 67.8975f,
                                       /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(17.3f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(95.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 1000.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 1000.0f,
                                        /* max_security_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, BuyAtCashWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 57.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(944.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 1000.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 57.0f,
                                       /* max_security_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(12.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(977.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(3.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 111.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 111.0f));
  EXPECT_FLOAT_EQ(19.8f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(2.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(21.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 111.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                       /* cash_amount = */ 111.0f,
                                       /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(15.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(55.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(16.0f, trader_account.total_fee);

  trader_account.security_balance = 12.3f;
  trader_account.cash_balance = 123.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 5.5f,
                                       /* cash_amount = */ 76.123f));
  EXPECT_FLOAT_EQ(24.4f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(48.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(8.0f, trader_account.total_fee);

  trader_account.security_balance = 12.3f;
  trader_account.cash_balance = 123.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.BuyAtCash(fee_config, /* price = */ 5.5f,
                                       /* cash_amount = */ 76.123f,
                                       /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(17.3f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(91.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(4.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 1000.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.BuyAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 1000.0f,
                                        /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);
}

TEST(BuyTest, SellWithoutFeeAndInfinitePrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  EXPECT_TRUE(trader_account.Sell(fee_config, /* price = */ 10.0f,
                                  /* security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 20.0f;
  trader_account.cash_balance = 0.0f;
  EXPECT_TRUE(trader_account.Sell(fee_config, /* price = */ 10.0f,
                                  /* security_amount = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 24.69f;
  trader_account.cash_balance = 55.5585f;
  EXPECT_TRUE(trader_account.Sell(fee_config, /* price = */ 5.5f,
                                  /* security_amount = */ 12.345f));
  EXPECT_FLOAT_EQ(12.345f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(123.456f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.Sell(fee_config, /* price = */ 10.0f,
                                   /* security_amount = */ 15.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, SellWithoutFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  EXPECT_TRUE(trader_account.Sell(fee_config, /* price = */ 10.0f,
                                  /* security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 20.0f;
  trader_account.cash_balance = 0.0f;
  EXPECT_TRUE(trader_account.Sell(fee_config, /* price = */ 10.0f,
                                  /* security_amount = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 24.6f;
  trader_account.cash_balance = 55.0f;
  EXPECT_TRUE(trader_account.Sell(fee_config, /* price = */ 5.5f,
                                  /* security_amount = */ 12.345f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(122.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.Sell(fee_config, /* price = */ 10.0f,
                                   /* security_amount = */ 15.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, SellWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.Sell(fee_config, /* price = */ 10.0f,
                                  /* security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(994.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(6.0f, trader_account.total_fee);

  trader_account.security_balance = 20.0f;
  trader_account.cash_balance = 0.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.Sell(fee_config, /* price = */ 10.0f,
                                  /* security_amount = */ 10.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(89.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(21.0f, trader_account.total_fee);

  trader_account.security_balance = 24.6f;
  trader_account.cash_balance = 47.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.Sell(fee_config, /* price = */ 5.5f,
                                  /* security_amount = */ 12.345f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(106.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(8.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.Sell(fee_config, /* price = */ 10.0f,
                                   /* security_amount = */ 15.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);
}

TEST(BuyTest, SellAtCashWithoutFeeAndInfinitePrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 50.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 50.0f,
                                        /* max_security_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(13.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(970.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 20.0f;
  trader_account.cash_balance = 0.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 100.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 50.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 100.0f,
                                        /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 24.69f;
  trader_account.cash_balance = 55.5585f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 5.5f,
                                        /* cash_amount = */ 67.8975f));
  EXPECT_FLOAT_EQ(12.345f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(123.456f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 17.345f;
  trader_account.cash_balance = 95.956f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 5.5f,
                                        /* cash_amount = */ 67.8975f,
                                        /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(12.345f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(123.456f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                         /* cash_amount = */ 1000.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                         /* cash_amount = */ 1000.0f,
                                         /* max_security_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, SellAtCashWithoutFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 50.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 12.0f;
  trader_account.cash_balance = 980.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 50.0f,
                                        /* max_security_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(1000.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 20.0f;
  trader_account.cash_balance = 0.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 100.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 50.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 100.0f,
                                        /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 24.6f;
  trader_account.cash_balance = 55.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 5.5f,
                                        /* cash_amount = */ 67.8975f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(122.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 17.3f;
  trader_account.cash_balance = 95.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 5.5f,
                                        /* cash_amount = */ 67.8975f,
                                        /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(122.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                         /* cash_amount = */ 1000.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  EXPECT_FALSE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                         /* cash_amount = */ 1000.0f,
                                         /* max_security_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(0.0f, trader_account.total_fee);
}

TEST(BuyTest, SellAtCashWithFeeAndLimitedPrecision) {
  TraderAccount trader_account;
  FeeConfig fee_config;

  fee_config.set_relative_fee(0.1f);
  fee_config.set_fixed_fee(1.0f);
  fee_config.set_minimum_fee(1.5f);

  trader_account.security_unit = 0.1f;
  trader_account.cash_unit = 1.0f;

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 950.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 50.0f));
  EXPECT_FLOAT_EQ(9.4f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(999.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(7.0f, trader_account.total_fee);

  trader_account.security_balance = 12.0f;
  trader_account.cash_balance = 980.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 50.0f,
                                        /* max_security_amount = */ 2.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(997.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(3.0f, trader_account.total_fee);

  trader_account.security_balance = 20.0f;
  trader_account.cash_balance = 0.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 100.0f));
  EXPECT_FLOAT_EQ(8.9f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(98.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(23.0f, trader_account.total_fee);

  trader_account.security_balance = 15.0f;
  trader_account.cash_balance = 50.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                        /* cash_amount = */ 100.0f,
                                        /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(94.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(16.0f, trader_account.total_fee);

  trader_account.security_balance = 24.6f;
  trader_account.cash_balance = 55.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 5.5f,
                                        /* cash_amount = */ 67.8975f));
  EXPECT_FLOAT_EQ(10.8f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(121.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(9.0f, trader_account.total_fee);

  trader_account.security_balance = 17.3f;
  trader_account.cash_balance = 95.0f;
  trader_account.total_fee = 0.0f;
  EXPECT_TRUE(trader_account.SellAtCash(fee_config, /* price = */ 5.5f,
                                        /* cash_amount = */ 67.8975f,
                                        /* max_security_amount = */ 5.0f));
  EXPECT_FLOAT_EQ(12.3f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(118.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(4.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                         /* cash_amount = */ 1000.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);

  trader_account.security_balance = 10.0f;
  trader_account.cash_balance = 100.0f;
  trader_account.total_fee = 10.0f;
  EXPECT_FALSE(trader_account.SellAtCash(fee_config, /* price = */ 10.0f,
                                         /* cash_amount = */ 1000.0f,
                                         /* max_security_amount = */ 20.0f));
  EXPECT_FLOAT_EQ(10.0f, trader_account.security_balance);
  EXPECT_FLOAT_EQ(100.0f, trader_account.cash_balance);
  EXPECT_FLOAT_EQ(10.0f, trader_account.total_fee);
}

}  // namespace trader
