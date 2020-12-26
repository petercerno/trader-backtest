// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef EVAL_EVAL_H
#define EVAL_EVAL_H

#include "base/account.h"
#include "base/base.h"
#include "base/trader.h"
#include "eval/eval.pb.h"
#include "logging/logger.h"

namespace trader {

// Executes an instance of a trader over a region of the OHLC history.
// Returns the final trader's ExecutionResult at the end of the execution.
ExecutionResult ExecuteTrader(const AccountConfig& account_config,
                              OhlcHistory::const_iterator ohlc_history_begin,
                              OhlcHistory::const_iterator ohlc_history_end,
                              Trader& trader, Logger* logger);

// Evaluates a single (type of) trader (as emitted by the trader_emitter)
// over one or more regions of the OHLC history (as defined by the
// eval_config). Returns trader's EvaluationResult.
EvaluationResult EvaluateTrader(const AccountConfig& account_config,
                                const EvaluationConfig& eval_config,
                                const OhlcHistory& ohlc_history,
                                const TraderEmitter& trader_emitter,
                                Logger* logger);

// Evaluates (in parallel) a batch of traders (as emitted by the vector of
// trader_emitters) over one or more regions of the OHLC history.
std::vector<EvaluationResult> EvaluateBatchOfTraders(
    const AccountConfig& account_config, const EvaluationConfig& eval_config,
    const OhlcHistory& ohlc_history,
    const std::vector<std::unique_ptr<TraderEmitter>>& trader_emitters);

}  // namespace trader

#endif  // EVAL_EVAL_H
