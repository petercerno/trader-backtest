// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef LIB_TRADER_SIDE_INPUT_H
#define LIB_TRADER_SIDE_INPUT_H

#include "lib/trader_base.h"

namespace trader {

// Side history wrapper for fast thread-safe read-only access.
class TraderSideInput {
 public:
  // Constructor. Expects non-empty side_history with increasing timestamps.
  explicit TraderSideInput(const SideHistory& side_history);
  virtual ~TraderSideInput() {}

  // Returns the number of signals per side input record.
  int GetNumberOfSignals() const { return num_signals_; }
  // Returns the number of side input records.
  int GetNumberOfRecords() const { return timestamp_sec_history_.size(); }
  // Returns the timestamp (in seconds) for the given side_input_index.
  int GetSideInputTimestamp(int side_input_index) const {
    return timestamp_sec_history_.at(side_input_index);
  }
  // Returns the signal for the given side_input_index and signal_index.
  float GetSideInputSignal(int side_input_index, int signal_index) const {
    assert(side_input_index >= 0 && side_input_index < GetNumberOfRecords());
    assert(signal_index >= 0 && signal_index < GetNumberOfSignals());
    return data_.at(side_input_index * num_signals_ + signal_index);
  }
  // Returns the latest side input index before (or at) the given timestamp.
  // Returns -1 if the first side input record is after the given timestamp.
  // This method runs in O(log N) where N is the number of side input records.
  int GetSideInputIndex(int timestamp_sec) const;
  // The same method as GetSideInputIndex, but with a hint about what was the
  // previous side_input_index. If the prev_side_input_index is -1, then the
  // hint is ignored. The time complexity is O(1) if the given timestamp is
  // close enough to the the previous timestamp.
  int GetSideInputIndex(int timestamp_sec, int prev_side_input_index) const;

 private:
  // Number of signals per side input record.
  int num_signals_ = 0;
  // All historical (increasing) side input timestamps (in seconds).
  std::vector<int> timestamp_sec_history_;
  // Flattened vector of all historical side input signals.
  std::vector<float> data_;
};

}  // namespace trader

#endif  // LIB_TRADER_SIDE_INPUT_H