// Copyright © 2019 Peter Cerno. All rights reserved.

#include "trader_base.h"
#include "util_test.h"

#include "gtest/gtest.h"

namespace trader {

using testing::AddPriceRecord;
using testing::ExpectNearPriceRecord;

TEST(HistorySubsetTest, Basic) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229400, 800.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483230000, 750.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483230600, 850.0f, 2.0e3f, &price_history);
  AddPriceRecord(1483231200, 800.0f, 1.5e3f, &price_history);

  std::pair<PriceHistory::const_iterator, PriceHistory::const_iterator>
      history_subset = HistorySubset(price_history,
                                     /* start_timestamp_sec = */ 0,
                                     /* end_timestamp_sec = */ 0);
  EXPECT_EQ(price_history.begin(), history_subset.first);
  EXPECT_EQ(price_history.end(), history_subset.second);

  history_subset = HistorySubset(price_history,
                                 /* start_timestamp_sec = */ 0,
                                 /* end_timestamp_sec = */ 1483230000);
  EXPECT_EQ(price_history.begin(), history_subset.first);
  EXPECT_EQ(price_history.begin() + 2, history_subset.second);

  history_subset = HistorySubset(price_history,
                                 /* start_timestamp_sec = */ 1483229400,
                                 /* end_timestamp_sec = */ 0);
  EXPECT_EQ(price_history.begin() + 1, history_subset.first);
  EXPECT_EQ(price_history.end(), history_subset.second);

  history_subset = HistorySubset(price_history,
                                 /* start_timestamp_sec = */ 1483228800,
                                 /* end_timestamp_sec = */ 1483231800);
  EXPECT_EQ(price_history.begin(), history_subset.first);
  EXPECT_EQ(price_history.end(), history_subset.second);

  history_subset = HistorySubset(price_history,
                                 /* start_timestamp_sec = */ 1483228800,
                                 /* end_timestamp_sec = */ 1483228800);
  EXPECT_EQ(price_history.begin(), history_subset.first);
  EXPECT_EQ(price_history.begin(), history_subset.second);

  history_subset = HistorySubset(price_history,
                                 /* start_timestamp_sec = */ 1483228800,
                                 /* end_timestamp_sec = */ 1483229400);
  EXPECT_EQ(price_history.begin(), history_subset.first);
  EXPECT_EQ(price_history.begin() + 1, history_subset.second);

  history_subset = HistorySubset(price_history,
                                 /* start_timestamp_sec = */ 1483228860,
                                 /* end_timestamp_sec = */ 1483229400);
  EXPECT_EQ(price_history.begin() + 1, history_subset.first);
  EXPECT_EQ(price_history.begin() + 1, history_subset.second);

  history_subset = HistorySubset(price_history,
                                 /* start_timestamp_sec = */ 1483229100,
                                 /* end_timestamp_sec = */ 1483230900);
  EXPECT_EQ(price_history.begin() + 1, history_subset.first);
  EXPECT_EQ(price_history.begin() + 4, history_subset.second);
}

TEST(HistorySubsetCopyTest, Basic) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229400, 800.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483230000, 750.0f, 2.0e3f, &price_history);
  AddPriceRecord(1483230600, 850.0f, 2.5e3f, &price_history);
  AddPriceRecord(1483231200, 650.0f, 3.0e3f, &price_history);

  PriceHistory history_subset_copy =
      HistorySubsetCopy(price_history,
                        /* start_timestamp_sec = */ 0,
                        /* end_timestamp_sec = */ 0);
  ASSERT_EQ(5, history_subset_copy.size());
  for (int i = 0; i < 5; ++i) {
    ExpectNearPriceRecord(price_history[i], history_subset_copy[i]);
  }

  history_subset_copy = HistorySubsetCopy(price_history,
                                          /* start_timestamp_sec = */ 0,
                                          /* end_timestamp_sec = */ 1483230600);
  ASSERT_EQ(3, history_subset_copy.size());
  for (int i = 0; i < 3; ++i) {
    ExpectNearPriceRecord(price_history[i], history_subset_copy[i]);
  }

  history_subset_copy =
      HistorySubsetCopy(price_history,
                        /* start_timestamp_sec = */ 1483230000,
                        /* end_timestamp_sec = */ 0);
  ASSERT_EQ(3, history_subset_copy.size());
  for (int i = 0; i < 3; ++i) {
    ExpectNearPriceRecord(price_history[i + 2], history_subset_copy[i]);
  }

  history_subset_copy =
      HistorySubsetCopy(price_history,
                        /* start_timestamp_sec = */ 1483229100,
                        /* end_timestamp_sec = */ 1483230900);
  ASSERT_EQ(3, history_subset_copy.size());
  for (int i = 0; i < 3; ++i) {
    ExpectNearPriceRecord(price_history[i + 1], history_subset_copy[i]);
  }

  history_subset_copy =
      HistorySubsetCopy(price_history,
                        /* start_timestamp_sec = */ 1483228800,
                        /* end_timestamp_sec = */ 1483228800);
  ASSERT_EQ(0, history_subset_copy.size());

  history_subset_copy =
      HistorySubsetCopy(price_history,
                        /* start_timestamp_sec = */ 1483230300,
                        /* end_timestamp_sec = */ 1483230600);
  ASSERT_EQ(0, history_subset_copy.size());
}

}  // namespace trader
