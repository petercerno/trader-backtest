// Copyright © 2019 Peter Cerno. All rights reserved.

#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include "trader_base.h"

namespace trader {
namespace testing {

// Adds PriceRecord to the price_history.
void AddPriceRecord(int timestamp_sec, float price, float volume,
                    PriceHistory* price_history);

// Compares the expected PriceRecord with the actual PriceRecord.
void ExpectNearPriceRecord(const PriceRecord& expected,
                           const PriceRecord& actual);

// Compares the expected values with the actual OhlcTick.
void ExpectNearOhlcTick(int timestamp_sec, float open, float high, float low,
                        float close, float volume,
                        const OhlcTick& actual_ohlc_tick);

}  // namespace testing
}  // namespace trader

#endif  // UTIL_TEST_H
