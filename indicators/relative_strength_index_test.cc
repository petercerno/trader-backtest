// Copyright © 2021 Peter Cerno. All rights reserved.

#include "indicators/relative_strength_index.h"

#include "gtest/gtest.h"
#include "indicators/test_util.h"

namespace trader {
using ::trader::testing::PrepareExampleOhlcHistory;

TEST(RelativeStrengthIndexTest, GetRSIWhenAdding8HourOhlcTicks) {
  OhlcHistory ohlc_history;
  PrepareExampleOhlcHistory(ohlc_history);

  RelativeStrengthIndex relative_strength_index(
      /*num_periods=*/3,
      /*period_size_sec=*/kSecondsPerDay);

  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(), 0.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(), 0.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(), 50.0f);
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 0);

  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 00:00
  // --- Daily History ---
  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 (Day 1)
  relative_strength_index.Update(ohlc_history[0]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(), 20.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(), 0.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(), 100.0f);
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 1);

  // O: 120  H: 180  L: 100  C: 150  V: 1000  T: 2017-01-01 08:00
  // --- Daily History ---
  // O: 100  H: 180  L:  80  C: 150  V: 2000  T: 2017-01-01 (Day 1)
  relative_strength_index.Update(ohlc_history[1]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(), 50.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(), 0.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(), 100.0f);
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 1);

  // O: 150  H: 250  L: 100  C: 140  V: 1000  T: 2017-01-01 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  relative_strength_index.Update(ohlc_history[2]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(), 40.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(), 0.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(), 100.0f);
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 1);

  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 (Day 2)
  relative_strength_index.Update(ohlc_history[3]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(),
      (40.0f + 0.0f) / 2.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(),
      (0.0f + 40.0f) / 2.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(),
                  100.0f - 100.0f / (1.0f + 20.0f / 20.0f));
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 2);

  // O: 100  H: 120  L:  20  C:  50  V: 1000  T: 2017-01-02 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  50  V: 2000  T: 2017-01-02 (Day 2)
  relative_strength_index.Update(ohlc_history[4]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(),
      (40.0f + 0.0f) / 2.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(),
      (0.0f + 90.0f) / 2.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(),
                  100.0f - 100.0f / (1.0f + 20.0f / 45.0f));
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 2);

  // O:  50  H: 100  L:  40  C:  80  V: 1000  T: 2017-01-02 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  relative_strength_index.Update(ohlc_history[5]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(),
      (40.0f + 0.0f) / 2.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(),
      (0.0f + 60.0f) / 2.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(),
                  100.0f - 100.0f / (1.0f + 20.0f / 30.0f));
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 2);

  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 (Day 3)
  relative_strength_index.Update(ohlc_history[6]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(),
      (40.0f + 0.0f + 70.0f) / 3.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(),
      (0.0f + 60.0 + 0.0f) / 3.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(),
                  100.0f - 100.0f / (1.0f + 110.0f / 60.0f));
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 3);

  // O: 150  H: 250  L: 120  C: 240  V: 1000  T: 2017-01-03 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 250  L:  50  C: 240  V: 2000  T: 2017-01-03 (Day 3)
  relative_strength_index.Update(ohlc_history[7]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(),
      (40.0f + 0.0f + 160.0f) / 3.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(),
      (0.0f + 60.0 + 0.0f) / 3.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(),
                  100.0f - 100.0f / (1.0f + 200.0f / 60.0f));
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 3);

  // O: 240  H: 450  L: 220  C: 400  V: 1000  T: 2017-01-03 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  relative_strength_index.Update(ohlc_history[8]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(),
      (40.0f + 0.0f + 320.0f) / 3.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(),
      (0.0f + 60.0 + 0.0f) / 3.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(),
                  100.0f - 100.0f / (1.0f + 360.0f / 60.0f));
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 3);

  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 (Day 4)
  relative_strength_index.Update(ohlc_history[9]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(),
      (1.0f - 1.0f / 3.0f) * (40.0f + 0.0f + 320.0f) / 3.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(),
      (1.0f - 1.0f / 3.0f) * (0.0f + 60.0 + 0.0f) / 3.0f +
          (1.0f / 3.0f) * 100.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetRelativeStrengthIndex(),
      100.0f - 100.0f / (1.0f + 80.0f / (40.0f / 3.0f + 100.0f / 3.0f)));
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 4);

  // O: 300  H: 700  L: 220  C: 650  V: 1000  T: 2017-01-04 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 700  L: 220  C: 650  V: 2000  T: 2017-01-04 (Day 4)
  relative_strength_index.Update(ohlc_history[10]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(),
      80.0f + (1.0f / 3.0f) * 250.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(),
      40.0f / 3.0f);
  EXPECT_FLOAT_EQ(relative_strength_index.GetRelativeStrengthIndex(),
                  100.0f - 100.0f / (1.0f + (80.0f + (1.0f / 3.0f) * 250.0f) /
                                                (40.0f / 3.0f)));
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 4);

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
  relative_strength_index.Update(ohlc_history[11]);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetUpwardChangeModifiedMovingAverage(),
      (1.0f - 1.0f / 3.0f) * (1.0f - 1.0f / 3.0f) *
              (80.0f + (1.0f / 3.0f) * 250.0f) +
          (1.0f / 3.0f) * 100.0f);
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetDownwardChangeModifiedMovingAverage(),
      (1.0f - 1.0f / 3.0f) * (1.0f - 1.0f / 3.0f) * (40.0f / 3.0f));
  EXPECT_FLOAT_EQ(
      relative_strength_index.GetRelativeStrengthIndex(),
      100.0f - 100.0f / (1.0f + ((1.0f - 1.0f / 3.0f) * (1.0f - 1.0f / 3.0f) *
                                     (80.0f + (1.0f / 3.0f) * 250.0f) +
                                 (1.0f / 3.0f) * 100.0f) /
                                    ((1.0f - 1.0f / 3.0f) *
                                     (1.0f - 1.0f / 3.0f) * (40.0f / 3.0f))));
  EXPECT_EQ(relative_strength_index.GetNumOhlcTicks(), 6);
}

}  // namespace trader
