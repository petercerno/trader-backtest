// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/test_util.h"

namespace trader {
namespace testing {
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

// Adds OHLC tick to the history 8h away from the last OHLC tick.
void Add8HourOhlcTick(float open, float high, float low, float close,
                      OhlcHistory* ohlc_history) {
  AddOhlcTickWithPeriod(open, high, low, close, kSecondsPer8Hours,
                        ohlc_history);
}
}  // namespace

void PrepareExampleOhlcHistory(OhlcHistory* ohlc_history) {
  Add8HourOhlcTick(100, 150, 80, 120, ohlc_history);  // 2017-01-01
  Add8HourOhlcTick(120, 180, 100, 150, ohlc_history);
  Add8HourOhlcTick(150, 250, 100, 140, ohlc_history);
  Add8HourOhlcTick(140, 150, 80, 100, ohlc_history);  // 2017-01-02
  Add8HourOhlcTick(100, 120, 20, 50, ohlc_history);
  Add8HourOhlcTick(50, 100, 40, 80, ohlc_history);
  Add8HourOhlcTick(80, 180, 50, 150, ohlc_history);  // 2017-01-03
  Add8HourOhlcTick(150, 250, 120, 240, ohlc_history);
  Add8HourOhlcTick(240, 450, 220, 400, ohlc_history);
  Add8HourOhlcTick(400, 450, 250, 300, ohlc_history);  // 2017-01-04
  Add8HourOhlcTick(300, 700, 220, 650, ohlc_history);
  Add8HourOhlcTick(650, 800, 600, 750, ohlc_history);  // 2017-01-06 08:00
  ohlc_history->back().set_timestamp_sec(ohlc_history->back().timestamp_sec() +
                                         1 * kSecondsPerDay +
                                         2 * kSecondsPer8Hours);
}

}  // namespace testing
}  // namespace trader