// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef INDICATORS_STOCHASTIC_OSCILLATOR_H
#define INDICATORS_STOCHASTIC_OSCILLATOR_H

#include "base/base.h"
#include "indicators/last_n_ohlc_ticks.h"
#include "indicators/util.h"

namespace trader {

// Calculates the Stochastic Oscillator (SO) based on the closing prices over
// the last N OHLC ticks with a specified period size (in seconds).
// Typical values for N are 5, 9, or 14 periods.
// We assume that the period is divisible by the period of update OHLC ticks.
// All Get methods run O(1) time.
// Based on: https://en.wikipedia.org/wiki/Stochastic_oscillator
//      and: https://www.investopedia.com/terms/s/stochasticoscillator.asp
class StochasticOscillator {
 public:
  // Constructor.
  // num_periods: Number N of periods over which we want to compute the SO.
  // period_size_sec: Period of the OHLC ticks (in seconds).
  StochasticOscillator(int num_periods, int period_size_sec);
  virtual ~StochasticOscillator() {}

  // Returns the lowest price over the last N OHLC ticks.
  virtual float GetLow() const;

  // Returns the highest price over the last N OHLC ticks.
  virtual float GetHigh() const;

  // Returns %K := 100 * (Price - Low_N) / (High_N - Low_N).
  virtual float GetK() const;

  // Returns %D-Fast := 3-period simple moving average of %K.
  virtual float GetFastD() const;

  // Returns %D-Slow := 3-period simple moving average of %D-Fast.
  virtual float GetSlowD() const;

  // Returns the number of seen OHLC ticks.
  virtual int GetNumOhlcTicks() const;

  // Updates the Stochastic Oscillator.
  // This method has the same time complexity as the LastNOhlcTicks::Update
  // method, i.e. O(1) when the given OHLC tick is near the last OHLC tick.
  // We assume that period_size_sec is divisible by the period of ohlc_tick.
  virtual void Update(const OhlcTick& ohlc_tick);

 private:
  // Auxiliary method called when the last OHLC tick was updated.
  void LastTickUpdated(const OhlcTick& ohlc_tick);
  // Auxiliary method called when a new OHLC tick was added.
  void NewTickAdded(const OhlcTick& ohlc_tick);
  // Updates the most recent %K based on the latest price.
  void UpdateK(const float latest_price);

  // Keeps track of the current OHLC tick.
  LastNOhlcTicks last_n_ohlc_ticks_;

  // Number of observed OHLC ticks.
  int num_ohlc_ticks_ = 0;
  // Most recent %K := 100 * (Price - Low_N) / (High_N - Low_N).
  float latest_k_ = 0;

  SlidingWindowMinimum sliding_window_min_;
  SlidingWindowMaximum sliding_window_max_;
  SimpleMovingAverageHelper sma_d_fast_;
  SimpleMovingAverageHelper sma_d_slow_;
};

}  // namespace trader

#endif  // INDICATORS_STOCHASTIC_OSCILLATOR_H