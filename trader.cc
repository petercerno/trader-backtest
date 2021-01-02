// Copyright © 2020 Peter Cerno. All rights reserved.

#include <gflags/gflags.h>

#include "base/base.h"
#include "eval/eval.h"
#include "logging/csv_logger.h"
#include "traders/trader_factory.h"
#include "util/proto.h"
#include "util/time.h"

DEFINE_string(input_ohlc_history_delimited_proto_file, "",
              "Input file containing the delimited OhlcRecord protos.");
DEFINE_string(output_exchange_log_file, "",
              "Output CSV file containing the exchange log.");
DEFINE_string(output_trader_log_file, "",
              "Output file containing the trader-dependent log.");
DEFINE_string(trader, "stop", "Trader to be executed. [rebalancing, stop].");

DEFINE_string(start_date_utc, "2017-01-01",
              "Start date YYYY-MM-DD in UTC (included).");
DEFINE_string(end_date_utc, "2021-01-01",
              "End date YYYY-MM-DD in UTC (excluded).");
DEFINE_int32(evaluation_period_months, 0, "Evaluation period in months.");

DEFINE_double(start_base_balance, 1.0, "Starting base amount.");
DEFINE_double(start_quote_balance, 0.0, "Starting quote amount.");

DEFINE_double(market_liquidity, 0.5,
              "Liquidity for executing market (stop) orders.");
DEFINE_double(max_volume_ratio, 0.5,
              "Fraction of tick volume used to fill the limit order.");
DEFINE_bool(evaluate_batch, false, "Batch evaluation.");

using namespace trader;

namespace {
// Returns the trader's AccountConfig based on the flags (and default values).
AccountConfig GetAccountConfig() {
  AccountConfig config;
  config.set_start_base_balance(FLAGS_start_base_balance);
  config.set_start_quote_balance(FLAGS_start_quote_balance);
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
  config.set_market_liquidity(FLAGS_market_liquidity);
  config.set_max_volume_ratio(FLAGS_max_volume_ratio);
  return config;
}

// Reads and returns the OHLC history based on the flags.
OhlcHistory GetOhlcHistoryFromFlags(long start_timestamp_sec,
                                    long end_timestamp_sec) {
  if (FLAGS_input_ohlc_history_delimited_proto_file.empty()) {
    return {};
  }
  OhlcHistory ohlc_history;
  auto start = std::chrono::high_resolution_clock::now();
  if (!ReadDelimitedMessagesFromFile<OhlcTick>(
          FLAGS_input_ohlc_history_delimited_proto_file, ohlc_history)) {
    return {};
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Loaded " << ohlc_history.size() << " OHLC ticks in "
            << duration.count() / 1000.0 << " seconds" << std::endl;
  OhlcHistory ohlc_history_subset =
      HistorySubsetCopy(ohlc_history, start_timestamp_sec, end_timestamp_sec);
  std::cout << "Selected " << ohlc_history_subset.size()
            << " OHLC ticks within the period: "
            << TimestampPeriodToString(start_timestamp_sec, end_timestamp_sec)
            << std::endl;
  return ohlc_history_subset;
}

// Opens the file for logging purposes.
std::unique_ptr<std::ofstream> OpenLogFile(const std::string& log_filename) {
  if (log_filename.empty()) {
    return nullptr;
  }
  if (FLAGS_evaluation_period_months > 0) {
    std::cerr << "Logging disabled when evaluating multiple periods"
              << std::endl;
    return nullptr;
  }
  auto log_stream = std::unique_ptr<std::ofstream>(new std::ofstream());
  log_stream->open(log_filename, std::ios::out | std::ios::trunc);
  if (!log_stream->is_open()) {
    std::cerr << "Cannot open file: " << log_filename << std::endl;
    return nullptr;
  }
  return log_stream;
}

void PrintBatchEvalResults(const std::vector<EvaluationResult>& eval_results,
                           size_t top_n) {
  const size_t eval_count = std::min(top_n, eval_results.size());
  for (int eval_index = 0; eval_index < eval_count; ++eval_index) {
    const EvaluationResult& eval_result = eval_results.at(eval_index);
    std::cout << eval_result.name() << ": " << std::setprecision(5)
              << eval_result.score() << std::endl;
  }
}

void PrintTraderEvalResult(const EvaluationResult& eval_result) {
  std::cout << "------------------ period ------------------"
            << "    trader & base gain    score    t&b volatility" << std::endl;
  for (const EvaluationResult::Period& period : eval_result.period()) {
    std::cout << "["
              << ConvertTimestampSecToDateTimeUTC(period.start_timestamp_sec())
              << " - "
              << ConvertTimestampSecToDateTimeUTC(period.end_timestamp_sec())
              << "):" << std::fixed << std::setprecision(2) << std::setw(10)
              << (period.final_gain() - 1.0f) * 100.0f << "%" << std::setw(10)
              << (period.base_final_gain() - 1.0f) * 100.0f << "%"
              << std::setprecision(3) << std::setw(9)
              << period.final_gain() / period.base_final_gain() << std::setw(9)
              << period.result().trader_volatility() << std::setw(9)
              << period.result().base_volatility() << std::endl;
  }
}
}  // namespace

int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  AccountConfig account_config = GetAccountConfig();

  long start_timestamp_sec = 0;
  long end_timestamp_sec = 0;
  if (FLAGS_start_date_utc.empty() || FLAGS_end_date_utc.empty() ||
      !ConvertDateUTCToTimestampSec(FLAGS_start_date_utc,
                                    start_timestamp_sec) ||
      !ConvertDateUTCToTimestampSec(FLAGS_end_date_utc, end_timestamp_sec)) {
    std::cerr << "Invalid time period" << std::endl;
    return 1;
  }

  EvaluationConfig eval_config;
  eval_config.set_start_timestamp_sec(start_timestamp_sec);
  eval_config.set_end_timestamp_sec(end_timestamp_sec);
  eval_config.set_evaluation_period_months(FLAGS_evaluation_period_months);

  OhlcHistory ohlc_history =
      GetOhlcHistoryFromFlags(start_timestamp_sec, end_timestamp_sec);
  if (ohlc_history.empty()) {
    std::cerr << "No input history" << std::endl;
    return 1;
  }

  auto start = std::chrono::high_resolution_clock::now();
  if (FLAGS_evaluate_batch) {
    eval_config.set_fast_eval(true);
    std::cout << std::endl << "Batch evaluation:" << std::endl;
    std::vector<std::unique_ptr<TraderEmitter>> trader_emitters =
        GetBatchOfTraders(FLAGS_trader);
    std::vector<EvaluationResult> eval_results =
        EvaluateBatchOfTraders(account_config, eval_config, ohlc_history,
                               /* side_input = */ nullptr, trader_emitters);
    std::sort(eval_results.begin(), eval_results.end(),
              [](const EvaluationResult& lhs, const EvaluationResult& rhs) {
                return lhs.score() > rhs.score();
              });
    PrintBatchEvalResults(eval_results, 20);
  } else {
    eval_config.set_fast_eval(false);
    std::unique_ptr<TraderEmitter> trader_emitter = GetTrader(FLAGS_trader);
    std::cout << std::endl
              << trader_emitter->GetName() << " evaluation:" << std::endl;
    std::unique_ptr<std::ofstream> exchange_log_stream =
        OpenLogFile(FLAGS_output_exchange_log_file);
    std::unique_ptr<std::ofstream> trader_log_stream =
        OpenLogFile(FLAGS_output_trader_log_file);
    CsvLogger logger(exchange_log_stream.get(), trader_log_stream.get());
    EvaluationResult eval_result =
        EvaluateTrader(account_config, eval_config, ohlc_history,
                       /* side_input = */ nullptr, *trader_emitter, &logger);
    PrintTraderEvalResult(eval_result);
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << std::endl
            << "Evaluated in " << duration.count() / 1000.0 << " seconds"
            << std::endl;

  // Optional: Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
