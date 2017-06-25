// Copyright © 2017 Peter Cerno. All rights reserved.

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <gflags/gflags.h>

#include "limit_trader.h"
#include "limit_trader_v2.h"
#include "stop_trader.h"
#include "trader_base.h"
#include "trader_eval.h"
#include "trader_io.h"
#include "util_proto.h"
#include "util_time.h"

// Input/Output files.
DEFINE_string(input_price_history_delimited_proto_file, "",
              "Input file containing the delimited PriceRecord protos.");
DEFINE_string(input_ohlc_history_delimited_proto_file, "",
              "Input file containing the delimited OhlcRecord protos.");
DEFINE_string(output_exchange_states_csv_file, "",
              "Ouptut CSV file containing the exchange account states.");

// Trader.
DEFINE_string(trader, "limit-v2",
              "Trader to be executed. [limit, limit-v2, stop].");
DEFINE_string(output_trader_log_file, "",
              "Ouptut file containing the trader-dependent log.");

// Time period and sampling.
DEFINE_string(start_date_utc, "2016-01-01",
              "Start date YYYY-MM-DD in UTC (included).");
DEFINE_string(end_date_utc, "2017-01-01",
              "End date YYYY-MM-DD in UTC (excluded).");
DEFINE_double(max_price_deviation_per_min, 0.02,
              "Maximum allowed price deviation per minute.");
DEFINE_int32(sampling_rate_sec, 300, "Sampling rate in seconds.");
DEFINE_int32(evaluation_period_months, 6, "Evaluation period in months.");

// Starting balance.
DEFINE_double(start_security_balance, 1.0, "Starting security amount.");
DEFINE_double(start_cash_balance, 0.0, "Starting cash amount.");

// Parameters of the exchange account.
DEFINE_double(order_execution_accuracy, 0.5,
              "Accuracy of executing market / stop orders at their price.");
DEFINE_double(limit_order_fill_volume_ratio, 0.5,
              "Ratio of (tick) volume used to fill the limit order. "
              "Not used if zero.");

// Evaluate a batch o parameters.
DEFINE_bool(evaluate_batch, false, "Batch evaluation.");

using namespace trader;

namespace {
const char kStopTraderName[] = "stop";
const char kLimitTraderName[] = "limit";
const char kLimitTraderV2Name[] = "limit-v2";

// Returns the ExchangeAccountConfig based on the flags (and default values).
ExchangeAccountConfig GetExchangeAccountConfig() {
  ExchangeAccountConfig config;
  config.set_start_security_balance(FLAGS_start_security_balance);
  config.set_start_cash_balance(FLAGS_start_cash_balance);
  config.set_security_balance_unit(0.01f);
  config.set_cash_balance_unit(0.01f);
  config.mutable_market_order_fee_config()->set_relative_fee(0.0025f);
  config.mutable_market_order_fee_config()->set_fixed_fee(0);
  config.mutable_market_order_fee_config()->set_minimum_fee(0);
  config.mutable_limit_order_fee_config()->set_relative_fee(0);
  config.mutable_limit_order_fee_config()->set_fixed_fee(0);
  config.mutable_limit_order_fee_config()->set_minimum_fee(0);
  config.mutable_stop_order_fee_config()->set_relative_fee(0.0025f);
  config.mutable_stop_order_fee_config()->set_fixed_fee(0);
  config.mutable_stop_order_fee_config()->set_minimum_fee(0);
  config.set_market_order_accuracy(FLAGS_order_execution_accuracy);
  config.set_stop_order_accuracy(FLAGS_order_execution_accuracy);
  config.set_limit_order_fill_volume_ratio(FLAGS_limit_order_fill_volume_ratio);
  return config;
}

// Returns the default stop trader instance.
TraderInstance GetStopTraderInstance() {
  StopTraderConfig config;
  config.set_stop_order_margin(0.2f);
  config.set_stop_order_move_margin(0.1f);
  config.set_stop_order_increase_per_day(0.2f);
  config.set_stop_order_decrease_per_day(0.2f);
  return TraderInstance(new StopTrader(config));
}

// Returns the default batch of stop traders.
TraderBatch GetStopTraderBatch() {
  return StopTrader::GetBatch(
      /* stop_order_margins = */ {0.15, 0.2, 0.25, 0.3},
      /* stop_order_move_margins = */ {0.05, 0.08, 0.1, 0.12},
      /* stop_order_increases_per_day = */ {0.1, 0.15, 0.2, 0.25},
      /* stop_order_decreases_per_day = */ {0.1, 0.15, 0.2, 0.25});
}

// Returns the default limit trader instance.
TraderInstance GetLimitTraderInstance() {
  LimitTraderConfig config;
  config.set_alpha_per_hour(0.03f);
  config.set_limit_order_margin(0.01f);
  return TraderInstance(new LimitTrader(config));
}

// Returns the default batch of limit traders.
TraderBatch GetLimitTraderBatch() {
  return LimitTrader::GetBatch(
      /* alphas_per_hour = */ {0.01f, 0.02f, 0.03f},
      /* limit_order_margins = */ {0.005f, 0.01f, 0.015f});
}

// Returns the default limit trader instance.
TraderInstance GetLimitTraderV2Instance() {
  LimitTraderV2Config config;
  config.set_alpha_per_hour(0.03f);
  config.set_limit_buy_margin(0.01f);
  config.set_limit_sell_margin(0.01f);
  return TraderInstance(new LimitTraderV2(config));
}

// Returns the default batch of limit traders.
TraderBatch GetLimitTraderV2Batch() {
  return LimitTraderV2::GetBatch(
      /* alphas_per_hour = */ {0.01f, 0.02f, 0.03f},
      /* limit_buy_margins = */ {0.005f, 0.01f, 0.015f},
      /* limit_sell_margins = */ {0.005f, 0.01f, 0.015f});
}

// Returns the default trader instance.
TraderInstance GetTraderInstance() {
  if (FLAGS_trader == kStopTraderName) {
    return GetStopTraderInstance();
  } else if (FLAGS_trader == kLimitTraderName) {
    return GetLimitTraderInstance();
  } else {
    assert(FLAGS_trader == kLimitTraderV2Name);  // Invalid trader.
    return GetLimitTraderV2Instance();
  }
}

// Returns the default batch of traders.
TraderBatch GetTraderBatch() {
  if (FLAGS_trader == kStopTraderName) {
    return GetStopTraderBatch();
  } else if (FLAGS_trader == kLimitTraderName) {
    return GetLimitTraderBatch();
  } else {
    assert(FLAGS_trader == kLimitTraderV2Name);  // Invalid trader.
    return GetLimitTraderV2Batch();
  }
}

// Reads and returns the price / OHLC history.
template <typename T>
std::vector<T> ReadHistory(const std::string& file_name) {
  std::vector<T> history;
  auto start = std::chrono::high_resolution_clock::now();
  if (!ReadDelimitedMessagesFromFile(file_name, &history)) {
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
  if (!FLAGS_input_price_history_delimited_proto_file.empty() &&
      FLAGS_sampling_rate_sec > 0) {
    PriceHistory price_history = ReadHistory<PriceRecord>(
        FLAGS_input_price_history_delimited_proto_file);
    PriceHistory price_history_clean =
        RemoveOutliers(price_history, FLAGS_max_price_deviation_per_min);
    std::cout << "Removed " << price_history.size() - price_history_clean.size()
              << " outliers" << std::endl;
    return Resample(price_history_clean, start_timestamp_sec, end_timestamp_sec,
                    FLAGS_sampling_rate_sec);
  } else if (!FLAGS_input_ohlc_history_delimited_proto_file.empty()) {
    OhlcHistory ohlc_history =
        ReadHistory<OhlcTick>(FLAGS_input_ohlc_history_delimited_proto_file);
    return HistorySubsetCopy(ohlc_history, start_timestamp_sec,
                             end_timestamp_sec);
  }
  return {};
}
}  // namespace

int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  ExchangeAccountConfig exchange_account_config = GetExchangeAccountConfig();

  long start_timestamp_sec = 0;
  long end_timestamp_sec = 0;
  if (!ConvertDateUTCToTimestampSec(FLAGS_start_date_utc,
                                    &start_timestamp_sec) ||
      !ConvertDateUTCToTimestampSec(FLAGS_end_date_utc, &end_timestamp_sec)) {
    std::cerr << "Invalid time period" << std::endl;
    return 1;
  }

  EvalConfig eval_config;
  eval_config.set_start_timestamp_sec(static_cast<int>(start_timestamp_sec));
  eval_config.set_end_timestamp_sec(static_cast<int>(end_timestamp_sec));
  eval_config.set_evaluation_period_months(FLAGS_evaluation_period_months);

  OhlcHistory ohlc_history =
      GetOhlcHistoryFromFlags(start_timestamp_sec, end_timestamp_sec);
  if (ohlc_history.empty()) {
    std::cerr << "No input history" << std::endl;
    return 1;
  }

  TraderEval trader_eval(exchange_account_config, eval_config, ohlc_history);

  if (FLAGS_evaluate_batch) {
    TraderBatch trader_batch = GetTraderBatch();
    std::vector<EvalResult> eval_results =
        trader_eval.EvaluateBatch(trader_batch);
    std::sort(eval_results.begin(), eval_results.end(),
              [](const EvalResult& lhs, const EvalResult& rhs) {
                return lhs.score() > rhs.score();
              });
    PrintEvalResults(eval_results, 20);
  } else {
    TraderInstance trader_instance = GetTraderInstance();
    std::ofstream trader_log_stream;
    if (!FLAGS_output_trader_log_file.empty()) {
      if (FLAGS_evaluation_period_months > 0) {
        std::cerr << "Logging disabled when evaluating multiple periods"
                  << std::endl;
      } else {
        trader_log_stream.open(FLAGS_output_trader_log_file,
                               std::ios::out | std::ios::trunc);
        if (!trader_log_stream.is_open()) {
          std::cerr << "Cannot open file: " << FLAGS_output_trader_log_file
                    << std::endl;
          return 1;
        }
        trader_instance->SetLogStream(&trader_log_stream);
      }
    }
    EvalResult eval_result = trader_eval.Evaluate(
        trader_instance.get(), /* keep_intermediate_states = */ true);
    for (const EvalResult::Period& period : eval_result.period()) {
      std::cout << "["
                << ConvertTimestampSecToDateUTC(period.start_timestamp_sec())
                << " - "
                << ConvertTimestampSecToDateUTC(period.end_timestamp_sec())
                << "): "
                << period.trader_final_gain() / period.base_final_gain()
                << std::endl;
    }
    if (eval_result.period_size() == 1 &&
        eval_result.period(0).exchange_account_state_size() > 0 &&
        !FLAGS_output_exchange_states_csv_file.empty()) {
      ExchangeAccountStates exchange_account_states;
      exchange_account_states.reserve(
          eval_result.period(0).exchange_account_state_size());
      for (const ExchangeAccountState& state :
           eval_result.period(0).exchange_account_state()) {
        exchange_account_states.push_back(state);
      }
      WriteExchangeAccountStatesToCsvFile(
          exchange_account_states, FLAGS_output_exchange_states_csv_file);
      PrintExchangeAccountStatesStats(exchange_account_states);
    }
  }

  // Optional: Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
