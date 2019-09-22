// Copyright © 2019 Peter Cerno. All rights reserved.

#include "trader_base.h"

#include "gtest/gtest.h"

namespace trader {
namespace testing {
constexpr float kEpsilon = 1.0e-6f;

void AddPriceRecord(int timestamp_sec, float price, float volume,
                    PriceHistory* price_history) {
  price_history->emplace_back();
  price_history->back().set_timestamp_sec(timestamp_sec);
  price_history->back().set_price(price);
  price_history->back().set_volume(volume);
}

void ExpectNearPriceRecord(const PriceRecord& expected,
                           const PriceRecord& actual) {
  EXPECT_EQ(expected.timestamp_sec(), actual.timestamp_sec());
  EXPECT_NEAR(expected.price(), actual.price(), kEpsilon);
  EXPECT_NEAR(expected.volume(), actual.volume(), kEpsilon);
}

void ExpectNearOhlcTick(int timestamp_sec, float open, float high, float low,
                        float close, float volume,
                        const OhlcTick& actual_ohlc_tick) {
  EXPECT_EQ(timestamp_sec, actual_ohlc_tick.timestamp_sec());
  EXPECT_NEAR(open, actual_ohlc_tick.open(), kEpsilon);
  EXPECT_NEAR(high, actual_ohlc_tick.high(), kEpsilon);
  EXPECT_NEAR(low, actual_ohlc_tick.low(), kEpsilon);
  EXPECT_NEAR(close, actual_ohlc_tick.close(), kEpsilon);
  EXPECT_NEAR(volume, actual_ohlc_tick.volume(), kEpsilon);
}

}  // namespace testing
}  // namespace trader
