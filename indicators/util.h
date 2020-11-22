// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef INDICATORS_UTIL_H
#define INDICATORS_UTIL_H

namespace trader {

// Calculates Exponential Moving Average (EMA) over the provided values.
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

}  // namespace trader

#endif  // INDICATORS_UTIL_H
