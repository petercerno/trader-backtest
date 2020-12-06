// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/stochastic_oscillator.h"

#include "gtest/gtest.h"

namespace trader {
namespace {
static constexpr int kSecondsPer8Hours = 8 * 60 * 60;

// Adds OHLC tick to the history. Does not set the timestamp_sec.
void AddOhlcTick(float open, float high, float low, float close,
                 OhlcHistory* ohlc_history) {
  ASSERT_LE(low, open);
  ASSERT_LE(low, high);
  ASSERT_LE(low, close);
  ASSERT_GE(high, open);
  ASSERT_GE(high, close);
  ohlc_history->emplace_back();
  OhlcTick* ohlc_tick = &ohlc_history->back();
  ohlc_tick->set_open(open);
  ohlc_tick->set_high(high);
  ohlc_tick->set_low(low);
  ohlc_tick->set_close(close);
  ohlc_tick->set_volume(1000.0f);
}

// Adds OHLC tick to the history period_sec away from the last OHLC tick.
void AddOhlcTickWithPeriod(float open, float high, float low, float close,
                           int period_sec, OhlcHistory* ohlc_history) {
  int timestamp_sec = 1483228800;  // 2017-01-01
  if (!ohlc_history->empty()) {
    ASSERT_FLOAT_EQ(ohlc_history->back().close(), open);
    timestamp_sec = ohlc_history->back().timestamp_sec() + period_sec;
  }
  AddOhlcTick(open, high, low, close, ohlc_history);
  ohlc_history->back().set_timestamp_sec(timestamp_sec);
}

void Add8HourOhlcTick(float open, float high, float low, float close,
                      OhlcHistory* ohlc_history) {
  AddOhlcTickWithPeriod(open, high, low, close, kSecondsPer8Hours,
                        ohlc_history);
}
}  // namespace

TEST(StochasticOscillatorTest, GetSOWhenAdding8HourOhlcTicks) {
  OhlcHistory ohlc_history;
  Add8HourOhlcTick(100, 150, 80, 120, &ohlc_history);  // 2017-01-01
  Add8HourOhlcTick(120, 180, 100, 150, &ohlc_history);
  Add8HourOhlcTick(150, 250, 100, 140, &ohlc_history);
  Add8HourOhlcTick(140, 150, 80, 100, &ohlc_history);  // 2017-01-02
  Add8HourOhlcTick(100, 120, 20, 50, &ohlc_history);
  Add8HourOhlcTick(50, 100, 40, 80, &ohlc_history);
  Add8HourOhlcTick(80, 180, 50, 150, &ohlc_history);  // 2017-01-03
  Add8HourOhlcTick(150, 250, 120, 240, &ohlc_history);
  Add8HourOhlcTick(240, 450, 220, 400, &ohlc_history);
  Add8HourOhlcTick(400, 450, 250, 300, &ohlc_history);  // 2017-01-04
  Add8HourOhlcTick(300, 700, 220, 650, &ohlc_history);
  Add8HourOhlcTick(650, 800, 600, 750, &ohlc_history);  // 2017-01-06 08:00
  ohlc_history.back().set_timestamp_sec(ohlc_history.back().timestamp_sec() +
                                        1 * kSecondsPerDay +
                                        2 * kSecondsPer8Hours);

  StochasticOscillator stochastic_oscillator(
      /*num_periods=*/2,
      /*period_size_sec=*/kSecondsPerDay);

  EXPECT_FLOAT_EQ(0.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(0.0f, stochastic_oscillator.GetHigh());
  EXPECT_FLOAT_EQ(0.0f, stochastic_oscillator.GetK());
  EXPECT_FLOAT_EQ(0.0f, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(0.0f, stochastic_oscillator.GetSlowD());
  EXPECT_EQ(0, stochastic_oscillator.GetNumOhlcTicks());

  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 00:00
  // --- Daily History ---
  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 (Day 1)
  stochastic_oscillator.Update(ohlc_history[0]);
  EXPECT_FLOAT_EQ(80.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(150.0f, stochastic_oscillator.GetHigh());
  EXPECT_FLOAT_EQ(100.0f * (120.0f - 80.0f) / (150.0f - 80.0f),
                  stochastic_oscillator.GetK());
  EXPECT_FLOAT_EQ(100.0f * (120.0f - 80.0f) / (150.0f - 80.0f),
                  stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(100.0f * (120.0f - 80.0f) / (150.0f - 80.0f),
                  stochastic_oscillator.GetSlowD());
  EXPECT_EQ(1, stochastic_oscillator.GetNumOhlcTicks());

  // O: 120  H: 180  L: 100  C: 150  V: 1000  T: 2017-01-01 08:00
  // --- Daily History ---
  // O: 100  H: 180  L:  80  C: 150  V: 2000  T: 2017-01-01 (Day 1)
  stochastic_oscillator.Update(ohlc_history[1]);
  EXPECT_FLOAT_EQ(80.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(180.0f, stochastic_oscillator.GetHigh());
  EXPECT_FLOAT_EQ(100.0f * (150.0f - 80.0f) / (180.0f - 80.0f),
                  stochastic_oscillator.GetK());
  EXPECT_FLOAT_EQ(100.0f * (150.0f - 80.0f) / (180.0f - 80.0f),
                  stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(100.0f * (150.0f - 80.0f) / (180.0f - 80.0f),
                  stochastic_oscillator.GetSlowD());
  EXPECT_EQ(1, stochastic_oscillator.GetNumOhlcTicks());

  // O: 150  H: 250  L: 100  C: 140  V: 1000  T: 2017-01-01 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  stochastic_oscillator.Update(ohlc_history[2]);
  EXPECT_FLOAT_EQ(80.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(250.0f, stochastic_oscillator.GetHigh());
  EXPECT_FLOAT_EQ(100.0f * (140.0f - 80.0f) / (250.0f - 80.0f),
                  stochastic_oscillator.GetK());
  EXPECT_FLOAT_EQ(100.0f * (140.0f - 80.0f) / (250.0f - 80.0f),
                  stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(100.0f * (140.0f - 80.0f) / (250.0f - 80.0f),
                  stochastic_oscillator.GetSlowD());
  EXPECT_EQ(1, stochastic_oscillator.GetNumOhlcTicks());

  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 (Day 2)
  stochastic_oscillator.Update(ohlc_history[3]);
  EXPECT_FLOAT_EQ(80.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(250.0f, stochastic_oscillator.GetHigh());
  float new_k = 100.0f * (100.0f - 80.0f) / (250.0f - 80.0f);
  EXPECT_FLOAT_EQ(new_k, stochastic_oscillator.GetK());
  float new_fast_d =
      (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) + new_k) / 2.0f;
  EXPECT_FLOAT_EQ(new_fast_d, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(
      (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) + new_fast_d) / 2.0f,
      stochastic_oscillator.GetSlowD());
  EXPECT_EQ(2, stochastic_oscillator.GetNumOhlcTicks());

  // O: 100  H: 120  L:  20  C:  50  V: 1000  T: 2017-01-02 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  50  V: 2000  T: 2017-01-02 (Day 2)
  stochastic_oscillator.Update(ohlc_history[4]);
  EXPECT_FLOAT_EQ(20.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(250.0f, stochastic_oscillator.GetHigh());
  new_k = 100.0f * (50.0f - 20.0f) / (250.0f - 20.0f);
  EXPECT_FLOAT_EQ(new_k, stochastic_oscillator.GetK());
  new_fast_d = (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) + new_k) / 2.0f;
  EXPECT_FLOAT_EQ(new_fast_d, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(
      (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) + new_fast_d) / 2.0f,
      stochastic_oscillator.GetSlowD());
  EXPECT_EQ(2, stochastic_oscillator.GetNumOhlcTicks());

  // O:  50  H: 100  L:  40  C:  80  V: 1000  T: 2017-01-02 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  stochastic_oscillator.Update(ohlc_history[5]);
  EXPECT_FLOAT_EQ(20.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(250.0f, stochastic_oscillator.GetHigh());
  new_k = 100.0f * (80.0f - 20.0f) / (250.0f - 20.0f);
  EXPECT_FLOAT_EQ(new_k, stochastic_oscillator.GetK());
  new_fast_d = (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) + new_k) / 2.0f;
  EXPECT_FLOAT_EQ(new_fast_d, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(
      (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) + new_fast_d) / 2.0f,
      stochastic_oscillator.GetSlowD());
  EXPECT_EQ(2, stochastic_oscillator.GetNumOhlcTicks());

  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 (Day 3)
  stochastic_oscillator.Update(ohlc_history[6]);
  EXPECT_FLOAT_EQ(20.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(180.0f, stochastic_oscillator.GetHigh());
  new_k = 100.0f * (150.0f - 20.0f) / (180.0f - 20.0f);
  EXPECT_FLOAT_EQ(new_k, stochastic_oscillator.GetK());
  new_fast_d = (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                100.0f * (80.0f - 20.0f) / (250.0f - 20.0f) + new_k) /
               3.0f;
  EXPECT_FLOAT_EQ(new_fast_d, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ((100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                   (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                    100.0f * (80.0f - 20.0f) / (250.0f - 20.0f)) /
                       2.0f +
                   new_fast_d) /
                      3.0f,
                  stochastic_oscillator.GetSlowD());
  EXPECT_EQ(3, stochastic_oscillator.GetNumOhlcTicks());

  // O: 150  H: 250  L: 120  C: 240  V: 1000  T: 2017-01-03 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 250  L:  50  C: 240  V: 2000  T: 2017-01-03 (Day 3)
  stochastic_oscillator.Update(ohlc_history[7]);
  EXPECT_FLOAT_EQ(20.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(250.0f, stochastic_oscillator.GetHigh());
  new_k = 100.0f * (240.0f - 20.0f) / (250.0f - 20.0f);
  EXPECT_FLOAT_EQ(new_k, stochastic_oscillator.GetK());
  new_fast_d = (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                100.0f * (80.0f - 20.0f) / (250.0f - 20.0f) + new_k) /
               3.0f;
  EXPECT_FLOAT_EQ(new_fast_d, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ((100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                   (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                    100.0f * (80.0f - 20.0f) / (250.0f - 20.0f)) /
                       2.0f +
                   new_fast_d) /
                      3.0f,
                  stochastic_oscillator.GetSlowD());
  EXPECT_EQ(3, stochastic_oscillator.GetNumOhlcTicks());

  // O: 240  H: 450  L: 220  C: 400  V: 1000  T: 2017-01-03 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  stochastic_oscillator.Update(ohlc_history[8]);
  EXPECT_FLOAT_EQ(20.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(450.0f, stochastic_oscillator.GetHigh());
  new_k = 100.0f * (400.0f - 20.0f) / (450.0f - 20.0f);
  EXPECT_FLOAT_EQ(new_k, stochastic_oscillator.GetK());
  new_fast_d = (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                100.0f * (80.0f - 20.0f) / (250.0f - 20.0f) + new_k) /
               3.0f;
  EXPECT_FLOAT_EQ(new_fast_d, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ((100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                   (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                    100.0f * (80.0f - 20.0f) / (250.0f - 20.0f)) /
                       2.0f +
                   new_fast_d) /
                      3.0f,
                  stochastic_oscillator.GetSlowD());
  EXPECT_EQ(3, stochastic_oscillator.GetNumOhlcTicks());

  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 (Day 4)
  stochastic_oscillator.Update(ohlc_history[9]);
  EXPECT_FLOAT_EQ(50.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(450.0f, stochastic_oscillator.GetHigh());
  new_k = 100.0f * (300.0f - 50.0f) / (450.0f - 50.0f);
  EXPECT_FLOAT_EQ(new_k, stochastic_oscillator.GetK());
  new_fast_d = (100.0f * (80.0f - 20.0f) / (250.0f - 20.0f) +
                100.0f * (400.0f - 20.0f) / (450.0f - 20.0f) + new_k) /
               3.0f;
  EXPECT_FLOAT_EQ(new_fast_d, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(((100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                    100.0f * (80.0f - 20.0f) / (250.0f - 20.0f)) /
                       2.0f +
                   (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                    100.0f * (80.0f - 20.0f) / (250.0f - 20.0f) +
                    100.0f * (400.0f - 20.0f) / (450.0f - 20.0f)) /
                       3.0f +
                   new_fast_d) /
                      3.0f,
                  stochastic_oscillator.GetSlowD());
  EXPECT_EQ(4, stochastic_oscillator.GetNumOhlcTicks());

  // O: 300  H: 700  L: 220  C: 650  V: 1000  T: 2017-01-04 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 700  L: 220  C: 650  V: 2000  T: 2017-01-04 (Day 4)
  stochastic_oscillator.Update(ohlc_history[10]);
  EXPECT_FLOAT_EQ(50.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(700.0f, stochastic_oscillator.GetHigh());
  new_k = 100.0f * (650.0f - 50.0f) / (700.0f - 50.0f);
  EXPECT_FLOAT_EQ(new_k, stochastic_oscillator.GetK());
  new_fast_d = (100.0f * (80.0f - 20.0f) / (250.0f - 20.0f) +
                100.0f * (400.0f - 20.0f) / (450.0f - 20.0f) + new_k) /
               3.0f;
  EXPECT_FLOAT_EQ(new_fast_d, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(((100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                    100.0f * (80.0f - 20.0f) / (250.0f - 20.0f)) /
                       2.0f +
                   (100.0f * (140.0f - 80.0f) / (250.0f - 80.0f) +
                    100.0f * (80.0f - 20.0f) / (250.0f - 20.0f) +
                    100.0f * (400.0f - 20.0f) / (450.0f - 20.0f)) /
                       3.0f +
                   new_fast_d) /
                      3.0f,
                  stochastic_oscillator.GetSlowD());
  EXPECT_EQ(4, stochastic_oscillator.GetNumOhlcTicks());

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
  stochastic_oscillator.Update(ohlc_history[11]);
  EXPECT_FLOAT_EQ(600.0f, stochastic_oscillator.GetLow());
  EXPECT_FLOAT_EQ(800.0f, stochastic_oscillator.GetHigh());
  new_k = 100.0f * (750.0f - 600.0f) / (800.0f - 600.0f);
  EXPECT_FLOAT_EQ(new_k, stochastic_oscillator.GetK());
  new_fast_d = (100.0f * (650.0f - 50.0f) / (700.0f - 50.0f) +
                100.0f * (650.0f - 220.0f) / (700.0f - 220.0f) + new_k) /
               3.0f;
  EXPECT_FLOAT_EQ(new_fast_d, stochastic_oscillator.GetFastD());
  EXPECT_FLOAT_EQ(((100.0f * (80.0f - 20.0f) / (250.0f - 20.0f) +
                    100.0f * (400.0f - 20.0f) / (450.0f - 20.0f) +
                    100.0f * (650.0f - 50.0f) / (700.0f - 50.0f)) /
                       3.0f +
                   (100.0f * (400.0f - 20.0f) / (450.0f - 20.0f) +
                    100.0f * (650.0f - 50.0f) / (700.0f - 50.0f) +
                    100.0f * (650.0f - 220.0f) / (700.0f - 220.0f)) /
                       3.0f +
                   new_fast_d) /
                      3.0f,
                  stochastic_oscillator.GetSlowD());
  EXPECT_EQ(6, stochastic_oscillator.GetNumOhlcTicks());
}

}  // namespace trader