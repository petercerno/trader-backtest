// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/util.h"

#include <cassert>

namespace trader {

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

}  // namespace trader
