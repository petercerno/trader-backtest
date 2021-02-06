// Copyright © 2021 Peter Cerno. All rights reserved.

#include "util/proto.h"

#include <sstream>

#include "gtest/gtest.h"
#include "util/example.pb.h"

namespace trader {

TEST(ReadWriteDelimitedTest, ReadWriteEmptyStream) {
  std::istringstream iss;
  std::vector<PriceRecord> messages;
  ASSERT_TRUE(ReadDelimitedMessagesFromIStream(iss, messages));
  ASSERT_EQ(0, messages.size());
}

TEST(ReadWriteDelimitedTest, ReadWriteSinglePriceRecord) {
  std::ostringstream oss;
  std::vector<PriceRecord> input_messages;
  input_messages.emplace_back();
  input_messages.back().set_timestamp_sec(1483228800);
  input_messages.back().set_price(700.0f);
  input_messages.back().set_volume(1.5e4f);
  WriteDelimitedMessagesToOStream(input_messages.begin(), input_messages.end(),
                                  oss, /* compress = */ true);
  std::istringstream iss(oss.str());
  std::vector<PriceRecord> messages;
  ReadDelimitedMessagesFromIStream(iss, messages);
  ASSERT_EQ(1, messages.size());
  EXPECT_EQ(1483228800, messages[0].timestamp_sec());
  EXPECT_FLOAT_EQ(700.0f, messages[0].price());
  EXPECT_FLOAT_EQ(1.5e4f, messages[0].volume());
}

TEST(ReadWriteDelimitedTest, ReadWriteMultipleOhlcTicks) {
  static constexpr int kNumTicks = 10;
  std::ostringstream oss;
  std::vector<OhlcTick> input_messages;
  for (int i = 0; i < kNumTicks; ++i) {
    input_messages.emplace_back();
    input_messages.back().set_open(100.0f + 10.0f * i);
    input_messages.back().set_high(120.0f + 20.0f * i);
    input_messages.back().set_low(80.0f + 10.0f * i);
    input_messages.back().set_close(110.0f + 10.0f * i);
    input_messages.back().set_volume(1.5e4f + 1.0e3f * i);
  }
  WriteDelimitedMessagesToOStream(input_messages.begin(), input_messages.end(),
                                  oss, /* compress = */ true);
  std::istringstream iss(oss.str());
  std::vector<OhlcTick> messages;
  ReadDelimitedMessagesFromIStream(iss, messages);
  ASSERT_EQ(kNumTicks, messages.size());
  for (int i = 0; i < kNumTicks; ++i) {
    EXPECT_FLOAT_EQ(100.0f + 10.0f * i, messages[i].open());
    EXPECT_FLOAT_EQ(120.0f + 20.0f * i, messages[i].high());
    EXPECT_FLOAT_EQ(80.0f + 10.0f * i, messages[i].low());
    EXPECT_FLOAT_EQ(110.0f + 10.0f * i, messages[i].close());
    EXPECT_FLOAT_EQ(1.5e4f + 1.0e3f * i, messages[i].volume());
  }
}

}  // namespace trader
