// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef LOGGING_LOGGER_H
#define LOGGING_LOGGER_H

#include "base/account.h"
#include "base/base.h"

namespace trader {

// Interface for logging exchange movements and trader internal state(s).
class Logger {
 public:
  Logger() {}
  virtual ~Logger() {}

  // Logs the current ohlc_tick and account.
  virtual void LogExchangeState(const OhlcTick& ohlc_tick,
                                const Account& account) = 0;
  // Logs the current ohlc_tick, account, and order, after executing
  // the given order.
  virtual void LogExchangeState(const OhlcTick& ohlc_tick,
                                const Account& account, const Order& order) = 0;

  // Logs the trader state.
  virtual void LogTraderState(const std::string& trader_state) = 0;
};

}  // namespace trader

#endif  // LOGGING_LOGGER_H
