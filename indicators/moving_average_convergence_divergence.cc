// Copyright © 2021 Peter Cerno. All rights reserved.

#include "indicators/moving_average_convergence_divergence.h"

namespace trader {

MovingAverageConvergenceDivergence::MovingAverageConvergenceDivergence(
    int fast_length, int slow_length, int signal_smoothing, int period_size_sec)
    : last_n_ohlc_ticks_(/*num_ohlc_ticks=*/1, period_size_sec),
      fast_weight_(/*smoothing=*/2.0f / (1.0f + fast_length)),
      slow_weight_(/*smoothing=*/2.0f / (1.0f + slow_length)),
      signal_weight_(/*smoothing=*/2.0f / (1.0f + signal_smoothing)) {
  last_n_ohlc_ticks_.RegisterLastTickUpdatedCallback(
      [this](const OhlcTick& old_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        // We have observed at least 1 OHLC tick.
        // The most recent OHLC tick was updated.
        assert(num_ohlc_ticks_ >= 1);
        LastTickUpdated(new_ohlc_tick);
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedCallback(
      [this](const OhlcTick& new_ohlc_tick) {
        // This is the very first observed OHLC tick.
        assert(num_ohlc_ticks_ == 0);
        NewTickAdded(new_ohlc_tick);
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedAndOldestTickRemovedCallback(
      [this](const OhlcTick& removed_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        // We have observed at least 1 OHLC tick.
        // New OHLC tick was added.
        assert(num_ohlc_ticks_ >= 1);
        NewTickAdded(new_ohlc_tick);
      });
}

float MovingAverageConvergenceDivergence::GetFastExponentialMovingAverage()
    const {
  return fast_ema_.GetExponentialMovingAverage();
}

float MovingAverageConvergenceDivergence::GetSlowExponentialMovingAverage()
    const {
  return slow_ema_.GetExponentialMovingAverage();
}

float MovingAverageConvergenceDivergence::GetMACDSeries() const {
  return GetFastExponentialMovingAverage() - GetSlowExponentialMovingAverage();
}

float MovingAverageConvergenceDivergence::GetMACDSignal() const {
  return signal_ema_.GetExponentialMovingAverage();
}

float MovingAverageConvergenceDivergence::GetDivergence() const {
  return GetMACDSeries() - GetMACDSignal();
}

int MovingAverageConvergenceDivergence::GetNumOhlcTicks() const {
  return num_ohlc_ticks_;
}

void MovingAverageConvergenceDivergence::Update(const OhlcTick& ohlc_tick) {
  last_n_ohlc_ticks_.Update(ohlc_tick);
}

void MovingAverageConvergenceDivergence::LastTickUpdated(
    const OhlcTick& ohlc_tick) {
  fast_ema_.UpdateCurrentValue(ohlc_tick.close(), fast_weight_);
  slow_ema_.UpdateCurrentValue(ohlc_tick.close(), slow_weight_);
  signal_ema_.UpdateCurrentValue(GetMACDSeries(), signal_weight_);
}

void MovingAverageConvergenceDivergence::NewTickAdded(
    const OhlcTick& ohlc_tick) {
  ++num_ohlc_ticks_;
  fast_ema_.AddNewValue(ohlc_tick.close(), fast_weight_);
  slow_ema_.AddNewValue(ohlc_tick.close(), slow_weight_);
  signal_ema_.AddNewValue(GetMACDSeries(), signal_weight_);
}

}  // namespace trader
