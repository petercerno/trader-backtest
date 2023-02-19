// Copyright © 2023 Peter Cerno. All rights reserved.

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"
#include "base/base.h"
#include "base/history.h"
#include "util/proto.h"
#include "util/time.h"

ABSL_FLAG(std::string, input_price_history_csv_file, "",
          "Input CSV file containing the historical prices.");
ABSL_FLAG(std::string, input_price_history_delimited_proto_file, "",
          "Input file containing the delimited PriceRecord protos.");
ABSL_FLAG(std::string, output_price_history_delimited_proto_file, "",
          "Output file containing the delimited PriceRecord protos.");

ABSL_FLAG(std::string, input_ohlc_history_csv_file, "",
          "Input CSV file containing the OHLC prices.");
ABSL_FLAG(std::string, input_ohlc_history_delimited_proto_file, "",
          "Input file containing the delimited OhlcRecord protos.");
ABSL_FLAG(std::string, output_ohlc_history_delimited_proto_file, "",
          "Output file containing the delimited OhlcRecord protos.");

ABSL_FLAG(std::string, input_side_history_csv_file, "",
          "Input CSV file containing the historical side inputs.");
ABSL_FLAG(std::string, output_side_history_delimited_proto_file, "",
          "Output file containing the delimited SideInputRecord protos.");

ABSL_FLAG(std::string, start_time, "2017-01-01",
          "Start date-time YYYY-MM-DD [hh:mm:ss] (included).");
ABSL_FLAG(std::string, end_time, "2021-01-01",
          "End date-time YYYY-MM-DD [hh:mm:ss] (excluded).");

ABSL_FLAG(double, max_price_deviation_per_min, 0.05,
          "Maximum allowed price deviation per minute.");
ABSL_FLAG(int, sampling_rate_sec, 300, "Sampling rate in seconds.");

ABSL_FLAG(int, top_n_gaps, 50, "Number of top biggest gaps to print.");
ABSL_FLAG(int, last_n_outliers, 20,
          "Number of last removed outliers to print.");

ABSL_FLAG(bool, compress, true,
          "Whether to compress the output protobuf file.");

using namespace trader;

namespace {
void LogInfo(absl::string_view str) { absl::PrintF("%s\n", str); }
void LogError(absl::string_view str) { absl::FPrintF(stderr, "%s\n", str); }
void CheckOk(const absl::Status status) {
  if (!status.ok()) {
    LogError(status.message());
    std::exit(EXIT_FAILURE);
  }
}

// Reads the input CSV file containing the historical prices.
absl::StatusOr<PriceHistory> ReadPriceHistoryFromCsvFile(
    const std::string& file_name, const absl::Time start_time,
    const absl::Time end_time) {
  const absl::Time latency_start_time = absl::Now();
  LogInfo(
      absl::StrFormat("Reading price history from CSV file: %s", file_name));
  std::ifstream infile(file_name);
  if (!infile.is_open()) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Cannot open the file: %s", file_name));
  }
  int row = 0;
  std::string line;
  const int64_t start_timestamp_sec = absl::ToUnixSeconds(start_time);
  const int64_t end_timestamp_sec = absl::ToUnixSeconds(end_time);
  int64_t timestamp_sec_prev = 0;
  int64_t timestamp_sec = 0;
  float price = 0;
  float volume = 0;
  PriceHistory price_history;
  while (std::getline(infile, line)) {
    ++row;
    std::sscanf(line.c_str(), "%lld,%f,%f", &timestamp_sec, &price, &volume);
    if (start_timestamp_sec > 0 && timestamp_sec < start_timestamp_sec) {
      continue;
    }
    if (end_timestamp_sec > 0 && timestamp_sec >= end_timestamp_sec) {
      break;
    }
    if (timestamp_sec <= 0 || timestamp_sec < timestamp_sec_prev) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Invalid timestamp on the line %d: %s", row, line));
    }
    if (price <= 0) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Invalid price on the line %d: %s", row, line));
    }
    if (volume < 0) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Invalid volume on the line %d: %s", row, line));
    }
    timestamp_sec_prev = timestamp_sec;
    price_history.emplace_back();
    price_history.back().set_timestamp_sec(timestamp_sec);
    price_history.back().set_price(price);
    price_history.back().set_volume(volume);
  }
  LogInfo(
      absl::StrFormat("Loaded %d records in %.3f seconds", price_history.size(),
                      absl::ToDoubleSeconds(absl::Now() - latency_start_time)));
  return price_history;
}

// Reads the input CSV file containing the OHLC prices.
absl::StatusOr<OhlcHistory> ReadOhlcHistoryFromCsvFile(
    const std::string& file_name, const absl::Time start_time,
    const absl::Time end_time) {
  const absl::Time latency_start_time = absl::Now();
  LogInfo(absl::StrFormat("Reading OHLC history from CSV file: %s", file_name));
  std::ifstream infile(file_name);
  if (!infile.is_open()) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Cannot open the file: %s", file_name));
  }
  int row = 0;
  std::string line;
  const int64_t start_timestamp_sec = absl::ToUnixSeconds(start_time);
  const int64_t end_timestamp_sec = absl::ToUnixSeconds(end_time);
  int64_t timestamp_sec_prev = 0;
  int64_t timestamp_sec = 0;
  float open = 0;
  float high = 0;
  float low = 0;
  float close = 0;
  float volume = 0;
  OhlcHistory ohlc_history;
  while (std::getline(infile, line)) {
    ++row;
    std::sscanf(line.c_str(), "%lld,%f,%f,%f,%f,%f",  // nowrap
                &timestamp_sec, &open, &high, &low, &close, &volume);
    if (start_timestamp_sec > 0 && timestamp_sec < start_timestamp_sec) {
      continue;
    }
    if (end_timestamp_sec > 0 && timestamp_sec >= end_timestamp_sec) {
      break;
    }
    if (timestamp_sec <= 0 || timestamp_sec < timestamp_sec_prev) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Invalid timestamp on the line %d: %s", row, line));
    }
    if (open <= 0 || high <= 0 || low <= 0 || close <= 0 || low > open ||
        low > high || low > close || high < open || high < close) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Invalid OHLC prices on the line %d: %s", row, line));
    }
    if (volume < 0) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Invalid volume on the line %d: %s", row, line));
    }
    timestamp_sec_prev = timestamp_sec;
    ohlc_history.emplace_back();
    ohlc_history.back().set_timestamp_sec(timestamp_sec);
    ohlc_history.back().set_open(open);
    ohlc_history.back().set_high(high);
    ohlc_history.back().set_low(low);
    ohlc_history.back().set_close(close);
    ohlc_history.back().set_volume(volume);
  }
  LogInfo(absl::StrFormat(
      "Loaded %d OHLC ticks in %.3f seconds", ohlc_history.size(),
      absl::ToDoubleSeconds(absl::Now() - latency_start_time)));
  return ohlc_history;
}

// Reads the input CSV file containing the historical side inputs.
absl::StatusOr<SideHistory> ReadSideHistoryFromCsvFile(
    const std::string& file_name, const absl::Time start_time,
    const absl::Time end_time) {
  const absl::Time latency_start_time = absl::Now();
  LogInfo(absl::StrFormat("Reading side history from CSV file: %s", file_name));
  std::ifstream infile(file_name);
  if (!infile.is_open()) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Cannot open the file: %s", file_name));
  }
  int row = 0;
  std::string line;
  const int64_t start_timestamp_sec = absl::ToUnixSeconds(start_time);
  const int64_t end_timestamp_sec = absl::ToUnixSeconds(end_time);
  int64_t timestamp_sec_prev = 0;
  int64_t timestamp_sec = 0;
  float signal = 0;
  int num_signals = 0;
  SideHistory side_history;
  while (std::getline(infile, line)) {
    ++row;
    std::replace(line.begin(), line.end(), ',', ' ');
    std::istringstream iss(line);
    if (!(iss >> timestamp_sec)) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "Cannot parse the timestamp on the line %d: %s", row, line));
    }
    if (start_timestamp_sec > 0 && timestamp_sec < start_timestamp_sec) {
      continue;
    }
    if (end_timestamp_sec > 0 && timestamp_sec >= end_timestamp_sec) {
      break;
    }
    if (timestamp_sec <= 0 || timestamp_sec <= timestamp_sec_prev) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Invalid timestamp on the line %d: %s", row, line));
    }
    timestamp_sec_prev = timestamp_sec;
    side_history.emplace_back();
    side_history.back().set_timestamp_sec(timestamp_sec);
    while (iss >> signal) {
      side_history.back().add_signal(signal);
    }
    if (num_signals == 0) {
      num_signals = side_history.back().signal_size();
    }
    if (num_signals == 0 || num_signals != side_history.back().signal_size()) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "Invalid number of signals on the line %d: %s", row, line));
    }
  }
  LogInfo(
      absl::StrFormat("Loaded %d records in %.3f seconds", side_history.size(),
                      absl::ToDoubleSeconds(absl::Now() - latency_start_time)));
  return side_history;
}

// Reads and returns the price / OHLC history.
template <typename T>
absl::StatusOr<std::vector<T>> ReadHistoryFromDelimitedProtoFile(
    const std::string& file_name, const absl::Time start_time,
    const absl::Time end_time, std::function<absl::Status(const T&)> validate) {
  const absl::Time latency_start_time = absl::Now();
  LogInfo(absl::StrFormat("Reading history from delimited proto file: %s",
                          file_name));
  int record_index = 0;
  const int64_t start_timestamp_sec = absl::ToUnixSeconds(start_time);
  const int64_t end_timestamp_sec = absl::ToUnixSeconds(end_time);
  int64_t timestamp_sec_prev = 0;
  std::vector<T> history;
  const absl::Status read_status = ReadDelimitedMessagesFromFile<T>(
      file_name,
      /*reader=*/
      [&history, start_timestamp_sec, end_timestamp_sec, validate,
       &record_index, &timestamp_sec_prev](const T& message) -> ReaderStatus {
        const int64_t timestamp_sec = message.timestamp_sec();
        if (start_timestamp_sec > 0 && timestamp_sec < start_timestamp_sec) {
          return ReaderSignal::kContinue;
        }
        if (end_timestamp_sec > 0 && timestamp_sec >= end_timestamp_sec) {
          return ReaderSignal::kBreak;
        }
        if (timestamp_sec <= 0 || timestamp_sec < timestamp_sec_prev) {
          return absl::InvalidArgumentError(
              absl::StrFormat("Invalid timestamp on the record %d:\n%s",
                              record_index, message.DebugString()));
        }
        const absl::Status validation_status = validate(message);
        if (!validation_status.ok()) {
          return absl::InvalidArgumentError(absl::StrFormat(
              "Invalid record %d:\n%s\n%s", record_index, message.DebugString(),
              validation_status.message()));
        }
        timestamp_sec_prev = timestamp_sec;
        history.push_back(message);
        ++record_index;
        return ReaderSignal::kContinue;
      });
  if (!read_status.ok()) {
    return read_status;
  }
  LogInfo(
      absl::StrFormat("Loaded %d records in %.3f seconds", history.size(),
                      absl::ToDoubleSeconds(absl::Now() - latency_start_time)));
  return history;
}

// Reads the input delimited proto file containing the PriceRecord protos.
absl::StatusOr<PriceHistory> ReadPriceHistoryFromDelimitedProtoFile(
    const std::string& file_name, const absl::Time start_time,
    const absl::Time end_time) {
  return ReadHistoryFromDelimitedProtoFile<PriceRecord>(
      file_name, start_time, end_time,
      /*validate=*/
      [](const PriceRecord& price_record) -> absl::Status {
        if (price_record.price() <= 0) {
          return absl::InvalidArgumentError("Invalid price");
        }
        if (price_record.volume() < 0) {
          return absl::InvalidArgumentError("Invalid volume");
        }
        return absl::OkStatus();
      });
}

// Reads the input delimited proto file containing the OhlcTick protos.
absl::StatusOr<OhlcHistory> ReadOhlcHistoryFromDelimitedProtoFile(
    const std::string& file_name, const absl::Time start_time,
    const absl::Time end_time) {
  return ReadHistoryFromDelimitedProtoFile<OhlcTick>(
      file_name, start_time, end_time,
      /*validate=*/
      [](const OhlcTick& ohlc_tick) -> absl::Status {
        if (ohlc_tick.open() <= 0 || ohlc_tick.high() <= 0 ||
            ohlc_tick.low() <= 0 || ohlc_tick.close() <= 0 ||
            ohlc_tick.low() > ohlc_tick.open() ||
            ohlc_tick.low() > ohlc_tick.high() ||
            ohlc_tick.low() > ohlc_tick.close() ||
            ohlc_tick.high() < ohlc_tick.open() ||
            ohlc_tick.high() < ohlc_tick.close()) {
          return absl::InvalidArgumentError("Invalid OHLC prices");
        }
        if (ohlc_tick.volume() < 0) {
          return absl::InvalidArgumentError("Invalid volume");
        }
        return absl::OkStatus();
      });
}

std::string DurationToString(const int64_t duration_sec) {
  const int64_t hours = duration_sec / 3600;
  const int64_t minutes = (duration_sec / 60) % 60;
  const int64_t seconds = duration_sec % 60;
  return absl::StrFormat("%d:%02d:%02d", hours, minutes, seconds);
}

// Prints the top_n largest (chronologically sorted) price history gaps.
void PrintPriceHistoryGaps(const PriceHistory& price_history, size_t top_n) {
  const std::vector<HistoryGap> history_gaps = GetPriceHistoryGaps(
      price_history.begin(), price_history.end(), /*start_timestamp_sec=*/0,
      /*end_timestamp_sec=*/0, top_n);
  for (const HistoryGap& history_gap : history_gaps) {
    const int64_t gap_duration_sec = history_gap.second - history_gap.first;
    LogInfo(absl::StrFormat(
        "%d [%s] - %d [%s]: %s", history_gap.first,
        FormatTimeUTC(absl::FromUnixSeconds(history_gap.first)),
        history_gap.second,
        FormatTimeUTC(absl::FromUnixSeconds(history_gap.second)),
        DurationToString(gap_duration_sec)));
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
      LogInfo("   ...");
    }
    LogInfo(absl::StrFormat(
        "%s %d [%s]: %.2f [%.4f]", (is_outlier ? " x" : "  "),
        price_record.timestamp_sec(),
        FormatTimeUTC(absl::FromUnixSeconds(price_record.timestamp_sec())),
        price_record.price(), price_record.volume()));
    index_prev = index;
  }
}

// Removes outliers and resamples the price history into OHLC history.
OhlcHistory ConvertPriceHistoryToOhlcHistory(
    const PriceHistory& price_history) {
  std::vector<size_t> outlier_indices;
  const PriceHistory price_history_clean = RemoveOutliers(
      /*begin=*/price_history.begin(),
      /*end=*/price_history.end(),
      absl::GetFlag(FLAGS_max_price_deviation_per_min), &outlier_indices);
  LogInfo(absl::StrFormat("Removed %d outliers", outlier_indices.size()));
  LogInfo(absl::StrFormat("Last %d outliers:",
                          absl::GetFlag(FLAGS_last_n_outliers)));
  PrintOutliersWithContext(
      /*begin=*/price_history.begin(),
      /*end=*/price_history.end(), outlier_indices,
      /*left_context_size=*/5,
      /*right_context_size=*/5,
      /*last_n=*/absl::GetFlag(FLAGS_last_n_outliers));
  const OhlcHistory ohlc_history =
      Resample(price_history_clean.begin(), price_history_clean.end(),
               absl::GetFlag(FLAGS_sampling_rate_sec));
  LogInfo(absl::StrFormat("Resampled %d records to %d OHLC ticks",
                          price_history_clean.size(), ohlc_history.size()));
  return ohlc_history;
}

// Writes history to delimited protobuf file.
template <typename T>
absl::Status WriteHistoryToDelimitedProtoFile(
    const std::vector<T>& history,
    const std::string& output_history_delimited_proto_file) {
  const absl::Time latency_start_time = absl::Now();
  LogInfo(absl::StrFormat("Writing %d records to the file: %s", history.size(),
                          output_history_delimited_proto_file));
  const absl::Status status = WriteDelimitedMessagesToFile(
      history.begin(), history.end(), output_history_delimited_proto_file,
      absl::GetFlag(FLAGS_compress));
  LogInfo(
      absl::StrFormat("Finished in %.3f seconds",
                      absl::ToDoubleSeconds(absl::Now() - latency_start_time)));
  return status;
}
}  // namespace

int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  absl::ParseCommandLine(argc, argv);

  const absl::StatusOr<absl::Time> start_time_status =
      ParseTime(absl::GetFlag(FLAGS_start_time));
  CheckOk(start_time_status.status());
  const absl::StatusOr<absl::Time> end_time_status =
      ParseTime(absl::GetFlag(FLAGS_end_time));
  CheckOk(end_time_status.status());
  const absl::Time start_time = start_time_status.value();
  const absl::Time end_time = end_time_status.value();
  LogInfo(absl::StrFormat("Selected time period:\n[%s - %s)",
                          FormatTimeUTC(start_time), FormatTimeUTC(end_time)));

  if (!absl::GetFlag(FLAGS_input_price_history_csv_file).empty() &&
      !absl::GetFlag(FLAGS_input_price_history_delimited_proto_file).empty()) {
    LogError("Cannot have two input price history files");
    std::exit(EXIT_FAILURE);
  }

  if (!absl::GetFlag(FLAGS_input_ohlc_history_csv_file).empty() &&
      !absl::GetFlag(FLAGS_input_ohlc_history_delimited_proto_file).empty()) {
    LogError("Cannot have two input OHLC history files");
    std::exit(EXIT_FAILURE);
  }

  const bool read_price_history =
      !absl::GetFlag(FLAGS_input_price_history_csv_file).empty() ||
      !absl::GetFlag(FLAGS_input_price_history_delimited_proto_file).empty();

  const bool read_ohlc_history =
      !absl::GetFlag(FLAGS_input_ohlc_history_csv_file).empty() ||
      !absl::GetFlag(FLAGS_input_ohlc_history_delimited_proto_file).empty();

  const bool read_side_history =
      !absl::GetFlag(FLAGS_input_side_history_csv_file).empty();

  const int num_history_files = (read_price_history ? 1 : 0) +
                                (read_ohlc_history ? 1 : 0) +
                                (read_side_history ? 1 : 0);

  if (num_history_files > 1) {
    LogError("Cannot read more than one input history file");
    std::exit(EXIT_FAILURE);
  }

  if (num_history_files == 0) {
    LogError("Input history file not specified");
    std::exit(EXIT_FAILURE);
  }

  const auto price_history_status =
      [start_time, end_time]() -> absl::StatusOr<PriceHistory> {
    if (!absl::GetFlag(FLAGS_input_price_history_csv_file).empty()) {
      return ReadPriceHistoryFromCsvFile(
          absl::GetFlag(FLAGS_input_price_history_csv_file), start_time,
          end_time);
    } else if (!absl::GetFlag(FLAGS_input_price_history_delimited_proto_file)
                    .empty()) {
      return ReadPriceHistoryFromDelimitedProtoFile(
          absl::GetFlag(FLAGS_input_price_history_delimited_proto_file),
          start_time, end_time);
    }
    return PriceHistory{};
  }();
  CheckOk(price_history_status.status());
  PriceHistory price_history = std::move(price_history_status.value());

  const auto ohlc_history_status = [start_time,
                                    end_time]() -> absl::StatusOr<OhlcHistory> {
    if (!absl::GetFlag(FLAGS_input_ohlc_history_csv_file).empty()) {
      return ReadOhlcHistoryFromCsvFile(
          absl::GetFlag(FLAGS_input_ohlc_history_csv_file), start_time,
          end_time);
    } else if (!absl::GetFlag(FLAGS_input_ohlc_history_delimited_proto_file)
                    .empty()) {
      return ReadOhlcHistoryFromDelimitedProtoFile(
          absl::GetFlag(FLAGS_input_ohlc_history_delimited_proto_file),
          start_time, end_time);
    }
    return OhlcHistory{};
  }();
  CheckOk(ohlc_history_status.status());
  OhlcHistory ohlc_history = std::move(ohlc_history_status.value());

  const auto side_history_status = [start_time,
                                    end_time]() -> absl::StatusOr<SideHistory> {
    if (!absl::GetFlag(FLAGS_input_side_history_csv_file).empty()) {
      return ReadSideHistoryFromCsvFile(
          absl::GetFlag(FLAGS_input_side_history_csv_file), start_time,
          end_time);
    }
    return SideHistory{};
  }();
  CheckOk(side_history_status.status());
  SideHistory side_history = std::move(side_history_status.value());

  if (!price_history.empty()) {
    LogInfo(absl::StrFormat("Top %d gaps:", absl::GetFlag(FLAGS_top_n_gaps)));
    PrintPriceHistoryGaps(price_history,
                          /*top_n=*/absl::GetFlag(FLAGS_top_n_gaps));
  }

  if (!price_history.empty() && ohlc_history.empty() &&
      !absl::GetFlag(FLAGS_output_ohlc_history_delimited_proto_file).empty()) {
    ohlc_history = ConvertPriceHistoryToOhlcHistory(price_history);
  }

  if (!price_history.empty() &&
      !absl::GetFlag(FLAGS_output_price_history_delimited_proto_file).empty()) {
    CheckOk(WriteHistoryToDelimitedProtoFile(
        price_history,
        absl::GetFlag(FLAGS_output_price_history_delimited_proto_file)));
  }

  if (!ohlc_history.empty() &&
      !absl::GetFlag(FLAGS_output_ohlc_history_delimited_proto_file).empty()) {
    CheckOk(WriteHistoryToDelimitedProtoFile(
        ohlc_history,
        absl::GetFlag(FLAGS_output_ohlc_history_delimited_proto_file)));
  }

  if (!side_history.empty() &&
      !absl::GetFlag(FLAGS_output_side_history_delimited_proto_file).empty()) {
    CheckOk(WriteHistoryToDelimitedProtoFile(
        side_history,
        absl::GetFlag(FLAGS_output_side_history_delimited_proto_file)));
  }

  // Optional: Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
