# Trader Backtest

High-performance backtesting engine written in C++ for evaluating trading strategies (restricted to a single trading pair, e.g. BTC/USD) and finding their optimal hyper-parameters. Arbitrage and margin trading are not supported. Licensed under [MIT](http://opensource.org/licenses/MIT)

THIS SOFTWARE IS FOR RESEARCH PURPOSES ONLY.

**DISCLAIMER**: WE DO NOT PROVIDE ANY FINANCIAL, INVESTMENT, LEGAL, TAX OR ANY OTHER PROFESSIONAL ADVICE. WE ARE NOT A BROKER, FINANCIAL ADVISOR, INVESTMENT ADVISOR, PORTFOLIO MANAGER OR TAX ADVISOR. YOU ACKNOWLEDGE AND AGREE THAT ONLY YOU ARE RESPONSIBLE FOR YOUR USE OF ANY INFORMATION THAT YOU OBTAIN FROM THIS REPOSITORY OR SOFTWARE. YOUR DECISIONS MADE IN RELIANCE ON THE SOFTWARE OR YOUR INTERPRETATIONS OF THE DATA ARE YOUR OWN FOR WHICH YOU HAVE FULL RESPONSIBILITY. YOU EXPRESSLY AGREE THAT YOUR USE OF THE SOFTWARE IS AT YOUR SOLE RISK.

This software might be useful for people who want both high performance and also full control over all low-level operations when backtesting their trading strategies. (If you are instead looking for an easy-to-use trading systems with some pre-defined strategies then take a look at this [video](https://www.youtube.com/watch?v=9Hv0BQwYlPw) from [Coin Bureau](https://www.youtube.com/channel/UCqK_GSMbpiV8spgD3ZGloSw).)

This software has the following dependencies:

 * **Dependencies**: [Protocol Buffers](https://github.com/protocolbuffers/protobuf), [GFlags](https://github.com/gflags/gflags), [Google Test](https://github.com/google/googletest), [Bazel](https://bazel.build/).
 * **Data Analysis Tools**: [Python3](https://www.python.org/), [Jupyter](https://jupyter.org/), [matplotlib](https://matplotlib.org/), [NumPy](http://www.numpy.org/), [Pandas](http://pandas.pydata.org/).

## Installation

First, install [Bazel](https://bazel.build/). Build everything (inside the project source code directory) by running:

``` 
bazel build ... 
```

Test everything by running:

``` 
bazel test ... 
```

If you have [Protocol Buffers](https://developers.google.com/protocol-buffers) installed, you can (optionally) compile the `.proto` files as follows:

```
protoc -I lib/ --cpp_out=lib/ trader.proto
protoc -I traders/ --cpp_out=traders/ trader_config.proto
```

## Project Structure

There are two main binaries you can run:

* `convert.cc` : Converts a CSV file into a more compact delimited protocol buffer file. Allows re-sampling price history into OHLC (Open High Low Close) history with fixed sampling rate.
* `trader.cc` : Loads OHLC history (in delimited protocol buffer file format) and evaluates one or more traders over it.

The source code is structured as follows:

* `lib/` : Core trader data structures, interfaces, trader execution and evaluation logic.
* `traders/` : Concrete trader implementations.
* `util/` : General utilities (not necessarily related to trading).

## Trader Execution

The trader is executed as follows:

* At every step the trader receives the latest OHLC tick `T[i]`, current account balances, and updates its internal state. The current time is at the end of the OHLC tick `T[i]` time period. (The trader is not updated on zero volume OHLC ticks. These OHLC ticks indicate a gap in a price history, which could have been caused by an unresponsive exchange or its API.)
* Then the trader needs to decide what orders to emit. There are no other active orders on the exchange at this moment (see the explanation below).
* Once the trader decides what orders to emit, the exchange will execute (or cancel) all these orders on the follow-up OHLC tick `T[i+1]` . The trader does not see the follow-up OHLC tick `T[i+1]` , so there is no peeking into the future.
* Once all orders are executed (or canceled) by the exchange, the trader receives the follow-up OHLC tick `T[i+1]` and the whole process repeats.

Note that at every step every order gets either executed or canceled by the exchange. This is a design simplification so that there are no active orders that the trader needs to maintain over time. In practice, however, we would not cancel orders if they would be re-emitted again. We would simply modify the existing orders (from the previous iteration) based on the updated state.

Also note that the OHLC history sampling rate defines the frequency at which the trader is updated and emits orders. Traders should be designed in a frequency-agnostic way. In other words, they should have similar behavior and performance regardless of how frequently they are called. Traders should not assume anything about how often and when exactly they are called. One reason for that is that the exchanges (or their APIs) sometimes become unresponsive for random periods of time (and we see that e.g. in the gaps in the historical price histories). Therefore, we encourage to test the traders on OHLC histories with various sampling rates and gaps.

## Trader Evaluation

When evaluating the trader performance, we compare it to the following benchmark:

* **Buy and hold**: Invest everything in the security (crypto-currency) and hold.

The *buy and hold* strategy is surprisingly hard to beat.

There are several options how to evaluate a trader:

* Evaluate a trader over a single time period. This is useful as we can log the exchange state and or trader's internal state into a CSV file and later analyze it e.g. using [JupyterLab](http://jupyter.org/index.html).
* Evaluate a trader over multiple time periods within a longer time period. This is useful as we can see the trader's performance across many different time periods. Here, however, we do not log anything.
* Evaluate a batch of traders over a single or multiple time periods (in parallel). This is useful to find optimal trader hyper-parameters.

## Example

Download some Bitcoin (BTC/USD) historical prices from [bitcoincharts](http://bitcoincharts.com/):

``` 
mkdir -p data
curl -o $(pwd)/data/bitstampUSD.csv.gz \
  "https://api.bitcoincharts.com/v1/csv/bitstampUSD.csv.gz"
gunzip $(pwd)/data/bitstampUSD.csv.gz
```

Convert the `bitstampUSD.csv` file into a more compact (delimited protocol buffer) representation:

``` 
bazel run :convert -- \
  --input_price_history_csv_file="/$(pwd)/data/bitstampUSD.csv" \
  --output_price_history_delimited_proto_file="/$(pwd)/data/bitstampUSD_2017_2019.dpb" \
  --start_date_utc="2017-01-01" \
  --end_date_utc="2020-01-01"
```

The `start_date_utc` and `end_date_utc` dates are optional. We use them to make the output smaller.

For trader evaluation we will need an OHLC history. It is possible to re-sample the original CSV file, but the delimited protocol buffer file will be much faster to read:

```
bazel run :convert -- \
  --input_price_history_delimited_proto_file="/$(pwd)/data/bitstampUSD_2017_2019.dpb" \
  --output_ohlc_history_delimited_proto_file="/$(pwd)/data/bitstampUSD_2017_2019_5min_ohlc.dpb" \
  --start_date_utc="2017-01-01" \
  --end_date_utc="2020-01-01" \
  --sampling_rate_sec=300
```

Now we can evaluate a simple *stop trader* over a single time period and log its internal state:

``` 
bazel run :trader -- \
  --input_ohlc_history_delimited_proto_file="/$(pwd)/data/bitstampUSD_2017_2019_5min_ohlc.dpb" \
  --trader="stop" \
  --output_exchange_log_file="/tmp/bitstampUSD.out.csv" \
  --output_trader_log_file="/tmp/stop_trader_log.csv" \
  --start_date_utc="2017-01-01" \
  --end_date_utc="2020-01-01" \
  --start_security_balance=1.0 \
  --start_cash_balance=0.0
```

The output:

```
Loaded 315360 records in 0.475 seconds
Selected 315360 OHLC ticks within the period: [2017-01-01 00:00:00 - 2020-01-01 00:00:00)

Trader evaluation:
[2017-01-01 00:00:00 - 2020-01-01 00:00:00): 0.424425
```

The evaluation output is the ratio of the trader's performance and the performance of the *buy and hold* strategy. As you can see, the trader significantly under-performs the *buy and hold* strategy.

It is more useful to evaluate the trader over multiple time periods:

```
bazel run :trader -- \
  --input_ohlc_history_delimited_proto_file="/$(pwd)/data/bitstampUSD_2017_2019_5min_ohlc.dpb" \
  --trader="stop" \
  --start_date_utc="2017-01-01" \
  --end_date_utc="2020-01-01" \
  --evaluation_period_months=6 \
  --start_security_balance=1.0 \
  --start_cash_balance=0.0
```

The output:

```
Loaded 315360 records in 0.493 seconds
Selected 315360 OHLC ticks within the period: [2017-01-01 00:00:00 - 2020-01-01 00:00:00)

Trader evaluation:
[2017-01-01 00:00:00 - 2017-07-01 00:00:00): 0.371817
[2017-02-01 00:00:00 - 2017-08-01 00:00:00): 0.368565
[2017-03-01 00:00:00 - 2017-09-01 00:00:00): 0.348312
[2017-04-01 00:00:00 - 2017-10-01 00:00:00): 0.492166
[2017-05-01 00:00:00 - 2017-11-01 00:00:00): 0.401846
[2017-06-01 00:00:00 - 2017-12-01 00:00:00): 0.762503
[2017-07-01 00:00:00 - 2018-01-01 00:00:00): 0.608762
[2017-08-01 00:00:00 - 2018-02-01 00:00:00): 0.826681
[2017-09-01 00:00:00 - 2018-03-01 00:00:00): 1.03835
[2017-10-01 00:00:00 - 2018-04-01 00:00:00): 1.28888
[2017-11-01 00:00:00 - 2018-05-01 00:00:00): 1.31269
[2017-12-01 00:00:00 - 2018-06-01 00:00:00): 1.50861
[2018-01-01 00:00:00 - 2018-07-01 00:00:00): 2.02192
[2018-02-01 00:00:00 - 2018-08-01 00:00:00): 1.16942
[2018-03-01 00:00:00 - 2018-09-01 00:00:00): 1.53868
[2018-04-01 00:00:00 - 2018-10-01 00:00:00): 1.17569
[2018-05-01 00:00:00 - 2018-11-01 00:00:00): 1.47733
[2018-06-01 00:00:00 - 2018-12-01 00:00:00): 1.51434
[2018-07-01 00:00:00 - 2019-01-01 00:00:00): 1.32682
[2018-08-01 00:00:00 - 2019-02-01 00:00:00): 1.46367
[2018-09-01 00:00:00 - 2019-03-01 00:00:00): 1.26095
[2018-10-01 00:00:00 - 2019-04-01 00:00:00): 1.04631
[2018-11-01 00:00:00 - 2019-05-01 00:00:00): 0.983815
[2018-12-01 00:00:00 - 2019-06-01 00:00:00): 0.547442
[2019-01-01 00:00:00 - 2019-07-01 00:00:00): 0.616196
[2019-02-01 00:00:00 - 2019-08-01 00:00:00): 0.617309
[2019-03-01 00:00:00 - 2019-09-01 00:00:00): 0.695815
[2019-04-01 00:00:00 - 2019-10-01 00:00:00): 0.755909
[2019-05-01 00:00:00 - 2019-11-01 00:00:00): 0.758907
[2019-06-01 00:00:00 - 2019-12-01 00:00:00): 1.10488
[2019-07-01 00:00:00 - 2020-01-01 00:00:00): 0.965082
```

## Warning

Keep in mind that there are many risks associated with trading, and backtesting itself does not guarantee the expected gains.

* The input price history might be damaged (it might contain gaps or wrong prices).
* There might be subtle bugs in the backtesting algorithm (or implementation).
* Exchanges might become unreliable or unresponsive. In the worst case they can go bust.

Note that it is also not possible to perfectly simulate the trader over the past historical data, since the trader's actions might potentially impact the market. In our examples we use historical prices based on historically executed orders (and their volumes), but we have no information about market depth. We will discuss some caveats corresponding to different order types below.

### Limit Order Caveats

Limit order guarantees price, but it does not guarantee its (full) execution. Imagine that you place a limit sell order of 10 BTC at 200 USD. Assume that the (historical) price jumps to 205 USD/BTC at some point, but the corresponding (traded) volume is only 5 BTC. It is clear that the order will be triggered, but it is not clear how much of the order will be filled. For this reason, we have introduced `max_volume_ratio` , which specifies the ratio of the (OHLC tick) volume that will be used to fill the limit order. For example, if it is set to 0.1 then only 0.5 BTC would be traded (and the remaining order would be canceled).

### Stop and Market Order Caveats

Stop order is a promise by the exchange that it will execute the corresponding market order if the price jumps above (or drops below) the specified target price. However, it is not clear when exactly that will happen so the trading price is not guaranteed. Similarly, the final price of the market order might be different from the current market price (which is defined in our algorithm as the opening price of the OHLC tick over which the market order is going to be executed). The reason is that there might not be enough liquidity at the market for executing the market order at its target price. Again, imagine that you place a stop buy order of 10 BTC at 200 USD. If the (historical) price jumps to 205 USD/BTC, the stop buy order will be executed fully, but it is not clear what would be the final price. It can be anything between 200 and 205 USD/BTC (or even more). Therefore, we have introduced `market_liquidity`, which specifies the accuracy of executing market / stop orders at their target price. If it is 1.0, the market / stop order will be executed exactly at its target price (in our case 200 USD/BTC). If it is 0.0, the market / stop order will be executed at the worst possible price w.r.t.the given OHLC tick (in our case 205 USD/BTC). Any value between 0.0 and 1.0 can be used. The execution price will be then interpolated between the target price and the worst possible price. Note that because we have no information about market depth we cannot really predict the final price of market / stop orders, especially for large volumes.

## Implementation Guidelines

We follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html). All code must be [clang-formatted](https://clang.llvm.org/docs/ClangFormat.html) (with Google predefined style), unit-tested and peer-reviewed (if possible).
