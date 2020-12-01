// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/util.h"

#include "gtest/gtest.h"

namespace trader {

TEST(SimpleMovingAverageHelperTest, GetSimpleMovingAverage) {
  SimpleMovingAverageHelper sma_helper(/*window_size=*/4);
  EXPECT_FLOAT_EQ(0, sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(0, sma_helper.GetNumValues());

  sma_helper.AddNewValue(100.0f);
  EXPECT_FLOAT_EQ(100.0f, sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(1, sma_helper.GetNumValues());

  sma_helper.UpdateCurrentValue(200.0f);
  EXPECT_FLOAT_EQ(200.0f, sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(1, sma_helper.GetNumValues());

  sma_helper.AddNewValue(100.0f);
  EXPECT_FLOAT_EQ((200.0f + 100.0f) / 2, sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(2, sma_helper.GetNumValues());

  sma_helper.UpdateCurrentValue(400.0f);
  EXPECT_FLOAT_EQ((200.0f + 400.0f) / 2, sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(2, sma_helper.GetNumValues());

  sma_helper.AddNewValue(300.0f);
  EXPECT_FLOAT_EQ((200.0f + 400.0f + 300.0f) / 3,
                  sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(3, sma_helper.GetNumValues());

  sma_helper.UpdateCurrentValue(600.0f);
  EXPECT_FLOAT_EQ((200.0f + 400.0f + 600.0f) / 3,
                  sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(3, sma_helper.GetNumValues());

  sma_helper.AddNewValue(100.0f);
  EXPECT_FLOAT_EQ((200.0f + 400.0f + 600.0f + 100.0f) / 4,
                  sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(4, sma_helper.GetNumValues());

  sma_helper.UpdateCurrentValue(200.0f);
  EXPECT_FLOAT_EQ((200.0f + 400.0f + 600.0f + 200.0f) / 4,
                  sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(4, sma_helper.GetNumValues());

  sma_helper.AddNewValue(400.0f);
  EXPECT_FLOAT_EQ((400.0f + 600.0f + 200.0f + 400.0f) / 4,
                  sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(5, sma_helper.GetNumValues());

  sma_helper.UpdateCurrentValue(100.0f);
  EXPECT_FLOAT_EQ((400.0f + 600.0f + 200.0f + 100.0f) / 4,
                  sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(5, sma_helper.GetNumValues());

  sma_helper.AddNewValue(500.0f);
  EXPECT_FLOAT_EQ((600.0f + 200.0f + 100.0f + 500.0f) / 4,
                  sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(6, sma_helper.GetNumValues());

  sma_helper.UpdateCurrentValue(300.0f);
  EXPECT_FLOAT_EQ((600.0f + 200.0f + 100.0f + 300.0f) / 4,
                  sma_helper.GetSimpleMovingAverage());
  EXPECT_EQ(6, sma_helper.GetNumValues());
}

TEST(ExponentialMovingAverageHelperTest, GetExponentialMovingAverage) {
  ExponentialMovingAverageHelper ema_helper;
  EXPECT_FLOAT_EQ(0, ema_helper.GetExponentialMovingAverage());
  EXPECT_EQ(0, ema_helper.GetNumValues());

  ema_helper.AddNewValue(100.0f, 0.1f);
  EXPECT_FLOAT_EQ(100.0f, ema_helper.GetExponentialMovingAverage());
  EXPECT_EQ(1, ema_helper.GetNumValues());

  ema_helper.UpdateCurrentValue(200.0f, 0.1f);
  EXPECT_FLOAT_EQ(200.0f, ema_helper.GetExponentialMovingAverage());
  EXPECT_EQ(1, ema_helper.GetNumValues());

  ema_helper.AddNewValue(100.0f, 0.1f);
  EXPECT_FLOAT_EQ(0.1f * 100.0f + 0.9f * 200.0f,
                  ema_helper.GetExponentialMovingAverage());
  EXPECT_EQ(2, ema_helper.GetNumValues());

  ema_helper.UpdateCurrentValue(400.0f, 0.1f);
  EXPECT_FLOAT_EQ(0.1f * 400.0f + 0.9f * 200.0f,
                  ema_helper.GetExponentialMovingAverage());
  EXPECT_EQ(2, ema_helper.GetNumValues());

  ema_helper.AddNewValue(100.0f, 0.1f);
  EXPECT_FLOAT_EQ(0.1f * 100.0f + 0.9f * (0.1f * 400.0f + 0.9f * 200.0f),
                  ema_helper.GetExponentialMovingAverage());
  EXPECT_EQ(3, ema_helper.GetNumValues());

  ema_helper.UpdateCurrentValue(200.0f, 0.1f);
  EXPECT_FLOAT_EQ(0.1f * 200.0f + 0.9f * (0.1f * 400.0f + 0.9f * 200.0f),
                  ema_helper.GetExponentialMovingAverage());
  EXPECT_EQ(3, ema_helper.GetNumValues());
}

TEST(SlidingWindowMinimumTest, GetSlidingWindowMinimum) {
  SlidingWindowMinimum sliding_window_minimum(/*window_size=*/5);
  // Values: []
  EXPECT_FLOAT_EQ(0, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(0, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(0, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.AddNewValue(100);
  // Values: [100]
  EXPECT_FLOAT_EQ(100, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(1, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(1, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(50);
  // Values: [50]
  EXPECT_FLOAT_EQ(50, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(1, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(1, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(150);
  // Values: [150]
  EXPECT_FLOAT_EQ(150, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(1, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(1, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.AddNewValue(100);
  // Values: [150, 100]
  EXPECT_FLOAT_EQ(100, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(2, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(2, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(50);
  // Values: [150, 50]
  EXPECT_FLOAT_EQ(50, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(2, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(2, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(200);
  // Values: [150, 200]
  EXPECT_FLOAT_EQ(150, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(2, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(2, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.AddNewValue(100);
  // Values: [150, 200, 100]
  EXPECT_FLOAT_EQ(100, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(3, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(3, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(300);
  // Values: [150, 200, 300]
  EXPECT_FLOAT_EQ(150, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(3, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(3, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.AddNewValue(100);
  // Values: [150, 200, 300, 100]
  EXPECT_FLOAT_EQ(100, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(4, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(4, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(250);
  // Values: [150, 200, 300, 250]
  EXPECT_FLOAT_EQ(150, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(4, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(4, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.AddNewValue(400);
  // Values: [150, 200, 300, 250, 400]
  EXPECT_FLOAT_EQ(150, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(5, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(100);
  // Values: [150, 200, 300, 250, 100]
  EXPECT_FLOAT_EQ(100, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(5, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(500);
  // Values: [150, 200, 300, 250, 500]
  EXPECT_FLOAT_EQ(150, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(5, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.AddNewValue(100);
  // Values: 150, [200, 300, 250, 500, 100]
  EXPECT_FLOAT_EQ(100, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(6, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(400);
  // Values: 150, [200, 300, 250, 500, 400]
  EXPECT_FLOAT_EQ(200, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(6, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.AddNewValue(600);
  // Values: 150, 200, [300, 250, 500, 400, 600]
  EXPECT_FLOAT_EQ(250, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(7, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(200);
  // Values: 150, 200, [300, 250, 500, 400, 200]
  EXPECT_FLOAT_EQ(200, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(7, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.AddNewValue(600);
  // Values: 150, 200, 300, [250, 500, 400, 200, 600]
  EXPECT_FLOAT_EQ(200, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(8, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(100);
  // Values: 150, 200, 300, [250, 500, 400, 200, 100]
  EXPECT_FLOAT_EQ(100, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(8, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());

  sliding_window_minimum.UpdateCurrentValue(500);
  // Values: 150, 200, 300, [250, 500, 400, 200, 500]
  EXPECT_FLOAT_EQ(200, sliding_window_minimum.GetSlidingWindowMinimum());
  EXPECT_EQ(8, sliding_window_minimum.GetNumValues());
  EXPECT_EQ(5, sliding_window_minimum.GetWindowSize());
}

TEST(SlidingWindowMaximumTest, GetSlidingWindowMaximum) {
  SlidingWindowMaximum sliding_window_maximum(/*window_size=*/5);
  // Values: []
  EXPECT_FLOAT_EQ(0, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(0, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(0, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.AddNewValue(100);
  // Values: [100]
  EXPECT_FLOAT_EQ(100, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(1, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(1, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(50);
  // Values: [50]
  EXPECT_FLOAT_EQ(50, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(1, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(1, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(150);
  // Values: [150]
  EXPECT_FLOAT_EQ(150, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(1, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(1, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.AddNewValue(100);
  // Values: [150, 100]
  EXPECT_FLOAT_EQ(150, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(2, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(2, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(50);
  // Values: [150, 50]
  EXPECT_FLOAT_EQ(150, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(2, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(2, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(200);
  // Values: [150, 200]
  EXPECT_FLOAT_EQ(200, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(2, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(2, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.AddNewValue(100);
  // Values: [150, 200, 100]
  EXPECT_FLOAT_EQ(200, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(3, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(3, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(300);
  // Values: [150, 200, 300]
  EXPECT_FLOAT_EQ(300, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(3, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(3, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.AddNewValue(100);
  // Values: [150, 200, 300, 100]
  EXPECT_FLOAT_EQ(300, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(4, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(4, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(250);
  // Values: [150, 200, 300, 250]
  EXPECT_FLOAT_EQ(300, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(4, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(4, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.AddNewValue(400);
  // Values: [150, 200, 300, 250, 400]
  EXPECT_FLOAT_EQ(400, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(5, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(100);
  // Values: [150, 200, 300, 250, 100]
  EXPECT_FLOAT_EQ(300, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(5, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(500);
  // Values: [150, 200, 300, 250, 500]
  EXPECT_FLOAT_EQ(500, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(5, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.AddNewValue(100);
  // Values: 150, [200, 300, 250, 500, 100]
  EXPECT_FLOAT_EQ(500, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(6, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(400);
  // Values: 150, [200, 300, 250, 500, 400]
  EXPECT_FLOAT_EQ(500, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(6, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.AddNewValue(600);
  // Values: 150, 200, [300, 250, 500, 400, 600]
  EXPECT_FLOAT_EQ(600, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(7, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(200);
  // Values: 150, 200, [300, 250, 500, 400, 200]
  EXPECT_FLOAT_EQ(500, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(7, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.AddNewValue(600);
  // Values: 150, 200, 300, [250, 500, 400, 200, 600]
  EXPECT_FLOAT_EQ(600, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(8, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(100);
  // Values: 150, 200, 300, [250, 500, 400, 200, 100]
  EXPECT_FLOAT_EQ(500, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(8, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());

  sliding_window_maximum.UpdateCurrentValue(500);
  // Values: 150, 200, 300, [250, 500, 400, 200, 500]
  EXPECT_FLOAT_EQ(500, sliding_window_maximum.GetSlidingWindowMaximum());
  EXPECT_EQ(8, sliding_window_maximum.GetNumValues());
  EXPECT_EQ(5, sliding_window_maximum.GetWindowSize());
}

}  // namespace trader