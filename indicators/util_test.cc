// Copyright © 2020 Peter Cerno. All rights reserved.

#include "indicators/util.h"

#include "gtest/gtest.h"

namespace trader {

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

}  // namespace trader