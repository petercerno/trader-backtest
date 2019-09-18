// Copyright © 2017 Peter Cerno. All rights reserved.

#include "trader_base.h"

#include <queue>

namespace trader {

namespace {
// Returns the downsampled timestamp (in seconds) according to the given
// sampling rate (in seconds).
int Downsample(int timestamp_sec, int sampling_rate_sec) {
  return sampling_rate_sec * (timestamp_sec / sampling_rate_sec);
}
}  // namespace

void TraderInterface::SetLogStream(std::ostream* os) { os_ = os; }

std::ostream* TraderInterface::LogStream() const { return os_; }

bool CheckPriceHistoryTimestamps(const PriceHistory& price_history) {
  for (size_t i = 1; i < price_history.size(); ++i) {
    if (price_history[i].timestamp_sec() <
        price_history[i - 1].timestamp_sec()) {
      return false;
    }
  }
  return true;
}

HistoryGaps GetPriceHistoryGaps(const PriceHistory& price_history,
                                long start_timestamp_sec,
                                long end_timestamp_sec, int top_n) {
  if (price_history.empty()) {
    return {};
  }
  const auto price_history_subset =
      HistorySubset(price_history, start_timestamp_sec, end_timestamp_sec);
  if (price_history_subset.first == price_history_subset.second) {
    return {};
  }
  auto gap_cmp = [](HistoryGap lhs, HistoryGap rhs) {
    const long length_delta = lhs.second - lhs.first - rhs.second + rhs.first;
    return length_delta > 0 || (length_delta == 0 && lhs.first < rhs.first);
  };
  std::priority_queue<HistoryGap, std::vector<HistoryGap>, decltype(gap_cmp)>
      gap_queue(gap_cmp);
  HistoryGaps history_gaps;
  if (start_timestamp_sec > 0) {
    gap_queue.push(HistoryGap{start_timestamp_sec,
                              price_history_subset.first->timestamp_sec()});
  }
  auto price_record_it_prev = price_history_subset.second;
  for (auto price_record_it = price_history_subset.first;
       price_record_it != price_history_subset.second; ++price_record_it) {
    if (price_record_it_prev != price_history_subset.second) {
      const PriceRecord& price_record_prev = *price_record_it_prev;
      const PriceRecord& price_record = *price_record_it;
      gap_queue.push(HistoryGap{price_record_prev.timestamp_sec(),
                                price_record.timestamp_sec()});
      if (gap_queue.size() > top_n) {
        gap_queue.pop();
      }
    }
    price_record_it_prev = price_record_it;
  }
  if (end_timestamp_sec > 0) {
    gap_queue.push(
        HistoryGap{price_record_it_prev->timestamp_sec(), end_timestamp_sec});
    if (gap_queue.size() > top_n) {
      gap_queue.pop();
    }
  }
  while (!gap_queue.empty()) {
    history_gaps.push_back(gap_queue.top());
    gap_queue.pop();
  }
  std::sort(history_gaps.begin(), history_gaps.end());
  return history_gaps;
}

PriceHistory RemoveOutliers(const PriceHistory& price_history,
                            float max_price_deviation_per_min) {
  PriceHistory price_history_clean;
  if (price_history.empty()) {
    return price_history_clean;
  }
  price_history_clean.push_back(price_history[0]);
  for (size_t i = 1; i < price_history.size(); ++i) {
    const PriceRecord& price_record_prev = price_history[i - 1];
    const PriceRecord& price_record = price_history[i];
    if (price_record.price() > 0 && price_record_prev.price() > 0 &&
        price_record.timestamp_sec() >= price_record_prev.timestamp_sec()) {
      const float duration_min =
          std::max(1.0f, static_cast<float>(price_record.timestamp_sec() -
                                            price_record_prev.timestamp_sec()) /
                             60.0f);
      const float deviation =
          std::abs(price_record.price() / price_record_prev.price() - 1.0f);
      if (deviation < max_price_deviation_per_min * std::sqrt(duration_min)) {
        price_history_clean.push_back(price_record);
      }
    }
  }
  return price_history_clean;
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
