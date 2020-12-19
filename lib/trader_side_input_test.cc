// Copyright © 2020 Peter Cerno. All rights reserved.

#include "lib/trader_side_input.h"

#include "gtest/gtest.h"

namespace trader {
namespace {
static constexpr int kSecondsPer4Hours = 4 * 60 * 60;
static constexpr int kSecondsPer8Hours = 8 * 60 * 60;

// Adds signals to the side_history. Does not set the timestamp_sec.
void AddSignals(const std::vector<float>& signals, SideHistory* side_history) {
  side_history->emplace_back();
  SideInputRecord* side_input = &side_history->back();
  for (const float signal : signals) {
    side_input->add_signal(signal);
  }
}

// Adds signals to the side_history period_sec away from the last record.
void AddSignalsWithPeriod(const std::vector<float>& signals, int period_sec,
                          SideHistory* side_history) {
  int timestamp_sec = 1483228800;  // 2017-01-01
  if (!side_history->empty()) {
    timestamp_sec = side_history->back().timestamp_sec() + period_sec;
  }
  AddSignals(signals, side_history);
  side_history->back().set_timestamp_sec(timestamp_sec);
}

// Adds signals to the side_history 8h away from the last record.
void Add8HourSignals(const std::vector<float>& signals,
                     SideHistory* side_history) {
  AddSignalsWithPeriod(signals, kSecondsPer8Hours, side_history);
}
}  // namespace

TEST(TraderSideInputTest, OneSignalSingleRecord) {
  SideHistory side_history;
  Add8HourSignals({10.0f}, &side_history);  // T: 1483228800

  TraderSideInput trader_side_input(side_history);
  ASSERT_EQ(1, trader_side_input.GetNumberOfRecords());
  ASSERT_EQ(1, trader_side_input.GetNumberOfSignals());

  EXPECT_EQ(1483228800, trader_side_input.GetSideInputTimestamp(0));
  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(0, 0));

  // (-inf, 1483200000)
  EXPECT_EQ(-1, trader_side_input.GetSideInputIndex(1483200000));
  EXPECT_EQ(-1, trader_side_input.GetSideInputIndex(1483200000, -1));
  // [1483228800, +inf)
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483228800));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483228800, -1));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483228800, 0));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483257600));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483257600, -1));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483257600, 0));
}

TEST(TraderSideInputTest, MultipleSignalsSingleRecord) {
  SideHistory side_history;
  Add8HourSignals({10.0f, 20.0f, 5.0f}, &side_history);  // T: 1483228800

  TraderSideInput trader_side_input(side_history);
  ASSERT_EQ(1, trader_side_input.GetNumberOfRecords());
  ASSERT_EQ(3, trader_side_input.GetNumberOfSignals());

  // (-inf, 1483200000)
  EXPECT_EQ(-1, trader_side_input.GetSideInputIndex(1483200000));
  EXPECT_EQ(-1, trader_side_input.GetSideInputIndex(1483200000, -1));
  // [1483228800, +inf)
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483228800));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483228800, -1));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483228800, 0));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483257600));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483257600, -1));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483257600, 0));
}

TEST(TraderSideInputTest, OneSignalMultipleRecords) {
  SideHistory side_history;
  Add8HourSignals({10.0f}, &side_history);  // T: 1483228800
  Add8HourSignals({20.0f}, &side_history);  // T: 1483257600
  Add8HourSignals({15.0f}, &side_history);  // T: 1483286400
  Add8HourSignals({5.0f}, &side_history);   // T: 1483315200
  Add8HourSignals({30.0f}, &side_history);  // T: 1483344000
  Add8HourSignals({25.0f}, &side_history);  // T: 1483372800
  Add8HourSignals({10.0f}, &side_history);  // T: 1483401600
  Add8HourSignals({15.0f}, &side_history);  // T: 1483430400
  Add8HourSignals({20.0f}, &side_history);  // T: 1483459200
  Add8HourSignals({5.0f}, &side_history);   // T: 1483488000

  TraderSideInput trader_side_input(side_history);
  ASSERT_EQ(10, trader_side_input.GetNumberOfRecords());
  ASSERT_EQ(1, trader_side_input.GetNumberOfSignals());

  EXPECT_EQ(1483228800, trader_side_input.GetSideInputTimestamp(0));
  EXPECT_EQ(1483257600, trader_side_input.GetSideInputTimestamp(1));
  EXPECT_EQ(1483286400, trader_side_input.GetSideInputTimestamp(2));
  EXPECT_EQ(1483315200, trader_side_input.GetSideInputTimestamp(3));
  EXPECT_EQ(1483344000, trader_side_input.GetSideInputTimestamp(4));
  EXPECT_EQ(1483372800, trader_side_input.GetSideInputTimestamp(5));
  EXPECT_EQ(1483401600, trader_side_input.GetSideInputTimestamp(6));
  EXPECT_EQ(1483430400, trader_side_input.GetSideInputTimestamp(7));
  EXPECT_EQ(1483459200, trader_side_input.GetSideInputTimestamp(8));
  EXPECT_EQ(1483488000, trader_side_input.GetSideInputTimestamp(9));

  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(0, 0));
  EXPECT_FLOAT_EQ(20.0f, trader_side_input.GetSideInputSignal(1, 0));
  EXPECT_FLOAT_EQ(15.0f, trader_side_input.GetSideInputSignal(2, 0));
  EXPECT_FLOAT_EQ(5.0f, trader_side_input.GetSideInputSignal(3, 0));
  EXPECT_FLOAT_EQ(30.0f, trader_side_input.GetSideInputSignal(4, 0));
  EXPECT_FLOAT_EQ(25.0f, trader_side_input.GetSideInputSignal(5, 0));
  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(6, 0));
  EXPECT_FLOAT_EQ(15.0f, trader_side_input.GetSideInputSignal(7, 0));
  EXPECT_FLOAT_EQ(20.0f, trader_side_input.GetSideInputSignal(8, 0));
  EXPECT_FLOAT_EQ(5.0f, trader_side_input.GetSideInputSignal(9, 0));

  // (-inf, 1483200000)
  EXPECT_EQ(-1, trader_side_input.GetSideInputIndex(1483200000));
  EXPECT_EQ(-1, trader_side_input.GetSideInputIndex(1483200000, -1));
  // [1483228800, 1483257600)
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483228800));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483228800, -1));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483228800, 0));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483243200));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483243200, -1));
  EXPECT_EQ(0, trader_side_input.GetSideInputIndex(1483243200, 0));
  // [1483257600, 1483286400)
  EXPECT_EQ(1, trader_side_input.GetSideInputIndex(1483257600));
  EXPECT_EQ(1, trader_side_input.GetSideInputIndex(1483257600, -1));
  EXPECT_EQ(1, trader_side_input.GetSideInputIndex(1483257600, 0));
  EXPECT_EQ(1, trader_side_input.GetSideInputIndex(1483257600, 1));
  EXPECT_EQ(1, trader_side_input.GetSideInputIndex(1483272000));
  EXPECT_EQ(1, trader_side_input.GetSideInputIndex(1483272000, -1));
  EXPECT_EQ(1, trader_side_input.GetSideInputIndex(1483272000, 0));
  EXPECT_EQ(1, trader_side_input.GetSideInputIndex(1483272000, 1));
  // [1483286400, 1483315200)
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483286400));
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483286400, -1));
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483286400, 0));
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483286400, 1));
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483286400, 2));
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483300800));
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483300800, -1));
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483300800, 0));
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483300800, 1));
  EXPECT_EQ(2, trader_side_input.GetSideInputIndex(1483300800, 2));
  for (int expected_index = 3; expected_index < 10; ++expected_index) {
    EXPECT_EQ(expected_index,
              trader_side_input.GetSideInputIndex(
                  1483228800 + kSecondsPer8Hours * expected_index));
    EXPECT_EQ(expected_index,
              trader_side_input.GetSideInputIndex(
                  1483228800 + kSecondsPer8Hours * expected_index +
                  kSecondsPer4Hours));
    for (int prev_side_input_index = -1;
         prev_side_input_index <= expected_index; ++prev_side_input_index) {
      EXPECT_EQ(expected_index,
                trader_side_input.GetSideInputIndex(
                    1483228800 + kSecondsPer8Hours * expected_index,
                    prev_side_input_index));
      EXPECT_EQ(expected_index,
                trader_side_input.GetSideInputIndex(
                    1483228800 + kSecondsPer8Hours * expected_index +
                        kSecondsPer4Hours,
                    prev_side_input_index));
    }
  }
}

TEST(TraderSideInputTest, MultipleSignalsMultipleRecords) {
  SideHistory side_history;
  Add8HourSignals({10.0f, 5.0f, 15.0f}, &side_history);   // T: 1483228800
  Add8HourSignals({20.0f, 10.0f, 30.0f}, &side_history);  // T: 1483257600
  Add8HourSignals({15.0f, 20.0f, 5.0f}, &side_history);   // T: 1483286400
  Add8HourSignals({5.0f, 10.0f, 5.0f}, &side_history);    // T: 1483315200
  Add8HourSignals({30.0f, 20.0f, 15.0f}, &side_history);  // T: 1483344000
  Add8HourSignals({25.0f, 20.0f, 10.0f}, &side_history);  // T: 1483372800
  Add8HourSignals({10.0f, 10.0f, 15.0f}, &side_history);  // T: 1483401600
  Add8HourSignals({15.0f, 5.0f, 30.0f}, &side_history);   // T: 1483430400
  Add8HourSignals({20.0f, 15.0f, 20.0f}, &side_history);  // T: 1483459200
  Add8HourSignals({5.0f, 25.0f, 10.0f}, &side_history);   // T: 1483488000

  TraderSideInput trader_side_input(side_history);
  ASSERT_EQ(10, trader_side_input.GetNumberOfRecords());
  ASSERT_EQ(3, trader_side_input.GetNumberOfSignals());

  EXPECT_EQ(1483228800, trader_side_input.GetSideInputTimestamp(0));
  EXPECT_EQ(1483257600, trader_side_input.GetSideInputTimestamp(1));
  EXPECT_EQ(1483286400, trader_side_input.GetSideInputTimestamp(2));
  EXPECT_EQ(1483315200, trader_side_input.GetSideInputTimestamp(3));
  EXPECT_EQ(1483344000, trader_side_input.GetSideInputTimestamp(4));
  EXPECT_EQ(1483372800, trader_side_input.GetSideInputTimestamp(5));
  EXPECT_EQ(1483401600, trader_side_input.GetSideInputTimestamp(6));
  EXPECT_EQ(1483430400, trader_side_input.GetSideInputTimestamp(7));
  EXPECT_EQ(1483459200, trader_side_input.GetSideInputTimestamp(8));
  EXPECT_EQ(1483488000, trader_side_input.GetSideInputTimestamp(9));

  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(0, 0));
  EXPECT_FLOAT_EQ(20.0f, trader_side_input.GetSideInputSignal(1, 0));
  EXPECT_FLOAT_EQ(15.0f, trader_side_input.GetSideInputSignal(2, 0));
  EXPECT_FLOAT_EQ(5.0f, trader_side_input.GetSideInputSignal(3, 0));
  EXPECT_FLOAT_EQ(30.0f, trader_side_input.GetSideInputSignal(4, 0));
  EXPECT_FLOAT_EQ(25.0f, trader_side_input.GetSideInputSignal(5, 0));
  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(6, 0));
  EXPECT_FLOAT_EQ(15.0f, trader_side_input.GetSideInputSignal(7, 0));
  EXPECT_FLOAT_EQ(20.0f, trader_side_input.GetSideInputSignal(8, 0));
  EXPECT_FLOAT_EQ(5.0f, trader_side_input.GetSideInputSignal(9, 0));

  EXPECT_FLOAT_EQ(5.0f, trader_side_input.GetSideInputSignal(0, 1));
  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(1, 1));
  EXPECT_FLOAT_EQ(20.0f, trader_side_input.GetSideInputSignal(2, 1));
  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(3, 1));
  EXPECT_FLOAT_EQ(20.0f, trader_side_input.GetSideInputSignal(4, 1));
  EXPECT_FLOAT_EQ(20.0f, trader_side_input.GetSideInputSignal(5, 1));
  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(6, 1));
  EXPECT_FLOAT_EQ(5.0f, trader_side_input.GetSideInputSignal(7, 1));
  EXPECT_FLOAT_EQ(15.0f, trader_side_input.GetSideInputSignal(8, 1));
  EXPECT_FLOAT_EQ(25.0f, trader_side_input.GetSideInputSignal(9, 1));

  EXPECT_FLOAT_EQ(15.0f, trader_side_input.GetSideInputSignal(0, 2));
  EXPECT_FLOAT_EQ(30.0f, trader_side_input.GetSideInputSignal(1, 2));
  EXPECT_FLOAT_EQ(5.0f, trader_side_input.GetSideInputSignal(2, 2));
  EXPECT_FLOAT_EQ(5.0f, trader_side_input.GetSideInputSignal(3, 2));
  EXPECT_FLOAT_EQ(15.0f, trader_side_input.GetSideInputSignal(4, 2));
  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(5, 2));
  EXPECT_FLOAT_EQ(15.0f, trader_side_input.GetSideInputSignal(6, 2));
  EXPECT_FLOAT_EQ(30.0f, trader_side_input.GetSideInputSignal(7, 2));
  EXPECT_FLOAT_EQ(20.0f, trader_side_input.GetSideInputSignal(8, 2));
  EXPECT_FLOAT_EQ(10.0f, trader_side_input.GetSideInputSignal(9, 2));

  for (int expected_index = -1; expected_index < 10; ++expected_index) {
    EXPECT_EQ(expected_index,
              trader_side_input.GetSideInputIndex(
                  1483228800 + kSecondsPer8Hours * expected_index));
    EXPECT_EQ(expected_index,
              trader_side_input.GetSideInputIndex(
                  1483228800 + kSecondsPer8Hours * expected_index +
                  kSecondsPer4Hours));
    for (int prev_side_input_index = -1;
         prev_side_input_index <= expected_index; ++prev_side_input_index) {
      EXPECT_EQ(expected_index,
                trader_side_input.GetSideInputIndex(
                    1483228800 + kSecondsPer8Hours * expected_index,
                    prev_side_input_index));
      EXPECT_EQ(expected_index,
                trader_side_input.GetSideInputIndex(
                    1483228800 + kSecondsPer8Hours * expected_index +
                        kSecondsPer4Hours,
                    prev_side_input_index));
    }
  }
}

}  // namespace trader