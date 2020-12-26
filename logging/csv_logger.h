// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef LOGGING_CSV_LOGGER_H
#define LOGGING_CSV_LOGGER_H

#include "logging/logger.h"

namespace trader {

// CSV logger of exchange movements and trader internal state(s).
class CsvLogger : public Logger {
 public:
  // Constructor. Does not take ownership of the provided output streams.
  // exchange_os: Output stream for exchange movements. Ignored if nullptr.
  // trader_os: Output stream for trader internal state(s). Ignored if nullptr.
  CsvLogger(std::ostream* exchange_os, std::ostream* trader_os)
      : exchange_os_(exchange_os), trader_os_(trader_os) {}
  virtual ~CsvLogger() {}

  // Logs the current ohlc_tick and account.
  void LogExchangeState(const OhlcTick& ohlc_tick,
                        const Account& account) override;
  // Logs the current ohlc_tick, account, and order, after executing
  // the given order.
  void LogExchangeState(const OhlcTick& ohlc_tick, const Account& account,
                        const Order& order) override;

  // Logs the trader state.
  void LogTraderState(const std::string& trader_state) override;

 private:
  std::ostream* exchange_os_;
  std::ostream* trader_os_;
};

}  // namespace trader

#endif  // LOGGING_CSV_LOGGER_H
