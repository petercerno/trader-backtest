// Copyright © 2017 Peter Cerno. All rights reserved.

#include "trader_base.h"

#include "gtest/gtest.h"

namespace trader {
namespace {
constexpr float kEpsilon = 1.0e-6f;

// Adds PriceRecord to the price_history.
void AddPriceRecord(int timestamp_sec, float price, float volume,
                    PriceHistory* price_history) {
  price_history->emplace_back();
  price_history->back().set_timestamp_sec(timestamp_sec);
  price_history->back().set_price(price);
  price_history->back().set_volume(volume);
}

// Compares the expected PriceRecord with the actual PriceRecord.
void ExpectNearPriceRecord(const PriceRecord& expected,
                           const PriceRecord& actual) {
  EXPECT_EQ(expected.timestamp_sec(), actual.timestamp_sec());
  EXPECT_NEAR(expected.price(), actual.price(), kEpsilon);
  EXPECT_NEAR(expected.volume(), actual.volume(), kEpsilon);
}

// Compares the expected values with the actual OhlcTick.
void ExpectNearOhlcTick(int timestamp_sec, float open, float high, float low,
                        float close, float volume,
                        const OhlcTick& actual_ohlc_tick) {
  EXPECT_EQ(timestamp_sec, actual_ohlc_tick.timestamp_sec());
  EXPECT_NEAR(open, actual_ohlc_tick.open(), kEpsilon);
  EXPECT_NEAR(high, actual_ohlc_tick.high(), kEpsilon);
  EXPECT_NEAR(low, actual_ohlc_tick.low(), kEpsilon);
  EXPECT_NEAR(close, actual_ohlc_tick.close(), kEpsilon);
  EXPECT_NEAR(volume, actual_ohlc_tick.volume(), kEpsilon);
}
}  // namespace

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

TEST(ResampleTest, EmptyOhlcHistory) {
  OhlcHistory ohlc_history = Resample(/* price_history = */ {},
                                      /* start_timestamp_sec = */ 0,
                                      /* end_timestamp_sec = */ 300,
                                      /* sampling_rate_sec = */ 10);
  ASSERT_EQ(0, ohlc_history.size());

  PriceHistory price_history;
  AddPriceRecord(0, 10.0f, 1.0e3f, &price_history);
  AddPriceRecord(60, 20.0f, 2.0e3f, &price_history);
  AddPriceRecord(120, 30.0f, 3.0e3f, &price_history);
  AddPriceRecord(180, 40.0f, 4.0e3f, &price_history);
  // WARNING: When the timestamp_sec is set to 0, it is not used.
  ohlc_history = Resample(price_history,
                          /* start_timestamp_sec = */ 60,
                          /* end_timestamp_sec = */ 60,
                          /* sampling_rate_sec = */ 300);
  ASSERT_EQ(0, ohlc_history.size());

  ohlc_history = Resample(price_history,
                          /* start_timestamp_sec = */ 130,
                          /* end_timestamp_sec = */ 180,
                          /* sampling_rate_sec = */ 10);
  ASSERT_EQ(0, ohlc_history.size());
}

TEST(ResampleTest, ResampleSinglePriceRecord) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  OhlcHistory ohlc_history = Resample(price_history,
                                      /* start_timestamp_sec = */ 0,
                                      /* end_timestamp_sec = */ 0,
                                      /* sampling_rate_sec = */ 300);
  ASSERT_EQ(1, ohlc_history.size());
  ExpectNearOhlcTick(1483228800, 700.0f, 700.0f, 700.0f, 700.0f, 1.0e3f,
                     ohlc_history[0]);

  ohlc_history = Resample(price_history,
                          /* start_timestamp_sec = */ 1483228800,
                          /* end_timestamp_sec = */ 1483229400,
                          /* sampling_rate_sec = */ 300);
  // We do not extrapolate beyond the price_history.
  ASSERT_EQ(1, ohlc_history.size());
  ExpectNearOhlcTick(1483228800, 700.0f, 700.0f, 700.0f, 700.0f, 1.0e3f,
                     ohlc_history[0]);
}

TEST(ResampleTest, ResampleMultiplePriceRecords1) {
  PriceHistory price_history;
  AddPriceRecord(1483228800, 700.0f, 1.0e3f, &price_history);
  AddPriceRecord(1483229400, 800.0f, 1.5e3f, &price_history);
  OhlcHistory ohlc_history = Resample(price_history,
                                      /* start_timestamp_sec = */ 0,
                                      /* end_timestamp_sec = */ 0,
                                      /* sampling_rate_sec = */ 300);
  ASSERT_EQ(3, ohlc_history.size());
  ExpectNearOhlcTick(1483228800, 700.0f, 700.0f, 700.0f, 700.0f, 1.0e3f,
                     ohlc_history[0]);
  ExpectNearOhlcTick(1483229100, 700.0f, 700.0f, 700.0f, 700.0f, 0.0f,
                     ohlc_history[1]);
  ExpectNearOhlcTick(1483229400, 800.0f, 800.0f, 800.0f, 800.0f, 1.5e3f,
                     ohlc_history[2]);

  ohlc_history = Resample(price_history,
                          /* start_timestamp_sec = */ 0,
                          /* end_timestamp_sec = */ 0,
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
  OhlcHistory ohlc_history = Resample(price_history,
                                      /* start_timestamp_sec = */ 1483228800,
                                      /* end_timestamp_sec = */ 1483229700,
                                      /* sampling_rate_sec = */ 300);
  ASSERT_EQ(3, ohlc_history.size());
  ExpectNearOhlcTick(1483228800, 700.0f, 750.0f, 650.0f, 720.0f, 6.0e3f,
                     ohlc_history[0]);
  ExpectNearOhlcTick(1483229100, 720.0f, 720.0f, 720.0f, 720.0f, 0.0f,
                     ohlc_history[1]);
  ExpectNearOhlcTick(1483229400, 800.0f, 850.0f, 750.0f, 850.0f, 8.0e3f,
                     ohlc_history[2]);

  ohlc_history = Resample(price_history,
                          /* start_timestamp_sec = */ 1483228800,
                          /* end_timestamp_sec = */ 1483229700,
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
