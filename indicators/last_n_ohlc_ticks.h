// Copyright © 2020 Peter Cerno. All rights reserved.

#ifndef INDICATORS_LAST_N_OHLC_TICKS_H
#define INDICATORS_LAST_N_OHLC_TICKS_H

#include "lib/trader_base.h"

namespace trader {

// Keeps track of the last N OHLC ticks with a specified period (in seconds).
// We assume that this period is divisible by the period of update OHLC ticks.
class LastNOhlcTicks {
 public:
  // Called after the last OHLC tick was updated, but no OHLC tick was added.
  // This happens when the OHLC tick provided in the Update method (below)
  // is fully contained in the period of the most recent OHLC tick in the deque.
  // old_ohlc_tick: The previous OHLC tick in the deque that was updated.
  // new_ohlc_tick: The updated (most recent) OHLC tick in the deque.
  using LastTickUpdatedCallback = std::function<void(
      const OhlcTick& old_ohlc_tick, const OhlcTick& new_ohlc_tick)>;
  // Called after a new OHLC tick was added to the deque.
  // This happens when the OHLC tick provided in the Update method (below)
  // starts after the period of the most recent OHLC tick in the deque.
  // new_ohlc_tick: The newly added (most recent) OHLC tick in the deque.
  using NewTickAddedCallback =
      std::function<void(const OhlcTick& new_ohlc_tick)>;
  // Called after a new OHLC tick was added to the deque and the oldest OHLC
  // tick was removed from the deque.
  // This happens when the deque grows beyond N OHLC ticks, so we need to remove
  // the oldest one.
  // removed_ohlc_tick: The oldest OHLC tick that was removed from deque.
  // new_ohlc_tick: The newly added (most recent) OHLC tick in the deque.
  using NewTickAddedAndOldestTickRemovedCallback = std::function<void(
      const OhlcTick& removed_ohlc_tick, const OhlcTick& new_ohlc_tick)>;

  // Constructor.
  // num_ohlc_ticks: Number N of OHLC ticks that we want to keep in the deque.
  // period_size_sec: Period of the kept OHLC ticks (in seconds).
  LastNOhlcTicks(int num_ohlc_ticks, int period_size_sec)
      : num_ohlc_ticks_(num_ohlc_ticks), period_size_sec_(period_size_sec) {
    assert(num_ohlc_ticks > 0);
    assert(period_size_sec > 0);
  }
  virtual ~LastNOhlcTicks() {}

  virtual void RegisterLastTickUpdatedCallback(
      LastTickUpdatedCallback last_tick_updated_callback);
  virtual void RegisterNewTickAddedCallback(
      NewTickAddedCallback new_tick_added_callback);
  virtual void RegisterNewTickAddedAndOldestTickRemovedCallback(
      NewTickAddedAndOldestTickRemovedCallback
          new_tick_added_and_oldest_tick_removed_callback);

  // Returns the deque of last N OHLC ticks.
  virtual const std::deque<OhlcTick>& GetLastNOhlcTicks() const;

  // Updates the deque of last N OHLC ticks.
  // Under normal circumstances this method runs in O(1) time.
  // The only exception is when the given OHLC tick is far in the future, in
  // which case we need to backfill all the intermediate zero volume OHLC ticks.
  // We assume that period_size_sec_ is divisible by the period of ohlc_tick.
  virtual void Update(const OhlcTick& ohlc_tick);

 private:
  int num_ohlc_ticks_ = 0;
  int period_size_sec_ = 0;

  std::deque<OhlcTick> last_n_ohlc_ticks_;

  LastTickUpdatedCallback last_tick_updated_callback_;
  NewTickAddedCallback new_tick_added_callback_;
  NewTickAddedAndOldestTickRemovedCallback
      new_tick_added_and_oldest_tick_removed_callback_;

  void NewOhlcTickAdded();
  void LastOhlcTickUpdated(const OhlcTick& old_ohlc_tick);
};

}  // namespace trader

#endif  // INDICATORS_LAST_N_OHLC_TICKS_H
