// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/simple_moving_average.h"

#include "gtest/gtest.h"
#include "indicators/test_util.h"

namespace trader {
using ::trader::testing::PrepareExampleOhlcHistory;

TEST(SimpleMovingAverageTest, Get3SMAWhenAdding8HourOhlcTicks) {
  OhlcHistory ohlc_history;
  PrepareExampleOhlcHistory(ohlc_history);

  SimpleMovingAverage simple_moving_average(/*num_ohlc_ticks=*/3,
                                            /*period_size_sec=*/kSecondsPerDay);

  EXPECT_FLOAT_EQ(0, simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(0, simple_moving_average.GetNumOhlcTicks());

  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 00:00
  // --- Daily History ---
  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 (Day 1)
  simple_moving_average.Update(ohlc_history[0]);
  EXPECT_FLOAT_EQ(120, simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(1, simple_moving_average.GetNumOhlcTicks());

  // O: 120  H: 180  L: 100  C: 150  V: 1000  T: 2017-01-01 08:00
  // --- Daily History ---
  // O: 100  H: 180  L:  80  C: 150  V: 2000  T: 2017-01-01 (Day 1)
  simple_moving_average.Update(ohlc_history[1]);
  EXPECT_FLOAT_EQ(150, simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(1, simple_moving_average.GetNumOhlcTicks());

  // O: 150  H: 250  L: 100  C: 140  V: 1000  T: 2017-01-01 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  simple_moving_average.Update(ohlc_history[2]);
  EXPECT_FLOAT_EQ(140, simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(1, simple_moving_average.GetNumOhlcTicks());

  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 (Day 2)
  simple_moving_average.Update(ohlc_history[3]);
  EXPECT_FLOAT_EQ((140.0f + 100.0f) / 2.0f,
                  simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(2, simple_moving_average.GetNumOhlcTicks());

  // O: 100  H: 120  L:  20  C:  50  V: 1000  T: 2017-01-02 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  50  V: 2000  T: 2017-01-02 (Day 2)
  simple_moving_average.Update(ohlc_history[4]);
  EXPECT_FLOAT_EQ((140.0f + 50.0f) / 2.0f,
                  simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(2, simple_moving_average.GetNumOhlcTicks());

  // O:  50  H: 100  L:  40  C:  80  V: 1000  T: 2017-01-02 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  simple_moving_average.Update(ohlc_history[5]);
  EXPECT_FLOAT_EQ((140.0f + 80.0f) / 2.0f,
                  simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(2, simple_moving_average.GetNumOhlcTicks());

  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 (Day 3)
  simple_moving_average.Update(ohlc_history[6]);
  EXPECT_FLOAT_EQ((140.0f + 80.0f + 150.0f) / 3.0f,
                  simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(3, simple_moving_average.GetNumOhlcTicks());

  // O: 150  H: 250  L: 120  C: 240  V: 1000  T: 2017-01-03 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 250  L:  50  C: 240  V: 2000  T: 2017-01-03 (Day 3)
  simple_moving_average.Update(ohlc_history[7]);
  EXPECT_FLOAT_EQ((140.0f + 80.0f + 240.0f) / 3.0f,
                  simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(3, simple_moving_average.GetNumOhlcTicks());

  // O: 240  H: 450  L: 220  C: 400  V: 1000  T: 2017-01-03 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  simple_moving_average.Update(ohlc_history[8]);
  EXPECT_FLOAT_EQ((140.0f + 80.0f + 400.0f) / 3.0f,
                  simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(3, simple_moving_average.GetNumOhlcTicks());

  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 (Day 4)
  simple_moving_average.Update(ohlc_history[9]);
  EXPECT_FLOAT_EQ((80.0f + 400.0f + 300.0f) / 3.0f,
                  simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(3, simple_moving_average.GetNumOhlcTicks());

  // O: 300  H: 700  L: 220  C: 650  V: 1000  T: 2017-01-04 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 700  L: 220  C: 650  V: 2000  T: 2017-01-04 (Day 4)
  simple_moving_average.Update(ohlc_history[10]);
  EXPECT_FLOAT_EQ((80.0f + 400.0f + 650.0f) / 3.0f,
                  simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(3, simple_moving_average.GetNumOhlcTicks());

  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-04 16:00
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-05 00:00 (+1 Day)
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-05 08:00
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-05 16:00
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-06 00:00 (+1 Day)
  // O: 650  H: 800  L: 600  C: 750  V: 1000  T: 2017-01-06 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 700  L: 220  C: 650  V: 2000  T: 2017-01-04 (Day 4)
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-05 (Day 5)
  // O: 650  H: 800  L: 600  C: 750  V: 1000  T: 2017-01-06 (Day 6)
  simple_moving_average.Update(ohlc_history[11]);
  EXPECT_FLOAT_EQ((650.0f + 650.0f + 750.0f) / 3.0f,
                  simple_moving_average.GetSimpleMovingAverage());
  EXPECT_EQ(3, simple_moving_average.GetNumOhlcTicks());
}

}  // namespace trader
