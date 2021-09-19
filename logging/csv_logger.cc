// Copyright © 2021 Peter Cerno. All rights reserved.

#include "logging/csv_logger.h"

#include "absl/strings/str_format.h"

namespace trader {
namespace {
// Returns a CSV representation of the given ohlc_tick.
std::string OhlcTickToCsv(const OhlcTick& ohlc_tick) {
  return absl::StrFormat("%d,%.3f,%.3f,%.3f,%.3f,%.3f",  // nowrap
                         ohlc_tick.timestamp_sec(),      // nowrap
                         ohlc_tick.open(),               // nowrap
                         ohlc_tick.high(),               // nowrap
                         ohlc_tick.low(),                // nowrap
                         ohlc_tick.close(),              // nowrap
                         ohlc_tick.volume());
}

// Returns a CSV representation of the given account.
std::string AccountToCsv(const Account& account) {
  return absl::StrFormat("%.3f,%.3f,%.3f",       // nowrap
                         account.base_balance,   // nowrap
                         account.quote_balance,  // nowrap
                         account.total_fee);
}

// Returns a CSV representation of the given order.
std::string OrderToCsv(const Order& order) {
  return absl::StrFormat(
      "%s,%s,%s,%s,%s",                // nowrap
      Order::Type_Name(order.type()),  // nowrap
      Order::Side_Name(order.side()),  // nowrap
      order.oneof_amount_case() == Order::kBaseAmount
          ? absl::StrFormat("%.3f", order.base_amount())
          : "",
      order.oneof_amount_case() == Order::kQuoteAmount
          ? absl::StrFormat("%.3f", order.quote_amount())
          : "",
      order.price() > 0 ? absl::StrFormat("%.3f", order.price()) : "");
}

// Returns a CSV representation of an empty order.
std::string EmptyOrderToCsv() { return ",,,,"; }
}  // namespace

void CsvLogger::LogExchangeState(const OhlcTick& ohlc_tick,
                                 const Account& account) {
  if (exchange_os_ != nullptr) {
    *exchange_os_ << absl::StrFormat("%s,%s,%s\n", OhlcTickToCsv(ohlc_tick),
                                     AccountToCsv(account), EmptyOrderToCsv());
  }
}

void CsvLogger::LogExchangeState(const OhlcTick& ohlc_tick,
                                 const Account& account, const Order& order) {
  if (exchange_os_ != nullptr) {
    *exchange_os_ << absl::StrFormat("%s,%s,%s\n", OhlcTickToCsv(ohlc_tick),
                                     AccountToCsv(account), OrderToCsv(order));
  }
}

void CsvLogger::LogTraderState(absl::string_view trader_state) {
  if (trader_os_ != nullptr) {
    *trader_os_ << trader_state << "\n";
  }
}

}  // namespace trader
