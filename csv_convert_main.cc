// Copyright © 2020 Peter Cerno. All rights reserved.

#include <gflags/gflags.h>

#include "lib/trader_base.h"
#include "lib/trader_history.h"
#include "util/util_proto.h"
#include "util/util_time.h"

// Input/Output files.
DEFINE_string(input_price_history_csv_file, "",
              "Input CSV file containing the historical prices.");
DEFINE_string(output_price_history_delimited_proto_file, "",
              "Output file containing the delimited PriceRecord protos.");

DEFINE_string(input_ohlc_history_csv_file, "",
              "Input CSV file containing the OHLC prices.");
DEFINE_string(output_ohlc_history_delimited_proto_file, "",
              "Output file containing the delimited OhlcRecord protos.");

// Time period and sampling.
DEFINE_string(start_date_utc, "", "Start date YYYY-MM-DD in UTC (included).");
DEFINE_string(end_date_utc, "", "End date YYYY-MM-DD in UTC (excluded).");

// These are used only when converting price history to OHLC history.
DEFINE_double(max_price_deviation_per_min, 0.05,
              "Maximum allowed price deviation per minute.");
DEFINE_int32(sampling_rate_sec, 300, "Sampling rate in seconds.");

// Output compression.
DEFINE_bool(compress, true, "Whether to compress the output protobuf file.");

using namespace trader;

namespace {
// Reads the input CSV file containing the historical prices and saves them
// into the given price_history vector.
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

// Reads the input CSV file containing the OHLC prices and saves them into the
// given ohlc_history vector.
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

// Reads the price history CSV file and outputs some stats (top gaps etc.).
bool ReadPriceHistory(const std::string& input_price_history_csv_file,
                      long start_timestamp_sec, long end_timestamp_sec,
                      PriceHistory* price_history) {
  std::cout << "Reading price history CSV file: "
            << input_price_history_csv_file << std::endl;
  if (!ReadPriceHistoryFromCsvFile(input_price_history_csv_file,
                                   price_history)) {
    std::cerr << "Failed to read price history CSV file: "
              << input_price_history_csv_file << std::endl;
    return false;
  }
  std::cout << "Loaded " << price_history->size() << " records" << std::endl;
  if (!CheckPriceHistoryTimestamps(*price_history)) {
    std::cerr << "Price history timestamps are not sorted" << std::endl;
    return false;
  }
  return true;
}

// Prints the top_n largest (chronologically sorted) price history gaps.
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

// Prints a subset of the given price history that covers the last_n outliers.
// Every outlier is surrounded by left_context_size of previous prices (if
// possible) and right_context_size of follow-up prices (if possible).
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

// Converts the price history CSV file to a delimited protobuf file.
// TODO: Remove all references to FLAGS_.
void ConvertPriceHistory(
    const std::string& input_price_history_csv_file,
    const std::string& output_price_history_delimited_proto_file,
    long start_timestamp_sec, long end_timestamp_sec) {
  auto start = std::chrono::high_resolution_clock::now();
  PriceHistory price_history;
  if (!ReadPriceHistory(input_price_history_csv_file, start_timestamp_sec,
                        end_timestamp_sec, &price_history)) {
    return;
  }
  const auto price_history_subset =
      HistorySubset(price_history, start_timestamp_sec, end_timestamp_sec);
  std::cout << "Selected "
            << std::distance(price_history_subset.first,
                             price_history_subset.second)
            << " records within the period: "
            << TimestampPeriodToString(start_timestamp_sec, end_timestamp_sec)
            << std::endl;
  std::cout << "Top 10 gaps:" << std::endl;
  PrintPriceHistoryGaps(/* begin = */ price_history_subset.first,
                        /* end = */ price_history_subset.second,
                        start_timestamp_sec, end_timestamp_sec,
                        /* top_n = */ 10);
  std::cerr << "Writing "
            << std::distance(price_history_subset.first,
                             price_history_subset.second)
            << " records to " << output_price_history_delimited_proto_file
            << std::endl;
  if (!WriteDelimitedMessagesToFile(
          price_history_subset.first, price_history_subset.second,
          output_price_history_delimited_proto_file, FLAGS_compress)) {
    std::cerr << "Failed to write price history file: "
              << output_price_history_delimited_proto_file << std::endl;
    return;
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Finished in " << duration.count() / 1000.0 << " seconds"
            << std::endl;
}

// Reads the price history CSV file, removes outliers, resamples the price
// history into OHLC history, and writes the OHLC history to a delimited
// protobuf file.
// TODO: Remove all references to FLAGS_.
void ConvertPriceHistoryToOhlcHistory(
    const std::string& input_price_history_csv_file,
    const std::string& output_ohlc_history_delimited_proto_file,
    long start_timestamp_sec, long end_timestamp_sec) {
  auto start = std::chrono::high_resolution_clock::now();
  PriceHistory price_history;
  if (!ReadPriceHistory(input_price_history_csv_file, start_timestamp_sec,
                        end_timestamp_sec, &price_history)) {
    return;
  }
  const auto price_history_subset =
      HistorySubset(price_history, start_timestamp_sec, end_timestamp_sec);
  std::cout << "Selected "
            << std::distance(price_history_subset.first,
                             price_history_subset.second)
            << " records within the period: "
            << TimestampPeriodToString(start_timestamp_sec, end_timestamp_sec)
            << std::endl;
  std::cout << "Top 10 gaps:" << std::endl;
  PrintPriceHistoryGaps(/* begin = */ price_history_subset.first,
                        /* end = */ price_history_subset.second,
                        start_timestamp_sec, end_timestamp_sec,
                        /* top_n = */ 10);
  std::vector<size_t> outlier_indices;
  PriceHistory price_history_clean = RemoveOutliers(
      /* begin = */ price_history_subset.first,
      /* end = */ price_history_subset.second,
      FLAGS_max_price_deviation_per_min, &outlier_indices);
  std::cout << "Removed " << outlier_indices.size() << " outliers" << std::endl;
  std::cout << "Last 10 outliers:" << std::endl;
  PrintOutliersWithContext(/* begin = */ price_history_subset.first,
                           /* end = */ price_history_subset.second,
                           outlier_indices,
                           /* left_context_size = */ 5,
                           /* right_context_size = */ 5, /* last_n = */ 10);
  OhlcHistory ohlc_history =
      Resample(price_history_clean.begin(), price_history_clean.end(),
               FLAGS_sampling_rate_sec);
  std::cout << "Resampled " << price_history_clean.size() << " records to "
            << ohlc_history.size() << " OHLC ticks" << std::endl;
  std::cerr << "Writing " << ohlc_history.size() << " OHLC ticks to "
            << output_ohlc_history_delimited_proto_file << std::endl;
  if (!WriteDelimitedMessagesToFile(ohlc_history.begin(), ohlc_history.end(),
                                    output_ohlc_history_delimited_proto_file,
                                    FLAGS_compress)) {
    std::cerr << "Failed to write OHLC history file: "
              << output_ohlc_history_delimited_proto_file << std::endl;
    return;
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Finished in " << duration.count() / 1000.0 << " seconds"
            << std::endl;
}

// Converts the OHLC history CSV file to delimited protobuf file.
// TODO: Remove all references to FLAGS_.
void ConvertOhlcHistory(
    const std::string& input_ohlc_history_csv_file,
    const std::string& output_ohlc_history_delimited_proto_file,
    long start_timestamp_sec, long end_timestamp_sec) {
  OhlcHistory ohlc_history;
  auto start = std::chrono::high_resolution_clock::now();
  std::cout << "Reading OHLC history CSV file: " << input_ohlc_history_csv_file
            << std::endl;
  if (!ReadOhlcHistoryFromCsvFile(input_ohlc_history_csv_file, &ohlc_history)) {
    std::cerr << "Failed to read OHLC history CSV file: "
              << input_ohlc_history_csv_file << std::endl;
    return;
  }
  std::cout << "Loaded " << ohlc_history.size() << " OHLC ticks" << std::endl;
  const auto ohlc_history_subset =
      HistorySubset(ohlc_history, start_timestamp_sec, end_timestamp_sec);
  std::cout << "Selected "
            << std::distance(ohlc_history_subset.first,
                             ohlc_history_subset.second)
            << " OHLC ticks within the period: "
            << TimestampPeriodToString(start_timestamp_sec, end_timestamp_sec)
            << std::endl;
  std::cerr << "Writing "
            << std::distance(ohlc_history_subset.first,
                             ohlc_history_subset.second)
            << " OHLC ticks to " << output_ohlc_history_delimited_proto_file
            << std::endl;
  if (!WriteDelimitedMessagesToFile(
          ohlc_history_subset.first, ohlc_history_subset.second,
          output_ohlc_history_delimited_proto_file, FLAGS_compress)) {
    std::cerr << "Failed to write OHLC history file: "
              << output_ohlc_history_delimited_proto_file << std::endl;
    return;
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Finished in " << duration.count() / 1000.0 << " seconds"
            << std::endl;
}
}  // namespace

int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  long start_timestamp_sec = 0;
  long end_timestamp_sec = 0;
  if (!ConvertDateUTCToTimestampSec(FLAGS_start_date_utc,
                                    &start_timestamp_sec)) {
    std::cerr << "Invalid start date: " << FLAGS_start_date_utc << std::endl;
    return 1;
  }
  if (!ConvertDateUTCToTimestampSec(FLAGS_end_date_utc, &end_timestamp_sec)) {
    std::cerr << "Invalid end date: " << FLAGS_end_date_utc << std::endl;
    return 1;
  }

  if (!FLAGS_input_price_history_csv_file.empty() &&
      !FLAGS_output_price_history_delimited_proto_file.empty()) {
    ConvertPriceHistory(FLAGS_input_price_history_csv_file,
                        FLAGS_output_price_history_delimited_proto_file,
                        start_timestamp_sec, end_timestamp_sec);
  }

  if (!FLAGS_input_ohlc_history_csv_file.empty() &&
      !FLAGS_output_ohlc_history_delimited_proto_file.empty()) {
    ConvertOhlcHistory(FLAGS_input_ohlc_history_csv_file,
                       FLAGS_output_ohlc_history_delimited_proto_file,
                       start_timestamp_sec, end_timestamp_sec);
  }

  if (!FLAGS_input_price_history_csv_file.empty() &&
      FLAGS_input_ohlc_history_csv_file.empty() &&
      !FLAGS_output_ohlc_history_delimited_proto_file.empty()) {
    ConvertPriceHistoryToOhlcHistory(
        FLAGS_input_price_history_csv_file,
        FLAGS_output_ohlc_history_delimited_proto_file, start_timestamp_sec,
        end_timestamp_sec);
  }

  // Optional: Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
