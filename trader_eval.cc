//  Copyright © 2017 Peter Cerno. All rights reserved.

#include "trader_eval.h"

#include <functional>
#include <future>

#include "exchange_account.h"
#include "util_time.h"

namespace trader {

using google::protobuf::RepeatedPtrField;

namespace {
// Returns the average of values returned by the selector over the given
// evaluation periods.
float GetPeriodAverage(
    const RepeatedPtrField<EvalResult::Period>& periods,
    std::function<float(const EvalResult::Period&)> selector) {
  if (periods.empty()) {
    return 0;
  }
  float sum = 0;
  for (const EvalResult::Period& period : periods) {
    sum += selector(period);
  }
  return sum / periods.size();
}
}  // namespace

TraderEval::TraderEval(const ExchangeAccountConfig& exchange_account_config,
                       const EvalConfig& eval_config,
                       const OhlcHistory& ohlc_history)
    : exchange_account_config_(exchange_account_config),
      eval_config_(eval_config),
      ohlc_history_(ohlc_history) {}

EvalResult TraderEval::Evaluate(const TraderInterface* trader,
                                bool keep_intermediate_states) const {
  EvalResult eval_result;
  *eval_result.mutable_exchange_account_config() = exchange_account_config_;
  *eval_result.mutable_eval_config() = eval_config_;
  *eval_result.mutable_trader() = trader->ToString();
  for (int month_offset = 0;; ++month_offset) {
    int start_eval_timestamp_sec = static_cast<int>(AddMonthsToTimestampSec(
        eval_config_.start_timestamp_sec(), month_offset));
    int end_eval_timestamp_sec =
        eval_config_.evaluation_period_months() > 0
            ? static_cast<int>(AddMonthsToTimestampSec(
                  start_eval_timestamp_sec,
                  eval_config_.evaluation_period_months()))
            : eval_config_.end_timestamp_sec();
    if (end_eval_timestamp_sec > eval_config_.end_timestamp_sec()) {
      break;
    }
    ExchangeAccount exchange_account(exchange_account_config_);
    TraderInstance trader_copy = trader->NewInstance();
    trader_copy->SetLogStream(trader->LogStream());
    ExchangeAccountStates states = exchange_account.Execute(
        ohlc_history_, start_eval_timestamp_sec, end_eval_timestamp_sec,
        keep_intermediate_states, trader_copy.get());
    EvalResult::Period* period = eval_result.add_period();
    period->set_start_timestamp_sec(start_eval_timestamp_sec);
    period->set_end_timestamp_sec(end_eval_timestamp_sec);
    period->set_trader_final_gain(GetTraderGain(states));
    period->set_trader_final_value(states.rbegin()->value());
    period->set_total_executed_orders(
        states.rbegin()->accumulated_executed_orders());
    period->set_total_security_amount(
        states.rbegin()->accumulated_security_amount());
    period->set_total_fee(states.rbegin()->accumulated_fee());
    period->set_base_final_gain(GetBaselineGain(states));
    period->set_base_final_value(states.begin()->value() *
                                 GetBaselineGain(states));
    for (const ExchangeAccountState& state : states) {
      *period->add_exchange_account_state() = state;
    }
    if (eval_config_.evaluation_period_months() == 0) {
      break;
    }
  }
  eval_result.set_score(GetPeriodAverage(
      eval_result.period(), [](const EvalResult::Period& period) {
        return period.trader_final_gain() / period.base_final_gain();
      }));
  eval_result.set_avg_trader_gain(GetPeriodAverage(
      eval_result.period(), [](const EvalResult::Period& period) {
        return period.trader_final_gain();
      }));
  eval_result.set_avg_total_executed_orders(GetPeriodAverage(
      eval_result.period(), [](const EvalResult::Period& period) {
        return period.total_executed_orders();
      }));
  eval_result.set_avg_total_security_amount(GetPeriodAverage(
      eval_result.period(), [](const EvalResult::Period& period) {
        return period.total_security_amount();
      }));
  eval_result.set_avg_total_fee(GetPeriodAverage(
      eval_result.period(),
      [](const EvalResult::Period& period) { return period.total_fee(); }));
  eval_result.set_avg_base_gain(GetPeriodAverage(
      eval_result.period(), [](const EvalResult::Period& period) {
        return period.base_final_gain();
      }));
  return eval_result;
}

EvalResults TraderEval::EvaluateBatch(const TraderBatch& batch) const {
  EvalResults eval_results;
  std::vector<std::future<EvalResult>> eval_result_futures;
  for (const TraderInstance& trader_instance : batch) {
    eval_result_futures.emplace_back(
        std::async(&TraderEval::Evaluate, this, trader_instance.get(),
                   /* keep_intermediate_states = */ false));
  }
  for (std::future<EvalResult>& eval_result_future : eval_result_futures) {
    eval_results.emplace_back(eval_result_future.get());
  }
  return eval_results;
}

float GetTraderGain(const ExchangeAccountStates& exchange_account_states) {
  return exchange_account_states.rbegin()->value() /
         exchange_account_states.begin()->value();
}

float GetBaselineGain(const ExchangeAccountStates& exchange_account_states) {
  return exchange_account_states.rbegin()->ohlc_tick().close() /
         exchange_account_states.begin()->ohlc_tick().close();
}

}  // namespace trader
