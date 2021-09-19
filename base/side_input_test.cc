// Copyright © 2021 Peter Cerno. All rights reserved.

#include "base/side_input.h"

#include "gtest/gtest.h"

namespace trader {
namespace {
static constexpr int kSecondsPer4Hours = 4 * 60 * 60;
static constexpr int kSecondsPer8Hours = 8 * 60 * 60;

// Adds signals to the side_history. Does not set the timestamp_sec.
void AddSignals(const std::vector<float>& signals, SideHistory& side_history) {
  side_history.emplace_back();
  SideInputRecord& side_input = side_history.back();
  for (const float signal : signals) {
    side_input.add_signal(signal);
  }
}

// Adds signals to the side_history period_sec away from the last record.
void AddSignalsWithPeriod(const std::vector<float>& signals, int period_sec,
                          SideHistory& side_history) {
  int64_t timestamp_sec = 1483228800;  // 2017-01-01
  if (!side_history.empty()) {
    timestamp_sec = side_history.back().timestamp_sec() + period_sec;
  }
  AddSignals(signals, side_history);
  side_history.back().set_timestamp_sec(timestamp_sec);
}

// Adds signals to the side_history 8h away from the last record.
void Add8HourSignals(const std::vector<float>& signals,
                     SideHistory& side_history) {
  AddSignalsWithPeriod(signals, kSecondsPer8Hours, side_history);
}
}  // namespace

TEST(SideInputTest, OneSignalSingleRecord) {
  SideHistory side_history;
  Add8HourSignals({10.0f}, side_history);  // T: 1483228800

  SideInput trader_side_input(side_history);
  ASSERT_EQ(trader_side_input.GetNumberOfRecords(), 1);
  ASSERT_EQ(trader_side_input.GetNumberOfSignals(), 1);

  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(0), 1483228800);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(0, 0), 10.0f);

  std::vector<float> side_input_signals;
  trader_side_input.GetSideInputSignals(0, side_input_signals);
  ASSERT_EQ(side_input_signals.size(), 1);
  EXPECT_FLOAT_EQ(side_input_signals[0], 10.0f);

  // (-inf, 1483200000)
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483200000), -1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483200000, -1), -1);
  // [1483228800, +inf)
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483228800), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483228800, -1), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483228800, 0), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600, -1), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600, 0), 0);
}

TEST(SideInputTest, MultipleSignalsSingleRecord) {
  SideHistory side_history;
  Add8HourSignals({10.0f, 20.0f, 5.0f}, side_history);  // T: 1483228800

  SideInput trader_side_input(side_history);
  ASSERT_EQ(trader_side_input.GetNumberOfRecords(), 1);
  ASSERT_EQ(trader_side_input.GetNumberOfSignals(), 3);

  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(0), 1483228800);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(0, 0), 10.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(0, 1), 20.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(0, 2), 5.0f);

  std::vector<float> side_input_signals;
  trader_side_input.GetSideInputSignals(0, side_input_signals);
  ASSERT_EQ(side_input_signals.size(), 3);
  EXPECT_FLOAT_EQ(side_input_signals[0], 10.0f);
  EXPECT_FLOAT_EQ(side_input_signals[1], 20.0f);
  EXPECT_FLOAT_EQ(side_input_signals[2], 5.0f);

  // (-inf, 1483200000)
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483200000), -1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483200000, -1), -1);
  // [1483228800, +inf)
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483228800), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483228800, -1), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483228800, 0), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600, -1), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600, 0), 0);
}

TEST(SideInputTest, OneSignalMultipleRecords) {
  SideHistory side_history;
  Add8HourSignals({10.0f}, side_history);  // T: 1483228800
  Add8HourSignals({20.0f}, side_history);  // T: 1483257600
  Add8HourSignals({15.0f}, side_history);  // T: 1483286400
  Add8HourSignals({5.0f}, side_history);   // T: 1483315200
  Add8HourSignals({30.0f}, side_history);  // T: 1483344000
  Add8HourSignals({25.0f}, side_history);  // T: 1483372800
  Add8HourSignals({10.0f}, side_history);  // T: 1483401600
  Add8HourSignals({15.0f}, side_history);  // T: 1483430400
  Add8HourSignals({20.0f}, side_history);  // T: 1483459200
  Add8HourSignals({5.0f}, side_history);   // T: 1483488000

  SideInput trader_side_input(side_history);
  ASSERT_EQ(trader_side_input.GetNumberOfRecords(), 10);
  ASSERT_EQ(trader_side_input.GetNumberOfSignals(), 1);

  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(0), 1483228800);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(1), 1483257600);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(2), 1483286400);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(3), 1483315200);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(4), 1483344000);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(5), 1483372800);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(6), 1483401600);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(7), 1483430400);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(8), 1483459200);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(9), 1483488000);

  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(0, 0), 10.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(1, 0), 20.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(2, 0), 15.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(3, 0), 5.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(4, 0), 30.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(5, 0), 25.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(6, 0), 10.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(7, 0), 15.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(8, 0), 20.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(9, 0), 5.0f);

  std::vector<float> side_input_signals;
  for (int side_input_index = 0; side_input_index < 10; ++side_input_index) {
    trader_side_input.GetSideInputSignals(side_input_index, side_input_signals);
    ASSERT_EQ(side_input_signals.size(), 1);
    EXPECT_FLOAT_EQ(side_input_signals[0],
                    trader_side_input.GetSideInputSignal(side_input_index, 0));
    side_input_signals.clear();
  }

  // (-inf, 1483200000)
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483200000), -1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483200000, -1), -1);
  // [1483228800, 1483257600)
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483228800), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483228800, -1), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483228800, 0), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483243200), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483243200, -1), 0);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483243200, 0), 0);
  // [1483257600, 1483286400)
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600), 1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600, -1), 1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600, 0), 1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483257600, 1), 1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483272000), 1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483272000, -1), 1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483272000, 0), 1);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483272000, 1), 1);
  // [1483286400, 1483315200)
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483286400), 2);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483286400, -1), 2);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483286400, 0), 2);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483286400, 1), 2);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483286400, 2), 2);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483300800), 2);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483300800, -1), 2);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483300800, 0), 2);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483300800, 1), 2);
  EXPECT_EQ(trader_side_input.GetSideInputIndex(1483300800, 2), 2);
  for (int expected_index = 3; expected_index < 10; ++expected_index) {
    EXPECT_EQ(trader_side_input.GetSideInputIndex(
                  1483228800 + kSecondsPer8Hours * expected_index),
              expected_index);
    EXPECT_EQ(trader_side_input.GetSideInputIndex(
                  1483228800 + kSecondsPer8Hours * expected_index +
                  kSecondsPer4Hours),
              expected_index);
    for (int prev_side_input_index = -1;
         prev_side_input_index <= expected_index; ++prev_side_input_index) {
      EXPECT_EQ(trader_side_input.GetSideInputIndex(
                    1483228800 + kSecondsPer8Hours * expected_index,
                    prev_side_input_index),
                expected_index);
      EXPECT_EQ(trader_side_input.GetSideInputIndex(
                    1483228800 + kSecondsPer8Hours * expected_index +
                        kSecondsPer4Hours,
                    prev_side_input_index),
                expected_index);
    }
  }
}

TEST(SideInputTest, MultipleSignalsMultipleRecords) {
  SideHistory side_history;
  Add8HourSignals({10.0f, 5.0f, 15.0f}, side_history);   // T: 1483228800
  Add8HourSignals({20.0f, 10.0f, 30.0f}, side_history);  // T: 1483257600
  Add8HourSignals({15.0f, 20.0f, 5.0f}, side_history);   // T: 1483286400
  Add8HourSignals({5.0f, 10.0f, 5.0f}, side_history);    // T: 1483315200
  Add8HourSignals({30.0f, 20.0f, 15.0f}, side_history);  // T: 1483344000
  Add8HourSignals({25.0f, 20.0f, 10.0f}, side_history);  // T: 1483372800
  Add8HourSignals({10.0f, 10.0f, 15.0f}, side_history);  // T: 1483401600
  Add8HourSignals({15.0f, 5.0f, 30.0f}, side_history);   // T: 1483430400
  Add8HourSignals({20.0f, 15.0f, 20.0f}, side_history);  // T: 1483459200
  Add8HourSignals({5.0f, 25.0f, 10.0f}, side_history);   // T: 1483488000

  SideInput trader_side_input(side_history);
  ASSERT_EQ(trader_side_input.GetNumberOfRecords(), 10);
  ASSERT_EQ(trader_side_input.GetNumberOfSignals(), 3);

  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(0), 1483228800);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(1), 1483257600);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(2), 1483286400);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(3), 1483315200);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(4), 1483344000);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(5), 1483372800);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(6), 1483401600);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(7), 1483430400);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(8), 1483459200);
  EXPECT_EQ(trader_side_input.GetSideInputTimestamp(9), 1483488000);

  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(0, 0), 10.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(1, 0), 20.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(2, 0), 15.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(3, 0), 5.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(4, 0), 30.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(5, 0), 25.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(6, 0), 10.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(7, 0), 15.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(8, 0), 20.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(9, 0), 5.0f);

  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(0, 1), 5.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(1, 1), 10.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(2, 1), 20.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(3, 1), 10.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(4, 1), 20.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(5, 1), 20.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(6, 1), 10.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(7, 1), 5.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(8, 1), 15.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(9, 1), 25.0f);

  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(0, 2), 15.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(1, 2), 30.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(2, 2), 5.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(3, 2), 5.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(4, 2), 15.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(5, 2), 10.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(6, 2), 15.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(7, 2), 30.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(8, 2), 20.0f);
  EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(9, 2), 10.0f);

  std::vector<float> side_input_signals;
  side_input_signals.reserve(3);
  for (int side_input_index = 0; side_input_index < 10; ++side_input_index) {
    trader_side_input.GetSideInputSignals(side_input_index, side_input_signals);
    ASSERT_EQ(side_input_signals.size(), 3);
    EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(side_input_index, 0),
                    side_input_signals[0]);
    EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(side_input_index, 1),
                    side_input_signals[1]);
    EXPECT_FLOAT_EQ(trader_side_input.GetSideInputSignal(side_input_index, 2),
                    side_input_signals[2]);
    side_input_signals.clear();
    EXPECT_GE(side_input_signals.capacity(), 3);
  }

  for (int expected_index = -1; expected_index < 10; ++expected_index) {
    EXPECT_EQ(trader_side_input.GetSideInputIndex(
                  1483228800 + kSecondsPer8Hours * expected_index),
              expected_index);
    EXPECT_EQ(trader_side_input.GetSideInputIndex(
                  1483228800 + kSecondsPer8Hours * expected_index +
                  kSecondsPer4Hours),
              expected_index);
    for (int prev_side_input_index = -1;
         prev_side_input_index <= expected_index; ++prev_side_input_index) {
      EXPECT_EQ(trader_side_input.GetSideInputIndex(
                    1483228800 + kSecondsPer8Hours * expected_index,
                    prev_side_input_index),
                expected_index);
      EXPECT_EQ(trader_side_input.GetSideInputIndex(
                    1483228800 + kSecondsPer8Hours * expected_index +
                        kSecondsPer4Hours,
                    prev_side_input_index),
                expected_index);
    }
  }
}

}  // namespace trader
