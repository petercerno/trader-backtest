// Copyright © 2019 Peter Cerno. All rights reserved.

#include "trader_util.h"
#include "util_test.h"

#include "gtest/gtest.h"

namespace trader {

using testing::AddPriceRecord;
using testing::ExpectNearOhlcTick;
using testing::ExpectNearPriceRecord;

TEST(CheckPriceHistoryTimestampsTest, NonDecreasing) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229400, 800.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483230000, 750.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483230000, 750.0f, 0.5e3f, &price_history);
  AddPriceRecord(1483230600, 850.0f, 2.0e3f, &price_history);
  AddPriceRecord(1483231200, 800.0f, 1.5e3f, &price_history);
  ASSERT_TRUE(CheckPriceHistoryTimestamps(price_history));
}

TEST(CheckPriceHistoryTimestampsTest, SingleSwap) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229400, 800.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483230600, 850.0f, 2.0e3f, &price_history);
  AddPriceRecord(1483230000, 750.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483231200, 800.0f, 1.5e3f, &price_history);
  ASSERT_FALSE(CheckPriceHistoryTimestamps(price_history));
}

TEST(GetPriceHistoryGapsTest, EmptyPriceHistory) {
  PriceHistory price_history;
  HistoryGaps history_gaps = GetPriceHistoryGaps(
      /* begin = */ price_history.begin(), /* end = */ price_history.end(),
      /* start_timestamp_sec = */ 0,
      /* end_timestamp_sec = */ 0,
      /* top_n = */ 2);
  ASSERT_TRUE(history_gaps.empty());
  history_gaps = GetPriceHistoryGaps(
      /* begin = */ price_history.begin(), /* end = */ price_history.end(),
      /* start_timestamp_sec = */ 1483228000,
      /* end_timestamp_sec = */ 1483233000,
      /* top_n = */ 2);
  ASSERT_TRUE(history_gaps.empty());
}

TEST(GetPriceHistoryGapsTest, SingleRecordPriceHistory) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  HistoryGaps history_gaps = GetPriceHistoryGaps(
      /* begin = */ price_history.begin(), /* end = */ price_history.end(),
      /* start_timestamp_sec = */ 0,
      /* end_timestamp_sec = */ 0,
      /* top_n = */ 2);
  ASSERT_TRUE(history_gaps.empty());
}

TEST(GetPriceHistoryGapsTest, MultipleRecordsPriceHistory) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483230000, 750.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483230600, 850.0f, 2.0e3f, &price_history);
  AddPriceRecord(1483230900, 800.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483231500, 820.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483231800, 840.0f, 1.0e3f, &price_history);
  HistoryGaps history_gaps = GetPriceHistoryGaps(
      /* begin = */ price_history.begin(), /* end = */ price_history.end(),
      /* start_timestamp_sec = */ 0,
      /* end_timestamp_sec = */ 0,
      /* top_n = */ 2);
  ASSERT_EQ(2, history_gaps.size());
  EXPECT_EQ(1483228800, history_gaps[0].first);
  EXPECT_EQ(1483230000, history_gaps[0].second);
  EXPECT_EQ(1483230000, history_gaps[1].first);
  EXPECT_EQ(1483230600, history_gaps[1].second);
  history_gaps = GetPriceHistoryGaps(/* begin = */ price_history.begin(),
                                     /* end = */ price_history.end(),
                                     /* start_timestamp_sec = */ 1483228000,
                                     /* end_timestamp_sec = */ 0,
                                     /* top_n = */ 2);
  ASSERT_EQ(2, history_gaps.size());
  EXPECT_EQ(1483228000, history_gaps[0].first);
  EXPECT_EQ(1483228800, history_gaps[0].second);
  EXPECT_EQ(1483228800, history_gaps[1].first);
  EXPECT_EQ(1483230000, history_gaps[1].second);
  history_gaps = GetPriceHistoryGaps(/* begin = */ price_history.begin(),
                                     /* end = */ price_history.end(),
                                     /* start_timestamp_sec = */ 0,
                                     /* end_timestamp_sec = */ 1483233000,
                                     /* top_n = */ 2);
  ASSERT_EQ(2, history_gaps.size());
  EXPECT_EQ(1483228800, history_gaps[0].first);
  EXPECT_EQ(1483230000, history_gaps[0].second);
  EXPECT_EQ(1483231800, history_gaps[1].first);
  EXPECT_EQ(1483233000, history_gaps[1].second);
  history_gaps = GetPriceHistoryGaps(/* begin = */ price_history.begin(),
                                     /* end = */ price_history.end(),
                                     /* start_timestamp_sec = */ 1483228000,
                                     /* end_timestamp_sec = */ 1483233000,
                                     /* top_n = */ 2);
  ASSERT_EQ(2, history_gaps.size());
  EXPECT_EQ(1483228800, history_gaps[0].first);
  EXPECT_EQ(1483230000, history_gaps[0].second);
  EXPECT_EQ(1483231800, history_gaps[1].first);
  EXPECT_EQ(1483233000, history_gaps[1].second);
  history_gaps = GetPriceHistoryGaps(/* begin = */ price_history.begin(),
                                     /* end = */ price_history.end(),
                                     /* start_timestamp_sec = */ 1483227000,
                                     /* end_timestamp_sec = */ 1483233000,
                                     /* top_n = */ 2);
  ASSERT_EQ(2, history_gaps.size());
  EXPECT_EQ(1483227000, history_gaps[0].first);
  EXPECT_EQ(1483228800, history_gaps[0].second);
  EXPECT_EQ(1483228800, history_gaps[1].first);
  EXPECT_EQ(1483230000, history_gaps[1].second);
  history_gaps = GetPriceHistoryGaps(/* begin = */ price_history.begin(),
                                     /* end = */ price_history.end(),
                                     /* start_timestamp_sec = */ 1483227000,
                                     /* end_timestamp_sec = */ 1483234000,
                                     /* top_n = */ 2);
  ASSERT_EQ(2, history_gaps.size());
  EXPECT_EQ(1483227000, history_gaps[0].first);
  EXPECT_EQ(1483228800, history_gaps[0].second);
  EXPECT_EQ(1483231800, history_gaps[1].first);
  EXPECT_EQ(1483234000, history_gaps[1].second);
  history_gaps = GetPriceHistoryGaps(/* begin = */ price_history.begin(),
                                     /* end = */ price_history.end(),
                                     /* start_timestamp_sec = */ 0,
                                     /* end_timestamp_sec = */ 0,
                                     /* top_n = */ 3);
  ASSERT_EQ(3, history_gaps.size());
  EXPECT_EQ(1483228800, history_gaps[0].first);
  EXPECT_EQ(1483230000, history_gaps[0].second);
  EXPECT_EQ(1483230000, history_gaps[1].first);
  EXPECT_EQ(1483230600, history_gaps[1].second);
  EXPECT_EQ(1483230900, history_gaps[2].first);
  EXPECT_EQ(1483231500, history_gaps[2].second);
  history_gaps = GetPriceHistoryGaps(/* begin = */ price_history.begin(),
                                     /* end = */ price_history.end(),
                                     /* start_timestamp_sec = */ 0,
                                     /* end_timestamp_sec = */ 0,
                                     /* top_n = */ 4);
  ASSERT_EQ(4, history_gaps.size());
  EXPECT_EQ(1483228800, history_gaps[0].first);
  EXPECT_EQ(1483230000, history_gaps[0].second);
  EXPECT_EQ(1483230000, history_gaps[1].first);
  EXPECT_EQ(1483230600, history_gaps[1].second);
  EXPECT_EQ(1483230600, history_gaps[2].first);
  EXPECT_EQ(1483230900, history_gaps[2].second);
  EXPECT_EQ(1483230900, history_gaps[3].first);
  EXPECT_EQ(1483231500, history_gaps[3].second);
}

TEST(RemoveOutliersTest, EmptyPriceHistory) {
  PriceHistory price_history;
  PriceHistory price_history_clean =
      RemoveOutliers(/* begin = */ price_history.begin(),
                     /* end = */ price_history.end(),
                     /* max_price_deviation_per_min = */ 0.02,
                     /* outlier_indices = */ nullptr);
  ASSERT_TRUE(price_history_clean.empty());
}

TEST(RemoveOutliersTest, EmptyPriceHistoryHasEmptyOutlierIndices) {
  PriceHistory price_history;
  std::vector<size_t> outlier_indices;
  PriceHistory price_history_clean = RemoveOutliers(
      /* begin = */ price_history.begin(),
      /* end = */ price_history.end(),
      /* max_price_deviation_per_min = */ 0.02, &outlier_indices);
  ASSERT_TRUE(price_history_clean.empty());
  ASSERT_TRUE(outlier_indices.empty());
}

TEST(RemoveOutliersTest, NoOutliers) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228860, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483228920, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228980, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229040, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229100, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229160, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229220, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229280, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229340, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229400, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229460, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229520, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229580, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229640, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229700, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229760, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229820, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229880, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229940, 695.0f, 1.5e3f, &price_history);
  // Without outlier_indices.
  PriceHistory price_history_clean =
      RemoveOutliers(/* begin = */ price_history.begin(),
                     /* end = */ price_history.end(),
                     /* max_price_deviation_per_min = */ 0.02,
                     /* outlier_indices = */ nullptr);
  ASSERT_EQ(20, price_history_clean.size());
  for (size_t i = 0; i < 20; ++i) {
    ExpectNearPriceRecord(price_history[i], price_history_clean[i]);
  }
  // With outlier_indices.
  std::vector<size_t> outlier_indices;
  price_history_clean = RemoveOutliers(
      /* begin = */ price_history.begin(),
      /* end = */ price_history.end(),
      /* max_price_deviation_per_min = */ 0.02, &outlier_indices);
  ASSERT_EQ(20, price_history_clean.size());
  for (size_t i = 0; i < 20; ++i) {
    ExpectNearPriceRecord(price_history[i], price_history_clean[i]);
  }
  ASSERT_EQ(0, outlier_indices.size());
}

TEST(RemoveOutliersTest, NonPositivePrice) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 0.01f, 1.0e3f, &price_history);
  AddPriceRecord(1483228860, 0.01f, 1.0e3f, &price_history);
  AddPriceRecord(1483228920, 0.00f, 1.0e3f, &price_history);  // Outlier.
  AddPriceRecord(1483228980, 0.01f, 1.0e3f, &price_history);
  AddPriceRecord(1483229040, 0.01f, 1.0e3f, &price_history);
  // Without outlier_indices.
  PriceHistory price_history_clean =
      RemoveOutliers(/* begin = */ price_history.begin(),
                     /* end = */ price_history.end(),
                     /* max_price_deviation_per_min = */ 0.02,
                     /* outlier_indices = */ nullptr);
  ASSERT_EQ(4, price_history_clean.size());
  ExpectNearPriceRecord(price_history[0], price_history_clean[0]);
  ExpectNearPriceRecord(price_history[1], price_history_clean[1]);
  ExpectNearPriceRecord(price_history[3], price_history_clean[2]);
  ExpectNearPriceRecord(price_history[4], price_history_clean[3]);
  // With outlier_indices.
  std::vector<size_t> outlier_indices;
  price_history_clean = RemoveOutliers(
      /* begin = */ price_history.begin(),
      /* end = */ price_history.end(),
      /* max_price_deviation_per_min = */ 0.02, &outlier_indices);
  ASSERT_EQ(4, price_history_clean.size());
  ExpectNearPriceRecord(price_history[0], price_history_clean[0]);
  ExpectNearPriceRecord(price_history[1], price_history_clean[1]);
  ExpectNearPriceRecord(price_history[3], price_history_clean[2]);
  ExpectNearPriceRecord(price_history[4], price_history_clean[3]);
  ASSERT_EQ(1, outlier_indices.size());
  EXPECT_EQ(2, outlier_indices[0]);
}

TEST(RemoveOutliersTest, NegativeVolume) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228860, 705.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228920, 700.0f, -0.05f, &price_history);  // Outlier.
  AddPriceRecord(1483228980, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229040, 695.0f, 1.0e3f, &price_history);
  // Without outlier_indices.
  PriceHistory price_history_clean =
      RemoveOutliers(/* begin = */ price_history.begin(),
                     /* end = */ price_history.end(),
                     /* max_price_deviation_per_min = */ 0.02,
                     /* outlier_indices = */ nullptr);
  ASSERT_EQ(4, price_history_clean.size());
  ExpectNearPriceRecord(price_history[0], price_history_clean[0]);
  ExpectNearPriceRecord(price_history[1], price_history_clean[1]);
  ExpectNearPriceRecord(price_history[3], price_history_clean[2]);
  ExpectNearPriceRecord(price_history[4], price_history_clean[3]);
  // With outlier_indices.
  std::vector<size_t> outlier_indices;
  price_history_clean = RemoveOutliers(
      /* begin = */ price_history.begin(),
      /* end = */ price_history.end(),
      /* max_price_deviation_per_min = */ 0.02, &outlier_indices);
  ASSERT_EQ(4, price_history_clean.size());
  ExpectNearPriceRecord(price_history[0], price_history_clean[0]);
  ExpectNearPriceRecord(price_history[1], price_history_clean[1]);
  ExpectNearPriceRecord(price_history[3], price_history_clean[2]);
  ExpectNearPriceRecord(price_history[4], price_history_clean[3]);
  ASSERT_EQ(1, outlier_indices.size());
  EXPECT_EQ(2, outlier_indices[0]);
}

TEST(RemoveOutliersTest, SimpleOutlier) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228860, 705.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228920, 750.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228980, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229040, 695.0f, 1.0e3f, &price_history);
  // Without outlier_indices.
  PriceHistory price_history_clean =
      RemoveOutliers(/* begin = */ price_history.begin(),
                     /* end = */ price_history.end(),
                     /* max_price_deviation_per_min = */ 0.02,
                     /* outlier_indices = */ nullptr);
  ASSERT_EQ(4, price_history_clean.size());
  ExpectNearPriceRecord(price_history[0], price_history_clean[0]);
  ExpectNearPriceRecord(price_history[1], price_history_clean[1]);
  ExpectNearPriceRecord(price_history[3], price_history_clean[2]);
  ExpectNearPriceRecord(price_history[4], price_history_clean[3]);
  // With outlier_indices.
  std::vector<size_t> outlier_indices;
  price_history_clean = RemoveOutliers(
      /* begin = */ price_history.begin(),
      /* end = */ price_history.end(),
      /* max_price_deviation_per_min = */ 0.02, &outlier_indices);
  ASSERT_EQ(4, price_history_clean.size());
  ExpectNearPriceRecord(price_history[0], price_history_clean[0]);
  ExpectNearPriceRecord(price_history[1], price_history_clean[1]);
  ExpectNearPriceRecord(price_history[3], price_history_clean[2]);
  ExpectNearPriceRecord(price_history[4], price_history_clean[3]);
  ASSERT_EQ(1, outlier_indices.size());
  EXPECT_EQ(2, outlier_indices[0]);
}

TEST(RemoveOutliersTest, NonPersistentOutliers) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228860, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483228920, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228980, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229040, 750.0f, 1.0e3f, &price_history);  // Outlier.
  AddPriceRecord(1483229100, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229160, 750.0f, 1.0e3f, &price_history);  // Outlier.
  AddPriceRecord(1483229220, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229280, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229340, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229400, 450.0f, 1.0e3f, &price_history);  // Outlier.
  AddPriceRecord(1483229460, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229520, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229580, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229640, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229700, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229760, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229820, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229880, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229940, 695.0f, 1.5e3f, &price_history);
  std::vector<size_t> outlier_indices;
  PriceHistory price_history_clean = RemoveOutliers(
      /* begin = */ price_history.begin(),
      /* end = */ price_history.end(),
      /* max_price_deviation_per_min = */ 0.02, &outlier_indices);
  ASSERT_EQ(17, price_history_clean.size());
  for (size_t i = 0; i < 4; ++i) {
    ExpectNearPriceRecord(price_history[i], price_history_clean[i]);
  }
  for (size_t i = 5; i < 6; ++i) {
    ExpectNearPriceRecord(price_history[i], price_history_clean[i - 1]);
  }
  for (size_t i = 7; i < 10; ++i) {
    ExpectNearPriceRecord(price_history[i], price_history_clean[i - 2]);
  }
  for (size_t i = 11; i < 20; ++i) {
    ExpectNearPriceRecord(price_history[i], price_history_clean[i - 3]);
  }
  ASSERT_EQ(3, outlier_indices.size());
  EXPECT_EQ(4, outlier_indices[0]);
  EXPECT_EQ(6, outlier_indices[1]);
  EXPECT_EQ(10, outlier_indices[2]);
}

TEST(RemoveOutliersTest, PersistentJumps) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228860, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483228920, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228980, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229040, 750.0f, 1.0e3f, &price_history);  // Persistent.
  AddPriceRecord(1483229100, 705.0f, 1.5e3f, &price_history);  // Outlier.
  AddPriceRecord(1483229160, 750.0f, 1.0e3f, &price_history);  // Persistent.
  AddPriceRecord(1483229220, 745.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229280, 750.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229340, 755.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229400, 750.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229460, 745.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229520, 750.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229580, 755.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229640, 700.0f, 1.0e3f, &price_history);  // Persistent.
  AddPriceRecord(1483229700, 695.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229760, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229820, 705.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229880, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229940, 695.0f, 1.5e3f, &price_history);
  std::vector<size_t> outlier_indices;
  PriceHistory price_history_clean = RemoveOutliers(
      /* begin = */ price_history.begin(),
      /* end = */ price_history.end(),
      /* max_price_deviation_per_min = */ 0.02, &outlier_indices);
  ASSERT_EQ(19, price_history_clean.size());
  for (size_t i = 0; i < 5; ++i) {
    ExpectNearPriceRecord(price_history[i], price_history_clean[i]);
  }
  for (size_t i = 6; i < 20; ++i) {
    ExpectNearPriceRecord(price_history[i], price_history_clean[i - 1]);
  }
  ASSERT_EQ(1, outlier_indices.size());
  EXPECT_EQ(5, outlier_indices[0]);
}

TEST(GetOutlierIndicesWithContextTest, NoOutliers) {
  std::vector<size_t> outlier_indices;
  std::map<size_t, bool> index_to_outlier =
      GetOutlierIndicesWithContext(outlier_indices,
                                   /* price_history_size = */ 100,
                                   /* left_context_size = */ 5,
                                   /* right_context_size = */ 5,
                                   /* last_n = */ 10);
  ASSERT_TRUE(index_to_outlier.empty());
}

TEST(GetOutlierIndicesWithContextTest, SingleOutlierAtTheBeginning) {
  std::vector<size_t> outlier_indices{3};
  std::map<size_t, bool> index_to_outlier =
      GetOutlierIndicesWithContext(outlier_indices,
                                   /* price_history_size = */ 100,
                                   /* left_context_size = */ 5,
                                   /* right_context_size = */ 5,
                                   /* last_n = */ 10);
  ASSERT_EQ(9, index_to_outlier.size());
  EXPECT_FALSE(index_to_outlier.at(0));
  EXPECT_FALSE(index_to_outlier.at(1));
  EXPECT_FALSE(index_to_outlier.at(2));
  EXPECT_TRUE(index_to_outlier.at(3));
  EXPECT_FALSE(index_to_outlier.at(4));
  EXPECT_FALSE(index_to_outlier.at(5));
  EXPECT_FALSE(index_to_outlier.at(6));
  EXPECT_FALSE(index_to_outlier.at(7));
  EXPECT_FALSE(index_to_outlier.at(8));
}

TEST(GetOutlierIndicesWithContextTest, SingleOutlierInTheMiddle) {
  std::vector<size_t> outlier_indices{50};
  std::map<size_t, bool> index_to_outlier =
      GetOutlierIndicesWithContext(outlier_indices,
                                   /* price_history_size = */ 100,
                                   /* left_context_size = */ 5,
                                   /* right_context_size = */ 5,
                                   /* last_n = */ 10);
  ASSERT_EQ(11, index_to_outlier.size());
  EXPECT_FALSE(index_to_outlier.at(45));
  EXPECT_FALSE(index_to_outlier.at(46));
  EXPECT_FALSE(index_to_outlier.at(47));
  EXPECT_FALSE(index_to_outlier.at(48));
  EXPECT_FALSE(index_to_outlier.at(49));
  EXPECT_TRUE(index_to_outlier.at(50));
  EXPECT_FALSE(index_to_outlier.at(51));
  EXPECT_FALSE(index_to_outlier.at(52));
  EXPECT_FALSE(index_to_outlier.at(53));
  EXPECT_FALSE(index_to_outlier.at(54));
  EXPECT_FALSE(index_to_outlier.at(55));
}

TEST(GetOutlierIndicesWithContextTest, SingleOutlierAtTheEnd) {
  std::vector<size_t> outlier_indices{97};
  std::map<size_t, bool> index_to_outlier =
      GetOutlierIndicesWithContext(outlier_indices,
                                   /* price_history_size = */ 100,
                                   /* left_context_size = */ 5,
                                   /* right_context_size = */ 5,
                                   /* last_n = */ 10);
  ASSERT_EQ(8, index_to_outlier.size());
  EXPECT_FALSE(index_to_outlier.at(92));
  EXPECT_FALSE(index_to_outlier.at(93));
  EXPECT_FALSE(index_to_outlier.at(94));
  EXPECT_FALSE(index_to_outlier.at(95));
  EXPECT_FALSE(index_to_outlier.at(96));
  EXPECT_TRUE(index_to_outlier.at(97));
  EXPECT_FALSE(index_to_outlier.at(98));
  EXPECT_FALSE(index_to_outlier.at(99));
}

TEST(GetOutlierIndicesWithContextTest, MultipleOutliersAtTheBeginning) {
  std::vector<size_t> outlier_indices{3, 4, 7};
  std::map<size_t, bool> index_to_outlier =
      GetOutlierIndicesWithContext(outlier_indices,
                                   /* price_history_size = */ 100,
                                   /* left_context_size = */ 5,
                                   /* right_context_size = */ 5,
                                   /* last_n = */ 10);
  ASSERT_EQ(13, index_to_outlier.size());
  EXPECT_FALSE(index_to_outlier.at(0));
  EXPECT_FALSE(index_to_outlier.at(1));
  EXPECT_FALSE(index_to_outlier.at(2));
  EXPECT_TRUE(index_to_outlier.at(3));
  EXPECT_TRUE(index_to_outlier.at(4));
  EXPECT_FALSE(index_to_outlier.at(5));
  EXPECT_FALSE(index_to_outlier.at(6));
  EXPECT_TRUE(index_to_outlier.at(7));
  EXPECT_FALSE(index_to_outlier.at(8));
  EXPECT_FALSE(index_to_outlier.at(9));
  EXPECT_FALSE(index_to_outlier.at(10));
  EXPECT_FALSE(index_to_outlier.at(11));
  EXPECT_FALSE(index_to_outlier.at(12));
}

TEST(GetOutlierIndicesWithContextTest, MultipleOutliersInTheMiddle) {
  std::vector<size_t> outlier_indices{50, 51, 53};
  std::map<size_t, bool> index_to_outlier =
      GetOutlierIndicesWithContext(outlier_indices,
                                   /* price_history_size = */ 100,
                                   /* left_context_size = */ 3,
                                   /* right_context_size = */ 3,
                                   /* last_n = */ 10);
  ASSERT_EQ(10, index_to_outlier.size());
  EXPECT_FALSE(index_to_outlier.at(47));
  EXPECT_FALSE(index_to_outlier.at(48));
  EXPECT_FALSE(index_to_outlier.at(49));
  EXPECT_TRUE(index_to_outlier.at(50));
  EXPECT_TRUE(index_to_outlier.at(51));
  EXPECT_FALSE(index_to_outlier.at(52));
  EXPECT_TRUE(index_to_outlier.at(53));
  EXPECT_FALSE(index_to_outlier.at(54));
  EXPECT_FALSE(index_to_outlier.at(55));
  EXPECT_FALSE(index_to_outlier.at(56));
}

TEST(GetOutlierIndicesWithContextTest, MultipleOutliersAtTheEnd) {
  std::vector<size_t> outlier_indices{94, 95, 97};
  std::map<size_t, bool> index_to_outlier =
      GetOutlierIndicesWithContext(outlier_indices,
                                   /* price_history_size = */ 100,
                                   /* left_context_size = */ 5,
                                   /* right_context_size = */ 5,
                                   /* last_n = */ 10);
  ASSERT_EQ(11, index_to_outlier.size());
  EXPECT_FALSE(index_to_outlier.at(89));
  EXPECT_FALSE(index_to_outlier.at(90));
  EXPECT_FALSE(index_to_outlier.at(91));
  EXPECT_FALSE(index_to_outlier.at(92));
  EXPECT_FALSE(index_to_outlier.at(93));
  EXPECT_TRUE(index_to_outlier.at(94));
  EXPECT_TRUE(index_to_outlier.at(95));
  EXPECT_FALSE(index_to_outlier.at(96));
  EXPECT_TRUE(index_to_outlier.at(97));
  EXPECT_FALSE(index_to_outlier.at(98));
  EXPECT_FALSE(index_to_outlier.at(99));
}

TEST(ResampleTest, EmptyPriceHistory) {
  PriceHistory price_history;
  OhlcHistory ohlc_history = Resample(/* begin = */ price_history.begin(),
                                      /* end = */ price_history.end(),
                                      /* sampling_rate_sec = */ 300);
  ASSERT_EQ(0, ohlc_history.size());
}

TEST(ResampleTest, EmptyOhlcHistory) {
  PriceHistory price_history;
  AddPriceRecord(0, 10.0f, 1.0e3f, &price_history);
  AddPriceRecord(60, 20.0f, 2.0e3f, &price_history);
  AddPriceRecord(120, 30.0f, 3.0e3f, &price_history);
  AddPriceRecord(180, 40.0f, 4.0e3f, &price_history);
  OhlcHistory ohlc_history = Resample(/* begin = */ price_history.begin() + 1,
                                      /* end = */ price_history.begin() + 1,
                                      /* sampling_rate_sec = */ 300);
  ASSERT_EQ(0, ohlc_history.size());
  ohlc_history = Resample(/* begin = */ price_history.begin() + 3,
                          /* end = */ price_history.begin() + 3,
                          /* sampling_rate_sec = */ 10);
  ASSERT_EQ(0, ohlc_history.size());
}

TEST(ResampleTest, ResampleSinglePriceRecord) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  OhlcHistory ohlc_history = Resample(/* begin = */ price_history.begin(),
                                      /* end = */ price_history.end(),
                                      /* sampling_rate_sec = */ 300);
  ASSERT_EQ(1, ohlc_history.size());
  ExpectNearOhlcTick(1483228800, 700.0f, 700.0f, 700.0f, 700.0f, 1.0e3f,
                     ohlc_history[0]);
}

TEST(ResampleTest, ResampleMultiplePriceRecords1) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229400, 800.0f, 1.5e3f, &price_history);
  OhlcHistory ohlc_history = Resample(/* begin = */ price_history.begin(),
                                      /* end = */ price_history.end(),
                                      /* sampling_rate_sec = */ 300);
  ASSERT_EQ(3, ohlc_history.size());
  ExpectNearOhlcTick(1483228800, 700.0f, 700.0f, 700.0f, 700.0f, 1.0e3f,
                     ohlc_history[0]);
  ExpectNearOhlcTick(1483229100, 700.0f, 700.0f, 700.0f, 700.0f, 0.0f,
                     ohlc_history[1]);
  ExpectNearOhlcTick(1483229400, 800.0f, 800.0f, 800.0f, 800.0f, 1.5e3f,
                     ohlc_history[2]);
  ohlc_history = Resample(/* begin = */ price_history.begin(),
                          /* end = */ price_history.end(),
                          /* sampling_rate_sec = */ 600);
  ASSERT_EQ(2, ohlc_history.size());
  ExpectNearOhlcTick(1483228800, 700.0f, 700.0f, 700.0f, 700.0f, 1.0e3f,
                     ohlc_history[0]);
  ExpectNearOhlcTick(1483229400, 800.0f, 800.0f, 800.0f, 800.0f, 1.5e3f,
                     ohlc_history[1]);
}

TEST(ResampleTest, ResampleMultiplePriceRecords2) {
  PriceHistory price_history;
  AddPriceRecord(1483228850, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483228900, 750.0f, 2.0e3f, &price_history);
  AddPriceRecord(1483228950, 650.0f, 2.0e3f, &price_history);
  AddPriceRecord(1483229000, 720.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229450, 800.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229500, 750.0f, 2.5e3f, &price_history);
  AddPriceRecord(1483229550, 800.0f, 1.5e3f, &price_history);
  AddPriceRecord(1483229600, 850.0f, 2.5e3f, &price_history);
  OhlcHistory ohlc_history = Resample(/* begin = */ price_history.begin(),
                                      /* end = */ price_history.end(),
                                      /* sampling_rate_sec = */ 300);
  ASSERT_EQ(3, ohlc_history.size());
  ExpectNearOhlcTick(1483228800, 700.0f, 750.0f, 650.0f, 720.0f, 6.0e3f,
                     ohlc_history[0]);
  ExpectNearOhlcTick(1483229100, 720.0f, 720.0f, 720.0f, 720.0f, 0.0f,
                     ohlc_history[1]);
  ExpectNearOhlcTick(1483229400, 800.0f, 850.0f, 750.0f, 850.0f, 8.0e3f,
                     ohlc_history[2]);
  ohlc_history = Resample(/* begin = */ price_history.begin(),
                          /* end = */ price_history.end(),
                          /* sampling_rate_sec = */ 150);
  ASSERT_EQ(6, ohlc_history.size());
  ExpectNearOhlcTick(1483228800, 700.0f, 750.0f, 700.0f, 750.0f, 3.0e3f,
                     ohlc_history[0]);
  ExpectNearOhlcTick(1483228950, 650.0f, 720.0f, 650.0f, 720.0f, 3.0e3f,
                     ohlc_history[1]);
  ExpectNearOhlcTick(1483229100, 720.0f, 720.0f, 720.0f, 720.0f, 0.0f,
                     ohlc_history[2]);
  ExpectNearOhlcTick(1483229250, 720.0f, 720.0f, 720.0f, 720.0f, 0.0f,
                     ohlc_history[3]);
  ExpectNearOhlcTick(1483229400, 800.0f, 800.0f, 750.0f, 750.0f, 4.0e3f,
                     ohlc_history[4]);
  ExpectNearOhlcTick(1483229550, 800.0f, 850.0f, 800.0f, 850.0f, 4.0e3f,
                     ohlc_history[5]);
}

}  // namespace trader
