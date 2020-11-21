// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef INDICATORS_EXPONENTIAL_MOVING_AVERAGE_H
#define INDICATORS_EXPONENTIAL_MOVING_AVERAGE_H

#include "indicators/last_n_ohlc_ticks.h"
#include "lib/trader_base.h"

namespace trader {

// Calculates Exponential Moving Average (EMA) of the closing prices over
// all (previous) OHLC ticks with a specified period (in seconds).
// We assume that the period is divisible by the period of update OHLC ticks.
// Based on: https://www.investopedia.com/terms/m/movingaverage.asp
//      and: https://www.investopedia.com/terms/e/ema.asp
class ExponentialMovingAverage {
 public:
  // Called after the Exponential Moving Average was updated.
  // exponential_moving_average: The new updated Exponential Moving Average.
  // num_ohlc_ticks: Number of seen OHLC ticks over which the EMA is computed.
  using ExponentialMovingAverageUpdatedCallback = std::function<void(
      const float exponential_moving_average, int num_ohlc_ticks)>;

  // Constructor.
  // smoothing: Smoothing factor, the most common choice is 2.
  // ema_length: EMA length, typically 10, 50, or 200.
  // period_size_sec: Period of the OHLC ticks (in seconds).
  ExponentialMovingAverage(float smoothing, int ema_length,
                           int period_size_sec);
  virtual ~ExponentialMovingAverage() {}

  virtual void RegisterExponentialMovingAverageUpdatedCallback(
      ExponentialMovingAverageUpdatedCallback
          exponential_moving_average_updated_callback);

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

 protected:
  // Computes the Exponential Moving Average based on the current (most recent)
  // OHLC tick and the previous (stored) Exponential Moving Average.
  // ohlc_tick: The most recent OHLC tick.
  virtual float ComputeExponentialMovingAverage(
      const OhlcTick& ohlc_tick) const;

 private:
  // Keeps track of the current OHLC tick.
  LastNOhlcTicks last_n_ohlc_ticks_;

  // Smoothing factor.
  float smoothing_ = 0;
  // Exponential Moving Average length.
  int ema_length_ = 0;
  // Number of seen OHLC ticks over which the EMA is computed.
  int num_ohlc_ticks_ = 0;
  // Current Exponential Moving Average.
  float current_ema_ = 0;
  // Previous Exponential Moving Average.
  float previous_ema_ = 0;

  ExponentialMovingAverageUpdatedCallback
      exponential_moving_average_updated_callback_;
};

}  // namespace trader

#endif  // INDICATORS_EXPONENTIAL_MOVING_AVERAGE_H
