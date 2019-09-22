// Copyright © 2019 Peter Cerno. All rights reserved.

#ifndef TRADER_IO_H
#define TRADER_IO_H

#include <string>
#include <vector>

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

// Writes exchange account states to the given csv file.
bool WriteExchangeAccountStatesToCsvFile(
    const ExchangeAccountStates& exchange_account_states,
    const std::string& file_name);

// Prints the top n largest (chronologically sorted) price history gaps within
// the interval [start_timestamp_sec, end_timestamp_sec).
void PrintPriceHistoryGaps(const PriceHistory& price_history,
                           long start_timestamp_sec, long end_timestamp_sec,
                           size_t top_n);

// Prints a subset of price_history that covers the last_n outliers.
// Every outlier is surrounded by left_context_size of previous prices (if
// possible) and right_context_size of follow-up prices (if possible).
void PrintOutliersWithContext(const PriceHistory& price_history,
                              const std::vector<size_t>& outlier_indices,
                              size_t left_context_size,
                              size_t right_context_size, size_t last_n);

// Prints some exchange account states statistics.
void PrintExchangeAccountStatesStats(
    const ExchangeAccountStates& exchange_account_states);

// Prints top_n evaluation results.
void PrintEvalResults(const EvalResults& eval_results, size_t top_n);

}  // namespace trader

#endif  // TRADER_IO_H
