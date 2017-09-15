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
  timestamp_sec = -1;
  ASSERT_FALSE(ConvertDateUTCToTimestampSec("1000-01-01", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_FALSE(ConvertDateUTCToTimestampSec("2017-00-01", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_FALSE(ConvertDateUTCToTimestampSec("2017-13-01", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_FALSE(ConvertDateUTCToTimestampSec("2017-01-00", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_FALSE(ConvertDateUTCToTimestampSec("2017-01-32", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
}

TEST(ConvertDateUTCToTimestampSecTest, Extended) {
  long timestamp_sec = -1;
  ASSERT_FALSE(ConvertDateUTCToTimestampSec("1970-01-01 ", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_FALSE(ConvertDateUTCToTimestampSec(" 2000-02-29", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_TRUE(
      ConvertDateUTCToTimestampSec("1970-01-01 00:00:00", &timestamp_sec));
  EXPECT_EQ(0, timestamp_sec);
  ASSERT_TRUE(
      ConvertDateUTCToTimestampSec("2000-02-29 00:00:00", &timestamp_sec));
  EXPECT_EQ(951782400, timestamp_sec);
  ASSERT_TRUE(
      ConvertDateUTCToTimestampSec("2000-02-29 00:00:10", &timestamp_sec));
  EXPECT_EQ(951782410, timestamp_sec);
  ASSERT_TRUE(
      ConvertDateUTCToTimestampSec("2017-01-01 00:05:00", &timestamp_sec));
  EXPECT_EQ(1483229100, timestamp_sec);
  ASSERT_TRUE(
      ConvertDateUTCToTimestampSec("2050-12-31 01:00:00", &timestamp_sec));
  EXPECT_EQ(2556061200, timestamp_sec);
  timestamp_sec = -1;
  ASSERT_FALSE(
      ConvertDateUTCToTimestampSec("1970-01-01 24:00:00", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_FALSE(
      ConvertDateUTCToTimestampSec("1970-01-01 00:60:00", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
  ASSERT_FALSE(
      ConvertDateUTCToTimestampSec("1970-01-01 00:00:60", &timestamp_sec));
  EXPECT_EQ(-1, timestamp_sec);
}

TEST(ConvertTimestampSecToDateUTCTest, Basic) {
  EXPECT_EQ("1970-01-01", ConvertTimestampSecToDateUTC(0));
  EXPECT_EQ("2000-02-29", ConvertTimestampSecToDateUTC(951782400));
  EXPECT_EQ("2000-02-29", ConvertTimestampSecToDateUTC(951782700));
  EXPECT_EQ("2000-02-29", ConvertTimestampSecToDateUTC(951786000));
  EXPECT_EQ("2017-01-01", ConvertTimestampSecToDateUTC(1483228800));
  EXPECT_EQ("2017-01-01", ConvertTimestampSecToDateUTC(1483229100));
  EXPECT_EQ("2050-12-30", ConvertTimestampSecToDateUTC(2556057599));
  EXPECT_EQ("2050-12-31", ConvertTimestampSecToDateUTC(2556057600));
}

TEST(ConvertTimestampSecToDateTimeUTCTest, Basic) {
  EXPECT_EQ("1970-01-01 00:00:00", ConvertTimestampSecToDateTimeUTC(0));
  EXPECT_EQ("2000-02-29 00:00:00", ConvertTimestampSecToDateTimeUTC(951782400));
  EXPECT_EQ("2000-02-29 00:00:10", ConvertTimestampSecToDateTimeUTC(951782410));
  EXPECT_EQ("2017-01-01 00:05:00",
            ConvertTimestampSecToDateTimeUTC(1483229100));
  EXPECT_EQ("2050-12-31 01:00:00",
            ConvertTimestampSecToDateTimeUTC(2556061200));
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
