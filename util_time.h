// Copyright © 2017 Peter Cerno. All rights reserved.

#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include <string>

namespace trader {

// Converts date YYYY-MM-DD in UTC to UNIX timestamp (in seconds).
bool ConvertDateUTCToTimestampSec(const std::string& date_utc,
                                  long* timestamp_sec);

// Converts UNIX timestamp (in seconds) to date YYYY-MM-DD in UTC.
std::string ConvertTimestampSecToDateUTC(long timestamp_sec);

// Adds months to UNIX timestamp (in seconds).
long AddMonthsToTimestampSec(long timestamp_sec, int months);

}  // namespace trader

#endif  // UTIL_TIME_H
