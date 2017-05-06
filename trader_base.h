// Copyright © 2017 Peter Cerno. All rights reserved.

#ifndef TRADER_BASE_H
#define TRADER_BASE_H

#include <algorithm>
#include <cmath>
#include <iterator>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "trader.pb.h"

namespace trader {

// Historical prices over time.
using PriceHistory = std::vector<PriceRecord>;

// Historical OHLC ticks over time.
using OhlcHistory = std::vector<OhlcTick>;

// Returns the resampled ohlc_history within the interval [start_timestamp_sec,
// end_timestamp_sec) according to the given sampling rate (in seconds).
OhlcHistory Resample(const PriceHistory& price_history,
                     long start_timestamp_sec, long end_timestamp_sec,
                     int sampling_rate_sec);

// Exchange account states over time.
using ExchangeAccountStates = std::vector<ExchangeAccountState>;

// Evaluation results for different configurations.
using EvalResults = std::vector<EvalResult>;

// Forward declaration of TraderInterface.
class TraderInterface;

// Single instance of a trader.
using TraderInstance = std::unique_ptr<TraderInterface>;
// Batch of traders.
using TraderBatch = std::vector<TraderInstance>;

// Trader interface. At the beginning of every OHLC tick, there are no active
// orders on the exchange. After processing the OHLC tick, the trader can emit
// zero or more orders. We assume that all the emitted orders can be executed on
// the exchange independently of each other. At the next OHLC tick, the exchange
// either executes or cancels every emitted order (independently), and then the
// whole process repeats again.
class TraderInterface {
 public:
  TraderInterface() {}
  virtual ~TraderInterface() {}

  // Updates the (internal) trader state and emits zero or more orders.
  // We assume that orders is not null and points to an empty vector.
  // This method is called consecutively (by the exchange) on every OHLC tick.
  // Trader can assume that there are no active orders when this method is
  // called. The emitted orders will be either executed or cancelled by the
  // exchange at the next OHLC tick.
  virtual void Update(const OhlcTick& ohlc_tick, float security_balance,
                      float cash_balance, std::vector<Order>* orders) = 0;

  // Sets the trader's logging stream. Nullptr disables the logging.
  // Does not take the ownership of the stream.
  // Traders can use the stream to log their internal trader-dependent info.
  // WARNING: The logging stream must NOT be destroyed before the trader.
  // WARNING: Logging is slow and should be disabled for batch optimization.
  virtual void SetLogStream(std::ostream* os);

  // Returns the trader's logging stream.
  virtual std::ostream* LogStream() const;

  // Returns the string representation of the trader (and its configuration).
  virtual std::string ToString() const = 0;

  // Returns a new (freshly initialized) instance of the same trader.
  // By default, the new instance does not inherit the logging stream.
  virtual TraderInstance NewInstance() const = 0;

 protected:
  // Trader's logging stream.
  std::ostream* os_ = nullptr;
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
