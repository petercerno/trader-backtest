// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/relative_strength_index.h"

namespace trader {

RelativeStrengthIndex::RelativeStrengthIndex(int num_periods,
                                             int period_size_sec)
    : num_periods_(num_periods),
      last_n_ohlc_ticks_(/*num_ohlc_ticks=*/2, period_size_sec) {
  last_n_ohlc_ticks_.RegisterLastTickUpdatedCallback(
      [this](const OhlcTick& old_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        // We have observed at least 1 OHLC tick.
        // The most recent OHLC tick was updated.
        assert(num_ohlc_ticks_ >= 1);
        LastTickUpdated();
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedCallback(
      [this](const OhlcTick& new_ohlc_tick) {
        // This is the first or second observed OHLC tick.
        assert(num_ohlc_ticks_ <= 1);
        NewTickAdded();
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedAndOldestTickRemovedCallback(
      [this](const OhlcTick& removed_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        // We have observed at least 2 OHLC ticks.
        // New OHLC tick was added.
        assert(num_ohlc_ticks_ >= 2);
        NewTickAdded();
      });
}

float RelativeStrengthIndex::GetUpwardChangeModifiedMovingAverage() const {
  return upward_change_mma_.GetExponentialMovingAverage();
}

float RelativeStrengthIndex::GetDownwardChangeModifiedMovingAverage() const {
  return downward_change_mma_.GetExponentialMovingAverage();
}

float RelativeStrengthIndex::GetRelativeStrengthIndex() const {
  const float upward_change_mma =
      upward_change_mma_.GetExponentialMovingAverage();
  const float downward_change_mma =
      downward_change_mma_.GetExponentialMovingAverage();
  if (upward_change_mma < 1.0e-6f && downward_change_mma < 1.0e-6f) {
    return 50.0f;
  }
  if (downward_change_mma < upward_change_mma * 1.0e-6f) {
    return 100.0f;
  }
  return 100.0f - 100.0f / (1.0f + upward_change_mma / downward_change_mma);
}

int RelativeStrengthIndex::GetNumOhlcTicks() const { return num_ohlc_ticks_; }

void RelativeStrengthIndex::Update(const OhlcTick& ohlc_tick) {
  last_n_ohlc_ticks_.Update(ohlc_tick);
}

void RelativeStrengthIndex::LastTickUpdated() {
  const float weight = GetModifiedMovingAverageWeight();
  const auto change = GetUpwardDownwardChange();
  upward_change_mma_.UpdateCurrentValue(change.first, weight);
  downward_change_mma_.UpdateCurrentValue(change.second, weight);
}

void RelativeStrengthIndex::NewTickAdded() {
  ++num_ohlc_ticks_;
  const float weight = GetModifiedMovingAverageWeight();
  const auto change = GetUpwardDownwardChange();
  upward_change_mma_.AddNewValue(change.first, weight);
  downward_change_mma_.AddNewValue(change.second, weight);
}

float RelativeStrengthIndex::GetModifiedMovingAverageWeight() const {
  return 1.0f / std::min(num_ohlc_ticks_, num_periods_);
}

std::pair<float, float> RelativeStrengthIndex::GetUpwardDownwardChange() const {
  float upward_change = 0;
  float downward_change = 0;
  const std::deque<OhlcTick> last_n_ohlc_ticks =
      last_n_ohlc_ticks_.GetLastNOhlcTicks();
  if (last_n_ohlc_ticks.size() == 1) {
    // We do not have a previous OHLC tick, so we use the opening and closing
    // price of the current OHLC tick.
    const OhlcTick& ohlc_tick = last_n_ohlc_ticks.at(0);
    if (ohlc_tick.close() >= ohlc_tick.open()) {
      upward_change = ohlc_tick.close() - ohlc_tick.open();
    } else {
      downward_change = ohlc_tick.open() - ohlc_tick.close();
    }
  } else {
    assert(last_n_ohlc_ticks.size() == 2);
    // We do have a previous OHLC tick, so we use the closing price of both
    // the previous and the current OHLC tick.
    const OhlcTick& prev_ohlc_tick = last_n_ohlc_ticks.at(0);
    const OhlcTick& ohlc_tick = last_n_ohlc_ticks.at(1);
    if (ohlc_tick.close() >= prev_ohlc_tick.close()) {
      upward_change = ohlc_tick.close() - prev_ohlc_tick.close();
    } else {
      downward_change = prev_ohlc_tick.close() - ohlc_tick.close();
    }
  }
  return {upward_change, downward_change};
}

}  // namespace trader
