// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef TRADER_IO_H
#define TRADER_IO_H

#include "trader_base.h"

namespace trader {

// Reads the input CSV file containing the historical prices and saves them
// into the given price_history vector.
bool ReadPriceHistoryFromCsvFile(const std::string& file_name,
                                 PriceHistory* price_history);

// Reads the input CSV file containing the OHLC prices and saves them into the
// given ohlc_history vector.
bool ReadOhlcHistoryFromCsvFile(const std::string& file_name,
                                OhlcHistory* ohlc_history);

// Prints the top_n largest (chronologically sorted) price history gaps.
void PrintPriceHistoryGaps(PriceHistory::const_iterator begin,
                           PriceHistory::const_iterator end,
                           long start_timestamp_sec, long end_timestamp_sec,
                           size_t top_n);

// Prints a subset of the given price history that covers the last_n outliers.
// Every outlier is surrounded by left_context_size of previous prices (if
// possible) and right_context_size of follow-up prices (if possible).
void PrintOutliersWithContext(PriceHistory::const_iterator begin,
                              PriceHistory::const_iterator end,
                              const std::vector<size_t>& outlier_indices,
                              size_t left_context_size,
                              size_t right_context_size, size_t last_n);

// Prints top_n evaluation results.
void PrintTraderEvalResults(
    const std::vector<TraderEvaluationResult>& trader_eval_results,
    size_t top_n);

}  // namespace trader

#endif  // TRADER_IO_H
