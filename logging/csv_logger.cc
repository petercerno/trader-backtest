// Copyright © 2021 Peter Cerno. All rights reserved.

#include "logging/csv_logger.h"

namespace trader {
namespace {
// Logs the ohlc_tick to the output stream os.
void LogOhlcTick(const OhlcTick& ohlc_tick, std::ostream* os) {
  *os << std::fixed << std::setprecision(0)  // nowrap
      << ohlc_tick.timestamp_sec() << ","    // nowrap
      << std::setprecision(3)                // nowrap
      << ohlc_tick.open() << ","             // nowrap
      << ohlc_tick.high() << ","             // nowrap
      << ohlc_tick.low() << ","              // nowrap
      << ohlc_tick.close() << ","            // nowrap
      << ohlc_tick.volume();
}

// Logs the account balances to the output stream os.
void LogAccount(const Account& account, std::ostream* os) {
  *os << std::fixed << std::setprecision(3)  // nowrap
      << account.base_balance << ","         // nowrap
      << account.quote_balance << ","        // nowrap
      << account.total_fee;
}

// Logs the order to the output stream os.
void LogOrder(const Order& order, std::ostream* os) {
  *os << Order::Type_Name(order.type()) << ","  // nowrap
      << Order::Side_Name(order.side()) << ","  // nowrap
      << std::fixed << std::setprecision(3);
  if (order.oneof_amount_case() == Order::kBaseAmount) {
    *os << order.base_amount();
  }
  *os << ",";
  if (order.oneof_amount_case() == Order::kQuoteAmount) {
    *os << order.quote_amount();
  }
  *os << ",";
  if (order.has_price() && order.price() > 0) {
    *os << order.price();
  }
}

// Logs empty order to the output stream os.
void LogEmptyOrder(std::ostream* os) { *os << ",,,,"; }
}  // namespace

void CsvLogger::LogExchangeState(const OhlcTick& ohlc_tick,
                                 const Account& account) {
  if (exchange_os_ == nullptr) {
    return;
  }
  LogOhlcTick(ohlc_tick, exchange_os_);
  *exchange_os_ << ",";
  LogAccount(account, exchange_os_);
  *exchange_os_ << ",";
  LogEmptyOrder(exchange_os_);
  *exchange_os_ << std::endl;
}

void CsvLogger::LogExchangeState(const OhlcTick& ohlc_tick,
                                 const Account& account, const Order& order) {
  if (exchange_os_ == nullptr) {
    return;
  }
  LogOhlcTick(ohlc_tick, exchange_os_);
  *exchange_os_ << ",";
  LogAccount(account, exchange_os_);
  *exchange_os_ << ",";
  LogOrder(order, exchange_os_);
  *exchange_os_ << std::endl;
}

void CsvLogger::LogTraderState(const std::string& state) {
  if (trader_os_ == nullptr) {
    return;
  }
  *trader_os_ << state << std::endl;
}

}  // namespace trader
