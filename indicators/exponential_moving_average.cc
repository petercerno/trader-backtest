// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/exponential_moving_average.h"

namespace trader {

ExponentialMovingAverage::ExponentialMovingAverage(float smoothing,
                                                   int ema_length,
                                                   int period_size_sec)
    : last_n_ohlc_ticks_(1, period_size_sec),
      weight_(smoothing / (1.0f + ema_length)) {
  last_n_ohlc_ticks_.RegisterLastTickUpdatedCallback(
      [this](const OhlcTick& old_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        // We have observed at least 1 OHLC tick.
        // The most recent OHLC tick is being updated.
        assert(ema_helper_.GetNumValues() >= 1);
        ema_helper_.UpdateCurrentValue(new_ohlc_tick.close(), weight_);
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedCallback(
      [this](const OhlcTick& new_ohlc_tick) {
        // This is the very first observed OHLC tick.
        assert(ema_helper_.GetNumValues() == 0);
        ema_helper_.AddNewValue(new_ohlc_tick.close(), weight_);
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedAndOldestTickRemovedCallback(
      [this](const OhlcTick& removed_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        // We have observed at least 1 OHLC tick.
        // New OHLC tick is being added.
        assert(ema_helper_.GetNumValues() >= 1);
        ema_helper_.AddNewValue(new_ohlc_tick.close(), weight_);
      });
}

float ExponentialMovingAverage::GetExponentialMovingAverage() const {
  return ema_helper_.GetExponentialMovingAverage();
}

int ExponentialMovingAverage::GetNumOhlcTicks() const {
  return ema_helper_.GetNumValues();
}

void ExponentialMovingAverage::Update(const OhlcTick& ohlc_tick) {
  last_n_ohlc_ticks_.Update(ohlc_tick);
}

}  // namespace trader
