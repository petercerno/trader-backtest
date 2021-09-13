// Copyright © 2021 Peter Cerno. All rights reserved.

#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"

namespace trader {

// Parses the input datetime string to absl::Time.
// Supports ISO 8601 format for date and time with UTC offset.
// Supported formats:
//   %Y-%m-%d (defaults to UTC timezone)
//   %Y-%m-%d %Ez (%Ez is RFC3339-compatible UTC offset (+hh:mm or -hh:mm))
//   %Y-%m-%d %H:%M:%S (defaults to UTC timezone)
//   %Y-%m-%d %H:%M:%S %Ez  (e.g. 1970-01-01 00:00:00 +00:00)
//   %Y-%m-%d%ET%H:%M:%S%Ez (e.g. 1970-01-01T00:00:00+00:00)
absl::StatusOr<absl::Time> ParseTime(absl::string_view datetime);

// Returns time formated as: %Y-%m-%d %H:%M:%S (in the UTC timezone).
std::string FormatTimeUTC(absl::Time time);

// Adds specified number of months to the given absl::Time.
absl::Time AddMonthsToTime(absl::Time time, int months);

// Adds specified number of months to the given UNIX timestamp (in sec).
int64_t AddMonthsToTimestampSec(int64_t timestamp_sec, int months);

}  // namespace trader

#endif  // UTIL_TIME_H
