// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef LOGGING_LOGGER_INTERFACE_H
#define LOGGING_LOGGER_INTERFACE_H

#include "lib/trader_account.h"
#include "lib/trader_base.h"

namespace trader {

// Interface for logging exchange movements and trader internal state(s).
class LoggerInterface {
 public:
  LoggerInterface() {}
  virtual ~LoggerInterface() {}

  // Logs the current ohlc_tick and trader_account.
  virtual void LogExchangeState(const OhlcTick& ohlc_tick,
                                const TraderAccount& trader_account) = 0;
  // Logs the current ohlc_tick, trader_account, and order, after executing
  // the given order.
  virtual void LogExchangeState(const OhlcTick& ohlc_tick,
                                const TraderAccount& trader_account,
                                const Order& order) = 0;

  // Logs the trader state.
  virtual void LogTraderState(const std::string& trader_state) = 0;
};

}  // namespace trader

#endif  // LOGGING_LOGGER_INTERFACE_H
