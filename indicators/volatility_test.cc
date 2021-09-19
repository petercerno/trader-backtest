// Copyright © 2021 Peter Cerno. All rights reserved.

#include "indicators/volatility.h"

#include "gtest/gtest.h"
#include "indicators/test_util.h"

namespace trader {
using ::trader::testing::PrepareExampleOhlcHistory;

TEST(RelativeStrengthIndexTest, GetVolatilityWhenAdding8HourOhlcTicks) {
  OhlcHistory ohlc_history;
  PrepareExampleOhlcHistory(ohlc_history);

  Volatility volatility(
      /*window_size=*/3,
      /*period_size_sec=*/kSecondsPerDay);

  float base_balance = 5.0f;
  float quote_balance = 1000.0f;

  EXPECT_FLOAT_EQ(volatility.GetVolatility(), 0.0f);
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 0);

  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 00:00
  // --- Daily History ---
  // O: 100  H: 150  L:  80  C: 120  V: 1000  T: 2017-01-01 (Day 1)
  volatility.Update(ohlc_history[0], base_balance, quote_balance);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), 0.0f);
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 1);

  // O: 120  H: 180  L: 100  C: 150  V: 1000  T: 2017-01-01 08:00
  // --- Daily History ---
  // O: 100  H: 180  L:  80  C: 150  V: 2000  T: 2017-01-01 (Day 1)
  volatility.Update(ohlc_history[1], base_balance, quote_balance);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), 0.0f);
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 1);

  // O: 150  H: 250  L: 100  C: 140  V: 1000  T: 2017-01-01 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  volatility.Update(ohlc_history[2], base_balance, quote_balance);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), 0.0f);
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 1);

  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  80  C: 100  V: 1000  T: 2017-01-02 (Day 2)
  volatility.Update(ohlc_history[3], base_balance, quote_balance);
  // Returns portfolio_1 value for a given price.
  const auto value1 = [base_balance, quote_balance](const float price) {
    return base_balance * price + quote_balance;
  };
  float m = (std::log(value1(140.0f) / value1(100.0f)) +
             std::log(value1(100.0f) / value1(140.0f))) /
            2;
  float v = (std::pow(std::log(value1(140.0f) / value1(100.0f)) - m, 2) +
             std::pow(std::log(value1(100.0f) / value1(140.0f)) - m, 2)) /
            2;
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 2);

  // O: 100  H: 120  L:  20  C:  50  V: 1000  T: 2017-01-02 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  50  V: 2000  T: 2017-01-02 (Day 2)
  volatility.Update(ohlc_history[4], base_balance, quote_balance);
  m = (std::log(value1(140.0f) / value1(100.0f)) +
       std::log(value1(50.0f) / value1(140.0f))) /
      2;
  v = (std::pow(std::log(value1(140.0f) / value1(100.0f)), 2) +
       std::pow(std::log(value1(50.0f) / value1(140.0f)), 2)) /
          2 -
      std::pow(m, 2);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 2);

  // O:  50  H: 100  L:  40  C:  80  V: 1000  T: 2017-01-02 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  volatility.Update(ohlc_history[5], base_balance, quote_balance);
  m = (std::log(value1(140.0f) / value1(100.0f)) +
       std::log(value1(80.0f) / value1(140.0f))) /
      2;
  v = (std::pow(std::log(value1(140.0f) / value1(100.0f)), 2) +
       std::pow(std::log(value1(80.0f) / value1(140.0f)), 2)) /
          2 -
      std::pow(m, 2);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 2);

  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 180  L:  50  C: 150  V: 1000  T: 2017-01-03 (Day 3)
  volatility.Update(ohlc_history[6], base_balance, quote_balance);
  m = (std::log(value1(140.0f) / value1(100.0f)) +
       std::log(value1(80.0f) / value1(140.0f)) +
       std::log(value1(150.0f) / value1(80.0f))) /
      3;
  v = (std::pow(std::log(value1(140.0f) / value1(100.0f)), 2) +
       std::pow(std::log(value1(80.0f) / value1(140.0f)), 2) +
       std::pow(std::log(value1(150.0f) / value1(80.0f)), 2)) /
          3 -
      std::pow(m, 2);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 3);

  // Let's update the portfolio by buying one unit of the base (crypto)currency.
  base_balance += 1.0f;
  quote_balance -= 150.0f;
  // Returns portfolio_2 value for a given price.
  const auto value2 = [base_balance, quote_balance](const float price) {
    return base_balance * price + quote_balance;
  };
  m = (std::log(value1(140.0f) / value1(100.0f)) +
       std::log(value1(80.0f) / value1(140.0f)) +
       std::log(value2(150.0f) / value1(80.0f))) /
      3;
  v = (std::pow(std::log(value1(140.0f) / value1(100.0f)), 2) +
       std::pow(std::log(value1(80.0f) / value1(140.0f)), 2) +
       std::pow(std::log(value2(150.0f) / value1(80.0f)), 2)) /
          3 -
      std::pow(m, 2);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));

  // O: 150  H: 250  L: 120  C: 240  V: 1000  T: 2017-01-03 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 250  L:  50  C: 240  V: 2000  T: 2017-01-03 (Day 3)
  volatility.Update(ohlc_history[7], base_balance, quote_balance);
  m = (std::log(value1(140.0f) / value1(100.0f)) +
       std::log(value1(80.0f) / value1(140.0f)) +
       std::log(value2(240.0f) / value1(80.0f))) /
      3;
  v = (std::pow(std::log(value1(140.0f) / value1(100.0f)), 2) +
       std::pow(std::log(value1(80.0f) / value1(140.0f)), 2) +
       std::pow(std::log(value2(240.0f) / value1(80.0f)), 2)) /
          3 -
      std::pow(m, 2);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 3);

  // O: 240  H: 450  L: 220  C: 400  V: 1000  T: 2017-01-03 16:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  volatility.Update(ohlc_history[8], base_balance, quote_balance);
  m = (std::log(value1(140.0f) / value1(100.0f)) +
       std::log(value1(80.0f) / value1(140.0f)) +
       std::log(value2(400.0f) / value1(80.0f))) /
      3;
  v = (std::pow(std::log(value1(140.0f) / value1(100.0f)), 2) +
       std::pow(std::log(value1(80.0f) / value1(140.0f)), 2) +
       std::pow(std::log(value2(400.0f) / value1(80.0f)), 2)) /
          3 -
      std::pow(m, 2);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 3);

  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 00:00 (+1 Day)
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 450  L: 250  C: 300  V: 1000  T: 2017-01-04 (Day 4)
  volatility.Update(ohlc_history[9], base_balance, quote_balance);
  m = (std::log(value1(80.0f) / value1(140.0f)) +
       std::log(value2(400.0f) / value1(80.0f)) +
       std::log(value2(300.0f) / value2(400.0f))) /
      3;
  v = (std::pow(std::log(value1(80.0f) / value1(140.0f)), 2) +
       std::pow(std::log(value2(400.0f) / value1(80.0f)), 2) +
       std::pow(std::log(value2(300.0f) / value2(400.0f)), 2)) /
          3 -
      std::pow(m, 2);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 4);

  // O: 300  H: 700  L: 220  C: 650  V: 1000  T: 2017-01-04 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 700  L: 220  C: 650  V: 2000  T: 2017-01-04 (Day 4)
  volatility.Update(ohlc_history[10], base_balance, quote_balance);
  m = (std::log(value1(80.0f) / value1(140.0f)) +
       std::log(value2(400.0f) / value1(80.0f)) +
       std::log(value2(650.0f) / value2(400.0f))) /
      3;
  v = (std::pow(std::log(value1(80.0f) / value1(140.0f)), 2) +
       std::pow(std::log(value2(400.0f) / value1(80.0f)), 2) +
       std::pow(std::log(value2(650.0f) / value2(400.0f)), 2)) /
          3 -
      std::pow(m, 2);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 4);

  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-04 16:00
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-05 00:00 (+1 Day)
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-05 08:00
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-05 16:00
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-06 00:00 (+1 Day)
  // O: 650  H: 800  L: 600  C: 750  V: 1000  T: 2017-01-06 08:00
  // --- Daily History ---
  // O: 100  H: 250  L:  80  C: 140  V: 3000  T: 2017-01-01 (Day 1)
  // O: 140  H: 150  L:  20  C:  80  V: 3000  T: 2017-01-02 (Day 2)
  // O:  80  H: 450  L:  50  C: 400  V: 3000  T: 2017-01-03 (Day 3)
  // O: 400  H: 700  L: 220  C: 650  V: 2000  T: 2017-01-04 (Day 4)
  // O: 650  H: 650  L: 650  C: 650  V:    0  T: 2017-01-05 (Day 5)
  // O: 650  H: 800  L: 600  C: 750  V: 1000  T: 2017-01-06 (Day 6)
  volatility.Update(ohlc_history[11], base_balance, quote_balance);
  m = (std::log(value2(650.0f) / value2(400.0f)) +
       std::log(value2(650.0f) / value2(650.0f)) +
       std::log(value2(750.0f) / value2(650.0f))) /
      3;
  v = (std::pow(std::log(value2(650.0f) / value2(400.0f)), 2) +
       std::pow(std::log(value2(650.0f) / value2(650.0f)), 2) +
       std::pow(std::log(value2(750.0f) / value2(650.0f)), 2)) /
          3 -
      std::pow(m, 2);
  EXPECT_FLOAT_EQ(volatility.GetVolatility(), std::sqrt(v));
  EXPECT_EQ(volatility.GetNumOhlcTicks(), 6);
}

}  // namespace trader
