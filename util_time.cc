// Copyright © 2019 Peter Cerno. All rights reserved.

#include "util_time.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace trader {
namespace {
// UNIX epoch, 00:00:00 UTC, Thursday, 1 January 1970.
static const std::chrono::system_clock::time_point kEpoch =
    std::chrono::system_clock::from_time_t(0);

// Converts struct tm to UNIX timestamp (in seconds).
long ConvertTmToTimestampSec(struct std::tm* const tm) {
  const std::chrono::system_clock::time_point time =
      std::chrono::system_clock::from_time_t(timegm(tm));
  return std::chrono::duration_cast<std::chrono::seconds>(time - kEpoch)
      .count();
}

// Converts UNIX timestamp (in seconds) to struct tm.
struct std::tm ConvertTimestampSecToTm(long timestamp_sec) {
  struct std::tm tm_result = {0};
  std::time_t time = std::chrono::system_clock::to_time_t(
      kEpoch + std::chrono::seconds(timestamp_sec));
  gmtime_r(&time, &tm_result);
  return tm_result;
}

// Returns true if the given year is a leap year.
bool IsLeapYear(int year) {
  if (year % 4 != 0) return false;
  if (year % 400 == 0) return true;
  if (year % 100 == 0) return false;
  return true;
}

// Returns number of days in a month.
int GetDaysInMonth(int year, int month) {
  static const int kDaysInMonths[] = {31, 28, 31, 30, 31, 30,
                                      31, 31, 30, 31, 30, 31};
  assert(month >= 0);
  assert(month < 12);
  int days = kDaysInMonths[month];
  if (month == 1 && IsLeapYear(year)) {
    // February of a leap year.
    days += 1;
  }
  return days;
}

// Adds months to struct tm.
std::tm AddMonthsToTm(const struct std::tm& tm, int months) {
  int tm_year = tm.tm_year + months / 12;
  int tm_mon = tm.tm_mon + months % 12;
  if (tm_mon > 11) {
    tm_year += 1;
    tm_mon -= 12;
  }
  int tm_mday = std::min(tm.tm_mday, GetDaysInMonth(tm_year + 1900, tm_mon));
  struct std::tm tm_result = {0};
  tm_result.tm_year = tm_year;
  tm_result.tm_mon = tm_mon;
  tm_result.tm_mday = tm_mday;
  tm_result.tm_hour = tm.tm_hour;
  tm_result.tm_min = tm.tm_min;
  tm_result.tm_sec = tm.tm_sec;
  return tm_result;
}
}  // namespace

bool ConvertDateUTCToTimestampSec(const std::string& datetime_utc,
                                  long* timestamp_sec) {
  assert(timestamp_sec != nullptr);  // Undefined timestamp_sec
  if (datetime_utc.empty()) {
    *timestamp_sec = 0;
    return true;
  }
  struct std::tm tm = {0};
  std::string date_parse{datetime_utc};
  if (datetime_utc.length() == 10) {
    std::replace(date_parse.begin(), date_parse.end(), '-', ' ');
    std::istringstream iss(date_parse);
    if (!(iss >> tm.tm_year >> tm.tm_mon >> tm.tm_mday)) {
      // Cannot parse datetime_utc.
      return false;
    }
  } else if (datetime_utc.length() == 19) {
    std::replace(date_parse.begin(), date_parse.end(), '-', ' ');
    std::replace(date_parse.begin(), date_parse.end(), ':', ' ');
    std::istringstream iss(date_parse);
    if (!(iss >> tm.tm_year >> tm.tm_mon >> tm.tm_mday >> tm.tm_hour >>
          tm.tm_min >> tm.tm_sec)) {
      // Cannot parse datetime_utc.
      return false;
    }
  } else {
    return false;
  }
  if (tm.tm_year < 1900 ||                  // nowrap
      tm.tm_mon < 1 || tm.tm_mon > 12 ||    // nowrap
      tm.tm_mday < 1 || tm.tm_mday > 31 ||  // nowrap
      tm.tm_hour < 0 || tm.tm_hour > 23 ||  // nowrap
      tm.tm_min < 0 || tm.tm_min > 59 ||    // nowrap
      tm.tm_sec < 0 || tm.tm_sec > 59) {
    // Invalid format.
    return false;
  }
  tm.tm_mon -= 1;
  tm.tm_year -= 1900;
  *timestamp_sec = ConvertTmToTimestampSec(&tm);
  return true;
}

std::string ConvertTimestampSecToDateUTC(long timestamp_sec) {
  struct std::tm tm(ConvertTimestampSecToTm(timestamp_sec));
  std::stringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%d");
  return ss.str();
}

std::string ConvertTimestampSecToDateTimeUTC(long timestamp_sec) {
  struct std::tm tm(ConvertTimestampSecToTm(timestamp_sec));
  std::stringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

long AddMonthsToTimestampSec(long timestamp_sec, int months) {
  struct std::tm tm(
      AddMonthsToTm(ConvertTimestampSecToTm(timestamp_sec), months));
  return ConvertTmToTimestampSec(&tm);
}

}  // namespace trader
