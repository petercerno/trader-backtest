// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef INDICATORS_TEST_UTIL_H
#define INDICATORS_TEST_UTIL_H

#include "gtest/gtest.h"
#include "lib/trader_base.h"

namespace trader {
namespace testing {

// Prepares the following OHLC history:
// O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 00:00
// O: 120  H: 180  L: 100  C: 150  V: 1000  T: 2017-01-01 08:00
// O: 150  H: 250  L: 100  C: 140  V: 1000  T: 2017-01-01 16:00
// O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 00:00 (+1 Day)
// O: 100  H: 120  L:  20  C:  50  V: 1000  T: 2017-01-02 08:00
// O:  50  H: 100  L:  40  C:  80  V: 1000  T: 2017-01-02 16:00
// O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 00:00 (+1 Day)
// O: 150  H: 250  L: 120  C: 240  V: 1000  T: 2017-01-03 08:00
// O: 240  H: 450  L: 220  C: 400  V: 1000  T: 2017-01-03 16:00
// O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 00:00 (+1 Day)
// O: 300  H: 700  L: 220  C: 650  V: 1000  T: 2017-01-04 08:00
// --- Gap --
// O: 650  H: 800  L: 600  C: 750  V: 1000  T: 2017-01-06 08:00
void PrepareExampleOhlcHistory(OhlcHistory& ohlc_history);

}  // namespace testing
}  // namespace trader

#endif  // INDICATORS_TEST_UTIL_H
