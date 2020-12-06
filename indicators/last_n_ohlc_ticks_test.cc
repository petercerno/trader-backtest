// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/last_n_ohlc_ticks.h"

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

void ExpectOhlcTick(int timestamp_sec, float open, float high, float low,
                    float close, float volume,
                    const OhlcTick& actual_ohlc_tick) {
  EXPECT_EQ(timestamp_sec, actual_ohlc_tick.timestamp_sec());
  EXPECT_FLOAT_EQ(open, actual_ohlc_tick.open());
  EXPECT_FLOAT_EQ(high, actual_ohlc_tick.high());
  EXPECT_FLOAT_EQ(low, actual_ohlc_tick.low());
  EXPECT_FLOAT_EQ(close, actual_ohlc_tick.close());
  EXPECT_FLOAT_EQ(volume, actual_ohlc_tick.volume());
}
}  // namespace

TEST(LastNOhlcTicksTest, GetLast3DaysWhenAdding8HourOhlcTicks) {
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

  LastNOhlcTicks last_n_days(/*num_ohlc_ticks=*/3,
                             /*period_size_sec=*/kSecondsPerDay);

  std::vector<OhlcTick> last_tick_updated_old_ohlc_tick;
  std::vector<OhlcTick> last_tick_updated_new_ohlc_tick;
  last_n_days.RegisterLastTickUpdatedCallback(
      [&last_tick_updated_old_ohlc_tick, &last_tick_updated_new_ohlc_tick](
          const OhlcTick& old_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        last_tick_updated_old_ohlc_tick.push_back(old_ohlc_tick);
        last_tick_updated_new_ohlc_tick.push_back(new_ohlc_tick);
      });

  std::vector<OhlcTick> new_tick_added_new_ohlc_tick;
  last_n_days.RegisterNewTickAddedCallback(
      [&new_tick_added_new_ohlc_tick](const OhlcTick& new_ohlc_tick) {
        new_tick_added_new_ohlc_tick.push_back(new_ohlc_tick);
      });

  std::vector<OhlcTick> new_tick_shifted_removed_ohlc_tick;
  std::vector<OhlcTick> new_tick_shifted_new_ohlc_tick;
  last_n_days.RegisterNewTickAddedAndOldestTickRemovedCallback(
      [&new_tick_shifted_removed_ohlc_tick, &new_tick_shifted_new_ohlc_tick](
          const OhlcTick& removed_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        new_tick_shifted_removed_ohlc_tick.push_back(removed_ohlc_tick);
        new_tick_shifted_new_ohlc_tick.push_back(new_ohlc_tick);
      });

  ASSERT_TRUE(last_n_days.GetLastNOhlcTicks().empty());

  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 00:00
  // --- Daily History ---
  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 (Day 1)
  last_n_days.Update(ohlc_history[0]);
  ASSERT_EQ(1, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483228800, 100, 150, 80, 120, 1000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ASSERT_EQ(0, last_tick_updated_old_ohlc_tick.size());
  ASSERT_EQ(0, last_tick_updated_new_ohlc_tick.size());
  ASSERT_EQ(1, new_tick_added_new_ohlc_tick.size());
  ExpectOhlcTick(1483228800, 100, 150, 80, 120, 1000,
                 new_tick_added_new_ohlc_tick.back());
  ASSERT_EQ(0, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_new_ohlc_tick.size());

  // O: 120  H: 180  L: 100  C: 150  V: 1000  T: 2017-01-01 08:00
  // --- Daily History ---
  // O: 100  H: 180  L:  80  C: 150  V: 2000  T: 2017-01-01 (Day 1)
  last_n_days.Update(ohlc_history[1]);
  ASSERT_EQ(1, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483228800, 100, 180, 80, 150, 2000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ASSERT_EQ(1, last_tick_updated_old_ohlc_tick.size());
  ExpectOhlcTick(1483228800, 100, 150, 80, 120, 1000,
                 last_tick_updated_old_ohlc_tick.back());
  ASSERT_EQ(1, last_tick_updated_new_ohlc_tick.size());
  ExpectOhlcTick(1483228800, 100, 180, 80, 150, 2000,
                 last_tick_updated_new_ohlc_tick.back());
  ASSERT_EQ(1, new_tick_added_new_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_new_ohlc_tick.size());

  // O: 150  H: 250  L: 100  C: 140  V: 1000  T: 2017-01-01 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  last_n_days.Update(ohlc_history[2]);
  ASSERT_EQ(1, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483228800, 100, 250, 80, 140, 3000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ASSERT_EQ(2, last_tick_updated_old_ohlc_tick.size());
  ExpectOhlcTick(1483228800, 100, 180, 80, 150, 2000,
                 last_tick_updated_old_ohlc_tick.back());
  ASSERT_EQ(2, last_tick_updated_new_ohlc_tick.size());
  ExpectOhlcTick(1483228800, 100, 250, 80, 140, 3000,
                 last_tick_updated_new_ohlc_tick.back());
  ASSERT_EQ(1, new_tick_added_new_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_new_ohlc_tick.size());

  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 (Day 2)
  last_n_days.Update(ohlc_history[3]);
  ASSERT_EQ(2, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483228800, 100, 250, 80, 140, 3000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ExpectOhlcTick(1483315200, 140, 150, 80, 100, 1000,
                 last_n_days.GetLastNOhlcTicks()[1]);
  ASSERT_EQ(2, last_tick_updated_old_ohlc_tick.size());
  ASSERT_EQ(2, last_tick_updated_new_ohlc_tick.size());
  ASSERT_EQ(2, new_tick_added_new_ohlc_tick.size());
  ExpectOhlcTick(1483315200, 140, 150, 80, 100, 1000,
                 new_tick_added_new_ohlc_tick.back());
  ASSERT_EQ(0, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_new_ohlc_tick.size());

  // O: 100  H: 120  L:  20  C:  50  V: 1000  T: 2017-01-02 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  50  V: 2000  T: 2017-01-02 (Day 2)
  last_n_days.Update(ohlc_history[4]);
  ASSERT_EQ(2, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483228800, 100, 250, 80, 140, 3000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ExpectOhlcTick(1483315200, 140, 150, 20, 50, 2000,
                 last_n_days.GetLastNOhlcTicks()[1]);
  ASSERT_EQ(3, last_tick_updated_old_ohlc_tick.size());
  ExpectOhlcTick(1483315200, 140, 150, 80, 100, 1000,
                 last_tick_updated_old_ohlc_tick.back());
  ASSERT_EQ(3, last_tick_updated_new_ohlc_tick.size());
  ExpectOhlcTick(1483315200, 140, 150, 20, 50, 2000,
                 last_tick_updated_new_ohlc_tick.back());
  ASSERT_EQ(2, new_tick_added_new_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_new_ohlc_tick.size());

  // O:  50  H: 100  L:  40  C:  80  V: 1000  T: 2017-01-02 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  last_n_days.Update(ohlc_history[5]);
  ASSERT_EQ(2, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483228800, 100, 250, 80, 140, 3000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ExpectOhlcTick(1483315200, 140, 150, 20, 80, 3000,
                 last_n_days.GetLastNOhlcTicks()[1]);
  ASSERT_EQ(4, last_tick_updated_old_ohlc_tick.size());
  ExpectOhlcTick(1483315200, 140, 150, 20, 50, 2000,
                 last_tick_updated_old_ohlc_tick.back());
  ASSERT_EQ(4, last_tick_updated_new_ohlc_tick.size());
  ExpectOhlcTick(1483315200, 140, 150, 20, 80, 3000,
                 last_tick_updated_new_ohlc_tick.back());
  ASSERT_EQ(2, new_tick_added_new_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_new_ohlc_tick.size());

  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 (Day 3)
  last_n_days.Update(ohlc_history[6]);
  ASSERT_EQ(3, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483228800, 100, 250, 80, 140, 3000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ExpectOhlcTick(1483315200, 140, 150, 20, 80, 3000,
                 last_n_days.GetLastNOhlcTicks()[1]);
  ExpectOhlcTick(1483401600, 80, 180, 50, 150, 1000,
                 last_n_days.GetLastNOhlcTicks()[2]);
  ASSERT_EQ(4, last_tick_updated_old_ohlc_tick.size());
  ASSERT_EQ(4, last_tick_updated_new_ohlc_tick.size());
  ASSERT_EQ(3, new_tick_added_new_ohlc_tick.size());
  ExpectOhlcTick(1483401600, 80, 180, 50, 150, 1000,
                 new_tick_added_new_ohlc_tick.back());
  ASSERT_EQ(0, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_new_ohlc_tick.size());

  // O: 150  H: 250  L: 120  C: 240  V: 1000  T: 2017-01-03 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 250  L:  50  C: 240  V: 2000  T: 2017-01-03 (Day 3)
  last_n_days.Update(ohlc_history[7]);
  ASSERT_EQ(3, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483228800, 100, 250, 80, 140, 3000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ExpectOhlcTick(1483315200, 140, 150, 20, 80, 3000,
                 last_n_days.GetLastNOhlcTicks()[1]);
  ExpectOhlcTick(1483401600, 80, 250, 50, 240, 2000,
                 last_n_days.GetLastNOhlcTicks()[2]);
  ASSERT_EQ(5, last_tick_updated_old_ohlc_tick.size());
  ExpectOhlcTick(1483401600, 80, 180, 50, 150, 1000,
                 last_tick_updated_old_ohlc_tick.back());
  ASSERT_EQ(5, last_tick_updated_new_ohlc_tick.size());
  ExpectOhlcTick(1483401600, 80, 250, 50, 240, 2000,
                 last_tick_updated_new_ohlc_tick.back());
  ASSERT_EQ(3, new_tick_added_new_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_new_ohlc_tick.size());

  // O: 240  H: 450  L: 220  C: 400  V: 1000  T: 2017-01-03 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  last_n_days.Update(ohlc_history[8]);
  ASSERT_EQ(3, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483228800, 100, 250, 80, 140, 3000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ExpectOhlcTick(1483315200, 140, 150, 20, 80, 3000,
                 last_n_days.GetLastNOhlcTicks()[1]);
  ExpectOhlcTick(1483401600, 80, 450, 50, 400, 3000,
                 last_n_days.GetLastNOhlcTicks()[2]);
  ASSERT_EQ(6, last_tick_updated_old_ohlc_tick.size());
  ExpectOhlcTick(1483401600, 80, 250, 50, 240, 2000,
                 last_tick_updated_old_ohlc_tick.back());
  ASSERT_EQ(6, last_tick_updated_new_ohlc_tick.size());
  ExpectOhlcTick(1483401600, 80, 450, 50, 400, 3000,
                 last_tick_updated_new_ohlc_tick.back());
  ASSERT_EQ(3, new_tick_added_new_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(0, new_tick_shifted_new_ohlc_tick.size());

  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 (Day 4)
  last_n_days.Update(ohlc_history[9]);
  ASSERT_EQ(3, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483315200, 140, 150, 20, 80, 3000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ExpectOhlcTick(1483401600, 80, 450, 50, 400, 3000,
                 last_n_days.GetLastNOhlcTicks()[1]);
  ExpectOhlcTick(1483488000, 400, 450, 250, 300, 1000,
                 last_n_days.GetLastNOhlcTicks()[2]);
  ASSERT_EQ(6, last_tick_updated_old_ohlc_tick.size());
  ASSERT_EQ(6, last_tick_updated_new_ohlc_tick.size());
  ASSERT_EQ(3, new_tick_added_new_ohlc_tick.size());
  ASSERT_EQ(1, new_tick_shifted_removed_ohlc_tick.size());
  ExpectOhlcTick(1483228800, 100, 250, 80, 140, 3000,
                 new_tick_shifted_removed_ohlc_tick.back());
  ASSERT_EQ(1, new_tick_shifted_new_ohlc_tick.size());
  ExpectOhlcTick(1483488000, 400, 450, 250, 300, 1000,
                 new_tick_shifted_new_ohlc_tick.back());

  // O: 300  H: 700  L: 220  C: 650  V: 1000  T: 2017-01-04 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 700  L: 220  C: 650  V: 2000  T: 2017-01-04 (Day 4)
  last_n_days.Update(ohlc_history[10]);
  ASSERT_EQ(3, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483315200, 140, 150, 20, 80, 3000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ExpectOhlcTick(1483401600, 80, 450, 50, 400, 3000,
                 last_n_days.GetLastNOhlcTicks()[1]);
  ExpectOhlcTick(1483488000, 400, 700, 220, 650, 2000,
                 last_n_days.GetLastNOhlcTicks()[2]);
  ASSERT_EQ(7, last_tick_updated_old_ohlc_tick.size());
  ExpectOhlcTick(1483488000, 400, 450, 250, 300, 1000,
                 last_tick_updated_old_ohlc_tick.back());
  ASSERT_EQ(7, last_tick_updated_new_ohlc_tick.size());
  ExpectOhlcTick(1483488000, 400, 700, 220, 650, 2000,
                 last_tick_updated_new_ohlc_tick.back());
  ASSERT_EQ(3, new_tick_added_new_ohlc_tick.size());
  ASSERT_EQ(1, new_tick_shifted_removed_ohlc_tick.size());
  ASSERT_EQ(1, new_tick_shifted_new_ohlc_tick.size());

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
  last_n_days.Update(ohlc_history[11]);
  ASSERT_EQ(3, last_n_days.GetLastNOhlcTicks().size());
  ExpectOhlcTick(1483488000, 400, 700, 220, 650, 2000,
                 last_n_days.GetLastNOhlcTicks()[0]);
  ExpectOhlcTick(1483574400, 650, 650, 650, 650, 0,
                 last_n_days.GetLastNOhlcTicks()[1]);
  ExpectOhlcTick(1483660800, 650, 800, 600, 750, 1000,
                 last_n_days.GetLastNOhlcTicks()[2]);
  ASSERT_EQ(7, last_tick_updated_old_ohlc_tick.size());
  ASSERT_EQ(7, last_tick_updated_new_ohlc_tick.size());
  ASSERT_EQ(3, new_tick_added_new_ohlc_tick.size());
  ASSERT_EQ(3, new_tick_shifted_removed_ohlc_tick.size());
  ExpectOhlcTick(1483315200, 140, 150, 20, 80, 3000,
                 new_tick_shifted_removed_ohlc_tick[1]);
  ExpectOhlcTick(1483401600, 80, 450, 50, 400, 3000,
                 new_tick_shifted_removed_ohlc_tick[2]);
  ASSERT_EQ(3, new_tick_shifted_new_ohlc_tick.size());
  ExpectOhlcTick(1483574400, 650, 650, 650, 650, 0,
                 new_tick_shifted_new_ohlc_tick[1]);
  ExpectOhlcTick(1483660800, 650, 800, 600, 750, 1000,
                 new_tick_shifted_new_ohlc_tick[2]);
}

}  // namespace trader
