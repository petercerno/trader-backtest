// Copyright © 2020 Peter Cerno. All rights reserved.

#include "base/side_input.h"

namespace trader {

SideInput::SideInput(const SideHistory& side_history)
    : num_signals_(side_history.front().signal_size()) {
  timestamp_sec_history_.reserve(side_history.size());
  data_.reserve(side_history.size() * num_signals_);
  int prev_timestamp_sec = 0;
  for (const SideInputRecord& side_input : side_history) {
    assert(side_input.signal_size() == num_signals_);
    assert(side_input.timestamp_sec() > prev_timestamp_sec);
    timestamp_sec_history_.push_back(side_input.timestamp_sec());
    prev_timestamp_sec = side_input.timestamp_sec();
    for (const float signal : side_input.signal()) {
      data_.push_back(signal);
    }
  }
}

int SideInput::GetSideInputIndex(int timestamp_sec) const {
  return std::upper_bound(timestamp_sec_history_.begin(),
                          timestamp_sec_history_.end(), timestamp_sec) -
         timestamp_sec_history_.begin() - 1;
}

int SideInput::GetSideInputIndex(int timestamp_sec,
                                 int prev_side_input_index) const {
  if (prev_side_input_index < 0) {
    return GetSideInputIndex(timestamp_sec);
  }
  assert(timestamp_sec >= timestamp_sec_history_.at(prev_side_input_index));
  if (prev_side_input_index == timestamp_sec_history_.size() - 1 ||
      timestamp_sec < timestamp_sec_history_.at(prev_side_input_index + 1)) {
    return prev_side_input_index;
  }
  if (prev_side_input_index == timestamp_sec_history_.size() - 2 ||
      timestamp_sec < timestamp_sec_history_.at(prev_side_input_index + 2)) {
    return prev_side_input_index + 1;
  }
  return std::upper_bound(
             timestamp_sec_history_.begin() + prev_side_input_index + 2,
             timestamp_sec_history_.end(), timestamp_sec) -
         timestamp_sec_history_.begin() - 1;
}

}  // namespace trader