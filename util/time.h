// Copyright © 2021 Peter Cerno. All rights reserved.

// Note taken from: https://en.cppreference.com/w/cpp/chrono/c/time
// The encoding of calendar time in std::time_t is unspecified, but most systems
// conform to the POSIX specification and return a value of integral type
// holding 86400 times the number of calendar days since the Epoch plus the
// number of seconds that have passed since the last midnight UTC. Most notably,
// POSIX time does not (and can not) take leap seconds into account, so that
// this integral value is not equal to the number of S.I. seconds that have
// passed since the epoch, but rather is reduced with the number of leap seconds
// that have occurred since the epoch. Implementations in which std::time_t is a
// 32-bit signed integer (many historical implementations) fail in the year
// 2038.

#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include <ctime>
#include <string>

namespace trader {

// Converts date YYYY-MM-DD in UTC to UNIX timestamp (in seconds).
// Also supports YYYY-MM-DD hh:mm:ss format.
// If empty string is provided, sets the output timestamp_sec to 0.
// Returns true on success.
bool ConvertDateUTCToTimestampSec(const std::string& datetime_utc,
                                  std::time_t& timestamp_sec);

// Converts UNIX timestamp (in seconds) to date YYYY-MM-DD in UTC.
std::string ConvertTimestampSecToDateUTC(std::time_t timestamp_sec);

// Converts UNIX timestamp (in seconds) to datetime YYYY-MM-DD hh:mm:ss in UTC.
std::string ConvertTimestampSecToDateTimeUTC(std::time_t timestamp_sec);

// Converts [start_timestamp_sec, end_timestamp_sec) period to string.
std::string TimestampPeriodToString(std::time_t start_timestamp_sec,
                                    std::time_t end_timestamp_sec);

// Converts duration (in seconds) to string hh:mm:ss.
std::string DurationToString(long duration_sec);

// Adds months to UNIX timestamp (in seconds).
std::time_t AddMonthsToTimestampSec(std::time_t timestamp_sec, int months);

}  // namespace trader

#endif  // UTIL_TIME_H
