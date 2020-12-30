// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/util.h"

namespace trader {

float SlidingWindowMeanAndVariance::GetMean() const {
  if (num_values_ == 0) {
    return 0;
  }
  return (current_value_ + window_sum_) / GetWindowSize();
}

float SlidingWindowMeanAndVariance::GetVariance() const {
  if (num_values_ == 0) {
    return 0;
  }
  const int window_size = GetWindowSize();
  const float mean = (current_value_ + window_sum_) / window_size;
  return (std::pow(current_value_, 2) + window_sum_2_) / window_size -
         std::pow(mean, 2);
}

float SlidingWindowMeanAndVariance::GetStandardDeviation() const {
  return std::sqrt(GetVariance());
}

int SlidingWindowMeanAndVariance::GetWindowSize() const {
  if (window_size_ == 0) {
    return num_values_;
  }
  return std::min(num_values_, window_size_);
}

void SlidingWindowMeanAndVariance::AddNewValue(float value) {
  if (num_values_ == 0 || window_size_ <= 1) {
    if (window_size_ == 0) {
      window_sum_ += current_value_;
      window_sum_2_ += std::pow(current_value_, 2);
    }
    current_value_ = value;
    ++num_values_;
    return;
  }
  window_.push_back(current_value_);
  window_sum_ += current_value_;
  window_sum_2_ += std::pow(current_value_, 2);
  current_value_ = value;
  ++num_values_;
  if (window_.size() >= window_size_) {
    const float popped_value = window_.front();
    window_sum_ -= popped_value;
    window_sum_2_ -= std::pow(popped_value, 2);
    window_.pop_front();
  }
}

void SlidingWindowMeanAndVariance::UpdateCurrentValue(float value) {
  assert(num_values_ >= 1);
  current_value_ = value;
}

void ExponentialMovingAverageHelper::AddNewValue(float value, float weight) {
  previous_ema_ = current_ema_;
  ++num_values_;
  UpdateCurrentValue(value, weight);
}

void ExponentialMovingAverageHelper::UpdateCurrentValue(float value,
                                                        float weight) {
  assert(num_values_ > 0);
  if (num_values_ == 1) {
    current_ema_ = value;
  } else {
    current_ema_ = weight * value + (1.0f - weight) * previous_ema_;
  }
}

void SlidingWindowMinimum::AddNewValue(float value) {
  if (num_values_ == 0 || window_size_ == 1) {
    current_value_ = value;
    current_sliding_window_minimum_ = value;
    ++num_values_;
    return;
  }
  // First we push the current_value_ to the window_ deque.
  // We need to pop all values that are >= current_value_, since these
  // can no longer be minimum. This keeps the window_ deque sorted.
  while (!window_.empty() && window_.back().first >= current_value_) {
    window_.pop_back();
  }
  window_.push_back({current_value_, num_values_});
  // The window covers num_values_ - window_.front().second + 1 values.
  // We need this to be at most window_size_ - 1 (excluding the current value).
  while (num_values_ - window_.front().second + 1 >= window_size_) {
    window_.pop_front();
  }
  ++num_values_;
  UpdateCurrentValue(value);
}

void SlidingWindowMinimum::UpdateCurrentValue(float value) {
  assert(num_values_ >= 1);
  current_value_ = value;
  current_sliding_window_minimum_ =
      window_.empty() ? current_value_
                      : std::min(window_.front().first, current_value_);
}

}  // namespace trader
