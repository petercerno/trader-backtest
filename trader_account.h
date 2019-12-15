// Copyright © 2019 Peter Cerno. All rights reserved.

#ifndef TRADER_ACCOUNT_H
#define TRADER_ACCOUNT_H

#include <limits>

#include "trader_base.h"

namespace trader {

// Keeps track of balances and implements methods for all exchange orders.
struct TraderAccount {
  // Security (crypto currency) balance (e.g. BTC balance).
  float security_balance = 0;
  // Cash balance in base currency (e.g. USD balance).
  float cash_balance = 0;
  // Total accumulated transaction fee (in base currency) over all executed
  // exchange orders.
  float total_fee = 0;

  // Smallest indivisible unit for security (crypto currency) balance.
  // Not used if zero.
  float security_unit = 0;
  // Smallest indivisible unit for cash balance. Not used if zero.
  float cash_unit = 0;

  // Liquidity for executing market (stop) orders w.r.t. the given OhlcTick
  // from the interval [0; 1].
  // If 1.0 then the market (stop) order will be executed at the opening price
  // (stop order price). This is the best price for the given order.
  // If 0.0 then the buy (sell) order will be executed at the highest (lowest)
  // price of the given OhlcTick. This is the worst price for the given order.
  // Anything in between 0.0 and 1.0 will be linearly interpolated.
  float market_liquidity = 1.0f;
  // Fraction of the OhlcTick volume that will be used to fill the limit order.
  // If the actual traded volume * max_volume_ratio is less than the limit
  // order size, then the limit order will be filled only partially.
  // Not used if zero.
  float max_volume_ratio = 0.0f;

  // Returns the fee (in base currency) based on the provided fee_config and
  // the given cash amount (in base currency) involved in the transaction.
  float GetFee(const FeeConfig& fee_config, float cash_amount) const;

  // Returns the price of the market buy order based on the market_liquidity
  // (defined above) and the given (follow up) ohlc_tick.
  float GetMarketBuyPrice(const OhlcTick& ohlc_tick) const;
  // Returns the price of the market sell order based on the market_liquidity
  // (defined above) and the given (follow up) ohlc_tick.
  float GetMarketSellPrice(const OhlcTick& ohlc_tick) const;
  // Returns the price of the stop buy order based on the market_liquidity
  // (defined above), the given (follow up) ohlc_tick, and the stop order price.
  float GetStopBuyPrice(const OhlcTick& ohlc_tick, float price) const;
  // Returns the price of the stop sell order based on the market_liquidity
  // (defined above), the given (follow up) OhlcTick, and the stop order price.
  float GetStopSellPrice(const OhlcTick& ohlc_tick, float price) const;
  // Returns the maximum tradeable security (crypto currency) amount based on
  // the max_volume_ratio (defined above) and the given (follow up) ohlc_tick.
  float GetMaxSecurityAmount(const OhlcTick& ohlc_tick) const;

  // ORDERS AT SPECIFIED PRICE

  // Buys the specified amount of security (crypto currency) at the given price.
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Returns true iff the order was executed successfully.
  bool Buy(const FeeConfig& fee_config, float price, float security_amount);
  // Buys as much security (crypto currency) as possible at the given price,
  // spending at most cash_amount cash (in base currency), and buying at most
  // max_security_amount security (crypto currency).
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Returns true iff the order was executed successfully.
  bool BuyAtCash(const FeeConfig& fee_config, float price, float cash_amount,
                 float max_security_amount = std::numeric_limits<float>::max());
  // Sells the specified amount of security (crypto currency) at the given
  // price. Transaction fee is estimated based on the provided fee_config and
  // the total cash amount (in base currency) involved in the transaction.
  // Returns true iff the order was executed successfully.
  bool Sell(const FeeConfig& fee_config, float price, float security_amount);
  // Sells as much security (crypto currency) as possible at the given price,
  // receiving at most cash_amount cash (in base currency), and selling at most
  // max_security_amount security (crypto currency).
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Returns true iff the order was executed successfully.
  bool SellAtCash(
      const FeeConfig& fee_config, float price, float cash_amount,
      float max_security_amount = std::numeric_limits<float>::max());

  // MARKET ORDERS

  // Executes market buy order for the specified amount of security (crypto
  // currency). Transaction fee is estimated based on the provided fee_config
  // and the total cash amount (in base currency) involved in the transaction.
  // Returns true iff the order was executed successfully.
  bool MarketBuy(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                 float security_amount);
  // Executes market buy order spending at most cash_amount cash (in base
  // currency). Transaction fee is estimated based on the provided fee_config
  // and the total cash amount (in base currency) involved in the transaction.
  // Returns true iff the order was executed successfully.
  bool MarketBuyAtCash(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                       float cash_amount);
  // Executes market sell order for the specified amount of security (crypto
  // currency). Transaction fee is estimated based on the provided fee_config
  // and the total cash amount (in base currency) involved in the transaction.
  // Returns true iff the order was executed successfully.
  bool MarketSell(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                  float security_amount);
  // Executes market sell order receiving at most cash_amount cash (in base
  // currency). Transaction fee is estimated based on the provided fee_config
  // and the total cash amount (in base currency) involved in the transaction.
  // Returns true iff the order was executed successfully.
  bool MarketSellAtCash(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                        float cash_amount);

  // STOP ORDERS

  // Executes stop buy order for the specified amount of security (crypto
  // currency) and at the specified stop order price.
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Stop buy order can be executed only if the price jumps above the stop
  // order price, i.e. when the given (follow-up) ohlc_tick's high price is
  // higher than or equal to the stop order price.
  // Returns true iff the order was executed successfully.
  bool StopBuy(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
               float stop_price, float security_amount);
  // Executes stop buy order for the specified amount of cash (in base
  // currency) and at the specified stop order price.
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Stop buy order can be executed only if the price jumps above the stop
  // order price, i.e. when the given (follow-up) ohlc_tick's high price is
  // higher than or equal to the stop order price.
  // Returns true iff the order was executed successfully.
  bool StopBuyAtCash(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                     float stop_price, float cash_amount);
  // Executes stop sell order for the specified amount of security (crypto
  // currency) and at the specified stop order price.
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Stop sell order can be executed only if the price drops below the stop
  // order price, i.e. when the given (follow-up) ohlc_tick's low price is
  // lower than or equal to the stop order price.
  // Returns true iff the order was executed successfully.
  bool StopSell(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                float stop_price, float security_amount);
  // Executes stop sell order for the specified amount of cash (in base
  // currency) and at the specified stop order price.
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Stop sell order can be executed only if the price drops below the stop
  // order price, i.e. when the given (follow-up) ohlc_tick's low price is
  // lower than or equal to the stop order price.
  // Returns true iff the order was executed successfully.
  bool StopSellAtCash(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                      float stop_price, float cash_amount);

  // LIMIT ORDERS

  // Executes limit buy order for the specified amount of security (crypto
  // currency) and at the specified limit order price.
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Limit buy order can be executed only if the price drops below the limit
  // order price, i.e. when the given (follow-up) ohlc_tick's low price is
  // lower than or equal to the limit order price.
  // Returns true iff the order was executed successfully.
  bool LimitBuy(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                float limit_price, float security_amount);
  // Executes limit buy order for the specified amount of cash (in base
  // currency) and at the specified limit order price.
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Limit buy order can be executed only if the price drops below the limit
  // order price, i.e. when the given (follow-up) ohlc_tick's low price is
  // lower than or equal to the limit order price.
  // Returns true iff the order was executed successfully.
  bool LimitBuyAtCash(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                      float limit_price, float cash_amount);
  // Executes limit sell order for the specified amount of security (crypto
  // currency) and at the specified limit order price.
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Limit sell order can be executed only if the price jumps above the limit
  // order price, i.e. when the given (follow-up) ohlc_tick's high price is
  // higher than or equal to the limit order price.
  // Returns true iff the order was executed successfully.
  bool LimitSell(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                 float limit_price, float security_amount);
  // Executes limit sell order for the specified amount of cash (in base
  // currency) and at the specified limit order price.
  // Transaction fee is estimated based on the provided fee_config and the total
  // cash amount (in base currency) involved in the transaction.
  // Limit sell order can be executed only if the price jumps above the limit
  // order price, i.e. when the given (follow-up) ohlc_tick's high price is
  // higher than or equal to the limit order price.
  // Returns true iff the order was executed successfully.
  bool LimitSellAtCash(const FeeConfig& fee_config, const OhlcTick& ohlc_tick,
                       float limit_price, float cash_amount);
};

}  // namespace trader

#endif  // TRADER_ACCOUNT_H
