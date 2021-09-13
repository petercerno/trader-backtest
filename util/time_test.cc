// Copyright © 2021 Peter Cerno. All rights reserved.

#include "util/time.h"

#include "gtest/gtest.h"

namespace trader {
namespace {
const absl::TimeZone utc = absl::UTCTimeZone();
}  // namespace

TEST(ParseTimeTest, Basic_Y_m_d_Format) {
  absl::StatusOr<absl::Time> time_status = ParseTime("Hello World!");
  EXPECT_EQ(time_status.status(),
            absl::Status(absl::StatusCode::kInvalidArgument,
                         "Cannot parse datetime: Hello World!"));

  time_status = ParseTime("1970-01-01");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::FormatTime(time_status.value(), utc),
            "1970-01-01T00:00:00+00:00");
  EXPECT_EQ(FormatTimeUTC(time_status.value()), "1970-01-01 00:00:00");

  time_status = ParseTime("2000-02-29");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::ToUnixSeconds(time_status.value()), 951782400);
  EXPECT_EQ(FormatTimeUTC(time_status.value()), "2000-02-29 00:00:00");

  time_status = ParseTime("2017-01-01");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::ToUnixSeconds(time_status.value()), 1483228800);
  EXPECT_EQ(FormatTimeUTC(time_status.value()), "2017-01-01 00:00:00");

  time_status = ParseTime("2017-01-01 +02:00");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::ToUnixSeconds(time_status.value()), 1483221600);

  EXPECT_FALSE(ParseTime("2017-00-01").ok());
  EXPECT_FALSE(ParseTime("2017-13-01").ok());
  EXPECT_FALSE(ParseTime("2017-01-00").ok());
  EXPECT_FALSE(ParseTime("2017-01-32").ok());
}

TEST(ParseTimeTest, Extended_Y_m_d_H_M_S_Format) {
  absl::StatusOr<absl::Time> time_status = ParseTime("1970-01-01 00:00:00");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::FormatTime(time_status.value(), utc),
            "1970-01-01T00:00:00+00:00");

  time_status = ParseTime("2000-02-29 00:00:00");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::ToUnixSeconds(time_status.value()), 951782400);
  EXPECT_EQ(FormatTimeUTC(time_status.value()), "2000-02-29 00:00:00");

  time_status = ParseTime("2017-01-01 00:05:00");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::ToUnixSeconds(time_status.value()), 1483229100);
  EXPECT_EQ(FormatTimeUTC(time_status.value()), "2017-01-01 00:05:00");

  time_status = ParseTime("2017-01-01 16:25:15 +02:00");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::FormatTime(time_status.value(), utc),
            "2017-01-01T14:25:15+00:00");
  EXPECT_EQ(FormatTimeUTC(time_status.value()), "2017-01-01 14:25:15");

  EXPECT_FALSE(ParseTime("2017-01-01 24:00:00").ok());
  EXPECT_FALSE(ParseTime("2017-01-01 00:60:00").ok());
  EXPECT_FALSE(ParseTime("2017-01-01 00:00:70").ok());
}

TEST(ParseTimeTest, RFC3339_Format) {
  absl::StatusOr<absl::Time> time_status =
      ParseTime("2017-01-01T16:25:15+02:00");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::FormatTime(time_status.value(), utc),
            "2017-01-01T14:25:15+00:00");
  EXPECT_EQ(FormatTimeUTC(time_status.value()), "2017-01-01 14:25:15");
}

TEST(AddMonthsToTimeTest, Basic) {
  absl::StatusOr<absl::Time> time_status = ParseTime("2021-03-01");
  ASSERT_TRUE(time_status.ok());

  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), 0), utc),
            "2021-03-01T00:00:00+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1614556800, 0), 1614556800);
  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), 1), utc),
            "2021-04-01T00:00:00+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1614556800, 1), 1617235200);
  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), 15), utc),
            "2022-06-01T00:00:00+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1614556800, 15), 1654041600);
  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), -1), utc),
            "2021-02-01T00:00:00+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1614556800, -1), 1612137600);
  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), -3), utc),
            "2020-12-01T00:00:00+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1614556800, -3), 1606780800);

  time_status = ParseTime("2017-01-01T16:25:15+02:00");
  ASSERT_TRUE(time_status.ok());
  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), 0), utc),
            "2017-01-01T14:25:15+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1483280715, 0), 1483280715);
  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), 5), utc),
            "2017-06-01T14:25:15+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1483280715, 5), 1496327115);
  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), 12), utc),
            "2018-01-01T14:25:15+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1483280715, 12), 1514816715);
  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), -1), utc),
            "2016-12-01T14:25:15+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1483280715, -1), 1480602315);
  EXPECT_EQ(absl::FormatTime(AddMonthsToTime(time_status.value(), -12), utc),
            "2016-01-01T14:25:15+00:00");
  EXPECT_EQ(AddMonthsToTimestampSec(1483280715, -12), 1451658315);
}

}  // namespace trader
