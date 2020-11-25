// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef INDICATORS_UTIL_H
#define INDICATORS_UTIL_H

#include <algorithm>
#include <cassert>
#include <deque>
#include <tuple>

namespace trader {

// Calculates the Exponential Moving Average (EMA) over the provided values.
// All methods run in O(1) time.
class ExponentialMovingAverageHelper {
 public:
  ExponentialMovingAverageHelper() {}
  ~ExponentialMovingAverageHelper() {}

  // Returns the current Exponential Moving Average.
  float GetExponentialMovingAverage() const { return current_ema_; }

  // Returns the number of values considered in the Exponential Moving Average.
  int GetNumValues() const { return num_values_; }

  // Adds a new value.
  // weight: Weight of the new value in the Exponential Moving Average.
  void AddNewValue(float value, float weight);

  // Updates the current (most recent) value.
  // Assumes that at least one value was added before.
  // weight: Weight of the updated value in the Exponential Moving Average.
  void UpdateCurrentValue(float value, float weight);

 private:
  // Current Exponential Moving Average (including the current value).
  float current_ema_ = 0;
  // Previous Exponential Moving Average (excluding the current value).
  float previous_ema_ = 0;
  // Number of values considered in the Exponential Moving Average.
  int num_values_ = 0;
};

// Calculates the sliding window minimum over the provided values.
// All methods run in amortized O(1) time.
class SlidingWindowMinimum {
 public:
  // Constructor.
  // window_size: Number of values in the sliding window.
  SlidingWindowMinimum(int window_size) : window_size_(window_size) {
    assert(window_size > 0);
  }
  ~SlidingWindowMinimum() {}

  // Returns the current sliding window minimum.
  float GetSlidingWindowMinimum() const {
    return current_sliding_window_minimum_;
  }

  // Returns the total number of added values.
  int GetNumValues() const { return num_values_; }

  // Returns the current size of the sliding window.
  int GetWindowSize() const { return std::min(num_values_, window_size_); }

  // Adds a new value (and shifts the window by one if more than window_size
  // values were added).
  void AddNewValue(float value);

  // Updates the current (most recent) value.
  // Assumes that at least one value was added before.
  void UpdateCurrentValue(float value);

 private:
  // Size of the sliding window.
  int window_size_ = 0;
  // Current (most recently added / updated) value.
  float current_value_ = 0;
  // Current sliding window minimum.
  float current_sliding_window_minimum_ = 0;
  // Deque of values within the sliding window (excluding the current value).
  std::deque<std::pair<float, int>> window_;
  // Total number of added values (including the current value).
  int num_values_ = 0;
};

// Calculates the sliding window maximum over the provided values.
// All methods run in amortized O(1) time.
class SlidingWindowMaximum {
 public:
  // Constructor.
  // window_size: Number of values in the sliding window.
  SlidingWindowMaximum(int window_size)
      : sliding_window_minimum_(window_size) {}
  ~SlidingWindowMaximum() {}

  // Returns the current sliding window maximum.
  float GetSlidingWindowMaximum() const {
    return -sliding_window_minimum_.GetSlidingWindowMinimum();
  }

  // Returns the total number of added values.
  int GetNumValues() const { return sliding_window_minimum_.GetNumValues(); }

  // Returns the current size of the sliding window.
  int GetWindowSize() const { return sliding_window_minimum_.GetWindowSize(); }

  // Adds a new value (and shifts the window by one if more than window_size
  // values were added).
  void AddNewValue(float value) { sliding_window_minimum_.AddNewValue(-value); }

  // Updates the current (most recent) value.
  // Assumes that at least one value was added before.
  void UpdateCurrentValue(float value) {
    sliding_window_minimum_.UpdateCurrentValue(-value);
  }

 private:
  SlidingWindowMinimum sliding_window_minimum_;
};

}  // namespace trader

#endif  // INDICATORS_UTIL_H
