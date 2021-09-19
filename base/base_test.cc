// Copyright © 2021 Peter Cerno. All rights reserved.

#include "base/base.h"

#include "gtest/gtest.h"

namespace trader {
namespace {
void AddPriceRecord(int64_t timestamp_sec, float price, float volume,
                    PriceHistory& price_history) {
  price_history.emplace_back();
  price_history.back().set_timestamp_sec(timestamp_sec);
  price_history.back().set_price(price);
  price_history.back().set_volume(volume);
}

void ExpectNearPriceRecord(const PriceRecord& actual,
                           const PriceRecord& expected) {
  EXPECT_EQ(actual.timestamp_sec(), expected.timestamp_sec());
  EXPECT_FLOAT_EQ(actual.price(), expected.price());
  EXPECT_FLOAT_EQ(actual.volume(), expected.volume());
}
}  // namespace

TEST(HistorySubsetTest, Basic) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, price_history);
  AddPriceRecord(1483229400, 800.0f, 1.5e3f, price_history);
  AddPriceRecord(1483230000, 750.0f, 1.0e3f, price_history);
  AddPriceRecord(1483230600, 850.0f, 2.0e3f, price_history);
  AddPriceRecord(1483231200, 800.0f, 1.5e3f, price_history);

  std::pair<PriceHistory::const_iterator, PriceHistory::const_iterator>
      history_subset = HistorySubset(price_history,
                                     /*start_timestamp_sec=*/0,
                                     /*end_timestamp_sec=*/0);
  EXPECT_EQ(history_subset.first, price_history.begin());
  EXPECT_EQ(history_subset.second, price_history.end());

  history_subset = HistorySubset(price_history,
                                 /*start_timestamp_sec=*/0,
                                 /*end_timestamp_sec=*/1483230000);
  EXPECT_EQ(history_subset.first, price_history.begin());
  EXPECT_EQ(history_subset.second, price_history.begin() + 2);

  history_subset = HistorySubset(price_history,
                                 /*start_timestamp_sec=*/1483229400,
                                 /*end_timestamp_sec=*/0);
  EXPECT_EQ(history_subset.first, price_history.begin() + 1);
  EXPECT_EQ(history_subset.second, price_history.end());

  history_subset = HistorySubset(price_history,
                                 /*start_timestamp_sec=*/1483228800,
                                 /*end_timestamp_sec=*/1483231800);
  EXPECT_EQ(history_subset.first, price_history.begin());
  EXPECT_EQ(history_subset.second, price_history.end());

  history_subset = HistorySubset(price_history,
                                 /*start_timestamp_sec=*/1483228800,
                                 /*end_timestamp_sec=*/1483228800);
  EXPECT_EQ(history_subset.first, price_history.begin());
  EXPECT_EQ(history_subset.second, price_history.begin());

  history_subset = HistorySubset(price_history,
                                 /*start_timestamp_sec=*/1483228800,
                                 /*end_timestamp_sec=*/1483229400);
  EXPECT_EQ(history_subset.first, price_history.begin());
  EXPECT_EQ(history_subset.second, price_history.begin() + 1);

  history_subset = HistorySubset(price_history,
                                 /*start_timestamp_sec=*/1483228860,
                                 /*end_timestamp_sec=*/1483229400);
  EXPECT_EQ(history_subset.first, price_history.begin() + 1);
  EXPECT_EQ(history_subset.second, price_history.begin() + 1);

  history_subset = HistorySubset(price_history,
                                 /*start_timestamp_sec=*/1483229100,
                                 /*end_timestamp_sec=*/1483230900);
  EXPECT_EQ(history_subset.first, price_history.begin() + 1);
  EXPECT_EQ(history_subset.second, price_history.begin() + 4);
}

TEST(HistorySubsetCopyTest, Basic) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, price_history);
  AddPriceRecord(1483229400, 800.0f, 1.5e3f, price_history);
  AddPriceRecord(1483230000, 750.0f, 2.0e3f, price_history);
  AddPriceRecord(1483230600, 850.0f, 2.5e3f, price_history);
  AddPriceRecord(1483231200, 650.0f, 3.0e3f, price_history);

  PriceHistory history_subset_copy =
      HistorySubsetCopy(price_history,
                        /*start_timestamp_sec=*/0,
                        /*end_timestamp_sec=*/0);
  ASSERT_EQ(history_subset_copy.size(), 5);
  for (size_t i = 0; i < 5; ++i) {
    ExpectNearPriceRecord(history_subset_copy[i], price_history[i]);
  }

  history_subset_copy = HistorySubsetCopy(price_history,
                                          /*start_timestamp_sec=*/0,
                                          /*end_timestamp_sec=*/1483230600);
  ASSERT_EQ(history_subset_copy.size(), 3);
  for (size_t i = 0; i < 3; ++i) {
    ExpectNearPriceRecord(history_subset_copy[i], price_history[i]);
  }

  history_subset_copy = HistorySubsetCopy(price_history,
                                          /*start_timestamp_sec=*/1483230000,
                                          /*end_timestamp_sec=*/0);
  ASSERT_EQ(history_subset_copy.size(), 3);
  for (size_t i = 0; i < 3; ++i) {
    ExpectNearPriceRecord(history_subset_copy[i], price_history[i + 2]);
  }

  history_subset_copy = HistorySubsetCopy(price_history,
                                          /*start_timestamp_sec=*/1483229100,
                                          /*end_timestamp_sec=*/1483230900);
  ASSERT_EQ(history_subset_copy.size(), 3);
  for (size_t i = 0; i < 3; ++i) {
    ExpectNearPriceRecord(history_subset_copy[i], price_history[i + 1]);
  }

  history_subset_copy = HistorySubsetCopy(price_history,
                                          /*start_timestamp_sec=*/1483228800,
                                          /*end_timestamp_sec=*/1483228800);
  ASSERT_EQ(history_subset_copy.size(), 0);

  history_subset_copy = HistorySubsetCopy(price_history,
                                          /*start_timestamp_sec=*/1483230300,
                                          /*end_timestamp_sec=*/1483230600);
  ASSERT_EQ(history_subset_copy.size(), 0);
}

}  // namespace trader
