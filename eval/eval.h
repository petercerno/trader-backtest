// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef EVAL_EVAL_H
#define EVAL_EVAL_H

#include "base/account.h"
#include "base/base.h"
#include "base/trader.h"
#include "eval/eval.pb.h"
#include "logging/logger.h"

namespace trader {

// Executes a given instance of trader over a region of the OHLC history.
// Returns the final trader account at the end of the execution.
ExecutionResult ExecuteTrader(const AccountConfig& account_config,
                              OhlcHistory::const_iterator ohlc_history_begin,
                              OhlcHistory::const_iterator ohlc_history_end,
                              TraderInterface& trader, LoggerInterface* logger);

// Evaluates a single (type of) trader (as emitted by the trader_factory)
// over one or more regions of the OHLC history (as defined by the
// eval_config). Returns trader's EvaluationResult.
EvaluationResult EvaluateTrader(const AccountConfig& account_config,
                                const EvaluationConfig& eval_config,
                                const OhlcHistory& ohlc_history,
                                const TraderFactoryInterface& trader_factory,
                                LoggerInterface* logger);

// Evaluates (in parallel) a batch of traders (as emitted by the vector of
// trader_factories) over one or more regions of the OHLC history.
std::vector<EvaluationResult> EvaluateBatchOfTraders(
    const AccountConfig& account_config, const EvaluationConfig& eval_config,
    const OhlcHistory& ohlc_history,
    const std::vector<std::unique_ptr<TraderFactoryInterface>>&
        trader_factories);

}  // namespace trader

#endif  // EVAL_EVAL_H
