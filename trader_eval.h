//  Copyright © 2017 Peter Cerno. All rights reserved.

#ifndef TRADER_EVAL_H
#define TRADER_EVAL_H

#include "trader_base.h"

namespace trader {

// Trader evaluator.
class TraderEval {
 public:
  TraderEval(const ExchangeAccountConfig& exchange_account_config,
             const EvalConfig& eval_config, const OhlcHistory& ohlc_history);
  virtual ~TraderEval() {}

  // Evaluates the given trader and returns the resulting EvalResult proto.
  // If keep_intermediate_states is true, then EvalResult::Period(s) have all
  // exchange account states filled in. Otherwise, only the first and the last
  // exchange account states are kept.
  EvalResult Evaluate(const TraderInterface* trader_instance,
                      bool keep_intermediate_states) const;

  // Asynchronously evaluates the given batch of traders and returns the
  // corresponding vector of EvalResult protos.
  EvalResults EvaluateBatch(const TraderBatch& batch) const;

  // Returns the evaluator's configuration.
  const EvalConfig& GetEvalConfig() const { return eval_config_; }

 private:
  // Exchange account configuration.
  ExchangeAccountConfig exchange_account_config_;
  // Evaluation configuration.
  EvalConfig eval_config_;
  // Historical OHLC ticks over time.
  OhlcHistory ohlc_history_;
};

// Returns trader's gain (after fees).
float GetTraderGain(const ExchangeAccountStates& exchange_account_states);
// Returns baseline gain.
float GetBaselineGain(const ExchangeAccountStates& exchange_account_states);

}  // namespace trader

#endif  // TRADER_EVAL_H
