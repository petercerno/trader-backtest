// Copyright © 2017 Peter Cerno. All rights reserved.

#include "util_time.h"

#include "gtest/gtest.h"

namespace trader {

TEST(ConvertDateUTCToTimestampSecTest, Basic) {
  long timestamp_sec = -1;
  ASSERT_FALSE(ConvertDateUTCToTimestampSec("", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_FALSE(ConvertDateUTCToTimestampSec("Hello World!", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_TRUE(ConvertDateUTCToTimestampSec("1970-01-01", &timestamp_sec));
  EXPECT_EQ(0, timestamp_sec);
  ASSERT_TRUE(ConvertDateUTCToTimestampSec("2000-02-29", &timestamp_sec));
  EXPECT_EQ(951782400, timestamp_sec);
  ASSERT_TRUE(ConvertDateUTCToTimestampSec("2017-01-01", &timestamp_sec));
  EXPECT_EQ(1483228800, timestamp_sec);
  ASSERT_TRUE(ConvertDateUTCToTimestampSec("2050-12-31", &timestamp_sec));
  EXPECT_EQ(2556057600, timestamp_sec);
}

TEST(AddMonthsToTimestampSec, Basic) {
  EXPECT_EQ(0, AddMonthsToTimestampSec(0, 0));
  EXPECT_EQ(1, AddMonthsToTimestampSec(1, 0));
  EXPECT_EQ(2678400, AddMonthsToTimestampSec(0, 1));
  EXPECT_EQ(5097600, AddMonthsToTimestampSec(0, 2));
  EXPECT_EQ(7776000, AddMonthsToTimestampSec(0, 3));
  EXPECT_EQ(28857600, AddMonthsToTimestampSec(0, 11));
  EXPECT_EQ(31536000, AddMonthsToTimestampSec(0, 12));
  EXPECT_EQ(2678410, AddMonthsToTimestampSec(10, 1));
  EXPECT_EQ(5097620, AddMonthsToTimestampSec(20, 2));
  EXPECT_EQ(7776030, AddMonthsToTimestampSec(30, 3));
  EXPECT_EQ(28857640, AddMonthsToTimestampSec(40, 11));
  EXPECT_EQ(31536050, AddMonthsToTimestampSec(50, 12));
  EXPECT_EQ(31536000, AddMonthsToTimestampSec(28857600, 1));
  EXPECT_EQ(34128000, AddMonthsToTimestampSec(31449600, 1));
  EXPECT_EQ(951782400, AddMonthsToTimestampSec(949276800, 1));
  EXPECT_EQ(954288000, AddMonthsToTimestampSec(951782400, 1));
}

}  // namespace trader
