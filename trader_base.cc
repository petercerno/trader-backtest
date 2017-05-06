// Copyright © 2017 Peter Cerno. All rights reserved.

#include "trader_base.h"

namespace trader {

namespace {
// Returns the downsampled timestamp (in seconds) according to the given
// sampling rate (in seconds).
int Downsample(int timestamp_sec, int sampling_rate_sec) {
  return sampling_rate_sec * (timestamp_sec / sampling_rate_sec);
}
}  // namespace

void TraderInterface::SetLogStream(std::ostream* os) { os_ = os; }

std::ostream* TraderInterface::LogStream() const {
  return os_;
}

OhlcHistory Resample(const PriceHistory& price_history,
                     long start_timestamp_sec, long end_timestamp_sec,
                     int sampling_rate_sec) {
  if (price_history.empty()) {
    return {};
  }
  OhlcHistory resampled_ohlc_history;
  const auto price_history_subset =
      HistorySubset(price_history, start_timestamp_sec, end_timestamp_sec);
  for (auto price_record_it = price_history_subset.first;
       price_record_it != price_history_subset.second; ++price_record_it) {
    const PriceRecord& price_record = *price_record_it;
    const int downsampled_timestamp_sec =
        Downsample(price_record.timestamp_sec(), sampling_rate_sec);
    while (!resampled_ohlc_history.empty() &&
           resampled_ohlc_history.back().timestamp_sec() + sampling_rate_sec <
               downsampled_timestamp_sec) {
      const int prev_timestamp_sec =
          resampled_ohlc_history.back().timestamp_sec();
      const float prev_close = resampled_ohlc_history.back().close();
      resampled_ohlc_history.emplace_back();
      resampled_ohlc_history.back().set_timestamp_sec(prev_timestamp_sec +
                                                      sampling_rate_sec);
      resampled_ohlc_history.back().set_open(prev_close);
      resampled_ohlc_history.back().set_high(prev_close);
      resampled_ohlc_history.back().set_low(prev_close);
      resampled_ohlc_history.back().set_close(prev_close);
      resampled_ohlc_history.back().set_volume(0);
    }
    if (resampled_ohlc_history.empty() ||
        resampled_ohlc_history.back().timestamp_sec() <
            downsampled_timestamp_sec) {
      resampled_ohlc_history.emplace_back();
      resampled_ohlc_history.back().set_timestamp_sec(
          downsampled_timestamp_sec);
      resampled_ohlc_history.back().set_open(price_record.price());
      resampled_ohlc_history.back().set_high(price_record.price());
      resampled_ohlc_history.back().set_low(price_record.price());
      resampled_ohlc_history.back().set_close(price_record.price());
      resampled_ohlc_history.back().set_volume(price_record.volume());
    } else {
      assert(resampled_ohlc_history.back().timestamp_sec() ==
             downsampled_timestamp_sec);
      resampled_ohlc_history.back().set_high(
          std::max(resampled_ohlc_history.back().high(), price_record.price()));
      resampled_ohlc_history.back().set_low(
          std::min(resampled_ohlc_history.back().low(), price_record.price()));
      resampled_ohlc_history.back().set_close(price_record.price());
      resampled_ohlc_history.back().set_volume(
          resampled_ohlc_history.back().volume() + price_record.volume());
    }
  }
  return resampled_ohlc_history;
}

}  // namespace trader
