// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/exponential_moving_average.h"

namespace trader {

ExponentialMovingAverage::ExponentialMovingAverage(float smoothing,
                                                   int ema_length,
                                                   int period_size_sec)
    : last_n_ohlc_ticks_(1, period_size_sec),
      smoothing_(smoothing),
      ema_length_(ema_length) {
  last_n_ohlc_ticks_.RegisterLastTickUpdatedCallback(
      [this](const OhlcTick& old_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        if (num_ohlc_ticks_ == 1) {
          // So far we have observed only 1 OHLC tick, which is being updated.
          current_ema_ = new_ohlc_tick.close();
        } else {
          // We have observed more than 1 OHLC tick. The current OHLC tick
          // is being updated, thus we also need to update the current EMA.
          assert(num_ohlc_ticks_ > 1);
          current_ema_ = ComputeExponentialMovingAverage(new_ohlc_tick);
        }
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedCallback(
      [this](const OhlcTick& new_ohlc_tick) {
        // This is the very first observed OHLC tick.
        assert(num_ohlc_ticks_ == 0);
        num_ohlc_ticks_ = 1;
        current_ema_ = new_ohlc_tick.close();
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedAndOldestTickRemovedCallback(
      [this](const OhlcTick& removed_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        // We have observed at least 1 OHLC tick. New OHLC tick is being added.
        assert(num_ohlc_ticks_ >= 1);
        ++num_ohlc_ticks_;
        previous_ema_ = current_ema_;
        current_ema_ = ComputeExponentialMovingAverage(new_ohlc_tick);
      });
}

void ExponentialMovingAverage::RegisterExponentialMovingAverageUpdatedCallback(
    ExponentialMovingAverageUpdatedCallback
        exponential_moving_average_updated_callback) {
  exponential_moving_average_updated_callback_ =
      exponential_moving_average_updated_callback;
}

float ExponentialMovingAverage::GetExponentialMovingAverage() const {
  return current_ema_;
}

int ExponentialMovingAverage::GetNumOhlcTicks() const {
  return num_ohlc_ticks_;
}

void ExponentialMovingAverage::Update(const OhlcTick& ohlc_tick) {
  last_n_ohlc_ticks_.Update(ohlc_tick);
  if (exponential_moving_average_updated_callback_) {
    exponential_moving_average_updated_callback_(GetExponentialMovingAverage(),
                                                 GetNumOhlcTicks());
  }
}

float ExponentialMovingAverage::ComputeExponentialMovingAverage(
    const OhlcTick& ohlc_tick) const {
  const float weight = smoothing_ / (1.0f + ema_length_);
  return weight * ohlc_tick.close() + (1.0f - weight) * previous_ema_;
}

}  // namespace trader
