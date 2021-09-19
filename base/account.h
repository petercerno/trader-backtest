// Copyright © 2021 Peter Cerno. All rights reserved.

#ifndef BASE_ACCOUNT_H
#define BASE_ACCOUNT_H

#include "base/base.h"

namespace trader {

// Keeps track of balances and implements methods for all exchange orders.
struct Account {
  // Base (crypto) currency balance (e.g. BTC balance when trading BTC/YYY).
  float base_balance = 0;
  // Quote currency balance (e.g. USD balance when trading XXX/USD).
  float quote_balance = 0;
  // Total accumulated transaction fee (in quote currency) over all executed
  // exchange orders. Transaction fee is based on the provided fee_config and
  // the total quote amount exchanged in the transaction, and is subtracted
  // from the quote currency balance (e.g. USD balance when trading XXX/USD).
  float total_fee = 0;

  // Smallest indivisible unit for base (crypto) currency balance.
  // Not used if zero.
  float base_unit = 0;
  // Smallest indivisible unit for quote currency balance.
  // Not used if zero.
  float quote_unit = 0;

  // Liquidity for executing market (stop) orders w.r.t. the given OHLC tick
  // from the interval [0; 1].
  // If 1.0 then the market (stop) order will be executed at the opening price
  // (stop order price). This is the best price for the given order.
  // If 0.0 then the buy (sell) order will be executed at the highest (lowest)
  // price of the given OHLC tick. This is the worst price for the given order.
  // Anything in between 0.0 and 1.0 will be linearly interpolated.
  float market_liquidity = 1.0f;
  // Fraction of the OHLC tick volume that will be used to fill the limit order.
  // If the actual traded volume * max_volume_ratio is less than the limit
  // order amount, then the limit order will be filled only partially.
  // Not used if zero.
  float max_volume_ratio = 0.0f;

  // Initializes the account based on the account_config.
  void InitAccount(const AccountConfig& account_config);

  // Returns the fee (in quote currency) based on the provided fee_config and
  // the given quote currency amount involved in the transaction.
  float GetFee(const FeeConfig& fee_config, float quote_amount) const;

  // Returns the price of the market buy order based on the market_liquidity
  // (defined above) when executed over the given OHLC tick.
  float GetMarketBuyPrice(const OhlcTick& ohlc_tick) const;
  // Returns the price of the market sell order based on the market_liquidity
  // (defined above) when executed over the given OHLC tick.
  float GetMarketSellPrice(const OhlcTick& ohlc_tick) const;
  // Returns the price of the stop buy order based on the market_liquidity
  // (defined above) and the stop order price when executed over the OHLC tick.
  float GetStopBuyPrice(const OhlcTick& ohlc_tick, float price) const;
  // Returns the price of the stop sell order based on the market_liquidity
  // (defined above) and the stop order price when executed over the OHLC tick.
  float GetStopSellPrice(const OhlcTick& ohlc_tick, float price) const;
  // Returns the maximum tradeable base (crypto) currency amount based on
  // the max_volume_ratio (defined above) and the given OHLC tick.
  float GetMaxBaseAmount(const OhlcTick& ohlc_tick) const;

  // ORDERS AT SPECIFIC PRICE

  // Buys the specified amount of base (crypto) currency at the given price.
  // Returns true iff the order was executed successfully.
  bool BuyBase(const FeeConfig& fee_config, float base_amount, float price);
  // Buys as much base (crypto) currency as possible at the given price,
  // spending at most quote_amount in quote currency.
  // It is possible to buy at most max_base_amount base (crypto) currency.
  // Returns true iff the order was executed successfully.
  bool BuyAtQuote(const FeeConfig& fee_config, float quote_amount, float price,
                  float max_base_amount = std::numeric_limits<float>::max());
  // Sells the specified amount of base (crypto) currency at the given price.
  // Returns true iff the order was executed successfully.
  bool SellBase(const FeeConfig& fee_config, float security_amount,
                float price);
  // Sells as much base (crypto) currency as possible at the given price,
  // receiving at most quote_amount in quote currency.
  // It is possible to sell at most max_base_amount base (crypto) currency.
  // Returns true iff the order was executed successfully.
  bool SellAtQuote(const FeeConfig& fee_config, float quote_amount, float price,
                   float max_base_amount = std::numeric_limits<float>::max());

  // MARKET ORDERS

  // Executes market buy order for the specified amount of base (crypto)
  // currency. Returns true iff the order was executed successfully.
  bool MarketBuy(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                 float base_amount);
  // Executes market buy order spending at most quote_amount in quote currency.
  // Returns true iff the order was executed successfully.
  bool MarketBuyAtQuote(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                        float quote_amount);
  // Executes market sell order for the specified amount of base (crypto)
  // currency. Returns true iff the order was executed successfully.
  bool MarketSell(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                  float base_amount);
  // Executes market sell order receiving at most quote_amount in quote
  // currency. Returns true iff the order was executed successfully.
  bool MarketSellAtQuote(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                         float quote_amount);

  // STOP ORDERS

  // Executes stop buy order for the specified amount of base (crypto) currency
  // and at the specified stop order price.
  // Stop buy order can be executed only if the price jumps above the stop
  // order price, i.e. when the given OHLC tick's high price is greater than or
  // equal to the stop order price.
  // Returns true iff the order was executed successfully.
  bool StopBuy(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
               float base_amount, float stop_price);
  // Executes stop buy order for the specified amount of quote currency and at
  // the specified stop order price.
  // Stop buy order can be executed only if the price jumps above the stop
  // order price, i.e. when the given OHLC tick's high price is greater than or
  // equal to the stop order price.
  // Returns true iff the order was executed successfully.
  bool StopBuyAtQuote(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                      float quote_amount, float stop_price);
  // Executes stop sell order for the specified amount of base (crypto) currency
  // and at the specified stop order price.
  // Stop sell order can be executed only if the price drops below the stop
  // order price, i.e. when the given OHLC tick's low price is lower than or
  // equal to the stop order price.
  // Returns true iff the order was executed successfully.
  bool StopSell(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                float base_amount, float stop_price);
  // Executes stop sell order for the specified amount of quote currency
  // and at the specified stop order price.
  // Stop sell order can be executed only if the price drops below the stop
  // order price, i.e. when the given OHLC tick's low price is lower than or
  // equal to the stop order price.
  // Returns true iff the order was executed successfully.
  bool StopSellAtQuote(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                       float quote_amount, float stop_price);

  // LIMIT ORDERS

  // Executes limit buy order for the specified amount of base (crypto) currency
  // and at the specified limit order price.
  // Limit buy order can be executed only if the price drops below the limit
  // order price, i.e. when the given OHLC tick's low price is lower than or
  // equal to the limit order price.
  // Returns true iff the order was executed successfully.
  bool LimitBuy(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                float base_amount, float limit_price);
  // Executes limit buy order for the specified amount of quote currency and at
  // the specified limit order price.
  // Limit buy order can be executed only if the price drops below the limit
  // order price, i.e. when the given OHLC tick's low price is lower than or
  // equal to the limit order price.
  // Returns true iff the order was executed successfully.
  bool LimitBuyAtQuote(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                       float quote_amount, float limit_price);
  // Executes limit sell order for the specified amount of base (crypto)
  // currency and at the specified limit order price.
  // Limit sell order can be executed only if the price jumps above the limit
  // order price, i.e. when the given OHLC tick's high price is greater than or
  // equal to the limit order price.
  // Returns true iff the order was executed successfully.
  bool LimitSell(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                 float base_amount, float limit_price);
  // Executes limit sell order for the specified amount of quote currency and
  // at the specified limit order price.
  // Limit sell order can be executed only if the price jumps above the limit
  // order price, i.e. when the given OHLC tick's high price is greater than or
  // equal to the limit order price.
  // Returns true iff the order was executed successfully.
  bool LimitSellAtQuote(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                        float quote_amount, float limit_price);

  // GENERAL ORDER EXECUTION

  // Executes the order over the given ohlc_tick.
  // Returns true iff the order was executed successfully.
  bool ExecuteOrder(const AccountConfig& account_config, const Order& order,
                    const OhlcTick& ohlc_tick);
};

}  // namespace trader

#endif  // BASE_ACCOUNT_H
