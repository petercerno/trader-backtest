// Copyright © 2019 Peter Cerno. All rights reserved.

#include "trader_util.h"

#include <algorithm>
#include <cmath>
#include <queue>

namespace trader {

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
                                long end_timestamp_sec, size_t top_n) {
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
                            float max_price_deviation_per_min,
                            std::vector<size_t>* outlier_indices) {
  static constexpr int MAX_LOOKAHEAD = 10;
  static constexpr int MIN_LOOKAHEAD_PERSISTENT = 3;
  PriceHistory price_history_clean;
  for (size_t i = 0; i < price_history.size(); ++i) {
    const PriceRecord& price_record = price_history[i];
    if (price_record.price() <= 0 || price_record.volume() < 0) {
      if (outlier_indices != nullptr) {
        outlier_indices->push_back(i);
      }
      continue;
    }
    if (price_history_clean.empty()) {
      price_history_clean.push_back(price_record);
      continue;
    }
    const PriceRecord& price_record_prev = price_history_clean.back();
    const float reference_price = price_record_prev.price();
    const float duration_min =
        std::max(1.0f, static_cast<float>(price_record.timestamp_sec() -
                                          price_record_prev.timestamp_sec()) /
                           60.0f);
    const float jump_factor =
        (1.0f + max_price_deviation_per_min) * std::sqrt(duration_min);
    const float jump_up_price = reference_price * jump_factor;
    const float jump_down_price = reference_price / jump_factor;
    const bool jumped_up = price_record.price() > jump_up_price;
    const bool jumped_down = price_record.price() < jump_down_price;
    bool is_outlier = false;
    if (jumped_up || jumped_down) {
      // Let's look ahead if this jump persists.
      int lookahead_count = 0;
      int lookahead_persistent_count = 0;
      const float middle_up_price =
          0.8f * jump_up_price + 0.2f * reference_price;
      const float middle_down_price =
          0.8f * jump_down_price + 0.2f * reference_price;
      for (size_t j = i + 1;
           j < price_history.size() && lookahead_count < MAX_LOOKAHEAD; ++j) {
        if (price_history[j].price() <= 0 || price_history[j].volume() < 0) {
          continue;
        }
        if ((jumped_up && price_history[j].price() > middle_up_price) ||
            (jumped_down && price_history[j].price() < middle_down_price)) {
          ++lookahead_persistent_count;
        }
        ++lookahead_count;
      }
      is_outlier = lookahead_persistent_count < MIN_LOOKAHEAD_PERSISTENT;
    }
    if (!is_outlier) {
      price_history_clean.push_back(price_record);
    } else if (outlier_indices != nullptr) {
      outlier_indices->push_back(i);
    }
  }
  return price_history_clean;
}

std::map<size_t, bool> GetOutlierIndicesWithContext(
    const std::vector<size_t>& outlier_indices, size_t price_history_size,
    size_t left_context_size, size_t right_context_size, size_t last_n) {
  std::map<size_t, bool> index_to_outlier;
  const size_t start_i = (last_n == 0 || outlier_indices.size() <= last_n)
                             ? 0
                             : outlier_indices.size() - last_n;
  for (size_t i = start_i; i < outlier_indices.size(); ++i) {
    const size_t j = outlier_indices[i];
    const size_t a = (j <= left_context_size) ? 0 : j - left_context_size;
    const size_t b = std::min(j + right_context_size + 1, price_history_size);
    for (size_t k = a; k < b; ++k) {
      // Keep existing outliers.
      index_to_outlier[k] |= false;
    }
    index_to_outlier[j] = true;
  }
  return index_to_outlier;
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
        sampling_rate_sec * (price_record.timestamp_sec() / sampling_rate_sec);
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
