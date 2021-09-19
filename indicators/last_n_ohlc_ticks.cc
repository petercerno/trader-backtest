// Copyright © 2021 Peter Cerno. All rights reserved.

#include "indicators/last_n_ohlc_ticks.h"

namespace trader {

void LastNOhlcTicks::RegisterLastTickUpdatedCallback(
    LastTickUpdatedCallback last_tick_updated_callback) {
  last_tick_updated_callback_ = last_tick_updated_callback;
}

void LastNOhlcTicks::RegisterNewTickAddedCallback(
    NewTickAddedCallback new_tick_added_callback) {
  new_tick_added_callback_ = new_tick_added_callback;
}

void LastNOhlcTicks::RegisterNewTickAddedAndOldestTickRemovedCallback(
    NewTickAddedAndOldestTickRemovedCallback
        new_tick_added_and_oldest_tick_removed_callback) {
  new_tick_added_and_oldest_tick_removed_callback_ =
      new_tick_added_and_oldest_tick_removed_callback;
}

const std::deque<OhlcTick>& LastNOhlcTicks::GetLastNOhlcTicks() const {
  return last_n_ohlc_ticks_;
}

void LastNOhlcTicks::Update(const OhlcTick& ohlc_tick) {
  const int64_t adjusted_timestamp_sec =
      period_size_sec_ * (ohlc_tick.timestamp_sec() / period_size_sec_);
  while (!last_n_ohlc_ticks_.empty() &&
         last_n_ohlc_ticks_.back().timestamp_sec() + period_size_sec_ <
             adjusted_timestamp_sec) {
    const int64_t prev_timestamp_sec =
        last_n_ohlc_ticks_.back().timestamp_sec();
    const float prev_close = last_n_ohlc_ticks_.back().close();
    last_n_ohlc_ticks_.emplace_back();
    OhlcTick* top_ohlc_tick = &last_n_ohlc_ticks_.back();
    top_ohlc_tick->set_timestamp_sec(prev_timestamp_sec + period_size_sec_);
    top_ohlc_tick->set_open(prev_close);
    top_ohlc_tick->set_high(prev_close);
    top_ohlc_tick->set_low(prev_close);
    top_ohlc_tick->set_close(prev_close);
    top_ohlc_tick->set_volume(0);
    NewOhlcTickAdded();
  }
  if (last_n_ohlc_ticks_.empty() ||
      last_n_ohlc_ticks_.back().timestamp_sec() < adjusted_timestamp_sec) {
    last_n_ohlc_ticks_.push_back(ohlc_tick);
    last_n_ohlc_ticks_.back().set_timestamp_sec(adjusted_timestamp_sec);
    NewOhlcTickAdded();
  } else {
    assert(last_n_ohlc_ticks_.back().timestamp_sec() == adjusted_timestamp_sec);
    OhlcTick* top_ohlc_tick = &last_n_ohlc_ticks_.back();
    const OhlcTick old_ohlc_tick_copy = *top_ohlc_tick;
    top_ohlc_tick->set_high(std::max(top_ohlc_tick->high(), ohlc_tick.high()));
    top_ohlc_tick->set_low(std::min(top_ohlc_tick->low(), ohlc_tick.low()));
    top_ohlc_tick->set_close(ohlc_tick.close());
    top_ohlc_tick->set_volume(top_ohlc_tick->volume() + ohlc_tick.volume());
    LastOhlcTickUpdated(old_ohlc_tick_copy);
  }
}

void LastNOhlcTicks::NewOhlcTickAdded() {
  if (last_n_ohlc_ticks_.size() <= num_ohlc_ticks_) {
    if (new_tick_added_callback_) {
      new_tick_added_callback_(last_n_ohlc_ticks_.back());
    }
  } else {
    const OhlcTick removed_ohlc_tick_copy = last_n_ohlc_ticks_.front();
    last_n_ohlc_ticks_.pop_front();
    if (new_tick_added_and_oldest_tick_removed_callback_) {
      new_tick_added_and_oldest_tick_removed_callback_(
          removed_ohlc_tick_copy, last_n_ohlc_ticks_.back());
    }
  }
}

void LastNOhlcTicks::LastOhlcTickUpdated(const OhlcTick& old_ohlc_tick) {
  if (last_tick_updated_callback_) {
    last_tick_updated_callback_(old_ohlc_tick, last_n_ohlc_ticks_.back());
  }
}

}  // namespace trader
