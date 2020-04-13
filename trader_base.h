// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef TRADER_BASE_H
#define TRADER_BASE_H

#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "trader.pb.h"

namespace trader {

// Historical prices over time.
using PriceHistory = std::vector<PriceRecord>;

// Historical OHLC ticks over time.
using OhlcHistory = std::vector<OhlcTick>;

// Every trader works as follows:
// - At every step the trader receives the latest OHLC tick T[i] and account
//   balances. The current time is at the end of the OHLC tick T[i] time period.
//   The trader updates its internal state based on this OHLC tick T[i].
// - Then trader needs to decide what orders to emit. There are no active orders
//   on the exchange at this moment (see the explanation below).
// - Once the trader decides, which orders to emit, the exchange will execute
//   (or cancel) all these orders on the follow-up OHLC tick T[i+1]. The trader
//   does not see the follow-up OHLC tick T[i+1], so the process is fair.
// - Once all orders are executed (or canceled) by the exchange, the trader
//   receives the follow-up OHLC tick T[i+1] and the whole process repeats.
// Note that at every step every order gets either executed or canceled by the
// exchange. This is a design simplification so that there are no active orders
// that the trader needs to maintain over time.
// In practice, however, we would not cancel orders if they would be re-emitted
// again. We would simply modify the existing orders if needed.
class TraderInterface {
 public:
  TraderInterface() {}
  virtual ~TraderInterface() {}

  // Updates the (internal) trader state and emits zero or more orders.
  // We assume that "orders" is not null and points to an empty vector.
  // This method is called consecutively (by the exchange) on every OHLC tick.
  // Trader can assume that there are no active orders when this method is
  // called. The emitted orders will be either executed or cancelled by the
  // exchange at the next OHLC tick.
  virtual void Update(const OhlcTick& ohlc_tick, float security_balance,
                      float cash_balance, std::vector<Order>* orders) = 0;

  // Outputs the internal trader state into the output stream "os".
  // Does nothing if "os" is nullptr.
  // Note that it is recommended to output the internal state as a string of
  // comma-separated values for easier analysis.
  virtual void LogInternalState(std::ostream* os) const = 0;
};

// Usually we want to evaluate the same trader over different time periods.
// This is where the trader factory comes in handy, as it can emit a new
// instance of the same trader (with the same configuration) whenever needed.
class TraderFactoryInterface {
 public:
  TraderFactoryInterface() {}
  virtual ~TraderFactoryInterface() {}

  // Returns a name identifying all traders emitted by this factory.
  // The name should be escaped for the CSV file format.
  virtual std::string GetTraderName() const = 0;

  // Returns a new (freshly initialized) instance of a trader.
  virtual std::unique_ptr<TraderInterface> NewTrader() const = 0;
};

// Returns a pair of iterators covering the time interval [start_timestamp_sec,
// end_timestamp_sec) of the given history.
template <typename T>
std::pair<typename std::vector<T>::const_iterator,
          typename std::vector<T>::const_iterator>
HistorySubset(const std::vector<T>& history, long start_timestamp_sec,
              long end_timestamp_sec) {
  const auto record_compare = [](const T& record, long timestamp_sec) {
    return record.timestamp_sec() < timestamp_sec;
  };
  const auto record_begin =
      start_timestamp_sec > 0
          ? std::lower_bound(history.begin(), history.end(),
                             start_timestamp_sec, record_compare)
          : history.begin();
  const auto record_end =
      end_timestamp_sec > 0
          ? std::lower_bound(history.begin(), history.end(), end_timestamp_sec,
                             record_compare)
          : history.end();
  return {record_begin, record_end};
}

// Returns the subset of the given history covering the time interval
// [start_timestamp_sec, end_timestamp_sec).
template <typename T>
std::vector<T> HistorySubsetCopy(const std::vector<T>& history,
                                 long start_timestamp_sec,
                                 long end_timestamp_sec) {
  std::vector<T> history_subset_copy;
  const auto history_subset =
      HistorySubset(history, start_timestamp_sec, end_timestamp_sec);
  const auto size = std::distance(history_subset.first, history_subset.second);
  if (size == 0) {
    return {};
  }
  history_subset_copy.reserve(size);
  std::copy(history_subset.first, history_subset.second,
            std::back_inserter(history_subset_copy));
  return history_subset_copy;
}

}  // namespace trader

#endif  // TRADER_BASE_H
