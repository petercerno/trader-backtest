// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef INDICATORS_VOLATILITY_H
#define INDICATORS_VOLATILITY_H

#include "base/base.h"
#include "indicators/last_n_ohlc_ticks.h"
#include "indicators/util.h"

namespace trader {

// Calculates the volatility of the portfolio, i.e. the standard deviation of
// the (daily) logarithmic returns.
// We assume that the period is divisible by the period of update OHLC ticks.
// Based on: https://en.wikipedia.org/wiki/Volatility_(finance)
class Volatility {
 public:
  // Constructor.
  // window_size: Size of the sliding window. Ignored if zero.
  // period_size_sec: Period of the OHLC ticks (in seconds). Typically daily.
  Volatility(int window_size, int period_size_sec);
  virtual ~Volatility() {}

  // Returns the standard deviation of the logarithmic returns.
  virtual float GetVolatility() const;

  // Returns the number of seen OHLC ticks. This method runs in O(1) time.
  virtual int GetNumOhlcTicks() const;

  // Updates volatility based on the latest OHLC tick and portfolio balances.
  // This method has the same time complexity as the LastNOhlcTicks::Update
  // method, i.e. O(1) when the given OHLC tick is near the last OHLC tick.
  // We assume that period_size_sec is divisible by the period of ohlc_tick.
  virtual void Update(const OhlcTick& ohlc_tick, float base_balance,
                      float quote_balance);

 private:
  // Returns the logarithmic return w.r.t. the previous portfolio value.
  float GetLogarithmicReturn() const;

  // Latest portfolio balances.
  float latest_base_balance_ = 0;
  float latest_quote_balance_ = 0;
  // Keeps track of the current and the previous OHLC tick.
  LastNOhlcTicks last_n_ohlc_ticks_;
  // Keeps track of the current and the previous portfolio value.
  // At the beginning we use the opening price of the very first OHLC tick
  // to estimate the previous portfolio value.
  std::deque<float> portfolio_value_;

  // Number of observed OHLC ticks.
  int num_ohlc_ticks_ = 0;

  // Sliding window for computing the variance of the logarithmic returns.
  SlidingWindowMeanAndVariance sliding_window_variance_;
};

}  // namespace trader

#endif  // INDICATORS_VOLATILITY_H