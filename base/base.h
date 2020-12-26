// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef BASE_BASE_H
#define BASE_BASE_H

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <deque>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "base/base.pb.h"

namespace trader {

constexpr int kSecondsPerMinute = 60;
constexpr int kSecondsPerHour = 60 * 60;
constexpr int kSecondsPerDay = 24 * 60 * 60;
constexpr int kSecondsPerWeek = 7 * 24 * 60 * 60;

// Historical prices over time.
using PriceHistory = std::vector<PriceRecord>;

// Historical OHLC ticks over time.
using OhlcHistory = std::vector<OhlcTick>;

// Historical side inputs.
using SideHistory = std::vector<SideInputRecord>;

// Returns a pair of iterators covering the time interval [start_timestamp_sec,
// end_timestamp_sec) of the given history.
template <typename T>
std::pair<typename std::vector<T>::const_iterator,
          typename std::vector<T>::const_iterator>
HistorySubset(const std::vector<T>& history, long start_timestamp_sec,
              long end_timestamp_sec) {
  const auto record_compare = [](const T& record, long timestamp_sec) {
    return record.timestamp_sec() < timestamp_sec;
  };
  const auto record_begin =
      start_timestamp_sec > 0
          ? std::lower_bound(history.begin(), history.end(),
                             start_timestamp_sec, record_compare)
          : history.begin();
  const auto record_end =
      end_timestamp_sec > 0
          ? std::lower_bound(history.begin(), history.end(), end_timestamp_sec,
                             record_compare)
          : history.end();
  return {record_begin, record_end};
}

// Returns the subset of the given history covering the time interval
// [start_timestamp_sec, end_timestamp_sec).
template <typename T>
std::vector<T> HistorySubsetCopy(const std::vector<T>& history,
                                 long start_timestamp_sec,
                                 long end_timestamp_sec) {
  std::vector<T> history_subset_copy;
  const auto history_subset =
      HistorySubset(history, start_timestamp_sec, end_timestamp_sec);
  const auto size = std::distance(history_subset.first, history_subset.second);
  if (size == 0) {
    return {};
  }
  history_subset_copy.reserve(size);
  std::copy(history_subset.first, history_subset.second,
            std::back_inserter(history_subset_copy));
  return history_subset_copy;
}

}  // namespace trader

#endif  // BASE_BASE_H
