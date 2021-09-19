// Copyright © 2021 Peter Cerno. All rights reserved.

#include "eval/eval.h"

#include <google/protobuf/text_format.h>
#include <google/protobuf/util/message_differencer.h>

#include "gtest/gtest.h"
#include "logging/csv_logger.h"
#include "util/time.h"

namespace trader {
using ::google::protobuf::Message;
using ::google::protobuf::TextFormat;
using ::google::protobuf::util::MessageDifferencer;

namespace {
// Compares two protobuf messages and outputs a diff if they differ.
void ExpectProtoEq(const Message& message, const Message& expected_message,
                   bool full_scope = true) {
  EXPECT_EQ(message.GetTypeName(), expected_message.GetTypeName());
  std::string diff_report;
  MessageDifferencer differencer;
  differencer.set_scope(full_scope ? MessageDifferencer::FULL
                                   : MessageDifferencer::PARTIAL);
  differencer.ReportDifferencesToString(&diff_report);
  EXPECT_TRUE(differencer.Compare(expected_message, message)) << diff_report;
}

// Adds OHLC tick to the history. Does not set the timestamp_sec.
void AddOhlcTick(float open, float high, float low, float close,
                 OhlcHistory& ohlc_history) {
  ASSERT_LE(low, open);
  ASSERT_LE(low, high);
  ASSERT_LE(low, close);
  ASSERT_GE(high, open);
  ASSERT_GE(high, close);
  ohlc_history.emplace_back();
  OhlcTick& ohlc_tick = ohlc_history.back();
  ohlc_tick.set_open(open);
  ohlc_tick.set_high(high);
  ohlc_tick.set_low(low);
  ohlc_tick.set_close(close);
  ohlc_tick.set_volume(1000.0f);
}

// Adds daily OHLC tick to the history.
void AddDailyOhlcTick(float open, float high, float low, float close,
                      OhlcHistory& ohlc_history) {
  int64_t timestamp_sec = 1483228800;  // 2017-01-01
  if (!ohlc_history.empty()) {
    ASSERT_FLOAT_EQ(open, ohlc_history.back().close());
    timestamp_sec = ohlc_history.back().timestamp_sec() + 24 * 60 * 60;
  }
  AddOhlcTick(open, high, low, close, ohlc_history);
  ohlc_history.back().set_timestamp_sec(timestamp_sec);
}

// Adds monthly OHLC tick to the history.
void AddMonthlyOhlcTick(float open, float high, float low, float close,
                        OhlcHistory& ohlc_history) {
  int64_t timestamp_sec = 1483228800;  // 2017-01-01
  if (!ohlc_history.empty()) {
    ASSERT_FLOAT_EQ(open, ohlc_history.back().close());
    timestamp_sec =
        AddMonthsToTimestampSec(ohlc_history.back().timestamp_sec(), 1);
  }
  AddOhlcTick(open, high, low, close, ohlc_history);
  ohlc_history.back().set_timestamp_sec(timestamp_sec);
}

void SetupDailyOhlcHistory(OhlcHistory& ohlc_history) {
  AddDailyOhlcTick(100, 150, 80, 120, ohlc_history);   // 2017-01-01
  AddDailyOhlcTick(120, 180, 100, 150, ohlc_history);  // 2017-01-02
  AddDailyOhlcTick(150, 250, 100, 140, ohlc_history);  // 2017-01-03
  AddDailyOhlcTick(140, 150, 80, 100, ohlc_history);   // 2017-01-04
  AddDailyOhlcTick(100, 120, 20, 50, ohlc_history);    // 2017-01-05
}

void SetupMonthlyOhlcHistory(OhlcHistory& ohlc_history) {
  AddMonthlyOhlcTick(100, 150, 80, 120, ohlc_history);   // 2017-01-01
  AddMonthlyOhlcTick(120, 180, 100, 150, ohlc_history);  // 2017-02-01
  AddMonthlyOhlcTick(150, 250, 100, 140, ohlc_history);  // 2017-03-01
  AddMonthlyOhlcTick(140, 150, 80, 100, ohlc_history);   // 2017-04-01
  AddMonthlyOhlcTick(100, 120, 20, 50, ohlc_history);    // 2017-05-01
  AddMonthlyOhlcTick(50, 100, 40, 80, ohlc_history);     // 2017-06-01
  AddMonthlyOhlcTick(80, 180, 50, 150, ohlc_history);    // 2017-07-01
  AddMonthlyOhlcTick(150, 250, 120, 240, ohlc_history);  // 2017-08-01
  AddMonthlyOhlcTick(240, 450, 220, 400, ohlc_history);  // 2017-09-01
  AddMonthlyOhlcTick(400, 450, 250, 300, ohlc_history);  // 2017-10-01
  AddMonthlyOhlcTick(300, 700, 220, 650, ohlc_history);  // 2017-11-01
  AddMonthlyOhlcTick(650, 800, 600, 750, ohlc_history);  // 2017-12-01
}

// Trader that buys and sells the base (crypto) currency at fixed prices.
class TestTrader : public Trader {
 public:
  TestTrader(float buy_price, float sell_price)
      : buy_price_(buy_price), sell_price_(sell_price) {}
  virtual ~TestTrader() {}

  void Update(const OhlcTick& ohlc_tick,
              const std::vector<float>& side_input_signals, float base_balance,
              float quote_balance, std::vector<Order>& orders) override {
    last_base_balance_ = base_balance;
    last_quote_balance_ = quote_balance;
    last_timestamp_sec_ = ohlc_tick.timestamp_sec();
    last_close_ = ohlc_tick.close();
    is_long_ = last_close_ * last_base_balance_ > last_quote_balance_;
    orders.emplace_back();
    Order& order = orders.back();
    if (is_long_) {
      order.set_type(Order_Type_LIMIT);
      order.set_side(Order_Side_SELL);
      order.set_base_amount(last_base_balance_);
      order.set_price(sell_price_);
    } else {
      order.set_type(Order_Type_LIMIT);
      order.set_side(Order_Side_BUY);
      order.set_quote_amount(last_quote_balance_);
      order.set_price(buy_price_);
    }
  }

  std::string GetInternalState() const override {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0)  // nowrap
       << last_timestamp_sec_ << ","          // nowrap
       << std::setprecision(3)                // nowrap
       << last_base_balance_ << ","           // nowrap
       << last_quote_balance_ << ","          // nowrap
       << last_close_ << ",";                 // nowrap
    if (is_long_) {
      ss << "IN_LONG,LIMIT_SELL@"  // nowrap
         << std::setprecision(0) << sell_price_;
    } else {
      ss << "IN_CASH,LIMIT_BUY@"  // nowrap
         << std::setprecision(0) << buy_price_;
    }
    return ss.str();
  }

 private:
  // Price at which we want to buy the base (crypto) currency.
  float buy_price_ = 0.0f;
  // Price at which we want to sell the base (crypto) currency.
  float sell_price_ = 0.0f;
  // Last seen trader account balance.
  float last_base_balance_ = 0.0f;
  float last_quote_balance_ = 0.0f;
  // Last seen UNIX timestamp (in seconds).
  int64_t last_timestamp_sec_ = 0;
  // Last seen close price.
  float last_close_ = 0.0f;
  // True iff most of the account value is in base (crypto) currency.
  bool is_long_ = false;
};

// Emitter that emits TestTrader as defined above.
class TestTraderEmitter : public TraderEmitter {
 public:
  TestTraderEmitter(float buy_price, float sell_price)
      : buy_price_(buy_price), sell_price_(sell_price) {}
  virtual ~TestTraderEmitter() {}

  std::string GetName() const override {
    std::stringstream trader_name_os;
    trader_name_os << std::fixed << std::setprecision(0) << "test-trader"
                   << "[" << buy_price_ << "|" << sell_price_ << "]";
    return trader_name_os.str();
  }

  std::unique_ptr<Trader> NewTrader() const override {
    // Note: std::make_unique is C++14 feature.
    return std::unique_ptr<TestTrader>(new TestTrader(buy_price_, sell_price_));
  }

 private:
  // Price at which we want to buy the base (crypto) currency.
  float buy_price_ = 0.0f;
  // Price at which we want to sell the base (crypto) currency.
  float sell_price_ = 0.0f;
};
}  // namespace

TEST(ExecuteTraderTest, LimitBuyAndSell) {
  AccountConfig account_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        base_unit: 0.1
        quote_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1)",
      &account_config));

  OhlcHistory ohlc_history;
  SetupDailyOhlcHistory(ohlc_history);

  TestTrader trader(/*buy_price=*/50, /*sell_price=*/200);

  std::stringstream exchange_os;
  std::stringstream trader_os;
  CsvLogger logger(&exchange_os, &trader_os);

  ExecutionResult result =
      ExecuteTrader(account_config, ohlc_history.begin(), ohlc_history.end(),
                    /*side_input=*/nullptr,
                    /*fast_eval=*/false, trader, &logger);

  ExecutionResult expected_result;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        end_base_balance: 32.3
        end_quote_balance: 21
        start_price: 120
        end_price: 50
        start_value: 1200
        end_value: 1636
        total_executed_orders: 2
        total_fee: 364
        base_volatility: 6.53697348
        trader_volatility: 2.36517286
        )",
      &expected_result));
  ExpectProtoEq(result, expected_result);

  std::stringstream expected_exchange_os;
  std::stringstream expected_trader_os;

  expected_exchange_os  // nowrap
      << "1483228800,100.000,150.000,80.000,120.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1483315200,120.000,180.000,100.000,150.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1483401600,150.000,250.000,100.000,140.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1483401600,150.000,250.000,100.000,140.000,"
      << "1000.000,0.000,1799.000,201.000,LIMIT,SELL,10.000,,200.000"
      << std::endl
      << "1483488000,140.000,150.000,80.000,100.000,"
      << "1000.000,0.000,1799.000,201.000,,,,," << std::endl
      << "1483574400,100.000,120.000,20.000,50.000,"
      << "1000.000,0.000,1799.000,201.000,,,,," << std::endl
      << "1483574400,100.000,120.000,20.000,50.000,"
      << "1000.000,32.300,21.000,364.000,LIMIT,BUY,,1799.000,50.000"
      << std::endl;

  expected_trader_os  // nowrap
      << "1483228800,10.000,0.000,120.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1483315200,10.000,0.000,150.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1483401600,0.000,1799.000,140.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1483488000,0.000,1799.000,100.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1483574400,32.300,21.000,50.000,IN_LONG,LIMIT_SELL@200" << std::endl;

  EXPECT_EQ(exchange_os.str(), expected_exchange_os.str());
  EXPECT_EQ(trader_os.str(), expected_trader_os.str());
}

TEST(ExecuteTraderTest, LimitBuyAndSellFastEval) {
  AccountConfig account_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        base_unit: 0.1
        quote_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1)",
      &account_config));

  OhlcHistory ohlc_history;
  SetupDailyOhlcHistory(ohlc_history);

  TestTrader trader(/*buy_price=*/50, /*sell_price=*/200);

  ExecutionResult result =
      ExecuteTrader(account_config, ohlc_history.begin(), ohlc_history.end(),
                    /*side_input=*/nullptr, /*fast_eval=*/true, trader,
                    /*logger=*/nullptr);

  ExecutionResult expected_result;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        end_base_balance: 32.3
        end_quote_balance: 21
        start_price: 120
        end_price: 50
        start_value: 1200
        end_value: 1636
        total_executed_orders: 2
        total_fee: 364
        )",
      &expected_result));
  ExpectProtoEq(result, expected_result);
}

TEST(EvaluateTraderTest, LimitBuyAndSellOnePeriod) {
  AccountConfig account_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        base_unit: 0.1
        quote_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1
        )",
      &account_config));

  OhlcHistory ohlc_history;
  SetupMonthlyOhlcHistory(ohlc_history);

  EvaluationConfig eval_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_timestamp_sec: 1483228800
        end_timestamp_sec: 1514764800
        evaluation_period_months: 0
        fast_eval: false
        )",
      &eval_config));

  TestTraderEmitter trader_emitter(/*buy_price=*/50,
                                   /*sell_price=*/200);
  EXPECT_EQ(trader_emitter.GetName(), "test-trader[50|200]");

  std::stringstream exchange_os;
  std::stringstream trader_os;
  CsvLogger logger(&exchange_os, &trader_os);

  EvaluationResult result =
      EvaluateTrader(account_config, eval_config, ohlc_history,
                     /*side_input=*/nullptr, trader_emitter, &logger);

  EvaluationResult expected_result;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        account_config {
            start_base_balance: 10
            start_quote_balance: 0
            base_unit: 0.1
            quote_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 0
            fast_eval: false
        }
        name: "test-trader[50|200]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            result {
                start_base_balance: 10
                start_quote_balance: 0
                end_base_balance: 0
                end_quote_balance: 5834
                start_price: 120
                end_price: 750
                start_value: 1200
                end_value: 5834
                total_executed_orders: 3
                total_fee: 1011
                base_volatility: 1.63992178
                trader_volatility: 1.3071624
            }
            final_gain: 4.86166668
            base_final_gain: 6.25
        }
        score: 0.777866662
        avg_gain: 4.86166668
        avg_base_gain: 6.25
        avg_total_executed_orders: 3
        avg_total_fee: 1011
        )",
      &expected_result));
  ExpectProtoEq(result, expected_result);

  std::stringstream expected_exchange_os;
  std::stringstream expected_trader_os;

  expected_exchange_os  // nowrap
      << "1483228800,100.000,150.000,80.000,120.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1485907200,120.000,180.000,100.000,150.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1488326400,150.000,250.000,100.000,140.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1488326400,150.000,250.000,100.000,140.000,"
      << "1000.000,0.000,1799.000,201.000,LIMIT,SELL,10.000,,200.000"
      << std::endl
      << "1491004800,140.000,150.000,80.000,100.000,"
      << "1000.000,0.000,1799.000,201.000,,,,," << std::endl
      << "1493596800,100.000,120.000,20.000,50.000,"
      << "1000.000,0.000,1799.000,201.000,,,,," << std::endl
      << "1493596800,100.000,120.000,20.000,50.000,"
      << "1000.000,32.300,21.000,364.000,LIMIT,BUY,,1799.000,50.000"
      << std::endl
      << "1496275200,50.000,100.000,40.000,80.000,"
      << "1000.000,32.300,21.000,364.000,,,,," << std::endl
      << "1498867200,80.000,180.000,50.000,150.000,"
      << "1000.000,32.300,21.000,364.000,,,,," << std::endl
      << "1501545600,150.000,250.000,120.000,240.000,"
      << "1000.000,32.300,21.000,364.000,,,,," << std::endl
      << "1501545600,150.000,250.000,120.000,240.000,"
      << "1000.000,0.000,5834.000,1011.000,LIMIT,SELL,32.300,,200.000"
      << std::endl
      << "1504224000,240.000,450.000,220.000,400.000,"
      << "1000.000,0.000,5834.000,1011.000,,,,," << std::endl
      << "1506816000,400.000,450.000,250.000,300.000,"
      << "1000.000,0.000,5834.000,1011.000,,,,," << std::endl
      << "1509494400,300.000,700.000,220.000,650.000,"
      << "1000.000,0.000,5834.000,1011.000,,,,," << std::endl
      << "1512086400,650.000,800.000,600.000,750.000,"
      << "1000.000,0.000,5834.000,1011.000,,,,," << std::endl;

  expected_trader_os  // nowrap
      << "1483228800,10.000,0.000,120.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1485907200,10.000,0.000,150.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1488326400,0.000,1799.000,140.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1491004800,0.000,1799.000,100.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1493596800,32.300,21.000,50.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1496275200,32.300,21.000,80.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1498867200,32.300,21.000,150.000,IN_LONG,LIMIT_SELL@200" << std::endl
      << "1501545600,0.000,5834.000,240.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1504224000,0.000,5834.000,400.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1506816000,0.000,5834.000,300.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1509494400,0.000,5834.000,650.000,IN_CASH,LIMIT_BUY@50" << std::endl
      << "1512086400,0.000,5834.000,750.000,IN_CASH,LIMIT_BUY@50" << std::endl;

  EXPECT_EQ(exchange_os.str(), expected_exchange_os.str());
  EXPECT_EQ(trader_os.str(), expected_trader_os.str());
}

TEST(EvaluateTraderTest, LimitBuyAndSellOnePeriodFastEval) {
  AccountConfig account_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        base_unit: 0.1
        quote_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1
        )",
      &account_config));

  OhlcHistory ohlc_history;
  SetupMonthlyOhlcHistory(ohlc_history);

  EvaluationConfig eval_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_timestamp_sec: 1483228800
        end_timestamp_sec: 1514764800
        evaluation_period_months: 0
        fast_eval: true
        )",
      &eval_config));

  TestTraderEmitter trader_emitter(/*buy_price=*/50,
                                   /*sell_price=*/200);
  EXPECT_EQ(trader_emitter.GetName(), "test-trader[50|200]");

  EvaluationResult result =
      EvaluateTrader(account_config, eval_config, ohlc_history,
                     /*side_input=*/nullptr, trader_emitter,
                     /*logger=*/nullptr);

  EvaluationResult expected_result;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        account_config {
            start_base_balance: 10
            start_quote_balance: 0
            base_unit: 0.1
            quote_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 0
            fast_eval: true
        }
        name: "test-trader[50|200]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            result {
                start_base_balance: 10
                start_quote_balance: 0
                end_base_balance: 0
                end_quote_balance: 5834
                start_price: 120
                end_price: 750
                start_value: 1200
                end_value: 5834
                total_executed_orders: 3
                total_fee: 1011
            }
            final_gain: 4.86166668
            base_final_gain: 6.25
        }
        score: 0.777866662
        avg_gain: 4.86166668
        avg_base_gain: 6.25
        avg_total_executed_orders: 3
        avg_total_fee: 1011
        )",
      &expected_result));
  ExpectProtoEq(result, expected_result);
}

TEST(EvaluateTraderTest, LimitBuyAndSellMultiple6MonthPeriods) {
  AccountConfig account_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        base_unit: 0.1
        quote_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1
        )",
      &account_config));

  OhlcHistory ohlc_history;
  SetupMonthlyOhlcHistory(ohlc_history);

  EvaluationConfig eval_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_timestamp_sec: 1483228800
        end_timestamp_sec: 1514764800
        evaluation_period_months: 6
        fast_eval: false
        )",
      &eval_config));

  TestTraderEmitter trader_emitter(/*buy_price=*/50,
                                   /*sell_price=*/200);
  EXPECT_EQ(trader_emitter.GetName(), "test-trader[50|200]");

  EvaluationResult result =
      EvaluateTrader(account_config, eval_config, ohlc_history,
                     /*side_input=*/nullptr, trader_emitter,
                     /*logger=*/nullptr);

  EvaluationResult expected_result;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        account_config {
            start_base_balance: 10
            start_quote_balance: 0
            base_unit: 0.1
            quote_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 6
            fast_eval: false
        }
        name: "test-trader[50|200]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1498867200
            result {
                start_base_balance: 10
                start_quote_balance: 0
                end_base_balance: 32.3
                end_quote_balance: 21
                start_price: 120
                end_price: 80
                start_value: 1200
                end_value: 2605
                total_executed_orders: 2
                total_fee: 364
                base_volatility: 1.47175908
                trader_volatility: 1.6604414
            }
            final_gain: 2.17083335
            base_final_gain: 0.666666687
        }
        period {
            start_timestamp_sec: 1485907200
            end_timestamp_sec: 1501545600
            result {
                start_base_balance: 10
                start_quote_balance: 0
                end_base_balance: 32.3
                end_quote_balance: 21
                start_price: 150
                end_price: 150
                start_value: 1500
                end_value: 4866
                total_executed_orders: 2
                total_fee: 364
                base_volatility: 1.74792
                trader_volatility: 1.90313399
            }
            final_gain: 3.244
            base_final_gain: 1
        }
        period {
            start_timestamp_sec: 1488326400
            end_timestamp_sec: 1504224000
            result {
                start_base_balance: 10
                start_quote_balance: 0
                end_base_balance: 0
                end_quote_balance: 1799
                start_price: 140
                end_price: 240
                start_value: 1400
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
                base_volatility: 1.84354067
                trader_volatility: 1.71938074
            }
            final_gain: 1.285
            base_final_gain: 1.71428573
        }
        period {
            start_timestamp_sec: 1491004800
            end_timestamp_sec: 1506816000
            result {
                start_base_balance: 10
                start_quote_balance: 0
                end_base_balance: 0
                end_quote_balance: 1799
                start_price: 100
                end_price: 400
                start_value: 1000
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
                base_volatility: 1.99806571
                trader_volatility: 1.71596301
            }
            final_gain: 1.799
            base_final_gain: 4
        }
        period {
            start_timestamp_sec: 1493596800
            end_timestamp_sec: 1509494400
            result {
                start_base_balance: 10
                start_quote_balance: 0
                end_base_balance: 0
                end_quote_balance: 1799
                start_price: 50
                end_price: 300
                start_value: 500
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
                base_volatility: 1.97951412
                trader_volatility: 1.63458216
            }
            final_gain: 3.598
            base_final_gain: 6
        }
        period {
            start_timestamp_sec: 1496275200
            end_timestamp_sec: 1512086400
            result {
                start_base_balance: 10
                start_quote_balance: 0
                end_base_balance: 0
                end_quote_balance: 1799
                start_price: 80
                end_price: 650
                start_value: 800
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
                base_volatility: 2.0282948
                trader_volatility: 1.23012161
            }
            final_gain: 2.24875
            base_final_gain: 8.125
        }
        period {
            start_timestamp_sec: 1498867200
            end_timestamp_sec: 1514764800
            result {
                start_base_balance: 10
                start_quote_balance: 0
                end_base_balance: 0
                end_quote_balance: 1799
                start_price: 150
                end_price: 750
                start_value: 1500
                end_value: 1799
                total_executed_orders: 1
                total_fee: 201
                base_volatility: 1.91392529
                trader_volatility: 1.0023737
            }
            final_gain: 1.19933331
            base_final_gain: 5
        }
        score: 0.75648129
        avg_gain: 2.22070217
        avg_base_gain: 3.78656459
        avg_total_executed_orders: 1.28571427
        avg_total_fee: 247.571426
        )",
      &expected_result));
  ExpectProtoEq(result, expected_result);
}

TEST(EvaluateBatchOfTradersTest, LimitBuyAndSellMultiple6MonthPeriods) {
  AccountConfig account_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        base_unit: 0.1
        quote_unit: 1
        limit_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.5
        max_volume_ratio: 0.1
        )",
      &account_config));

  OhlcHistory ohlc_history;
  SetupMonthlyOhlcHistory(ohlc_history);

  EvaluationConfig eval_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_timestamp_sec: 1483228800
        end_timestamp_sec: 1514764800
        evaluation_period_months: 6
        fast_eval: false
        )",
      &eval_config));

  std::vector<std::unique_ptr<TraderEmitter>> trader_emitters;
  trader_emitters.emplace_back(new TestTraderEmitter(/*buy_price=*/50,
                                                     /*sell_price=*/200));
  trader_emitters.emplace_back(new TestTraderEmitter(/*buy_price=*/40,
                                                     /*sell_price=*/250));
  trader_emitters.emplace_back(new TestTraderEmitter(/*buy_price=*/30,
                                                     /*sell_price=*/500));

  std::vector<EvaluationResult> eval_results =
      EvaluateBatchOfTraders(account_config, eval_config, ohlc_history,
                             /*side_input=*/nullptr, trader_emitters);

  ASSERT_EQ(eval_results.size(), 3);
  EvaluationResult expected_result[3];

  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        account_config {
            start_base_balance: 10
            start_quote_balance: 0
            base_unit: 0.1
            quote_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 6
            fast_eval: false
        }
        name: "test-trader[50|200]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1498867200
            final_gain: 2.17083335
            base_final_gain: 0.666666687
        }
        period {
            start_timestamp_sec: 1485907200
            end_timestamp_sec: 1501545600
            final_gain: 3.244
            base_final_gain: 1
        }
        period {
            start_timestamp_sec: 1488326400
            end_timestamp_sec: 1504224000
            final_gain: 1.285
            base_final_gain: 1.71428573
        }
        period {
            start_timestamp_sec: 1491004800
            end_timestamp_sec: 1506816000
            final_gain: 1.799
            base_final_gain: 4
        }
        period {
            start_timestamp_sec: 1493596800
            end_timestamp_sec: 1509494400
            final_gain: 3.598
            base_final_gain: 6
        }
        period {
            start_timestamp_sec: 1496275200
            end_timestamp_sec: 1512086400
            final_gain: 2.24875
            base_final_gain: 8.125
        }
        period {
            start_timestamp_sec: 1498867200
            end_timestamp_sec: 1514764800
            final_gain: 1.19933331
            base_final_gain: 5
        }
        score: 0.75648129
        avg_gain: 2.22070217
        avg_base_gain: 3.78656459
        avg_total_executed_orders: 1.28571427
        avg_total_fee: 247.571426
        )",
      &expected_result[0]));
  ExpectProtoEq(eval_results[0], expected_result[0],
                /*full_scope=*/false);

  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        account_config {
            start_base_balance: 10
            start_quote_balance: 0
            base_unit: 0.1
            quote_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 6
            fast_eval: false
        }
        name: "test-trader[40|250]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1498867200
            final_gain: 3.38833332
            base_final_gain: 0.666666687
        }
        period {
            start_timestamp_sec: 1485907200
            end_timestamp_sec: 1501545600
            final_gain: 5.06733322
            base_final_gain: 1
        }
        period {
            start_timestamp_sec: 1488326400
            end_timestamp_sec: 1504224000
            final_gain: 1.60642862
            base_final_gain: 1.71428573
        }
        period {
            start_timestamp_sec: 1491004800
            end_timestamp_sec: 1506816000
            final_gain: 2.249
            base_final_gain: 4
        }
        period {
            start_timestamp_sec: 1493596800
            end_timestamp_sec: 1509494400
            final_gain: 4.498
            base_final_gain: 6
        }
        period {
            start_timestamp_sec: 1496275200
            end_timestamp_sec: 1512086400
            final_gain: 2.81125
            base_final_gain: 8.125
        }
        period {
            start_timestamp_sec: 1498867200
            end_timestamp_sec: 1514764800
            final_gain: 1.49933338
            base_final_gain: 5
        }
        score: 1.00773919
        avg_gain: 3.01709747
        avg_base_gain: 3.78656459
        avg_total_executed_orders: 1.28571427
        avg_total_fee: 309
        )",
      &expected_result[1]));
  ExpectProtoEq(eval_results[1], expected_result[1],
                /*full_scope=*/false);

  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        account_config {
            start_base_balance: 10
            start_quote_balance: 0
            base_unit: 0.1
            quote_unit: 1
            limit_order_fee_config {
                relative_fee: 0.1
                fixed_fee: 1
                minimum_fee: 1.5
            }
            market_liquidity: 0.5
            max_volume_ratio: 0.1
        }
        eval_config {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1514764800
            evaluation_period_months: 6
            fast_eval: false
        }
        name: "test-trader[30|500]"
        period {
            start_timestamp_sec: 1483228800
            end_timestamp_sec: 1498867200
            final_gain: 0.666666687
            base_final_gain: 0.666666687
        }
        period {
            start_timestamp_sec: 1485907200
            end_timestamp_sec: 1501545600
            final_gain: 1
            base_final_gain: 1
        }
        period {
            start_timestamp_sec: 1488326400
            end_timestamp_sec: 1504224000
            final_gain: 1.71428573
            base_final_gain: 1.71428573
        }
        period {
            start_timestamp_sec: 1491004800
            end_timestamp_sec: 1506816000
            final_gain: 4
            base_final_gain: 4
        }
        period {
            start_timestamp_sec: 1493596800
            end_timestamp_sec: 1509494400
            final_gain: 6
            base_final_gain: 6
        }
        period {
            start_timestamp_sec: 1496275200
            end_timestamp_sec: 1512086400
            final_gain: 5.62375
            base_final_gain: 8.125
        }
        period {
            start_timestamp_sec: 1498867200
            end_timestamp_sec: 1514764800
            final_gain: 2.99933338
            base_final_gain: 5
        }
        score: 0.881993413
        avg_gain: 3.14343381
        avg_base_gain: 3.78656459
        avg_total_executed_orders: 0.285714298
        avg_total_fee: 143.142853
        )",
      &expected_result[2]));
  ExpectProtoEq(eval_results[2], expected_result[2],
                /*full_scope=*/false);
}

namespace {
// Adds signals to the side_history.
void AddSignals(const std::vector<float>& signals, int64_t timestamp_sec,
                SideHistory& side_history) {
  side_history.emplace_back();
  SideInputRecord& side_input = side_history.back();
  side_input.set_timestamp_sec(timestamp_sec);
  for (const float signal : signals) {
    side_input.add_signal(signal);
  }
}

// Trader that buys and sells the base (crypto) currency based on a side input.
class TestTraderWithSideInput : public Trader {
 public:
  TestTraderWithSideInput() {}
  virtual ~TestTraderWithSideInput() {}

  void Update(const OhlcTick& ohlc_tick,
              const std::vector<float>& side_input_signals, float base_balance,
              float quote_balance, std::vector<Order>& orders) override {
    last_base_balance_ = base_balance;
    last_quote_balance_ = quote_balance;
    last_timestamp_sec_ = ohlc_tick.timestamp_sec();
    last_close_ = ohlc_tick.close();
    if (side_input_signals.empty()) {
      return;
    }
    // The first signal is a buy/sell recommendation:
    // 0: DO_NOTHING, 1: SHOULD_BUY, 2: SHOULD_SELL.
    // The second signal is the age (in seconds) of the side input.
    assert(side_input_signals.size() == 2);
    last_buy_sell_recommendation_ = static_cast<int>(side_input_signals[0]);
    last_side_input_age_sec_ = static_cast<int>(side_input_signals[1]);
    assert(last_buy_sell_recommendation_ >= 0 &&
           last_buy_sell_recommendation_ <= 2);
    assert(last_side_input_age_sec_ >= 0);
    if (last_buy_sell_recommendation_ == 1 &&
        last_side_input_age_sec_ <= kSecondsPerHour &&
        last_quote_balance_ > 0) {
      orders.emplace_back();
      Order& order = orders.back();
      order.set_type(Order_Type_MARKET);
      order.set_side(Order_Side_BUY);
      order.set_quote_amount(last_quote_balance_);
    } else if (last_buy_sell_recommendation_ == 2 &&
               last_side_input_age_sec_ <= kSecondsPerHour &&
               last_base_balance_ > 0) {
      orders.emplace_back();
      Order& order = orders.back();
      order.set_type(Order_Type_MARKET);
      order.set_side(Order_Side_SELL);
      order.set_base_amount(last_base_balance_);
    }
  }

  std::string GetInternalState() const override {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0)  // nowrap
       << last_timestamp_sec_ << ","          // nowrap
       << std::setprecision(3)                // nowrap
       << last_base_balance_ << ","           // nowrap
       << last_quote_balance_ << ","          // nowrap
       << last_close_ << ",";                 // nowrap
    if (last_buy_sell_recommendation_ == -1) {
      ss << "UNKNOWN,";
    } else if (last_side_input_age_sec_ > kSecondsPerHour) {
      ss << "TOO_OLD,";
    } else {
      switch (last_buy_sell_recommendation_) {
        case 0:
          ss << "DO_NOTHING,";
          break;
        case 1:
          ss << "SHOULD_BUY,";
          break;
        case 2:
          ss << "SHOULD_SELL,";
          break;
        default:
          assert(false);
      }
    }
    ss << last_side_input_age_sec_;
    return ss.str();
  }

 private:
  // Last seen trader account balance.
  float last_base_balance_ = 0.0f;
  float last_quote_balance_ = 0.0f;
  // Last seen UNIX timestamp (in seconds).
  int64_t last_timestamp_sec_ = 0;
  // Last seen close price.
  float last_close_ = 0.0f;
  // Last seen side input signal.
  int last_buy_sell_recommendation_ = -1;
  // Last seen side input age (in seconds).
  int last_side_input_age_sec_ = -1;
};

// Emitter that emits TestTrader as defined above.
class TestTraderWithSideInputEmitter : public TraderEmitter {
 public:
  TestTraderWithSideInputEmitter() {}
  virtual ~TestTraderWithSideInputEmitter() {}

  std::string GetName() const override { return "test-trader-with-side-input"; }

  std::unique_ptr<Trader> NewTrader() const override {
    // Note: std::make_unique is C++14 feature.
    return std::unique_ptr<TestTraderWithSideInput>(
        new TestTraderWithSideInput());
  }
};
}  // namespace

TEST(ExecuteTraderWithSideInputTest, MarketBuyAndSell) {
  AccountConfig account_config;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        base_unit: 0.1
        quote_unit: 1
        market_order_fee_config {
            relative_fee: 0.1
            fixed_fee: 1
            minimum_fee: 1.5
        }
        market_liquidity: 0.8
        max_volume_ratio: 0.5)",
      &account_config));

  OhlcHistory ohlc_history;
  AddDailyOhlcTick(100, 150, 80, 120, ohlc_history);   // 2017-01-01
  AddDailyOhlcTick(120, 180, 100, 150, ohlc_history);  // 2017-01-02
  AddDailyOhlcTick(150, 250, 100, 140, ohlc_history);  // 2017-01-03
  AddDailyOhlcTick(140, 150, 80, 100, ohlc_history);   // 2017-01-04
  AddDailyOhlcTick(100, 120, 20, 50, ohlc_history);    // 2017-01-05

  SideHistory side_history;
  AddSignals(/*signals=*/{2},
             /*timestamp_sec=*/1483315200 - kSecondsPerHour,
             side_history);  // SHOULD_SELL on 2017-01-02 -1h
  AddSignals(/*signals=*/{1}, /*timestamp_sec=*/1483488000,
             side_history);  // SHOULD_BUY on 2017-01-04
  AddSignals(/*signals=*/{0},
             /*timestamp_sec=*/1483574400,
             side_history);  // DO_NOTHING on 2017-01-05
  SideInput side_input(side_history);

  TestTraderWithSideInput trader;

  std::stringstream exchange_os;
  std::stringstream trader_os;
  CsvLogger logger(&exchange_os, &trader_os);

  ExecutionResult result = ExecuteTrader(account_config, ohlc_history.begin(),
                                         ohlc_history.end(), &side_input,
                                         /*fast_eval=*/false, trader, &logger);

  ExecutionResult expected_result;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"(
        start_base_balance: 10
        start_quote_balance: 0
        end_base_balance: 10.8
        end_quote_balance: 21
        start_price: 120
        end_price: 50
        start_value: 1200
        end_value: 561
        total_executed_orders: 2
        total_fee: 255
        base_volatility: 6.53697348
        trader_volatility: 7.15005541
        )",
      &expected_result));
  ExpectProtoEq(result, expected_result);

  std::stringstream expected_exchange_os;
  std::stringstream expected_trader_os;

  expected_exchange_os  // nowrap
      << "1483228800,100.000,150.000,80.000,120.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1483315200,120.000,180.000,100.000,150.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1483401600,150.000,250.000,100.000,140.000,"
      << "1000.000,10.000,0.000,0.000,,,,," << std::endl
      << "1483401600,150.000,250.000,100.000,140.000,"
      << "1000.000,0.000,1259.000,141.000,MARKET,SELL,10.000,," << std::endl
      << "1483488000,140.000,150.000,80.000,100.000,"
      << "1000.000,0.000,1259.000,141.000,,,,," << std::endl
      << "1483574400,100.000,120.000,20.000,50.000,"
      << "1000.000,0.000,1259.000,141.000,,,,," << std::endl
      << "1483574400,100.000,120.000,20.000,50.000,"
      << "1000.000,10.800,21.000,255.000,MARKET,BUY,,1259.000," << std::endl;

  expected_trader_os  // nowrap
      << "1483228800,10.000,0.000,120.000,UNKNOWN,-1" << std::endl
      << "1483315200,10.000,0.000,150.000,SHOULD_SELL,3600" << std::endl
      << "1483401600,0.000,1259.000,140.000,TOO_OLD,90000" << std::endl
      << "1483488000,0.000,1259.000,100.000,SHOULD_BUY,0" << std::endl
      << "1483574400,10.800,21.000,50.000,DO_NOTHING,0" << std::endl;

  EXPECT_EQ(exchange_os.str(), expected_exchange_os.str());
  EXPECT_EQ(trader_os.str(), expected_trader_os.str());
}

}  // namespace trader
