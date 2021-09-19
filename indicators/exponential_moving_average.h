// Copyright © 2021 Peter Cerno. All rights reserved.

#ifndef INDICATORS_EXPONENTIAL_MOVING_AVERAGE_H
#define INDICATORS_EXPONENTIAL_MOVING_AVERAGE_H

#include "base/base.h"
#include "indicators/last_n_ohlc_ticks.h"
#include "indicators/util.h"

namespace trader {

// Calculates the Exponential Moving Average (EMA) of the closing prices over
// all (previous) OHLC ticks with a specified period size (in seconds).
// We assume that the period is divisible by the period of update OHLC ticks.
// Based on: https://www.investopedia.com/terms/m/movingaverage.asp
//      and: https://www.investopedia.com/terms/e/ema.asp
class ExponentialMovingAverage {
 public:
  // Constructor.
  // smoothing: Smoothing factor, the most common choice is 2.
  // ema_length: EMA length, typically 10, 50, or 200.
  // period_size_sec: Period of the OHLC ticks (in seconds).
  ExponentialMovingAverage(float smoothing, int ema_length,
                           int period_size_sec);
  virtual ~ExponentialMovingAverage() {}

  // Returns Exponential Moving Average (of closing prices) over all (previous)
  // OHLC ticks. This method runs in O(1) time.
  virtual float GetExponentialMovingAverage() const;

  // Returns the number of seen OHLC ticks. This method runs in O(1) time.
  virtual int GetNumOhlcTicks() const;

  // Updates the Exponential Moving Average.
  // This method has the same time complexity as the LastNOhlcTicks::Update
  // method, i.e. O(1) when the given OHLC tick is near the last OHLC tick.
  // We assume that period_size_sec is divisible by the period of ohlc_tick.
  virtual void Update(const OhlcTick& ohlc_tick);

 private:
  // Keeps track of the current OHLC tick.
  LastNOhlcTicks last_n_ohlc_ticks_;

  // Weight of the new closing prices in the Exponential Moving Average.
  float weight_ = 0;
  // Exponential Moving Average helper.
  ExponentialMovingAverageHelper ema_helper_;
};

}  // namespace trader

#endif  // INDICATORS_EXPONENTIAL_MOVING_AVERAGE_H
