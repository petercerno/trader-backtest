// Copyright © 2021 Peter Cerno. All rights reserved.

#include "util/time.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <sstream>

// According to https://en.cppreference.com/w/cpp/chrono/c/mktime
// std::mktime converts local calendar time to a time since epoch as a time_t
// object. However, we need to convert UTC calendar time, for which there is no
// convenient std function. Thus we have decided to use a non-standard (i.e. not
// in POSIX or C standard) function timegm. This function is defined as
// _mkgmtime in Windows.
#ifdef _WIN32
#define timegm _mkgmtime
#endif

namespace trader {
namespace {
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
                                  std::time_t& timestamp_sec) {
  if (datetime_utc.empty()) {
    timestamp_sec = 0;
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
  timestamp_sec = timegm(&tm);
  return true;
}

std::string ConvertTimestampSecToDateUTC(std::time_t timestamp_sec) {
  std::stringstream ss;
  ss << std::put_time(std::gmtime(&timestamp_sec), "%Y-%m-%d");
  return ss.str();
}

std::string ConvertTimestampSecToDateTimeUTC(std::time_t timestamp_sec) {
  std::stringstream ss;
  ss << std::put_time(std::gmtime(&timestamp_sec), "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

std::string TimestampPeriodToString(std::time_t start_timestamp_sec,
                                    std::time_t end_timestamp_sec) {
  std::stringstream ss;
  const std::string start_str =
      start_timestamp_sec > 0
          ? ConvertTimestampSecToDateTimeUTC(start_timestamp_sec)
          : "BEGIN";
  const std::string end_str =
      end_timestamp_sec > 0
          ? ConvertTimestampSecToDateTimeUTC(end_timestamp_sec)
          : "END";
  ss << "[" << start_str << " - " << end_str << ")";
  return ss.str();
}

std::string DurationToString(long duration_sec) {
  const long hours = duration_sec / 3600;
  const long minutes = (duration_sec / 60) % 60;
  const long seconds = duration_sec % 60;
  std::stringstream ss;
  ss << hours << ":"                                         // nowrap
     << std::setfill('0') << std::setw(2) << minutes << ":"  // nowrap
     << std::setfill('0') << std::setw(2) << seconds;
  return ss.str();
}

std::time_t AddMonthsToTimestampSec(std::time_t timestamp_sec, int months) {
  struct std::tm tm(AddMonthsToTm(*std::gmtime(&timestamp_sec), months));
  return timegm(&tm);
}

}  // namespace trader
