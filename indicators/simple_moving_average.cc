// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/simple_moving_average.h"

namespace trader {

SimpleMovingAverage::SimpleMovingAverage(int num_ohlc_ticks,
                                         int period_size_sec)
    : last_n_ohlc_ticks_(num_ohlc_ticks, period_size_sec) {
  last_n_ohlc_ticks_.RegisterLastTickUpdatedCallback(
      [this](const OhlcTick& old_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        sum_close_price_ += new_ohlc_tick.close() - old_ohlc_tick.close();
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedCallback(
      [this](const OhlcTick& new_ohlc_tick) {
        sum_close_price_ += new_ohlc_tick.close();
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedAndOldestTickRemovedCallback(
      [this](const OhlcTick& removed_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        sum_close_price_ += new_ohlc_tick.close() - removed_ohlc_tick.close();
      });
}

float SimpleMovingAverage::GetSimpleMovingAverage() const {
  const int num_ohlc_ticks = GetNumOhlcTicks();
  if (num_ohlc_ticks == 0) {
    return 0;
  }
  return sum_close_price_ / num_ohlc_ticks;
}

int SimpleMovingAverage::GetNumOhlcTicks() const {
  return last_n_ohlc_ticks_.GetLastNOhlcTicks().size();
}

void SimpleMovingAverage::Update(const OhlcTick& ohlc_tick) {
  last_n_ohlc_ticks_.Update(ohlc_tick);
}

}  // namespace trader
