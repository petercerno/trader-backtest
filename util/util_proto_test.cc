// Copyright © 2020 Peter Cerno. All rights reserved.

#include "util/util_proto.h"

#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "util/example.pb.h"

using google::protobuf::io::IstreamInputStream;
using google::protobuf::io::OstreamOutputStream;

namespace trader {

TEST(ReadWriteDelimitedTest, ReadWriteEmptyStream) {
  std::ostringstream oss;
  { OstreamOutputStream output_stream(&oss); }

  std::istringstream iss(oss.str());
  std::vector<PriceRecord> messages;
  {
    IstreamInputStream input_stream(&iss);
    PriceRecord message;
    while (ReadDelimitedFrom(input_stream, message)) {
      messages.emplace_back(message);
    }
  }
  ASSERT_EQ(0, messages.size());
}

TEST(ReadWriteDelimitedTest, ReadWriteSinglePriceRecord) {
  std::ostringstream oss;
  {
    OstreamOutputStream output_stream(&oss);
    PriceRecord price_record;
    price_record.set_timestamp_sec(1483228800);
    price_record.set_price(700.0f);
    price_record.set_volume(1.5e4f);
    ASSERT_TRUE(WriteDelimitedTo(price_record, output_stream));
  }

  std::istringstream iss(oss.str());
  std::vector<PriceRecord> messages;
  {
    IstreamInputStream input_stream(&iss);
    PriceRecord message;
    while (ReadDelimitedFrom(input_stream, message)) {
      messages.emplace_back(message);
    }
  }
  ASSERT_EQ(1, messages.size());
  EXPECT_EQ(1483228800, messages[0].timestamp_sec());
  EXPECT_FLOAT_EQ(700.0f, messages[0].price());
  EXPECT_FLOAT_EQ(1.5e4f, messages[0].volume());
}

TEST(ReadWriteDelimitedTest, ReadWriteMultipleOhlcTicks) {
  constexpr int kNumTicks = 10;
  std::ostringstream oss;
  {
    OstreamOutputStream output_stream(&oss);
    for (int i = 0; i < kNumTicks; ++i) {
      OhlcTick ohlc_tick;
      ohlc_tick.set_open(100.0f + 10.0f * i);
      ohlc_tick.set_high(120.0f + 20.0f * i);
      ohlc_tick.set_low(80.0f + 10.0f * i);
      ohlc_tick.set_close(110.0f + 10.0f * i);
      ohlc_tick.set_volume(1.5e4f + 1.0e3f * i);
      ASSERT_TRUE(WriteDelimitedTo(ohlc_tick, output_stream));
    }
  }

  std::istringstream iss(oss.str());
  std::vector<OhlcTick> messages;
  {
    IstreamInputStream input_stream(&iss);
    OhlcTick message;
    while (ReadDelimitedFrom(input_stream, message)) {
      messages.emplace_back(message);
    }
  }
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
