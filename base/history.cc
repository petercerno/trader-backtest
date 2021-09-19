// Copyright © 2021 Peter Cerno. All rights reserved.

#include "base/history.h"

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

std::vector<HistoryGap> GetPriceHistoryGaps(PriceHistory::const_iterator begin,
                                            PriceHistory::const_iterator end,
                                            int64_t start_timestamp_sec,
                                            int64_t end_timestamp_sec,
                                            size_t top_n) {
  if (begin == end) {
    return {};
  }
  auto gap_cmp = [](HistoryGap lhs, HistoryGap rhs) {
    const int64_t length_delta =
        lhs.second - lhs.first - rhs.second + rhs.first;
    return length_delta > 0 || (length_delta == 0 && lhs.first < rhs.first);
  };
  std::priority_queue<HistoryGap, std::vector<HistoryGap>, decltype(gap_cmp)>
      gap_queue(gap_cmp);
  std::vector<HistoryGap> history_gaps;
  if (start_timestamp_sec > 0) {
    gap_queue.push(HistoryGap{start_timestamp_sec, begin->timestamp_sec()});
  }
  auto it_prev = end;
  for (auto it = begin; it != end; ++it) {
    if (it_prev != end) {
      gap_queue.push(HistoryGap{it_prev->timestamp_sec(), it->timestamp_sec()});
      if (gap_queue.size() > top_n) {
        gap_queue.pop();
      }
    }
    it_prev = it;
  }
  if (end_timestamp_sec > 0) {
    gap_queue.push(HistoryGap{it_prev->timestamp_sec(), end_timestamp_sec});
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

PriceHistory RemoveOutliers(PriceHistory::const_iterator begin,
                            PriceHistory::const_iterator end,
                            float max_price_deviation_per_min,
                            std::vector<size_t>* outlier_indices) {
  static constexpr int MAX_LOOKAHEAD = 10;
  static constexpr int MIN_LOOKAHEAD_PERSISTENT = 3;
  PriceHistory price_history_clean;
  for (auto it = begin; it != end; ++it) {
    const PriceRecord& price_record = *it;
    const size_t price_record_index = std::distance(begin, it);
    if (price_record.price() <= 0 || price_record.volume() < 0) {
      if (outlier_indices != nullptr) {
        outlier_indices->push_back(price_record_index);
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
      int lookahead = 0;
      int lookahead_persistent = 0;
      const float middle_up_price =
          0.8f * jump_up_price + 0.2f * reference_price;
      const float middle_down_price =
          0.8f * jump_down_price + 0.2f * reference_price;
      for (auto jt = it + 1; jt != end && lookahead < MAX_LOOKAHEAD; ++jt) {
        if (jt->price() <= 0 || jt->volume() < 0) {
          continue;
        }
        if ((jumped_up && jt->price() > middle_up_price) ||
            (jumped_down && jt->price() < middle_down_price)) {
          ++lookahead_persistent;
        }
        ++lookahead;
      }
      is_outlier = lookahead_persistent < MIN_LOOKAHEAD_PERSISTENT;
    }
    if (!is_outlier) {
      price_history_clean.push_back(price_record);
    } else if (outlier_indices != nullptr) {
      outlier_indices->push_back(price_record_index);
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
      // Keep the existing outliers.
      index_to_outlier[k] |= false;
    }
    index_to_outlier[j] = true;
  }
  return index_to_outlier;
}

OhlcHistory Resample(PriceHistory::const_iterator begin,
                     PriceHistory::const_iterator end, int sampling_rate_sec) {
  OhlcHistory resampled_ohlc_history;
  for (auto it = begin; it != end; ++it) {
    const int64_t downsampled_timestamp_sec =
        sampling_rate_sec * (it->timestamp_sec() / sampling_rate_sec);
    while (!resampled_ohlc_history.empty() &&
           resampled_ohlc_history.back().timestamp_sec() + sampling_rate_sec <
               downsampled_timestamp_sec) {
      const int64_t prev_timestamp_sec =
          resampled_ohlc_history.back().timestamp_sec();
      const float prev_close = resampled_ohlc_history.back().close();
      resampled_ohlc_history.emplace_back();
      OhlcTick* ohlc_tick = &resampled_ohlc_history.back();
      ohlc_tick->set_timestamp_sec(prev_timestamp_sec + sampling_rate_sec);
      ohlc_tick->set_open(prev_close);
      ohlc_tick->set_high(prev_close);
      ohlc_tick->set_low(prev_close);
      ohlc_tick->set_close(prev_close);
      ohlc_tick->set_volume(0);
    }
    if (resampled_ohlc_history.empty() ||
        resampled_ohlc_history.back().timestamp_sec() <
            downsampled_timestamp_sec) {
      resampled_ohlc_history.emplace_back();
      OhlcTick* ohlc_tick = &resampled_ohlc_history.back();
      ohlc_tick->set_timestamp_sec(downsampled_timestamp_sec);
      ohlc_tick->set_open(it->price());
      ohlc_tick->set_high(it->price());
      ohlc_tick->set_low(it->price());
      ohlc_tick->set_close(it->price());
      ohlc_tick->set_volume(it->volume());
    } else {
      assert(resampled_ohlc_history.back().timestamp_sec() ==
             downsampled_timestamp_sec);
      OhlcTick* ohlc_tick = &resampled_ohlc_history.back();
      ohlc_tick->set_high(std::max(ohlc_tick->high(), it->price()));
      ohlc_tick->set_low(std::min(ohlc_tick->low(), it->price()));
      ohlc_tick->set_close(it->price());
      ohlc_tick->set_volume(ohlc_tick->volume() + it->volume());
    }
  }
  return resampled_ohlc_history;
}

}  // namespace trader
