// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef TRADER_UTIL_H
#define TRADER_UTIL_H

#include "trader_base.h"

namespace trader {

// Returns true iff the price history has non-decreasing timestamps.
bool CheckPriceHistoryTimestamps(const PriceHistory& price_history);

// Gap in the price history, represented as a pair of timestamps (in seconds).
using HistoryGap = std::pair<long, long>;

// Returns the top_n largest (chronologically sorted) price history gaps.
std::vector<HistoryGap> GetPriceHistoryGaps(PriceHistory::const_iterator begin,
                                            PriceHistory::const_iterator end,
                                            long start_timestamp_sec,
                                            long end_timestamp_sec,
                                            size_t top_n);

// Returns price history with removed outliers.
// max_price_deviation_per_min is maximum allowed price deviation per minute.
// outlier_indices is an optional output vector of removed outlier indices.
PriceHistory RemoveOutliers(PriceHistory::const_iterator begin,
                            PriceHistory::const_iterator end,
                            float max_price_deviation_per_min,
                            std::vector<size_t>* outlier_indices);

// Returns a map from price_history indices to booleans indicating whether the
// indices correspond to outliers or not (taking the last_n outlier_indices).
// Every outlier has left_context_size indices to the left (if possible) and
// right_context_size indices to the right (if possible).
// price_history_size is the size of the original price history.
std::map<size_t, bool> GetOutlierIndicesWithContext(
    const std::vector<size_t>& outlier_indices, size_t price_history_size,
    size_t left_context_size, size_t right_context_size, size_t last_n);

// Returns the resampled ohlc_history with the given sampling rate (in seconds).
OhlcHistory Resample(PriceHistory::const_iterator begin,
                     PriceHistory::const_iterator end, int sampling_rate_sec);

}  // namespace trader

#endif  // TRADER_UTIL_H
