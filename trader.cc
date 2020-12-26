// Copyright © 2020 Peter Cerno. All rights reserved.

#include <gflags/gflags.h>

#include "base/base.h"
#include "eval/eval.h"
#include "logging/csv_logger.h"
#include "traders/limit_trader.h"
#include "traders/rebalancing_trader.h"
#include "traders/stop_trader.h"
#include "util/proto.h"
#include "util/time.h"

DEFINE_string(input_ohlc_history_delimited_proto_file, "",
              "Input file containing the delimited OhlcRecord protos.");
DEFINE_string(output_exchange_log_file, "",
              "Output CSV file containing the exchange log.");
DEFINE_string(output_log_file, "",
              "Output file containing the trader-dependent log.");
DEFINE_string(trader, "limit",
              "Trader to be executed. [limit, rebalancing, stop].");

DEFINE_string(start_date_utc, "2016-01-01",
              "Start date YYYY-MM-DD in UTC (included).");
DEFINE_string(end_date_utc, "2017-01-01",
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
static constexpr char kLimitTraderName[] = "limit";
static constexpr char kRebalancingTraderName[] = "rebalancing";
static constexpr char kStopTraderName[] = "stop";

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

// Returns the default limit trader emitter.
std::unique_ptr<TraderEmitter> GetDefaultLimitTraderEmitter() {
  LimitTraderConfig config;
  config.set_alpha_per_hour(0.03f);
  config.set_limit_buy_margin(0.01f);
  config.set_limit_sell_margin(0.01f);
  return std::unique_ptr<TraderEmitter>(new LimitTraderEmitter(config));
}

// Returns the default batch of limit traders.
std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfLimitTraders() {
  return LimitTraderEmitter::GetBatchOfTraders(
      /* alphas_per_hour = */ {0.01f, 0.02f, 0.03f},
      /* limit_buy_margins = */ {0.005f, 0.01f, 0.015f},
      /* limit_sell_margins = */ {0.005f, 0.01f, 0.015f});
}

// Returns the default rebalancing trader emitter.
std::unique_ptr<TraderEmitter> GetDefaultRebalancingTraderEmitter() {
  RebalancingTraderConfig config;
  config.set_alpha(4.0f);
  config.set_beta(0.1f);
  config.set_upper_deviation(0.2f);
  config.set_lower_deviation(0.2f);
  return std::unique_ptr<TraderEmitter>(new RebalancingTraderEmitter(config));
}

// Returns the default batch of rebalancing traders.
std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfRebalancingTraders() {
  return RebalancingTraderEmitter::GetBatchOfTraders(
      /* alphas = */ {0.5f, 1.0f, 2.0f, 4.0f, 9.0f},
      /* deviations = */ {0.1f, 0.2f, 0.3f, 0.4f});
}

// Returns the default stop trader emitter.
std::unique_ptr<TraderEmitter> GetDefaultStopTraderEmitter() {
  StopTraderConfig config;
  config.set_stop_order_margin(0.1f);
  config.set_stop_order_move_margin(0.1f);
  config.set_stop_order_increase_per_day(0.01f);
  config.set_stop_order_decrease_per_day(0.1f);
  return std::unique_ptr<TraderEmitter>(new StopTraderEmitter(config));
}

// Returns the default batch of stop traders.
std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfStopTraders() {
  return StopTraderEmitter::GetBatchOfTraders(
      /* stop_order_margins = */ {0.05, 0.1, 0.15, 0.2},
      /* stop_order_move_margins = */ {0.05, 0.1, 0.15, 0.2},
      /* stop_order_increases_per_day = */ {0.01, 0.05, 0.1},
      /* stop_order_decreases_per_day = */ {0.01, 0.05, 0.1});
}

// Returns the default trader emitter.
std::unique_ptr<TraderEmitter> GetDefaultTraderEmitter() {
  if (FLAGS_trader == kLimitTraderName) {
    return GetDefaultLimitTraderEmitter();
  } else if (FLAGS_trader == kRebalancingTraderName) {
    return GetDefaultRebalancingTraderEmitter();
  } else {
    assert(FLAGS_trader == kStopTraderName);
    return GetDefaultStopTraderEmitter();
  }
}

// Returns the default batch of traders.
std::vector<std::unique_ptr<TraderEmitter>> GetDefaultBatchOfTraders() {
  if (FLAGS_trader == kLimitTraderName) {
    return GetBatchOfLimitTraders();
  } else if (FLAGS_trader == kRebalancingTraderName) {
    return GetBatchOfRebalancingTraders();
  } else {
    assert(FLAGS_trader == kStopTraderName);
    return GetBatchOfStopTraders();
  }
}

// Reads and returns the price / OHLC history.
template <typename T>
std::vector<T> ReadHistory(const std::string& file_name) {
  std::vector<T> history;
  auto start = std::chrono::high_resolution_clock::now();
  if (!ReadDelimitedMessagesFromFile<T>(file_name, history)) {
    return {};
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "Loaded " << history.size() << " records in "
            << duration.count() / 1000.0 << " seconds" << std::endl;
  return history;
}

// Gets the OHLC history based on flags.
OhlcHistory GetOhlcHistoryFromFlags(long start_timestamp_sec,
                                    long end_timestamp_sec) {
  if (!FLAGS_input_ohlc_history_delimited_proto_file.empty()) {
    OhlcHistory ohlc_history =
        ReadHistory<OhlcTick>(FLAGS_input_ohlc_history_delimited_proto_file);
    OhlcHistory ohlc_history_subset =
        HistorySubsetCopy(ohlc_history, start_timestamp_sec, end_timestamp_sec);
    std::cout << "Selected " << ohlc_history_subset.size()
              << " OHLC ticks within the period: "
              << TimestampPeriodToString(start_timestamp_sec, end_timestamp_sec)
              << std::endl;
    return ohlc_history_subset;
  }
  return {};
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

// Prints top_n evaluation results.
void PrintTraderEvalResults(const std::vector<EvaluationResult>& eval_results,
                            size_t top_n) {
  const size_t eval_count = std::min(top_n, eval_results.size());
  for (int eval_index = 0; eval_index < eval_count; ++eval_index) {
    const EvaluationResult& eval_result = eval_results.at(eval_index);
    std::cout << eval_result.name() << ": " << eval_result.score() << std::endl;
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

  if (FLAGS_evaluate_batch) {
    std::cout << std::endl << "Batch evaluation:" << std::endl;
    std::vector<std::unique_ptr<TraderEmitter>> trader_emitters =
        GetDefaultBatchOfTraders();
    std::vector<EvaluationResult> eval_results = EvaluateBatchOfTraders(
        account_config, eval_config, ohlc_history, trader_emitters);
    std::sort(eval_results.begin(), eval_results.end(),
              [](const EvaluationResult& lhs, const EvaluationResult& rhs) {
                return lhs.score() > rhs.score();
              });
    PrintTraderEvalResults(eval_results, 20);
  } else {
    std::cout << std::endl << "Trader evaluation:" << std::endl;
    std::unique_ptr<TraderEmitter> trader_emitter = GetDefaultTraderEmitter();
    std::unique_ptr<std::ofstream> exchange_log_stream =
        OpenLogFile(FLAGS_output_exchange_log_file);
    std::unique_ptr<std::ofstream> log_stream =
        OpenLogFile(FLAGS_output_log_file);
    CsvLogger logger(exchange_log_stream.get(), log_stream.get());
    EvaluationResult eval_result = EvaluateTrader(
        account_config, eval_config, ohlc_history, *trader_emitter, &logger);
    for (const EvaluationResult::Period& period : eval_result.period()) {
      std::cout << "["
                << ConvertTimestampSecToDateTimeUTC(
                       period.start_timestamp_sec())
                << " - "
                << ConvertTimestampSecToDateTimeUTC(period.end_timestamp_sec())
                << "): " << period.final_gain() / period.base_final_gain()
                << std::endl;
    }
  }

  // Optional: Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
