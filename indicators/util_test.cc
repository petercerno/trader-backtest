// Copyright © 2021 Peter Cerno. All rights reserved.

#include "indicators/util.h"

#include "gtest/gtest.h"

namespace trader {

TEST(SlidingWindowMeanAndVarianceTest, GetMeanAndVarianceWithoutWindow) {
  SlidingWindowMeanAndVariance sliding_window_mean(/*window_size=*/0);
  float m = 0;
  float v = 0;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 0);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 0);

  sliding_window_mean.AddNewValue(100.0f);
  m = 100.0f;
  v = 0;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.UpdateCurrentValue(200.0f);
  m = 200.0f;
  v = 0;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.AddNewValue(100.0f);
  m = (200.0f + 100.0f) / 2;
  v = (std::pow(200.0f - m, 2) + std::pow(100.0f - m, 2)) / 2;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 2);

  sliding_window_mean.UpdateCurrentValue(400.0f);
  m = (200.0f + 400.0f) / 2;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2)) / 2;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 2);

  sliding_window_mean.AddNewValue(300.0f);
  m = (200.0f + 400.0f + 300.0f) / 3;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(300.0f - m, 2)) /
      3;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 3);

  sliding_window_mean.UpdateCurrentValue(600.0f);
  m = (200.0f + 400.0f + 600.0f) / 3;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2)) /
      3;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 3);

  sliding_window_mean.AddNewValue(100.0f);
  m = (200.0f + 400.0f + 600.0f + 100.0f) / 4;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2) + std::pow(100.0f - m, 2)) /
      4;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 4);

  sliding_window_mean.UpdateCurrentValue(200.0f);
  m = (200.0f + 400.0f + 600.0f + 200.0f) / 4;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2) + std::pow(200.0f - m, 2)) /
      4;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 4);

  sliding_window_mean.AddNewValue(400.0f);
  m = (200.0f + 400.0f + 600.0f + 200.0f + 400.0f) / 5;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2) + std::pow(200.0f - m, 2) +
       std::pow(400.0f - m, 2)) /
      5;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 5);

  sliding_window_mean.UpdateCurrentValue(100.0f);
  m = (200.0f + 400.0f + 600.0f + 200.0f + 100.0f) / 5;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2) + std::pow(200.0f - m, 2) +
       std::pow(100.0f - m, 2)) /
      5;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 5);

  sliding_window_mean.AddNewValue(500.0f);
  m = (200.0f + 400.0f + 600.0f + 200.0f + 100.0f + 500.0f) / 6;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2) + std::pow(200.0f - m, 2) +
       std::pow(100.0f - m, 2) + std::pow(500.0f - m, 2)) /
      6;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 6);

  sliding_window_mean.UpdateCurrentValue(300.0f);
  m = (200.0f + 400.0f + 600.0f + 200.0f + 100.0f + 300.0f) / 6;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2) + std::pow(200.0f - m, 2) +
       std::pow(100.0f - m, 2) + std::pow(300.0f - m, 2)) /
      6;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 6);
}

TEST(SlidingWindowMeanAndVarianceTest, GetMeanAndVarianceWithWindowSize1) {
  SlidingWindowMeanAndVariance sliding_window_mean(/*window_size=*/1);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 0);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 0);

  sliding_window_mean.AddNewValue(100.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 100.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.UpdateCurrentValue(200.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 200.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.AddNewValue(100.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 100.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.UpdateCurrentValue(400.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 400.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.AddNewValue(300.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 300.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.UpdateCurrentValue(600.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 600.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.AddNewValue(100.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 100.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.UpdateCurrentValue(200.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 200.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.AddNewValue(400.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 400.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.UpdateCurrentValue(100.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 100.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.AddNewValue(500.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 500.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.UpdateCurrentValue(300.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), 300.0f);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), 0);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), 0);
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);
}

TEST(SlidingWindowMeanAndVarianceTest, GetMeanAndVarianceWithWindowSize4) {
  SlidingWindowMeanAndVariance sliding_window_mean(/*window_size=*/4);
  float m = 0;
  float v = 0;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 0);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 0);

  sliding_window_mean.AddNewValue(100.0f);
  m = 100.0f;
  v = 0;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.UpdateCurrentValue(200.0f);
  m = 200.0f;
  v = 0;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 1);

  sliding_window_mean.AddNewValue(100.0f);
  m = (200.0f + 100.0f) / 2;
  v = (std::pow(200.0f - m, 2) + std::pow(100.0f - m, 2)) / 2;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 2);

  sliding_window_mean.UpdateCurrentValue(400.0f);
  m = (200.0f + 400.0f) / 2;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2)) / 2;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 2);

  sliding_window_mean.AddNewValue(300.0f);
  m = (200.0f + 400.0f + 300.0f) / 3;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(300.0f - m, 2)) /
      3;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 3);

  sliding_window_mean.UpdateCurrentValue(600.0f);
  m = (200.0f + 400.0f + 600.0f) / 3;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2)) /
      3;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 3);

  sliding_window_mean.AddNewValue(100.0f);
  m = (200.0f + 400.0f + 600.0f + 100.0f) / 4;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2) + std::pow(100.0f - m, 2)) /
      4;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 4);

  sliding_window_mean.UpdateCurrentValue(200.0f);
  m = (200.0f + 400.0f + 600.0f + 200.0f) / 4;
  v = (std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2) +
       std::pow(600.0f - m, 2) + std::pow(200.0f - m, 2)) /
      4;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 4);

  sliding_window_mean.AddNewValue(400.0f);
  m = (400.0f + 600.0f + 200.0f + 400.0f) / 4;
  v = (std::pow(400.0f - m, 2) + std::pow(600.0f - m, 2) +
       std::pow(200.0f - m, 2) + std::pow(400.0f - m, 2)) /
      4;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 4);

  sliding_window_mean.UpdateCurrentValue(100.0f);
  m = (400.0f + 600.0f + 200.0f + 100.0f) / 4;
  v = (std::pow(400.0f - m, 2) + std::pow(600.0f - m, 2) +
       std::pow(200.0f - m, 2) + std::pow(100.0f - m, 2)) /
      4;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 4);

  sliding_window_mean.AddNewValue(500.0f);
  m = (600.0f + 200.0f + 100.0f + 500.0f) / 4;
  v = (std::pow(600.0f - m, 2) + std::pow(200.0f - m, 2) +
       std::pow(100.0f - m, 2) + std::pow(500.0f - m, 2)) /
      4;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 4);

  sliding_window_mean.UpdateCurrentValue(300.0f);
  m = (600.0f + 200.0f + 100.0f + 300.0f) / 4;
  v = (std::pow(600.0f - m, 2) + std::pow(200.0f - m, 2) +
       std::pow(100.0f - m, 2) + std::pow(300.0f - m, 2)) /
      4;
  EXPECT_FLOAT_EQ(sliding_window_mean.GetMean(), m);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetVariance(), v);
  EXPECT_FLOAT_EQ(sliding_window_mean.GetStandardDeviation(), std::sqrt(v));
  EXPECT_EQ(sliding_window_mean.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_mean.GetWindowSize(), 4);
}

TEST(ExponentialMovingAverageHelperTest, GetExponentialMovingAverage) {
  ExponentialMovingAverageHelper ema_helper;
  EXPECT_FLOAT_EQ(ema_helper.GetExponentialMovingAverage(), 0);
  EXPECT_EQ(ema_helper.GetNumValues(), 0);

  ema_helper.AddNewValue(100.0f, 0.1f);
  EXPECT_FLOAT_EQ(ema_helper.GetExponentialMovingAverage(), 100.0f);
  EXPECT_EQ(ema_helper.GetNumValues(), 1);

  ema_helper.UpdateCurrentValue(200.0f, 0.1f);
  EXPECT_FLOAT_EQ(ema_helper.GetExponentialMovingAverage(), 200.0f);
  EXPECT_EQ(ema_helper.GetNumValues(), 1);

  ema_helper.AddNewValue(100.0f, 0.1f);
  EXPECT_FLOAT_EQ(ema_helper.GetExponentialMovingAverage(),
                  0.1f * 100.0f + 0.9f * 200.0f);
  EXPECT_EQ(ema_helper.GetNumValues(), 2);

  ema_helper.UpdateCurrentValue(400.0f, 0.1f);
  EXPECT_FLOAT_EQ(ema_helper.GetExponentialMovingAverage(),
                  0.1f * 400.0f + 0.9f * 200.0f);
  EXPECT_EQ(ema_helper.GetNumValues(), 2);

  ema_helper.AddNewValue(100.0f, 0.1f);
  EXPECT_FLOAT_EQ(ema_helper.GetExponentialMovingAverage(),
                  0.1f * 100.0f + 0.9f * (0.1f * 400.0f + 0.9f * 200.0f));
  EXPECT_EQ(ema_helper.GetNumValues(), 3);

  ema_helper.UpdateCurrentValue(200.0f, 0.1f);
  EXPECT_FLOAT_EQ(ema_helper.GetExponentialMovingAverage(),
                  0.1f * 200.0f + 0.9f * (0.1f * 400.0f + 0.9f * 200.0f));
  EXPECT_EQ(ema_helper.GetNumValues(), 3);
}

TEST(SlidingWindowMinimumTest, GetSlidingWindowMinimum) {
  SlidingWindowMinimum sliding_window_minimum(/*window_size=*/5);
  // Values: []
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 0);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 0);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 0);

  sliding_window_minimum.AddNewValue(100);
  // Values: [100]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 100);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 1);

  sliding_window_minimum.UpdateCurrentValue(50);
  // Values: [50]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 50);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 1);

  sliding_window_minimum.UpdateCurrentValue(150);
  // Values: [150]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 150);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 1);

  sliding_window_minimum.AddNewValue(100);
  // Values: [150, 100]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 100);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 2);

  sliding_window_minimum.UpdateCurrentValue(50);
  // Values: [150, 50]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 50);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 2);

  sliding_window_minimum.UpdateCurrentValue(200);
  // Values: [150, 200]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 150);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 2);

  sliding_window_minimum.AddNewValue(100);
  // Values: [150, 200, 100]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 100);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 3);

  sliding_window_minimum.UpdateCurrentValue(300);
  // Values: [150, 200, 300]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 150);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 3);

  sliding_window_minimum.AddNewValue(100);
  // Values: [150, 200, 300, 100]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 100);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 4);

  sliding_window_minimum.UpdateCurrentValue(250);
  // Values: [150, 200, 300, 250]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 150);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 4);

  sliding_window_minimum.AddNewValue(400);
  // Values: [150, 200, 300, 250, 400]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 150);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);

  sliding_window_minimum.UpdateCurrentValue(100);
  // Values: [150, 200, 300, 250, 100]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 100);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);

  sliding_window_minimum.UpdateCurrentValue(500);
  // Values: [150, 200, 300, 250, 500]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 150);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);

  sliding_window_minimum.AddNewValue(100);
  // Values: 150, [200, 300, 250, 500, 100]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 100);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);

  sliding_window_minimum.UpdateCurrentValue(400);
  // Values: 150, [200, 300, 250, 500, 400]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 200);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);

  sliding_window_minimum.AddNewValue(600);
  // Values: 150, 200, [300, 250, 500, 400, 600]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 250);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 7);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);

  sliding_window_minimum.UpdateCurrentValue(200);
  // Values: 150, 200, [300, 250, 500, 400, 200]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 200);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 7);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);

  sliding_window_minimum.AddNewValue(600);
  // Values: 150, 200, 300, [250, 500, 400, 200, 600]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 200);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 8);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);

  sliding_window_minimum.UpdateCurrentValue(100);
  // Values: 150, 200, 300, [250, 500, 400, 200, 100]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 100);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 8);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);

  sliding_window_minimum.UpdateCurrentValue(500);
  // Values: 150, 200, 300, [250, 500, 400, 200, 500]
  EXPECT_FLOAT_EQ(sliding_window_minimum.GetSlidingWindowMinimum(), 200);
  EXPECT_EQ(sliding_window_minimum.GetNumValues(), 8);
  EXPECT_EQ(sliding_window_minimum.GetWindowSize(), 5);
}

TEST(SlidingWindowMaximumTest, GetSlidingWindowMaximum) {
  SlidingWindowMaximum sliding_window_maximum(/*window_size=*/5);
  // Values: []
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 0);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 0);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 0);

  sliding_window_maximum.AddNewValue(100);
  // Values: [100]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 100);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 1);

  sliding_window_maximum.UpdateCurrentValue(50);
  // Values: [50]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 50);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 1);

  sliding_window_maximum.UpdateCurrentValue(150);
  // Values: [150]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 150);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 1);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 1);

  sliding_window_maximum.AddNewValue(100);
  // Values: [150, 100]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 150);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 2);

  sliding_window_maximum.UpdateCurrentValue(50);
  // Values: [150, 50]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 150);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 2);

  sliding_window_maximum.UpdateCurrentValue(200);
  // Values: [150, 200]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 200);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 2);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 2);

  sliding_window_maximum.AddNewValue(100);
  // Values: [150, 200, 100]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 200);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 3);

  sliding_window_maximum.UpdateCurrentValue(300);
  // Values: [150, 200, 300]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 300);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 3);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 3);

  sliding_window_maximum.AddNewValue(100);
  // Values: [150, 200, 300, 100]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 300);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 4);

  sliding_window_maximum.UpdateCurrentValue(250);
  // Values: [150, 200, 300, 250]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 300);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 4);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 4);

  sliding_window_maximum.AddNewValue(400);
  // Values: [150, 200, 300, 250, 400]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 400);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);

  sliding_window_maximum.UpdateCurrentValue(100);
  // Values: [150, 200, 300, 250, 100]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 300);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);

  sliding_window_maximum.UpdateCurrentValue(500);
  // Values: [150, 200, 300, 250, 500]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 500);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 5);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);

  sliding_window_maximum.AddNewValue(100);
  // Values: 150, [200, 300, 250, 500, 100]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 500);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);

  sliding_window_maximum.UpdateCurrentValue(400);
  // Values: 150, [200, 300, 250, 500, 400]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 500);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 6);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);

  sliding_window_maximum.AddNewValue(600);
  // Values: 150, 200, [300, 250, 500, 400, 600]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 600);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 7);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);

  sliding_window_maximum.UpdateCurrentValue(200);
  // Values: 150, 200, [300, 250, 500, 400, 200]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 500);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 7);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);

  sliding_window_maximum.AddNewValue(600);
  // Values: 150, 200, 300, [250, 500, 400, 200, 600]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 600);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 8);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);

  sliding_window_maximum.UpdateCurrentValue(100);
  // Values: 150, 200, 300, [250, 500, 400, 200, 100]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 500);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 8);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);

  sliding_window_maximum.UpdateCurrentValue(500);
  // Values: 150, 200, 300, [250, 500, 400, 200, 500]
  EXPECT_FLOAT_EQ(sliding_window_maximum.GetSlidingWindowMaximum(), 500);
  EXPECT_EQ(sliding_window_maximum.GetNumValues(), 8);
  EXPECT_EQ(sliding_window_maximum.GetWindowSize(), 5);
}

}  // namespace trader
