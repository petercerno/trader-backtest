// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef INDICATORS_SIMPLE_MOVING_AVERAGE_H
#define INDICATORS_SIMPLE_MOVING_AVERAGE_H

#include "indicators/last_n_ohlc_ticks.h"
#include "lib/trader_base.h"

namespace trader {

// Calculates Simple Moving Average (SMA) of the closing prices over the last N
// OHLC ticks with a specified period (in seconds).
// We assume that the period is divisible by the period of update OHLC ticks.
// Based on: https://www.investopedia.com/terms/m/movingaverage.asp
//      and: https://www.investopedia.com/terms/s/sma.asp
class SimpleMovingAverage {
 public:
  // Called after the Simple Moving Average was updated.
  // simple_moving_average: The new updated Simple Moving Average.
  // num_ohlc_ticks: The number of OHLC ticks over which the SMA was computed.
  using SimpleMovingAverageUpdatedCallback = std::function<void(
      const float simple_moving_average, const int num_ohlc_ticks)>;

  // Constructor.
  // num_ohlc_ticks: Number N of OHLC ticks over which we want to compute SMA.
  // period_size_sec: Period of the OHLC ticks (in seconds).
  SimpleMovingAverage(int num_ohlc_ticks, int period_size_sec);
  virtual ~SimpleMovingAverage() {}

  virtual void RegisterSimpleMovingAverageUpdatedCallback(
      SimpleMovingAverageUpdatedCallback
          simple_moving_average_updated_callback);

  // Returns Simple Moving Average (of closing prices) over the last
  // (at most) N OHLC ticks (in the deque). This method runs in O(1) time.
  virtual float GetSimpleMovingAverage() const;

  // Returns the number of OHLC ticks (in the deque) over which the Simple
  // Moving Average is computed. This method runs in O(1) time.
  virtual int GetNumOhlcTicks() const;

  // Updates the Simple Moving Average (and the corresponding deque).
  // This method has the same time complexity as the LastNOhlcTicks::Update
  // method, i.e. O(1) when the given OHLC tick is near the last OHLC tick.
  // We assume that period_size_sec is divisible by the period of ohlc_tick.
  virtual void Update(const OhlcTick& ohlc_tick);

 private:
  // Keeps track of the last N OHLC ticks.
  LastNOhlcTicks last_n_ohlc_ticks_;

  // Sum of the closing prices over the last N OHLC ticks (in the deque).
  float sum_close_price_ = 0;

  SimpleMovingAverageUpdatedCallback simple_moving_average_updated_callback_;
};

}  // namespace trader

#endif  // INDICATORS_SIMPLE_MOVING_AVERAGE_H
