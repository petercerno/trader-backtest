// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef LIB_TRADER_INTERFACE_H
#define LIB_TRADER_INTERFACE_H

#include "lib/trader_base.h"

namespace trader {

// The trader is executed as follows:
// - At every step the trader receives the latest OHLC tick `T[i]`, current
//   account balances, and updates its internal state. The current time is at
//   the end of the OHLC tick `T[i]` time period. (The trader is not updated on
//   zero volume OHLC ticks. These OHLC ticks indicate a gap in a price history,
//   which could have been caused by an unresponsive exchange or its API.)
// - Then the trader needs to decide what orders to emit. There are no other
//   active orders on the exchange at this moment (see the explanation below).
// - Once the trader decides what orders to emit, the exchange will execute
//   (or cancel) all these orders on the follow-up OHLC tick T[i+1]. The trader
//   does not see the follow-up OHLC tick T[i+1], so there is no peeking into
//   the future.
// - Once all orders are executed (or canceled) by the exchange, the trader
//   receives the follow-up OHLC tick T[i+1] and the whole process repeats.
// Note that at every step every order gets either executed or canceled by the
// exchange. This is a design simplification so that there are no active orders
// that the trader needs to maintain over time.
// In practice, however, we would not cancel orders if they would be re-emitted
// again. We would simply modify the existing orders (from the previous
// iteration) based on the updated state.
// Also note that the OHLC history sampling rate defines the frequency at which
// the trader is updated and emits orders. Traders should be designed in
// a frequency-agnostic way. In other words, they should have similar behavior
// and performance regardless of how frequently they are called. Traders should
// not assume anything about how often and when exactly they are called. One
// reason for that is that the exchanges (or their APIs) sometimes become
// unresponsive for random periods of time (and we see that e.g. in the gaps
// in the price histories). Therefore, we encourage to test the traders on OHLC
// histories with various sampling rates and gaps.
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
  virtual void Update(const OhlcTick& ohlc_tick, float base_balance,
                      float quote_balance, std::vector<Order>* orders) = 0;

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

}  // namespace trader

#endif  // LIB_TRADER_INTERFACE_H
