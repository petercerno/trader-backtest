// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef INDICATORS_RELATIVE_STRENGTH_INDEX_H
#define INDICATORS_RELATIVE_STRENGTH_INDEX_H

#include "indicators/last_n_ohlc_ticks.h"
#include "indicators/util.h"
#include "lib/trader_base.h"

namespace trader {

// Calculates the Relative Strength Index (RSI) of the closing prices over
// all (previous) OHLC ticks with a specified period size (in seconds).
// We assume that the period is divisible by the period of update OHLC ticks.
// Upward change U and downward change D are calculated using an N-period
// smoothed or modified moving average (SMMA or MMA).
// Typically one uses N = 14 day periods.
// Based on: https://en.wikipedia.org/wiki/Relative_strength_index
//      and: https://www.investopedia.com/terms/r/rsi.asp
class RelativeStrengthIndex {
 public:
  // Constructor.
  // num_periods: Number N of periods over which we want to compute the RSI.
  // period_size_sec: Period of the OHLC ticks (in seconds).
  RelativeStrengthIndex(int num_periods, int period_size_sec);
  virtual ~RelativeStrengthIndex() {}

  // Returns the smoothed or modified moving average for the upward change U.
  virtual float GetUpwardChangeModifiedMovingAverage() const;
  // Returns the smoothed or modified moving average for the downward change D.
  virtual float GetDownwardChangeModifiedMovingAverage() const;

  // Returns Relative Strength Index (of closing prices) over all (previous)
  // OHLC ticks. This method runs in O(1) time.
  virtual float GetRelativeStrengthIndex() const;

  // Returns the number of seen OHLC ticks. This method runs in O(1) time.
  virtual int GetNumOhlcTicks() const;

  // Updates the Relative Strength Index.
  // This method has the same time complexity as the LastNOhlcTicks::Update
  // method, i.e. O(1) when the given OHLC tick is near the last OHLC tick.
  // We assume that period_size_sec is divisible by the period of ohlc_tick.
  virtual void Update(const OhlcTick& ohlc_tick);

 private:
  // Auxiliary method called when the last OHLC tick was updated.
  void LastTickUpdated();
  // Auxiliary method called when a new OHLC tick was added.
  void NewTickAdded();
  // Computes the weight for the smoothed or modified moving average.
  float GetModifiedMovingAverageWeight() const;
  // Computes the most recent upward change U and downward change D.
  std::pair<float, float> GetUpwardDownwardChange() const;

  // Number N of periods over which we want to compute the RSI.
  int num_periods_ = 0;
  // Keeps track of the current and the previous OHLC tick.
  LastNOhlcTicks last_n_ohlc_ticks_;

  // Number of observed OHLC ticks.
  int num_ohlc_ticks_ = 0;
  // Smoothed or modified moving average (MMA) for the upward change U.
  ExponentialMovingAverageHelper upward_change_mma_;
  // Smoothed or modified moving average (MMA) for the downward change D.
  ExponentialMovingAverageHelper downward_change_mma_;
};

}  // namespace trader

#endif  // INDICATORS_RELATIVE_STRENGTH_INDEX_H