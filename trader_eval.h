//  Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef TRADER_EVAL_H
#define TRADER_EVAL_H

#include "trader_account.h"
#include "trader_base.h"

namespace trader {

// Executes "order" over the given "ohlc_tick" and updates the "trader_account".
// Returns true iff the order was executed successfully.
bool ExecuteOrder(const TraderAccountConfig& trader_account_config,
                  const Order& order, const OhlcTick& ohlc_tick,
                  TraderAccount* trader_account);

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

#endif  // TRADER_EVAL_H
