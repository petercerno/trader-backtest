// Copyright © 2021 Peter Cerno. All rights reserved.

#include "util/proto.h"

#include <sstream>

#include "gtest/gtest.h"
#include "util/example.pb.h"

namespace trader {

TEST(ReadWriteDelimitedTest, ReadWriteEmptyStream) {
  std::istringstream iss;
  std::vector<PriceRecord> messages;
  ASSERT_TRUE(ReadDelimitedMessagesFromIStream(iss, messages).ok());
  ASSERT_EQ(messages.size(), 0);
}

TEST(ReadWriteDelimitedTest, ReadWriteSinglePriceRecord) {
  std::ostringstream oss;
  std::vector<PriceRecord> input_messages;
  input_messages.emplace_back();
  input_messages.back().set_timestamp_sec(1483228800);
  input_messages.back().set_price(700.0f);
  input_messages.back().set_volume(1.5e4f);
  ASSERT_TRUE(WriteDelimitedMessagesToOStream(input_messages.begin(),
                                              input_messages.end(), oss,
                                              /*compress=*/true)
                  .ok());
  std::istringstream iss(oss.str());
  std::vector<PriceRecord> messages;
  ASSERT_TRUE(ReadDelimitedMessagesFromIStream(iss, messages).ok());
  ASSERT_EQ(messages.size(), 1);
  EXPECT_EQ(messages[0].timestamp_sec(), 1483228800);
  EXPECT_FLOAT_EQ(messages[0].price(), 700.0f);
  EXPECT_FLOAT_EQ(messages[0].volume(), 1.5e4f);
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
  ASSERT_TRUE(WriteDelimitedMessagesToOStream(input_messages.begin(),
                                              input_messages.end(), oss,
                                              /*compress=*/true)
                  .ok());
  std::istringstream iss(oss.str());
  std::vector<OhlcTick> messages;
  ASSERT_TRUE(ReadDelimitedMessagesFromIStream(iss, messages).ok());
  ASSERT_EQ(messages.size(), kNumTicks);
  for (int i = 0; i < kNumTicks; ++i) {
    EXPECT_FLOAT_EQ(messages[i].open(), 100.0f + 10.0f * i);
    EXPECT_FLOAT_EQ(messages[i].high(), 120.0f + 20.0f * i);
    EXPECT_FLOAT_EQ(messages[i].low(), 80.0f + 10.0f * i);
    EXPECT_FLOAT_EQ(messages[i].close(), 110.0f + 10.0f * i);
    EXPECT_FLOAT_EQ(messages[i].volume(), 1.5e4f + 1.0e3f * i);
  }
}

}  // namespace trader
