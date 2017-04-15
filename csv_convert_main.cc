// Copyright © 2017 Peter Cerno. All rights reserved.

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include <gflags/gflags.h>

#include "trader_io.h"
#include "util_proto.h"
#include "util_time.h"

// Input/Output files.
DEFINE_string(input_price_history_csv_file, "",
              "Input CSV file containing the historical prices.");
DEFINE_string(output_price_history_delimited_proto_file, "",
              "Ouptut file containing the delimited PriceRecord protos.");

DEFINE_string(input_ohlc_history_csv_file, "",
              "Input CSV file containing the OHLC prices.");
DEFINE_string(output_ohlc_history_delimited_proto_file, "",
              "Ouptut file containing the delimited OhlcRecord protos.");

using namespace trader;

// Converts the price history csv file if enabled by flags.
void ConvertPriceHistoryIfPossible() {
  PriceHistory price_history;
  auto start = std::chrono::high_resolution_clock::now();
  if (!FLAGS_input_price_history_csv_file.empty() &&
      !FLAGS_output_price_history_delimited_proto_file.empty() &&
      ReadPriceHistoryFromCsvFile(FLAGS_input_price_history_csv_file,
                                  &price_history) &&
      WriteDelimitedMessagesToFile(
          price_history, FLAGS_output_price_history_delimited_proto_file)) {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);
    std::cout << "Converted " << price_history.size() << " price records in "
              << duration.count() / 1000.0 << " seconds" << std::endl;
  }
}

// Converts the OHLC history csv file if enabled by flags.
void ConvertOhlcHistoryIfPossible() {
  OhlcHistory ohlc_history;
  auto start = std::chrono::high_resolution_clock::now();
  if (!FLAGS_input_ohlc_history_csv_file.empty() &&
      !FLAGS_output_ohlc_history_delimited_proto_file.empty() &&
      ReadOhlcHistoryFromCsvFile(FLAGS_input_ohlc_history_csv_file,
                                 &ohlc_history) &&
      WriteDelimitedMessagesToFile(
          ohlc_history, FLAGS_output_ohlc_history_delimited_proto_file)) {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);
    std::cout << "Converted " << ohlc_history.size() << " OHLC records in "
              << duration.count() / 1000.0 << " seconds" << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  ConvertPriceHistoryIfPossible();
  ConvertOhlcHistoryIfPossible();

  // Optional: Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
