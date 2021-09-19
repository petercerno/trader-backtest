// Copyright © 2021 Peter Cerno. All rights reserved.

#ifndef INDICATORS_MOVING_AVERAGE_CONVERGENCE_DIVERGENCE_H
#define INDICATORS_MOVING_AVERAGE_CONVERGENCE_DIVERGENCE_H

#include "base/base.h"
#include "indicators/last_n_ohlc_ticks.h"
#include "indicators/util.h"

namespace trader {

// Calculates the Moving Average Convergence/Divergence (MACD) of the closing
// prices over OHLC ticks with a specified period size (in seconds).
// We assume that the period is divisible by the period of update OHLC ticks.
// All Get methods run O(1) time.
// Based on: https://en.wikipedia.org/wiki/MACD
//      and: https://www.investopedia.com/terms/m/macd.asp
class MovingAverageConvergenceDivergence {
 public:
  // Constructor.
  // fast_length: Period for fast (short period) EMA (typically 12 days).
  // slow_length: Period for slow (long period) EMA (typically 26 days).
  // signal_smoothing: Period for signal (average) series (typically 9 days).
  // period_size_sec: Period of the OHLC ticks (in seconds).
  MovingAverageConvergenceDivergence(int fast_length, int slow_length,
                                     int signal_smoothing, int period_size_sec);
  virtual ~MovingAverageConvergenceDivergence() {}

  // Returns the fast Exponential Moving Average (over closing prices).
  virtual float GetFastExponentialMovingAverage() const;
  // Returns the slow Exponential Moving Average (over closing prices).
  virtual float GetSlowExponentialMovingAverage() const;
  // Returns the difference between "fast" (short period) and "slow" (longer
  // period) Exponential Moving Average (over closing prices).
  virtual float GetMACDSeries() const;
  // Returns the Exponential Moving Average of the MACD Series itself (with
  // signal smoothing period).
  virtual float GetMACDSignal() const;
  // Returns the difference between the MACD Series and the MACD Signal.
  virtual float GetDivergence() const;

  // Returns the number of seen OHLC ticks.
  virtual int GetNumOhlcTicks() const;

  // Updates the Moving Average Convergence/Divergence (MACD).
  // This method has the same time complexity as the LastNOhlcTicks::Update
  // method, i.e. O(1) when the given OHLC tick is near the last OHLC tick.
  // We assume that period_size_sec is divisible by the period of ohlc_tick.
  virtual void Update(const OhlcTick& ohlc_tick);

 private:
  // Auxiliary method called when the last OHLC tick was updated.
  void LastTickUpdated(const OhlcTick& ohlc_tick);
  // Auxiliary method called when a new OHLC tick was added.
  void NewTickAdded(const OhlcTick& ohlc_tick);

  // Keeps track of the current OHLC tick.
  LastNOhlcTicks last_n_ohlc_ticks_;

  // Number of observed OHLC ticks.
  int num_ohlc_ticks_ = 0;

  // Weight for the Fast Exponential Moving Average.
  float fast_weight_;
  // Weight for the Slow Exponential Moving Average.
  float slow_weight_;
  // Weight for the Exponential Moving Average of the MACD Series.
  float signal_weight_;

  // Fast Exponential Moving Average.
  ExponentialMovingAverageHelper fast_ema_;
  // Slow Exponential Moving Average.
  ExponentialMovingAverageHelper slow_ema_;
  // Exponential Moving Average of the MACD Series.
  ExponentialMovingAverageHelper signal_ema_;
};

}  // namespace trader

#endif  // INDICATORS_MOVING_AVERAGE_CONVERGENCE_DIVERGENCE_H
