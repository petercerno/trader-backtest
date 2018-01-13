// Copyright © 2017 Peter Cerno. All rights reserved.

#include "trader_io.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace trader {
namespace {
// Converts the type of the order to string.
std::string OrderTypeToStr(const Order& order) {
  if (order.has_type()) {
    switch (order.type()) {
      case Order::MARKET:
        return "MARKET";
      case Order::LIMIT:
        return "LIMIT";
      case Order::STOP:
        return "STOP";
      default:
        assert(false);  // Invalid order type.
    }
  }
  return "NaN";
}

// Converts the price of the order to string.
std::string OrderPriceToStr(const Order& order) {
  if (order.has_price()) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << order.price();
    return ss.str();
  }
  return "NaN";
}
}  // namespace

bool ReadPriceHistoryFromCsvFile(const std::string& file_name,
                                 PriceHistory* price_history) {
  if (price_history == nullptr) {
    std::cerr << "Undefined price_history" << std::endl;
    return false;
  }
  std::ifstream infile(file_name);
  if (!infile.is_open()) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  int row = 0;
  std::string line;
  int timestamp_sec;
  float price;
  float volume;
  while (std::getline(infile, line)) {
    ++row;
    std::replace(line.begin(), line.end(), ',', ' ');
    std::istringstream iss(line);
    if (!(iss >> timestamp_sec >> price >> volume)) {
      std::cerr << "Cannot parse line " << row << ": " << line << std::endl;
      return false;
    }
    price_history->emplace_back();
    price_history->back().set_timestamp_sec(timestamp_sec);
    price_history->back().set_price(price);
    price_history->back().set_volume(volume);
  }
  return true;
}

bool ReadOhlcHistoryFromCsvFile(const std::string& file_name,
                                OhlcHistory* ohlc_history) {
  if (ohlc_history == nullptr) {
    std::cerr << "Undefined ohlc_history" << std::endl;
    return false;
  }
  std::ifstream infile(file_name);
  if (!infile.is_open()) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  int row = 0;
  std::string line;
  int timestamp_sec;
  float open;
  float high;
  float low;
  float close;
  float volume;
  while (std::getline(infile, line)) {
    ++row;
    std::replace(line.begin(), line.end(), ',', ' ');
    std::istringstream iss(line);
    if (!(iss >> timestamp_sec >> open >> high >> low >> close >> volume)) {
      std::cerr << "Cannot parse line " << row << ": " << line << std::endl;
      return false;
    }
    ohlc_history->emplace_back();
    ohlc_history->back().set_timestamp_sec(timestamp_sec);
    ohlc_history->back().set_open(open);
    ohlc_history->back().set_high(high);
    ohlc_history->back().set_low(low);
    ohlc_history->back().set_close(close);
    ohlc_history->back().set_volume(volume);
  }
  return true;
}

bool WriteExchangeAccountStatesToCsvFile(
    const ExchangeAccountStates& exchange_account_states,
    const std::string& file_name) {
  static const char kSeparator = ',';
  std::ofstream outfile;
  outfile.open(file_name, std::ios::out | std::ios::trunc);
  if (!outfile.is_open()) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  outfile << "timestamp_sec" << kSeparator     // nowrap
          << "open" << kSeparator              // nowrap
          << "high" << kSeparator              // nowrap
          << "low" << kSeparator               // nowrap
          << "close" << kSeparator             // nowrap
          << "buy_order_type" << kSeparator    // nowrap
          << "buy_order_price" << kSeparator   // nowrap
          << "sell_order_type" << kSeparator   // nowrap
          << "sell_order_price" << kSeparator  // nowrap
          << "transaction_fee" << kSeparator   // nowrap
          << "accumulated_fee" << kSeparator   // nowrap
          << "security_balance" << kSeparator  // nowrap
          << "cash_balance" << kSeparator      // nowrap
          << "value" << std::endl;
  for (const ExchangeAccountState& state : exchange_account_states) {
    Order buy_order;
    Order sell_order;
    for (const Order& order : state.order()) {
      if (order.side() == Order_Side_BUY) {
        assert(!buy_order.has_side());  // Multiple buy orders not supported.
        buy_order = order;
      } else {
        assert(order.side() == Order_Side_SELL);  // Invalid order side.
        assert(!sell_order.has_side());  // Multiple sell orders not supported.
        sell_order = order;
      }
    }
    outfile << state.ohlc_tick().timestamp_sec() << kSeparator  // nowrap
            << state.ohlc_tick().open() << kSeparator           // nowrap
            << state.ohlc_tick().high() << kSeparator           // nowrap
            << state.ohlc_tick().low() << kSeparator            // nowrap
            << state.ohlc_tick().close() << kSeparator          // nowrap
            << OrderTypeToStr(buy_order) << kSeparator          // nowrap
            << OrderPriceToStr(buy_order) << kSeparator         // nowrap
            << OrderTypeToStr(sell_order) << kSeparator         // nowrap
            << OrderPriceToStr(sell_order) << kSeparator        // nowrap
            << state.transaction_fee() << kSeparator            // nowrap
            << state.accumulated_fee() << kSeparator            // nowrap
            << state.security_balance() << kSeparator           // nowrap
            << state.cash_balance() << kSeparator               // nowrap
            << state.value() << std::endl;
  }
  return true;
}

void PrintExchangeAccountStatesStats(
    const ExchangeAccountStates& exchange_account_states) {
  std::cout << exchange_account_states.begin()->value() << " -> "
            << exchange_account_states.rbegin()->value() << " + "
            << exchange_account_states.rbegin()->accumulated_fee() << " fees"
            << std::endl;
  std::cout << exchange_account_states.begin()->value() << " -> "
            << exchange_account_states.begin()->value() *
                   exchange_account_states.rbegin()->ohlc_tick().close() /
                   exchange_account_states.begin()->ohlc_tick().close()
            << " base" << std::endl;
}

void PrintEvalResults(const EvalResults& eval_results, size_t top_n) {
  for (int eval_index = 0; eval_index < std::min(top_n, eval_results.size());
       ++eval_index) {
    const EvalResult& eval_result = eval_results.at(eval_index);
    std::cout << eval_result.trader() << ": " << eval_result.score()
              << std::endl;
  }
}

}  // namespace trader
