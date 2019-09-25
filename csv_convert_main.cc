// Copyright © 2019 Peter Cerno. All rights reserved.

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include <gflags/gflags.h>

#include "trader_base.h"
#include "trader_io.h"
#include "trader_util.h"
#include "util_proto.h"
#include "util_time.h"

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
  if (!CheckPriceHistoryTimestamps(*price_history)) {
    std::cerr << "Price history timestamps are not sorted" << std::endl;
    return false;
  }
  HistoryGaps history_gaps = GetPriceHistoryGaps(
      *price_history, start_timestamp_sec, end_timestamp_sec,
      /* top_n = */ 10);
  std::cout << "Top 10 gaps:" << std::endl;
  for (const HistoryGap& history_gap : history_gaps) {
    const long gap_duration_sec = history_gap.second - history_gap.first;
    std::cout << "[" << ConvertTimestampSecToDateTimeUTC(history_gap.first)
              << " - " << ConvertTimestampSecToDateTimeUTC(history_gap.second)
              << "] Duration: " << DurationToString(gap_duration_sec)
              << std::endl;
  }
  return true;
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
  std::cerr << "Writing price history file: "
            << output_price_history_delimited_proto_file << std::endl;
  if (!WriteDelimitedMessagesToFile(
          price_history_subset.first, price_history_subset.second,
          output_price_history_delimited_proto_file, FLAGS_compress)) {
    std::cerr << "Failed to write price history file: "
              << output_price_history_delimited_proto_file << std::endl;
    return;
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Converted " << price_history.size() << " price records in "
            << duration.count() / 1000.0 << " seconds" << std::endl;
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
  std::vector<size_t> outlier_indices;
  PriceHistory price_history_clean = RemoveOutliers(
      price_history, FLAGS_max_price_deviation_per_min, &outlier_indices);
  std::cout << std::endl
            << "Removed " << outlier_indices.size() << " outliers" << std::endl;
  std::cout << "Last 10 outliers:" << std::endl;
  PrintOutliersWithContext(price_history, outlier_indices,
                           /* left_context_size = */ 5,
                           /* right_context_size = */ 5, /* last_n = */ 10);
  OhlcHistory ohlc_history =
      Resample(price_history_clean, start_timestamp_sec, end_timestamp_sec,
               FLAGS_sampling_rate_sec);
  if (!WriteDelimitedMessagesToFile(ohlc_history.begin(), ohlc_history.end(),
                                    output_ohlc_history_delimited_proto_file,
                                    FLAGS_compress)) {
    std::cerr << "Failed to write OHLC history file: "
              << output_ohlc_history_delimited_proto_file << std::endl;
    return;
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Converted " << price_history.size() << " price records to "
            << ohlc_history.size() << " OHLC records in "
            << duration.count() / 1000.0 << " seconds" << std::endl;
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
  const auto ohlc_history_subset =
      HistorySubset(ohlc_history, start_timestamp_sec, end_timestamp_sec);
  if (!WriteDelimitedMessagesToFile(
          ohlc_history_subset.first, ohlc_history_subset.second,
          output_ohlc_history_delimited_proto_file, FLAGS_compress)) {
    std::cerr << "Failed to write OHLC history file: "
              << output_ohlc_history_delimited_proto_file << std::endl;
    return;
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Converted " << ohlc_history.size() << " OHLC records in "
            << duration.count() / 1000.0 << " seconds" << std::endl;
}

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
