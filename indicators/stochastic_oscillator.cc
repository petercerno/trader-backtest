// Copyright © 2021 Peter Cerno. All rights reserved.

#include "indicators/stochastic_oscillator.h"

namespace trader {

StochasticOscillator::StochasticOscillator(int num_periods, int period_size_sec)
    : last_n_ohlc_ticks_(/*num_ohlc_ticks=*/1, period_size_sec),
      sliding_window_min_(/*window_size=*/num_periods),
      sliding_window_max_(/*window_size=*/num_periods),
      d_fast_(/*window_size=*/3),
      d_slow_(/*window_size=*/3) {
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

float StochasticOscillator::GetLow() const {
  return sliding_window_min_.GetSlidingWindowMinimum();
}

float StochasticOscillator::GetHigh() const {
  return sliding_window_max_.GetSlidingWindowMaximum();
}

float StochasticOscillator::GetK() const { return latest_k_; }

float StochasticOscillator::GetFastD() const { return d_fast_.GetMean(); }

float StochasticOscillator::GetSlowD() const { return d_slow_.GetMean(); }

int StochasticOscillator::GetNumOhlcTicks() const { return num_ohlc_ticks_; }

void StochasticOscillator::Update(const OhlcTick& ohlc_tick) {
  last_n_ohlc_ticks_.Update(ohlc_tick);
}

void StochasticOscillator::LastTickUpdated(const OhlcTick& ohlc_tick) {
  sliding_window_min_.UpdateCurrentValue(ohlc_tick.low());
  sliding_window_max_.UpdateCurrentValue(ohlc_tick.high());
  UpdateK(ohlc_tick.close());
  d_fast_.UpdateCurrentValue(GetK());
  d_slow_.UpdateCurrentValue(GetFastD());
}

void StochasticOscillator::NewTickAdded(const OhlcTick& ohlc_tick) {
  ++num_ohlc_ticks_;
  sliding_window_min_.AddNewValue(ohlc_tick.low());
  sliding_window_max_.AddNewValue(ohlc_tick.high());
  UpdateK(ohlc_tick.close());
  d_fast_.AddNewValue(GetK());
  d_slow_.AddNewValue(GetFastD());
}

void StochasticOscillator::UpdateK(const float latest_price) {
  assert(num_ohlc_ticks_ >= 1);
  const float price_min = GetLow();
  const float price_max = GetHigh();
  const float price_span = price_max - price_min;
  if (price_span < 1.0e-6f) {
    latest_k_ = 50;
    return;
  }
  latest_k_ = 100.0f * (latest_price - price_min) / price_span;
}

}  // namespace trader
