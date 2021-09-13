// Copyright © 2021 Peter Cerno. All rights reserved.

#include "util/time.h"

#include <type_traits>

#include "absl/strings/str_cat.h"

namespace trader {
namespace {
const char* const kAllowedTimeFormats[]{
    absl::RFC3339_sec,   "%Y-%m-%d %H:%M:%S %Ez",
    "%Y-%m-%d %H:%M:%S", "%Y-%m-%d %Ez",
    "%Y-%m-%d",
};
const size_t kNumberOfAllowedTimeFormats =
    std::extent<decltype(kAllowedTimeFormats)>::value;
}  // namespace

absl::StatusOr<absl::Time> ParseTime(absl::string_view datetime) {
  absl::Time time;
  for (size_t i = 0; i < kNumberOfAllowedTimeFormats; ++i) {
    if (absl::ParseTime(kAllowedTimeFormats[i], datetime, &time,
                        /*err=*/nullptr)) {
      return time;
    }
  }

  return absl::InvalidArgumentError(
      absl::StrCat("Cannot parse datetime: ", datetime));
}

std::string FormatTimeUTC(absl::Time time) {
  return absl::FormatTime("%Y-%m-%d %H:%M:%S", time, absl::UTCTimeZone());
}

absl::Time AddMonthsToTime(absl::Time time, int months) {
  const auto cs = absl::ToCivilSecond(time, absl::UTCTimeZone());
  return absl::FromCivil(
      absl::CivilSecond(cs.year(), cs.month() + months, cs.day(),  // nowrap
                        cs.hour(), cs.minute(), cs.second()),
      absl::UTCTimeZone());
}

int64_t AddMonthsToTimestampSec(int64_t timestamp_sec, int months) {
  return absl::ToUnixSeconds(
      AddMonthsToTime(absl::FromUnixSeconds(timestamp_sec), months));
}

}  // namespace trader
