// Copyright © 2017 Peter Cerno. All rights reserved.

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

// Writes exchange account states to the given csv file.
bool WriteExchangeAccountStatesToCsvFile(
    const ExchangeAccountStates& exchange_account_states,
    const std::string& file_name);

// Prints some exchange account states statistics.
void PrintExchangeAccountStatesStats(
    const ExchangeAccountStates& exchange_account_states);

// Prints top_n evaluation results.
void PrintEvalResults(const EvalResults& eval_results, size_t top_n);

}  // namespace trader

#endif  // TRADER_IO_H
