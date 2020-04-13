// Copyright © 2020 Peter Cerno. All rights reserved.

#include "trader_io.h"

#include "trader_util.h"
#include "util_time.h"

namespace trader {

bool ReadPriceHistoryFromCsvFile(const std::string& file_name,
                                 PriceHistory* price_history) {
  if (price_history == nullptr) {
    std::cerr << "Undefined price_history" << std::endl;
    return false;
  }
  std::ifstream infile(file_name);
  if (!infile.is_open()) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  int row = 0;
  std::string line;
  int timestamp_sec;
  float price;
  float volume;
  while (std::getline(infile, line)) {
    ++row;
    std::replace(line.begin(), line.end(), ',', ' ');
    std::istringstream iss(line);
    if (!(iss >> timestamp_sec >> price >> volume)) {
      std::cerr << "Cannot parse line " << row << ": " << line << std::endl;
      return false;
    }
    price_history->emplace_back();
    price_history->back().set_timestamp_sec(timestamp_sec);
    price_history->back().set_price(price);
    price_history->back().set_volume(volume);
  }
  return true;
}

bool ReadOhlcHistoryFromCsvFile(const std::string& file_name,
                                OhlcHistory* ohlc_history) {
  if (ohlc_history == nullptr) {
    std::cerr << "Undefined ohlc_history" << std::endl;
    return false;
  }
  std::ifstream infile(file_name);
  if (!infile.is_open()) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  int row = 0;
  std::string line;
  int timestamp_sec;
  float open;
  float high;
  float low;
  float close;
  float volume;
  while (std::getline(infile, line)) {
    ++row;
    std::replace(line.begin(), line.end(), ',', ' ');
    std::istringstream iss(line);
    if (!(iss >> timestamp_sec >> open >> high >> low >> close >> volume)) {
      std::cerr << "Cannot parse line " << row << ": " << line << std::endl;
      return false;
    }
    ohlc_history->emplace_back();
    ohlc_history->back().set_timestamp_sec(timestamp_sec);
    ohlc_history->back().set_open(open);
    ohlc_history->back().set_high(high);
    ohlc_history->back().set_low(low);
    ohlc_history->back().set_close(close);
    ohlc_history->back().set_volume(volume);
  }
  return true;
}

void PrintPriceHistoryGaps(PriceHistory::const_iterator begin,
                           PriceHistory::const_iterator end,
                           long start_timestamp_sec, long end_timestamp_sec,
                           size_t top_n) {
  std::vector<HistoryGap> history_gaps = GetPriceHistoryGaps(
      begin, end, start_timestamp_sec, end_timestamp_sec, top_n);
  for (const HistoryGap& history_gap : history_gaps) {
    const long gap_duration_sec = history_gap.second - history_gap.first;
    std::cout << history_gap.first << " ["
              << ConvertTimestampSecToDateTimeUTC(history_gap.first) << "] - "
              << history_gap.second << " ["
              << ConvertTimestampSecToDateTimeUTC(history_gap.second)
              << "]: " << DurationToString(gap_duration_sec) << std::endl;
  }
}

void PrintOutliersWithContext(PriceHistory::const_iterator begin,
                              PriceHistory::const_iterator end,
                              const std::vector<size_t>& outlier_indices,
                              size_t left_context_size,
                              size_t right_context_size, size_t last_n) {
  const size_t price_history_size = std::distance(begin, end);
  std::map<size_t, bool> index_to_outlier = GetOutlierIndicesWithContext(
      outlier_indices, price_history_size, left_context_size,
      right_context_size, last_n);
  size_t index_prev = 0;
  for (const auto& index_outlier_pair : index_to_outlier) {
    const size_t index = index_outlier_pair.first;
    const bool is_outlier = index_outlier_pair.second;
    assert(index < price_history_size);
    const PriceRecord& price_record = *(begin + index);
    if (index_prev > 0 && index > index_prev + 1) {
      std::cout << "   ..." << std::endl;
    }
    std::cout << (is_outlier ? " x " : "   ")  // nowrap
              << price_record.timestamp_sec()  // nowrap
              << " ["                          // nowrap
              << ConvertTimestampSecToDateTimeUTC(price_record.timestamp_sec())
              << "]: "                               // nowrap
              << std::fixed << std::setprecision(2)  // nowrap
              << price_record.price() << " ["        // nowrap
              << std::fixed << std::setprecision(4)  // nowrap
              << price_record.volume() << "]"        // nowrap
              << std::endl;
    index_prev = index;
  }
}

void PrintTraderEvalResults(
    const std::vector<TraderEvaluationResult>& trader_eval_results,
    size_t top_n) {
  const size_t eval_count = std::min(top_n, trader_eval_results.size());
  for (int eval_index = 0; eval_index < eval_count; ++eval_index) {
    const TraderEvaluationResult& trader_eval_result =
        trader_eval_results.at(eval_index);
    std::cout << trader_eval_result.trader_name() << ": "
              << trader_eval_result.score() << std::endl;
  }
}

}  // namespace trader
