// Copyright © 2019 Peter Cerno. All rights reserved.

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include <gflags/gflags.h>

#include "trader_base.h"
#include "trader_io.h"
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

// Time period.
DEFINE_string(start_date_utc, "", "Start date YYYY-MM-DD in UTC (included).");
DEFINE_string(end_date_utc, "", "End date YYYY-MM-DD in UTC (excluded).");

// Output compression.
DEFINE_bool(compress, true, "Whether to compress the output file.");

using namespace trader;

// Converts the price history csv file if enabled by flags.
void ConvertPriceHistoryIfPossible(long start_timestamp_sec,
                                   long end_timestamp_sec) {
  const std::string& input_file = FLAGS_input_price_history_csv_file;
  const std::string& output_file =
      FLAGS_output_price_history_delimited_proto_file;
  if (input_file.empty() || output_file.empty()) {
    return;
  }
  PriceHistory price_history;
  auto start = std::chrono::high_resolution_clock::now();
  std::cout << "Reading price history CSV file: " << input_file << std::endl;
  if (!ReadPriceHistoryFromCsvFile(input_file, &price_history)) {
    std::cerr << "Failed to read price history CSV file: " << input_file
              << std::endl;
    return;
  }
  const auto price_history_subset =
      HistorySubset(price_history, start_timestamp_sec, end_timestamp_sec);
  std::cerr << "Writing price history file: " << output_file << std::endl;
  if (!WriteDelimitedMessagesToFile(price_history_subset.first,
                                    price_history_subset.second, output_file,
                                    FLAGS_compress)) {
    std::cerr << "Failed to write price history file: " << output_file
              << std::endl;
    return;
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Converted " << price_history.size() << " price records in "
            << duration.count() / 1000.0 << " seconds" << std::endl;
}

// Converts the OHLC history csv file if enabled by flags.
void ConvertOhlcHistoryIfPossible(long start_timestamp_sec,
                                  long end_timestamp_sec) {
  const std::string& input_file = FLAGS_input_ohlc_history_csv_file;
  const std::string& output_file =
      FLAGS_output_ohlc_history_delimited_proto_file;
  if (input_file.empty() || output_file.empty()) {
    return;
  }
  OhlcHistory ohlc_history;
  auto start = std::chrono::high_resolution_clock::now();
  std::cout << "Reading OHLC history CSV file: " << input_file << std::endl;
  if (!ReadOhlcHistoryFromCsvFile(input_file, &ohlc_history)) {
    std::cerr << "Failed to read OHLC history CSV file: " << input_file
              << std::endl;
    return;
  }
  const auto ohlc_history_subset =
      HistorySubset(ohlc_history, start_timestamp_sec, end_timestamp_sec);
  if (!WriteDelimitedMessagesToFile(ohlc_history_subset.first,
                                    ohlc_history_subset.second, output_file,
                                    FLAGS_compress)) {
    std::cerr << "Failed to write OHLC history file: " << output_file
              << std::endl;
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
                                    &start_timestamp_sec) ||
      !ConvertDateUTCToTimestampSec(FLAGS_end_date_utc, &end_timestamp_sec)) {
    std::cerr << "Invalid time period" << std::endl;
    return 1;
  }

  ConvertPriceHistoryIfPossible(start_timestamp_sec, end_timestamp_sec);
  ConvertOhlcHistoryIfPossible(start_timestamp_sec, end_timestamp_sec);

  // Optional: Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
