// Copyright © 2019 Peter Cerno. All rights reserved.

#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include <string>

namespace trader {

// Converts date YYYY-MM-DD in UTC to UNIX timestamp (in seconds).
// Also supports YYYY-MM-DD hh:mm:ss format.
// If empty string is provided, sets the output timestamp_sec to 0.
// Returns true on success.
bool ConvertDateUTCToTimestampSec(const std::string& datetime_utc,
                                  long* timestamp_sec);

// Converts UNIX timestamp (in seconds) to date YYYY-MM-DD in UTC.
std::string ConvertTimestampSecToDateUTC(long timestamp_sec);

// Converts UNIX timestamp (in seconds) to datetime YYYY-MM-DD hh:mm:ss in UTC.
std::string ConvertTimestampSecToDateTimeUTC(long timestamp_sec);

// Converts duration (in seconds) to string hh:mm:ss.
std::string DurationToString(long duration_sec);

// Adds months to UNIX timestamp (in seconds).
long AddMonthsToTimestampSec(long timestamp_sec, int months);

}  // namespace trader

#endif  // UTIL_TIME_H
