// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/moving_average_convergence_divergence.h"

#include "gtest/gtest.h"
#include "indicators/test_util.h"

namespace trader {
using ::trader::testing::PrepareExampleOhlcHistory;

TEST(MovingAverageConvergenceDivergenceTest, GetMACDWhenAdding8HourOhlcTicks) {
  OhlcHistory ohlc_history;
  PrepareExampleOhlcHistory(ohlc_history);

  MovingAverageConvergenceDivergence macd(
      /*fast_length=*/4,
      /*slow_length=*/7,
      /*signal_smoothing=*/3,
      /*period_size_sec=*/kSecondsPerDay);
  EXPECT_FLOAT_EQ(0, macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(0, macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ(0, macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(0, macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(0, macd.GetDivergence());
  EXPECT_EQ(0, macd.GetNumOhlcTicks());

  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 00:00
  // --- Daily History ---
  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 (Day 1)
  macd.Update(ohlc_history[0]);
  EXPECT_FLOAT_EQ(120, macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(120, macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ(0, macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(0, macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(0, macd.GetDivergence());
  EXPECT_EQ(1, macd.GetNumOhlcTicks());

  // O: 120  H: 180  L: 100  C: 150  V: 1000  T: 2017-01-01 08:00
  // --- Daily History ---
  // O: 100  H: 180  L:  80  C: 150  V: 2000  T: 2017-01-01 (Day 1)
  macd.Update(ohlc_history[1]);
  EXPECT_FLOAT_EQ(150, macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(150, macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ(0, macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(0, macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(0, macd.GetDivergence());
  EXPECT_EQ(1, macd.GetNumOhlcTicks());

  // O: 150  H: 250  L: 100  C: 140  V: 1000  T: 2017-01-01 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  macd.Update(ohlc_history[2]);
  EXPECT_FLOAT_EQ(140, macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(140, macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ(0, macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(0, macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(0, macd.GetDivergence());
  EXPECT_EQ(1, macd.GetNumOhlcTicks());

  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 (Day 2)
  macd.Update(ohlc_history[3]);
  EXPECT_FLOAT_EQ(
      140.0f * (1.0f - 2.0f / (1.0f + 4.0f)) + 100.0f * 2.0f / (1.0f + 4.0f),
      macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(
      140.0f * (1.0f - 2.0f / (1.0f + 7.0f)) + 100.0f * 2.0f / (1.0f + 7.0f),
      macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ(
      (140.0f * (1.0f - 2.0f / (1.0f + 4.0f)) + 100.0f * 2.0f / (1.0f + 4.0f)) -
          (140.0f * (1.0f - 2.0f / (1.0f + 7.0f)) +
           100.0f * 2.0f / (1.0f + 7.0f)),
      macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(0.0f * (1.0f - 2.0f / (1.0f + 3.0f)) +
                      ((140.0f * (1.0f - 2.0f / (1.0f + 4.0f)) +
                        100.0f * 2.0f / (1.0f + 4.0f)) -
                       (140.0f * (1.0f - 2.0f / (1.0f + 7.0f)) +
                        100.0f * 2.0f / (1.0f + 7.0f))) *
                          2.0f / (1.0f + 3.0f),
                  macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(((140.0f * (1.0f - 2.0f / (1.0f + 4.0f)) +
                    100.0f * 2.0f / (1.0f + 4.0f)) -
                   (140.0f * (1.0f - 2.0f / (1.0f + 7.0f)) +
                    100.0f * 2.0f / (1.0f + 7.0f))) -
                      (0.0f * (1.0f - 2.0f / (1.0f + 3.0f)) +
                       ((140.0f * (1.0f - 2.0f / (1.0f + 4.0f)) +
                         100.0f * 2.0f / (1.0f + 4.0f)) -
                        (140.0f * (1.0f - 2.0f / (1.0f + 7.0f)) +
                         100.0f * 2.0f / (1.0f + 7.0f))) *
                           2.0f / (1.0f + 3.0f)),
                  macd.GetDivergence());
  EXPECT_EQ(2, macd.GetNumOhlcTicks());

  // O: 100  H: 120  L:  20  C:  50  V: 1000  T: 2017-01-02 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  50  V: 2000  T: 2017-01-02 (Day 2)
  macd.Update(ohlc_history[4]);
  EXPECT_FLOAT_EQ(140.0f * (1.0f - 0.4f) + 50.0f * 0.4f,
                  macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(140.0f * (1.0f - 0.25f) + 50.0f * 0.25f,
                  macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ((140.0f * (1.0f - 0.4f) + 50.0f * 0.4f) -
                      (140.0f * (1.0f - 0.25f) + 50.0f * 0.25f),
                  macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(0.0f * 0.5f + ((140.0f * (1.0f - 0.4f) + 50.0f * 0.4f) -
                                 (140.0f * (1.0f - 0.25f) + 50.0f * 0.25f)) *
                                    0.5f,
                  macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(
      ((140.0f * (1.0f - 0.4f) + 50.0f * 0.4f) -
       (140.0f * (1.0f - 0.25f) + 50.0f * 0.25f)) -
          (0.0f * 0.5f + ((140.0f * (1.0f - 0.4f) + 50.0f * 0.4f) -
                          (140.0f * (1.0f - 0.25f) + 50.0f * 0.25f)) *
                             0.5f),
      macd.GetDivergence());
  EXPECT_EQ(2, macd.GetNumOhlcTicks());

  // O:  50  H: 100  L:  40  C:  80  V: 1000  T: 2017-01-02 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  macd.Update(ohlc_history[5]);
  EXPECT_FLOAT_EQ(140.0f * (1.0f - 0.4f) + 80.0f * 0.4f,
                  macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(140.0f * (1.0f - 0.25f) + 80.0f * 0.25f,
                  macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ((140.0f * (1.0f - 0.4f) + 80.0f * 0.4f) -
                      (140.0f * (1.0f - 0.25f) + 80.0f * 0.25f),
                  macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(0.0f * 0.5f + ((140.0f * (1.0f - 0.4f) + 80.0f * 0.4f) -
                                 (140.0f * (1.0f - 0.25f) + 80.0f * 0.25f)) *
                                    0.5f,
                  macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(macd.GetMACDSeries() - macd.GetMACDSignal(),
                  macd.GetDivergence());
  EXPECT_EQ(2, macd.GetNumOhlcTicks());

  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 (Day 3)
  macd.Update(ohlc_history[6]);
  EXPECT_FLOAT_EQ(116.0f * (1.0f - 0.4f) + 150.0f * 0.4f,
                  macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(125.0f * (1.0f - 0.25f) + 150.0f * 0.25f,
                  macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ((116.0f * (1.0f - 0.4f) + 150.0f * 0.4f) -
                      (125.0f * (1.0f - 0.25f) + 150.0f * 0.25f),
                  macd.GetMACDSeries());
  EXPECT_FLOAT_EQ((0.0f * 0.5f + (116.0f - 125.0f) * 0.5f) * 0.5f +
                      ((116.0f * (1.0f - 0.4f) + 150.0f * 0.4f) -
                       (125.0f * (1.0f - 0.25f) + 150.0f * 0.25f)) *
                          0.5f,
                  macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(macd.GetMACDSeries() - macd.GetMACDSignal(),
                  macd.GetDivergence());
  EXPECT_EQ(3, macd.GetNumOhlcTicks());

  // O: 150  H: 250  L: 120  C: 240  V: 1000  T: 2017-01-03 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 250  L:  50  C: 240  V: 2000  T: 2017-01-03 (Day 3)
  macd.Update(ohlc_history[7]);
  EXPECT_FLOAT_EQ(116.0f * (1.0f - 0.4f) + 240.0f * 0.4f,
                  macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(125.0f * (1.0f - 0.25f) + 240.0f * 0.25f,
                  macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ((116.0f * (1.0f - 0.4f) + 240.0f * 0.4f) -
                      (125.0f * (1.0f - 0.25f) + 240.0f * 0.25f),
                  macd.GetMACDSeries());
  EXPECT_FLOAT_EQ((0.0f * 0.5f + (116.0f - 125.0f) * 0.5f) * 0.5f +
                      ((116.0f * (1.0f - 0.4f) + 240.0f * 0.4f) -
                       (125.0f * (1.0f - 0.25f) + 240.0f * 0.25f)) *
                          0.5f,
                  macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(macd.GetMACDSeries() - macd.GetMACDSignal(),
                  macd.GetDivergence());
  EXPECT_EQ(3, macd.GetNumOhlcTicks());

  // O: 240  H: 450  L: 220  C: 400  V: 1000  T: 2017-01-03 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  macd.Update(ohlc_history[8]);
  EXPECT_FLOAT_EQ(116.0f * (1.0f - 0.4f) + 400.0f * 0.4f,
                  macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(125.0f * (1.0f - 0.25f) + 400.0f * 0.25f,
                  macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ((116.0f * (1.0f - 0.4f) + 400.0f * 0.4f) -
                      (125.0f * (1.0f - 0.25f) + 400.0f * 0.25f),
                  macd.GetMACDSeries());
  EXPECT_FLOAT_EQ((0.0f * 0.5f + (116.0f - 125.0f) * 0.5f) * 0.5f +
                      ((116.0f * (1.0f - 0.4f) + 400.0f * 0.4f) -
                       (125.0f * (1.0f - 0.25f) + 400.0f * 0.25f)) *
                          0.5f,
                  macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(macd.GetMACDSeries() - macd.GetMACDSignal(),
                  macd.GetDivergence());
  EXPECT_EQ(3, macd.GetNumOhlcTicks());

  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 (Day 4)
  macd.Update(ohlc_history[9]);
  EXPECT_FLOAT_EQ(229.6f * (1.0f - 0.4f) + 300.0f * 0.4f,
                  macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(193.75f * (1.0f - 0.25f) + 300.0f * 0.25f,
                  macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ((229.6f * (1.0f - 0.4f) + 300.0f * 0.4f) -
                      (193.75f * (1.0f - 0.25f) + 300.0f * 0.25f),
                  macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(((0.0f * 0.5f + (116.0f - 125.0f) * 0.5f) * 0.5f +
                   ((116.0f * (1.0f - 0.4f) + 400.0f * 0.4f) -
                    (125.0f * (1.0f - 0.25f) + 400.0f * 0.25f)) *
                       0.5f) *
                          0.5f +
                      ((229.6f * (1.0f - 0.4f) + 300.0f * 0.4f) -
                       (193.75f * (1.0f - 0.25f) + 300.0f * 0.25f)) *
                          0.5f,
                  macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(macd.GetMACDSeries() - macd.GetMACDSignal(),
                  macd.GetDivergence());
  EXPECT_EQ(4, macd.GetNumOhlcTicks());

  // O: 300  H: 700  L: 220  C: 650  V: 1000  T: 2017-01-04 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 700  L: 220  C: 650  V: 2000  T: 2017-01-04 (Day 4)
  macd.Update(ohlc_history[10]);
  EXPECT_FLOAT_EQ(229.6f * (1.0f - 0.4f) + 650.0f * 0.4f,
                  macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(193.75f * (1.0f - 0.25f) + 650.0f * 0.25f,
                  macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ((229.6f * (1.0f - 0.4f) + 650.0f * 0.4f) -
                      (193.75f * (1.0f - 0.25f) + 650.0f * 0.25f),
                  macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(((0.0f * 0.5f + (116.0f - 125.0f) * 0.5f) * 0.5f +
                   ((116.0f * (1.0f - 0.4f) + 400.0f * 0.4f) -
                    (125.0f * (1.0f - 0.25f) + 400.0f * 0.25f)) *
                       0.5f) *
                          0.5f +
                      ((229.6f * (1.0f - 0.4f) + 650.0f * 0.4f) -
                       (193.75f * (1.0f - 0.25f) + 650.0f * 0.25f)) *
                          0.5f,
                  macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(macd.GetMACDSeries() - macd.GetMACDSignal(),
                  macd.GetDivergence());
  EXPECT_EQ(4, macd.GetNumOhlcTicks());

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
  macd.Update(ohlc_history[11]);
  EXPECT_FLOAT_EQ(498.656f * (1.0f - 0.4f) + 750.0f * 0.4f,
                  macd.GetFastExponentialMovingAverage());
  EXPECT_FLOAT_EQ(393.359375f * (1.0f - 0.25f) + 750.0f * 0.25f,
                  macd.GetSlowExponentialMovingAverage());
  EXPECT_FLOAT_EQ((498.656f * (1.0f - 0.4f) + 750.0f * 0.4f) -
                      (393.359375f * (1.0f - 0.25f) + 750.0f * 0.25f),
                  macd.GetMACDSeries());
  EXPECT_FLOAT_EQ(((((0.0f * 0.5f + (116.0f - 125.0f) * 0.5f) * 0.5f +
                     ((116.0f * (1.0f - 0.4f) + 400.0f * 0.4f) -
                      (125.0f * (1.0f - 0.25f) + 400.0f * 0.25f)) *
                         0.5f) *
                        0.5f +
                    ((229.6f * (1.0f - 0.4f) + 650.0f * 0.4f) -
                     (193.75f * (1.0f - 0.25f) + 650.0f * 0.25f)) *
                        0.5f) *
                       0.5f +
                   ((397.76f * (1.0f - 0.4f) + 650.0f * 0.4f) -
                    (307.8125f * (1.0f - 0.25f) + 650.0f * 0.25f)) *
                       0.5f) *
                          0.5f +
                      ((498.656f * (1.0f - 0.4f) + 750.0f * 0.4f) -
                       (393.359375f * (1.0f - 0.25f) + 750.0f * 0.25f)) *
                          0.5f,
                  macd.GetMACDSignal());
  EXPECT_FLOAT_EQ(macd.GetMACDSeries() - macd.GetMACDSignal(),
                  macd.GetDivergence());
  EXPECT_EQ(6, macd.GetNumOhlcTicks());
}

}  // namespace trader
