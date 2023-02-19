// Copyright © 2023 Peter Cerno. All rights reserved.

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "base/base.h"
#include "base/side_input.h"
#include "eval/eval.h"
#include "logging/csv_logger.h"
#include "traders/trader_factory.h"
#include "util/proto.h"
#include "util/time.h"

ABSL_FLAG(std::string, input_ohlc_history_delimited_proto_file, "",
          "Input file containing the delimited OhlcRecord protos.");
ABSL_FLAG(std::string, input_side_history_delimited_proto_file, "",
          "Input file containing the delimited SideInputRecord protos.");
ABSL_FLAG(std::string, output_exchange_log_file, "",
          "Output CSV file containing the exchange log.");
ABSL_FLAG(std::string, output_trader_log_file, "",
          "Output file containing the trader-dependent log.");
ABSL_FLAG(std::string, trader, "stop",
          "Trader to be executed. [rebalancing, stop].");

ABSL_FLAG(std::string, start_time, "2017-01-01",
          "Start date-time YYYY-MM-DD [hh:mm:ss] (included).");
ABSL_FLAG(std::string, end_time, "2021-01-01",
          "End date-time YYYY-MM-DD [hh:mm:ss] (excluded).");
ABSL_FLAG(int, evaluation_period_months, 0, "Evaluation period in months.");

ABSL_FLAG(double, start_base_balance, 1.0, "Starting base amount.");
ABSL_FLAG(double, start_quote_balance, 0.0, "Starting quote amount.");

ABSL_FLAG(double, market_liquidity, 0.5,
          "Liquidity for executing market (stop) orders.");
ABSL_FLAG(double, max_volume_ratio, 0.5,
          "Fraction of tick volume used to fill the limit order.");
ABSL_FLAG(bool, evaluate_batch, false, "Batch evaluation.");

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

// Returns the trader's AccountConfig based on the flags (and default values).
AccountConfig GetAccountConfig() {
  AccountConfig config;
  config.set_start_base_balance(absl::GetFlag(FLAGS_start_base_balance));
  config.set_start_quote_balance(absl::GetFlag(FLAGS_start_quote_balance));
  config.set_base_unit(0.00001f);
  config.set_quote_unit(0.01f);
  config.mutable_market_order_fee_config()->set_relative_fee(0.005f);
  config.mutable_market_order_fee_config()->set_fixed_fee(0);
  config.mutable_market_order_fee_config()->set_minimum_fee(0);
  config.mutable_limit_order_fee_config()->set_relative_fee(0.005f);
  config.mutable_limit_order_fee_config()->set_fixed_fee(0);
  config.mutable_limit_order_fee_config()->set_minimum_fee(0);
  config.mutable_stop_order_fee_config()->set_relative_fee(0.005f);
  config.mutable_stop_order_fee_config()->set_fixed_fee(0);
  config.mutable_stop_order_fee_config()->set_minimum_fee(0);
  config.set_market_liquidity(absl::GetFlag(FLAGS_market_liquidity));
  config.set_max_volume_ratio(absl::GetFlag(FLAGS_max_volume_ratio));
  return config;
}

// Returns a vector of records of type T read from the delimited_proto_file.
template <typename T>
absl::StatusOr<std::vector<T>> ReadHistory(
    const std::string& delimited_proto_file, absl::Time start_time,
    absl::Time end_time) {
  const absl::Time latency_start_time = absl::Now();
  std::vector<T> history;
  const absl::Status status =
      ReadDelimitedMessagesFromFile<T>(delimited_proto_file, history);
  if (!status.ok()) {
    return status;
  }
  LogInfo(
      absl::StrFormat("- Loaded %d records in %.3f seconds", history.size(),
                      absl::ToDoubleSeconds(absl::Now() - latency_start_time)));
  const std::vector<T> history_subset = HistorySubsetCopy<T>(
      history, absl::ToUnixSeconds(start_time), absl::ToUnixSeconds(end_time));
  LogInfo(
      absl::StrFormat("- Selected %d records within the time period: [%s - %s)",
                      history_subset.size(),      // nowrap
                      FormatTimeUTC(start_time),  // nowrap
                      FormatTimeUTC(end_time)));
  return history_subset;
}

// Opens the file log_filename for logging purposes.
absl::StatusOr<std::unique_ptr<std::ofstream>> OpenLogFile(
    const std::string& log_filename) {
  if (log_filename.empty()) {
    return nullptr;
  }
  if (absl::GetFlag(FLAGS_evaluation_period_months) > 0) {
    return absl::FailedPreconditionError(
        "Logging disabled when evaluating multiple periods");
  }
  auto log_stream = absl::make_unique<std::ofstream>();
  log_stream->open(log_filename, std::ios::out | std::ios::trunc);
  if (!log_stream->is_open()) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Cannot open the file: %s", log_filename));
  }
  return log_stream;
}

void PrintBatchEvalResults(const std::vector<EvaluationResult>& eval_results,
                           size_t top_n) {
  const size_t eval_count = std::min(top_n, eval_results.size());
  for (size_t eval_index = 0; eval_index < eval_count; ++eval_index) {
    const EvaluationResult& eval_result = eval_results.at(eval_index);
    LogInfo(
        absl::StrFormat("%s: %.5f", eval_result.name(), eval_result.score()));
  }
}

void PrintTraderEvalResult(const EvaluationResult& eval_result) {
  LogInfo(absl::StrCat("------------------ period ------------------",
                       "    trader & base gain    score    t&b volatility"));
  for (const EvaluationResult::Period& period : eval_result.period()) {
    LogInfo(absl::StrFormat(
        "[%s - %s): %9.2f%% %9.2f%% %8.3f %8.3f %8.3f",
        FormatTimeUTC(absl::FromUnixSeconds(period.start_timestamp_sec())),
        FormatTimeUTC(absl::FromUnixSeconds(period.end_timestamp_sec())),
        (period.final_gain() - 1.0f) * 100.0f,
        (period.base_final_gain() - 1.0f) * 100.0f,
        period.final_gain() / period.base_final_gain(),
        period.result().trader_volatility(),
        period.result().base_volatility()));
  }
}
}  // namespace

int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  absl::ParseCommandLine(argc, argv);

  const AccountConfig account_config = GetAccountConfig();
  LogInfo("Trader AccountConfig:");
  LogInfo(account_config.DebugString());

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

  EvaluationConfig eval_config;
  eval_config.set_start_timestamp_sec(absl::ToUnixSeconds(start_time));
  eval_config.set_end_timestamp_sec(absl::ToUnixSeconds(end_time));
  eval_config.set_evaluation_period_months(
      absl::GetFlag(FLAGS_evaluation_period_months));

  LogInfo("\nTrader EvaluationConfig:");
  LogInfo(eval_config.DebugString());

  LogInfo(absl::StrFormat(
      "Reading OHLC history from: %s",
      absl::GetFlag(FLAGS_input_ohlc_history_delimited_proto_file)));
  absl::StatusOr<OhlcHistory> ohlc_history_status = ReadHistory<OhlcTick>(
      absl::GetFlag(FLAGS_input_ohlc_history_delimited_proto_file),  // nowrap
      start_time, end_time);
  CheckOk(ohlc_history_status.status());
  const OhlcHistory& ohlc_history = ohlc_history_status.value();

  std::unique_ptr<SideInput> side_input;
  if (!absl::GetFlag(FLAGS_input_side_history_delimited_proto_file).empty()) {
    LogInfo(absl::StrFormat(
        "Reading side history from: %s",
        absl::GetFlag(FLAGS_input_side_history_delimited_proto_file)));
    absl::StatusOr<SideHistory> side_history_status =
        ReadHistory<SideInputRecord>(
            absl::GetFlag(FLAGS_input_side_history_delimited_proto_file),
            start_time, end_time);
    CheckOk(side_history_status.status());
    side_input = absl::make_unique<SideInput>(side_history_status.value());
  }

  const absl::Time latency_start_time = absl::Now();
  if (absl::GetFlag(FLAGS_evaluate_batch)) {
    eval_config.set_fast_eval(true);
    LogInfo("\nBatch evaluation:");
    std::vector<std::unique_ptr<TraderEmitter>> trader_emitters =
        GetBatchOfTraders(absl::GetFlag(FLAGS_trader));
    std::vector<EvaluationResult> eval_results =
        EvaluateBatchOfTraders(account_config, eval_config, ohlc_history,
                               side_input.get(), trader_emitters);
    std::sort(eval_results.begin(), eval_results.end(),
              [](const EvaluationResult& lhs, const EvaluationResult& rhs) {
                return lhs.score() > rhs.score();
              });
    PrintBatchEvalResults(eval_results, 20);
  } else {
    eval_config.set_fast_eval(false);
    std::unique_ptr<TraderEmitter> trader_emitter =
        GetTrader(absl::GetFlag(FLAGS_trader));
    LogInfo(absl::StrFormat("\n%s evaluation:", trader_emitter->GetName()));
    absl::StatusOr<std::unique_ptr<std::ofstream>> exchange_log_stream_status =
        OpenLogFile(absl::GetFlag(FLAGS_output_exchange_log_file));
    CheckOk(exchange_log_stream_status.status());
    absl::StatusOr<std::unique_ptr<std::ofstream>> trader_log_stream_status =
        OpenLogFile(absl::GetFlag(FLAGS_output_trader_log_file));
    CheckOk(trader_log_stream_status.status());
    CsvLogger logger(exchange_log_stream_status.value().get(),
                     trader_log_stream_status.value().get());
    EvaluationResult eval_result =
        EvaluateTrader(account_config, eval_config, ohlc_history,
                       side_input.get(), *trader_emitter, &logger);
    PrintTraderEvalResult(eval_result);
  }
  LogInfo(
      absl::StrFormat("\nEvaluated in %.3f seconds",
                      absl::ToDoubleSeconds(absl::Now() - latency_start_time)));

  // Optional: Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
