// Copyright © 2020 Peter Cerno. All rights reserved.

#include <gflags/gflags.h>

#include "lib/trader_base.h"
#include "lib/trader_history.h"
#include "util/util_proto.h"
#include "util/util_time.h"

DEFINE_string(input_price_history_csv_file, "",
              "Input CSV file containing the historical prices.");
DEFINE_string(input_price_history_delimited_proto_file, "",
              "Input file containing the delimited PriceRecord protos.");
DEFINE_string(output_price_history_delimited_proto_file, "",
              "Output file containing the delimited PriceRecord protos.");

DEFINE_string(input_ohlc_history_csv_file, "",
              "Input CSV file containing the OHLC prices.");
DEFINE_string(input_ohlc_history_delimited_proto_file, "",
              "Input file containing the delimited OhlcRecord protos.");
DEFINE_string(output_ohlc_history_delimited_proto_file, "",
              "Output file containing the delimited OhlcRecord protos.");

DEFINE_string(start_date_utc, "", "Start date YYYY-MM-DD in UTC (included).");
DEFINE_string(end_date_utc, "", "End date YYYY-MM-DD in UTC (excluded).");

DEFINE_double(max_price_deviation_per_min, 0.05,
              "Maximum allowed price deviation per minute.");
DEFINE_int32(sampling_rate_sec, 300, "Sampling rate in seconds.");

DEFINE_int32(top_n_gaps, 50, "Number of top biggest gaps to print.");
DEFINE_int32(last_n_outliers, 20, "Number of last removed outliers to print.");

DEFINE_bool(compress, true, "Whether to compress the output protobuf file.");

using namespace trader;

namespace {
// Reads the input CSV file containing the historical prices and saves them
// into the given price_history vector.
bool ReadPriceHistoryFromCsvFile(const std::string& file_name,
                                 long start_timestamp_sec,
                                 long end_timestamp_sec,
                                 PriceHistory* price_history) {
  assert(price_history != nullptr);
  std::cout << "Reading price history from CSV file: " << file_name << std::endl
            << std::flush;
  auto start = std::chrono::high_resolution_clock::now();
  std::ifstream infile(file_name);
  if (!infile.is_open()) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  int row = 0;
  std::string line;
  int timestamp_sec_prev = 0;
  int timestamp_sec = 0;
  float price = 0;
  float volume = 0;
  while (std::getline(infile, line)) {
    ++row;
    std::replace(line.begin(), line.end(), ',', ' ');
    std::istringstream iss(line);
    if (!(iss >> timestamp_sec >> price >> volume)) {
      std::cerr << "Cannot parse line " << row << ": " << line << std::endl;
      return false;
    }
    if (start_timestamp_sec > 0 && timestamp_sec < start_timestamp_sec) {
      continue;
    }
    if (end_timestamp_sec > 0 && timestamp_sec >= end_timestamp_sec) {
      break;
    }
    if (timestamp_sec <= 0 || timestamp_sec < timestamp_sec_prev) {
      std::cerr << "Invalid timestamp on line " << row << ": " << line
                << std::endl;
      return false;
    }
    if (price <= 0) {
      std::cerr << "Invalid price on line " << row << ": " << line << std::endl;
      return false;
    }
    if (volume < 0) {
      std::cerr << "Invalid volume on line " << row << ": " << line
                << std::endl;
      return false;
    }
    timestamp_sec_prev = timestamp_sec;
    price_history->emplace_back();
    price_history->back().set_timestamp_sec(timestamp_sec);
    price_history->back().set_price(price);
    price_history->back().set_volume(volume);
  }
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Loaded " << price_history->size() << " records in "
            << duration.count() / 1000.0 << " seconds" << std::endl
            << std::flush;
  return true;
}

// Reads the input CSV file containing the OHLC prices and saves them into the
// given ohlc_history vector.
bool ReadOhlcHistoryFromCsvFile(const std::string& file_name,
                                long start_timestamp_sec,
                                long end_timestamp_sec,
                                OhlcHistory* ohlc_history) {
  assert(ohlc_history != nullptr);
  std::cout << "Reading OHLC history from CSV file: " << file_name << std::endl
            << std::flush;
  auto start = std::chrono::high_resolution_clock::now();
  std::ifstream infile(file_name);
  if (!infile.is_open()) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  int row = 0;
  std::string line;
  int timestamp_sec_prev = 0;
  int timestamp_sec = 0;
  float open = 0;
  float high = 0;
  float low = 0;
  float close = 0;
  float volume = 0;
  while (std::getline(infile, line)) {
    ++row;
    std::replace(line.begin(), line.end(), ',', ' ');
    std::istringstream iss(line);
    if (!(iss >> timestamp_sec >> open >> high >> low >> close >> volume)) {
      std::cerr << "Cannot parse line " << row << ": " << line << std::endl;
      return false;
    }
    if (start_timestamp_sec > 0 && timestamp_sec < start_timestamp_sec) {
      continue;
    }
    if (end_timestamp_sec > 0 && timestamp_sec >= end_timestamp_sec) {
      break;
    }
    if (timestamp_sec <= 0 || timestamp_sec < timestamp_sec_prev) {
      std::cerr << "Invalid timestamp on line " << row << ": " << line
                << std::endl;
      return false;
    }
    if (open <= 0 || high <= 0 || low <= 0 || close <= 0 || low > open ||
        low > high || low > close || high < open || high < close) {
      std::cerr << "Invalid OHLC prices on line " << row << ": " << line
                << std::endl;
      return false;
    }
    if (volume < 0) {
      std::cerr << "Invalid volume on line " << row << ": " << line
                << std::endl;
      return false;
    }
    timestamp_sec_prev = timestamp_sec;
    ohlc_history->emplace_back();
    ohlc_history->back().set_timestamp_sec(timestamp_sec);
    ohlc_history->back().set_open(open);
    ohlc_history->back().set_high(high);
    ohlc_history->back().set_low(low);
    ohlc_history->back().set_close(close);
    ohlc_history->back().set_volume(volume);
  }
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Loaded " << ohlc_history->size() << " OHLC ticks in "
            << duration.count() / 1000.0 << " seconds" << std::endl
            << std::flush;
  return true;
}

// Reads and returns the price / OHLC history.
template <typename T>
bool ReadHistoryFromDelimitedProtoFile(
    const std::string& file_name, long start_timestamp_sec,
    long end_timestamp_sec, std::function<bool(int, const T&)> validate,
    std::vector<T>* history) {
  assert(history != nullptr);
  std::cout << "Reading history from delimited proto file: " << file_name
            << std::endl
            << std::flush;
  auto start = std::chrono::high_resolution_clock::now();
  int record_index = 0;
  int timestamp_sec_prev = 0;
  if (!ReadDelimitedMessagesFromFile<T>(
          file_name,
          /* reader = */
          [history, start_timestamp_sec, end_timestamp_sec, validate,
           &record_index,
           &timestamp_sec_prev](const T& message) -> ReaderStatus {
            const int timestamp_sec = message.timestamp_sec();
            if (start_timestamp_sec > 0 &&
                timestamp_sec < start_timestamp_sec) {
              return ReaderStatus::kContinue;
            }
            if (end_timestamp_sec > 0 && timestamp_sec >= end_timestamp_sec) {
              return ReaderStatus::kBreak;
            }
            if (timestamp_sec <= 0 || timestamp_sec < timestamp_sec_prev) {
              std::cerr << "Invalid timestamp on record " << record_index
                        << ": " << message.DebugString() << std::endl;
              return ReaderStatus::kFailure;
            }
            if (!validate(record_index, message)) {
              std::cerr << "Invalid record " << record_index << ": "
                        << message.DebugString() << std::endl;
              return ReaderStatus::kFailure;
            }
            timestamp_sec_prev = timestamp_sec;
            history->push_back(message);
            ++record_index;
            return ReaderStatus::kContinue;
          })) {
    return false;
  }
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Loaded " << history->size() << " records in "
            << duration.count() / 1000.0 << " seconds" << std::endl
            << std::flush;
  return true;
}

// Reads the input delimited proto file containing the PriceRecord protos and
// saves them into the given price_history vector.
bool ReadPriceHistoryFromDelimitedProtoFile(const std::string& file_name,
                                            long start_timestamp_sec,
                                            long end_timestamp_sec,
                                            PriceHistory* price_history) {
  return ReadHistoryFromDelimitedProtoFile<PriceRecord>(
      file_name, start_timestamp_sec, end_timestamp_sec,
      /* validate = */
      [](int record_index, const PriceRecord& price_record) -> bool {
        if (price_record.price() <= 0) {
          std::cerr << "Invalid price on record " << record_index << std::endl;
          return false;
        }
        if (price_record.volume() < 0) {
          std::cerr << "Invalid volume on record " << record_index << std::endl;
          return false;
        }
        return true;
      },
      price_history);
}

// Reads the input delimited proto file containing the OhlcTick protos and
// saves them into the given ohlc_history vector.
bool ReadOhlcHistoryFromDelimitedProtoFile(const std::string& file_name,
                                           long start_timestamp_sec,
                                           long end_timestamp_sec,
                                           OhlcHistory* ohlc_history) {
  return ReadHistoryFromDelimitedProtoFile<OhlcTick>(
      file_name, start_timestamp_sec, end_timestamp_sec,
      /* validate = */
      [](int record_index, const OhlcTick& ohlc_tick) -> bool {
        if (ohlc_tick.open() <= 0 || ohlc_tick.high() <= 0 ||
            ohlc_tick.low() <= 0 || ohlc_tick.close() <= 0 ||
            ohlc_tick.low() > ohlc_tick.open() ||
            ohlc_tick.low() > ohlc_tick.high() ||
            ohlc_tick.low() > ohlc_tick.close() ||
            ohlc_tick.high() < ohlc_tick.open() ||
            ohlc_tick.high() < ohlc_tick.close()) {
          std::cerr << "Invalid OHLC prices on record " << record_index
                    << std::endl;
          return false;
        }
        if (ohlc_tick.volume() < 0) {
          std::cerr << "Invalid volume on record " << record_index << std::endl;
          return false;
        }
        return true;
      },
      ohlc_history);
}

// Prints the top_n largest (chronologically sorted) price history gaps.
void PrintPriceHistoryGaps(const PriceHistory& price_history, size_t top_n) {
  std::vector<HistoryGap> history_gaps = GetPriceHistoryGaps(
      price_history.begin(), price_history.end(), /* start_timestamp_sec = */ 0,
      /* end_timestamp_sec = */ 0, top_n);
  for (const HistoryGap& history_gap : history_gaps) {
    const long gap_duration_sec = history_gap.second - history_gap.first;
    std::cout << history_gap.first << " ["
              << ConvertTimestampSecToDateTimeUTC(history_gap.first) << "] - "
              << history_gap.second << " ["
              << ConvertTimestampSecToDateTimeUTC(history_gap.second)
              << "]: " << DurationToString(gap_duration_sec) << std::endl;
  }
  std::cout << std::flush;
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
  std::cout << std::flush;
}

// Removes outliers and resamples the price history into OHLC history.
OhlcHistory ConvertPriceHistoryToOhlcHistory(
    const PriceHistory& price_history) {
  std::vector<size_t> outlier_indices;
  const PriceHistory price_history_clean = RemoveOutliers(
      /* begin = */ price_history.begin(),
      /* end = */ price_history.end(), FLAGS_max_price_deviation_per_min,
      &outlier_indices);
  std::cout << "Removed " << outlier_indices.size() << " outliers" << std::endl;
  std::cout << "Last " << FLAGS_last_n_outliers << " outliers:" << std::endl;
  PrintOutliersWithContext(
      /* begin = */ price_history.begin(),
      /* end = */ price_history.end(), outlier_indices,
      /* left_context_size = */ 5,
      /* right_context_size = */ 5, /* last_n = */ FLAGS_last_n_outliers);
  const OhlcHistory ohlc_history =
      Resample(price_history_clean.begin(), price_history_clean.end(),
               FLAGS_sampling_rate_sec);
  std::cout << "Resampled " << price_history_clean.size() << " records to "
            << ohlc_history.size() << " OHLC ticks" << std::endl;
  return ohlc_history;
}

// Writes history to delimited protobuf file.
template <typename T>
bool WriteHistoryToDelimitedProtoFile(
    const std::vector<T>& history,
    const std::string& output_history_delimited_proto_file) {
  auto start = std::chrono::high_resolution_clock::now();
  std::cout << "Writing " << history.size() << " records to "
            << output_history_delimited_proto_file << std::endl;
  if (!WriteDelimitedMessagesToFile(history.begin(), history.end(),
                                    output_history_delimited_proto_file,
                                    FLAGS_compress)) {
    std::cerr << "Failed to write history file: "
              << output_history_delimited_proto_file << std::endl;
    return false;
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Finished in " << duration.count() / 1000.0 << " seconds"
            << std::endl
            << std::flush;
  return true;
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
      !FLAGS_input_price_history_delimited_proto_file.empty()) {
    std::cerr << "Cannot have two input price history files" << std::endl;
    return 1;
  }

  if (!FLAGS_input_ohlc_history_csv_file.empty() &&
      !FLAGS_input_ohlc_history_delimited_proto_file.empty()) {
    std::cerr << "Cannot have two input OHLC history files" << std::endl;
    return 1;
  }

  const bool read_price_history =
      !FLAGS_input_price_history_csv_file.empty() ||
      !FLAGS_input_price_history_delimited_proto_file.empty();

  const bool read_ohlc_history =
      !FLAGS_input_ohlc_history_csv_file.empty() ||
      !FLAGS_input_ohlc_history_delimited_proto_file.empty();

  if (read_price_history && read_ohlc_history) {
    std::cerr << "Cannot read both price history and ohlc history" << std::endl;
    return 1;
  }

  if (!read_price_history && !read_ohlc_history) {
    std::cerr << "Input history file not specified" << std::endl;
    return 1;
  }

  PriceHistory price_history;
  if (!FLAGS_input_price_history_csv_file.empty()) {
    if (!ReadPriceHistoryFromCsvFile(FLAGS_input_price_history_csv_file,
                                     start_timestamp_sec, end_timestamp_sec,
                                     &price_history)) {
      return 1;
    }
  } else if (!FLAGS_input_price_history_delimited_proto_file.empty()) {
    if (!ReadPriceHistoryFromDelimitedProtoFile(
            FLAGS_input_price_history_delimited_proto_file, start_timestamp_sec,
            end_timestamp_sec, &price_history)) {
      return 1;
    }
  }

  OhlcHistory ohlc_history;
  if (!FLAGS_input_ohlc_history_csv_file.empty()) {
    if (!ReadOhlcHistoryFromCsvFile(FLAGS_input_ohlc_history_csv_file,
                                    start_timestamp_sec, end_timestamp_sec,
                                    &ohlc_history)) {
      return 1;
    }
  } else if (!FLAGS_input_ohlc_history_delimited_proto_file.empty()) {
    if (!ReadOhlcHistoryFromDelimitedProtoFile(
            FLAGS_input_ohlc_history_delimited_proto_file, start_timestamp_sec,
            end_timestamp_sec, &ohlc_history)) {
      return 1;
    }
  }

  if (!price_history.empty()) {
    std::cout << "Top " << FLAGS_top_n_gaps << " gaps:" << std::endl;
    PrintPriceHistoryGaps(price_history,
                          /* top_n = */ FLAGS_top_n_gaps);
  }

  if (!price_history.empty() && ohlc_history.empty() &&
      !FLAGS_output_ohlc_history_delimited_proto_file.empty()) {
    ohlc_history = ConvertPriceHistoryToOhlcHistory(price_history);
  }

  if (!price_history.empty() &&
      !FLAGS_output_price_history_delimited_proto_file.empty()) {
    if (!WriteHistoryToDelimitedProtoFile(
            price_history, FLAGS_output_price_history_delimited_proto_file)) {
      return 1;
    }
  }

  if (!ohlc_history.empty() &&
      !FLAGS_output_ohlc_history_delimited_proto_file.empty()) {
    if (!WriteHistoryToDelimitedProtoFile(
            ohlc_history, FLAGS_output_ohlc_history_delimited_proto_file)) {
      return 1;
    }
  }

  // Optional: Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
