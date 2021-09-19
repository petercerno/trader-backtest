// Copyright © 2021 Peter Cerno. All rights reserved.

#include "indicators/volatility.h"

namespace trader {

Volatility::Volatility(int window_size, int period_size_sec)
    : last_n_ohlc_ticks_(/*num_ohlc_ticks=*/2, period_size_sec),
      sliding_window_variance_(window_size) {
  last_n_ohlc_ticks_.RegisterLastTickUpdatedCallback(
      [this](const OhlcTick& old_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        // We have observed at least 1 OHLC tick.
        // The most recent OHLC tick was updated.
        assert(num_ohlc_ticks_ >= 1);
        portfolio_value_.back() = latest_base_balance_ * new_ohlc_tick.close() +
                                  latest_quote_balance_;
        sliding_window_variance_.UpdateCurrentValue(GetLogarithmicReturn());
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedCallback(
      [this](const OhlcTick& new_ohlc_tick) {
        // This is the first or second observed OHLC tick.
        assert(num_ohlc_ticks_ <= 1);
        if (num_ohlc_ticks_ == 0) {
          assert(portfolio_value_.empty());
          portfolio_value_.push_back(latest_base_balance_ *
                                         new_ohlc_tick.open() +
                                     latest_quote_balance_);
        } else {
          portfolio_value_.pop_front();
        }
        portfolio_value_.push_back(latest_base_balance_ *
                                       new_ohlc_tick.close() +
                                   latest_quote_balance_);
        ++num_ohlc_ticks_;
        sliding_window_variance_.AddNewValue(GetLogarithmicReturn());
      });
  last_n_ohlc_ticks_.RegisterNewTickAddedAndOldestTickRemovedCallback(
      [this](const OhlcTick& removed_ohlc_tick, const OhlcTick& new_ohlc_tick) {
        // We have observed at least 2 OHLC ticks.
        // New OHLC tick was added.
        assert(num_ohlc_ticks_ >= 2);
        portfolio_value_.pop_front();
        portfolio_value_.push_back(latest_base_balance_ *
                                       new_ohlc_tick.close() +
                                   latest_quote_balance_);
        ++num_ohlc_ticks_;
        sliding_window_variance_.AddNewValue(GetLogarithmicReturn());
      });
}

float Volatility::GetVolatility() const {
  return sliding_window_variance_.GetStandardDeviation();
}

int Volatility::GetNumOhlcTicks() const { return num_ohlc_ticks_; }

void Volatility::Update(const OhlcTick& ohlc_tick, float base_balance,
                        float quote_balance) {
  latest_base_balance_ = base_balance;
  latest_quote_balance_ = quote_balance;
  last_n_ohlc_ticks_.Update(ohlc_tick);
}

float Volatility::GetLogarithmicReturn() const {
  assert(portfolio_value_.size() == 2);
  return std::log(portfolio_value_.at(0) / portfolio_value_.at(1));
}

}  // namespace trader
