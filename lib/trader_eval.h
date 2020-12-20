// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef LIB_TRADER_EVAL_H
#define LIB_TRADER_EVAL_H

#include "lib/trader_account.h"
#include "lib/trader_interface.h"

namespace trader {

// Executes a given instance of "trader" over a region of the OHLC history.
// Returns the final trader account at the end of the execution.
// "exchange_os" is an optional output stream for logging exchange operations.
// "trader_os" is an optional output stream for logging internal trader state.
TraderExecutionResult ExecuteTrader(
    const TraderAccountConfig& trader_account_config,
    OhlcHistory::const_iterator ohlc_history_begin,
    OhlcHistory::const_iterator ohlc_history_end, TraderInterface* trader,
    std::ostream* exchange_os, std::ostream* trader_os);

// Evaluates a single type of trader (as defined by the "trader_factory")
// over one or more regions of the OHLC history (as defined by the
// "trader_eval_config"). Returns TraderEvaluationResult.
// "exchange_os" is an optional output stream for logging exchange operations.
// "trader_os" is an optional output stream for logging internal trader state.
TraderEvaluationResult EvaluateTrader(
    const TraderAccountConfig& trader_account_config,
    const TraderEvaluationConfig& trader_eval_config,
    const OhlcHistory& ohlc_history,
    const TraderFactoryInterface& trader_factory, std::ostream* exchange_os,
    std::ostream* trader_os);

// Evaluates (in parallel) a batch of traders (as defined by the vector of
// "trader_factories") over one or more regions of the OHLC history.
std::vector<TraderEvaluationResult> EvaluateBatchOfTraders(
    const TraderAccountConfig& trader_account_config,
    const TraderEvaluationConfig& trader_eval_config,
    const OhlcHistory& ohlc_history,
    const std::vector<std::unique_ptr<TraderFactoryInterface>>&
        trader_factories);

}  // namespace trader

#endif  // LIB_TRADER_EVAL_H
