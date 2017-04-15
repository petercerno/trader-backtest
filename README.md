# TraderBacktest

High-performance backtesting engine (written in C++) for evaluating trading strategies and finding their optimal hyper-parameters.

 - **Dependencies**: [Protocol Buffers](https://github.com/google/protobuf), [GFlags](https://github.com/gflags/gflags), [Google Test](https://github.com/google/googletest), [[Bazel](https://bazel.build/)].
 - **Data Analysis Tools**: [Python3](https://www.python.org/), [Jupyter](http://jupyter.org/index.html), [matplotlib](https://matplotlib.org/), [NumPy](http://www.numpy.org/), [Pandas](http://pandas.pydata.org/).

This project was tested on [Mac OS X](https://www.apple.com/macos/) and [Ubuntu](https://www.ubuntu.com/download/desktop).

## Installation

First, install [Bazel](https://bazel.build/). Build everything (inside the project source code directory) by running:

```
bazel build :all
```

Test everything by running:

```
bazel test :all
```

It is also possible to build the project manually (or within your favorite C++ IDE). However, you need to link the above dependencies and compile the `trader.proto` file by running:

```
protoc -I=. --cpp_out=. ./trader.proto
```

This will create files: `trader.pb.h` and `trader.pb.cc` which also need to be included in the project.

## Trader Interface

We assume that the trader being backtested works as follows: The trader wakes up regularly (e.g. every few seconds) and is given the actual account balances and the most recent security (or crypto-currency) price. The trader then might emit new limit, stop, or market order(s). We currently do not support arbitrage trading between multiple exchanges. For simplicity, we assume that at the beginning of every tick there are no active orders on the exchange (thus there is no need to update the existing orders). When the trader emits new orders (after processing a tick), the emitted orders are either executed, or canceled by the exchange at the very next tick. We assume that all emitted orders can be executed  independently of each other.

## Trader Evaluation

When evaluating the trader performance, we use the following two benchmarks:

- **Cash**: Do not buy any security (crypto-currency), keep everything in cash.
- **Buy and hold**: Invest everything in the security (crypto-currency) and hold.

The *buy and hold* strategy is more important as the security (crypto-currency) prices tend to grow over time.

## Example

Download some Bitcoin (BTC) historical prices from [bitcoincharts](http://bitcoincharts.com/):

```
curl -o /tmp/bitstampUSD.csv.gz \
  "https://api.bitcoincharts.com/v1/csv/bitstampUSD.csv.gz"
gunzip /tmp/bitstampUSD.csv.gz
```

Go to the project source code directory.

Convert the `bitstampUSD.csv` file to a more compact (delimited protobuf) representation:

```
bazel run :csv_convert_main -- \
  --input_price_history_csv_file="/tmp/bitstampUSD.csv" \
  --output_price_history_delimited_proto_file="/tmp/bitstampUSD.dpb"
```

**Note**: It is significantly faster to read and parse the delimited protobuf file than the CSV file.

Now evaluate a very simple *limit trader* over some fixed time period, e.g. [2016-01-01; 2017-01-01). The limit trader keeps track of exponentially smoothed security (crypto-currency) price and at every tick it places one limit sell (or limit buy) order at 1% above (or 1% below) the smoothed price (depending on whether it has more funds allocated in the crypto-currency or cash). You can find the corresponding implementation in: `limit_trader.h` and `limit_trader.cc`.

```
bazel run :trader_main -- \
  --input_price_history_delimited_proto_file="/tmp/bitstampUSD.dpb" \
  --output_exchange_states_csv_file="/tmp/bitstampUSD.out.csv" \
  --trader="limit" \
  --start_date_utc="2016-01-01" \
  --end_date_utc="2017-01-01" \
  --sampling_rate_sec=300 \
  --evaluation_period_months=0 \
  --start_security_balance=1.0 \
  --start_cash_balance=0.0 \
  --evaluate_batch=false
```

**Note**: The flag `sampling_rate_sec` specifies how often the trader 'wakes up'. In practice, we simply re-sample the given input price history with this sampling rate and feed the trader with the corresponding OHLC (Open, High, Low, Close) ticks.

You should get a similar output as follows:

```
Loaded 11026148 records in 4.898 seconds
[2016-01-01 - 2017-01-01): 1.23245
431.06 -> 1190.92 + 0 fees
431.06 -> 966.3 base
```

As you can see, the limit trader was able to convert 431.06 USD to 1190.92 USD over the year 2016, which beats the baseline *buy and hold* strategy by 23%.

You can also analyze the detailed output of the trader: `/tmp/bitstampUSD.out.csv`. Install [Jupyter](http://jupyter.org/index.html) and the following powerful data analysis tools: [matplotlib](https://matplotlib.org/), [NumPy](http://www.numpy.org/), and [Pandas](http://pandas.pydata.org/):

```
pip3 install --upgrade pip
sudo pip3 install jupyter matplotlib numpy pandas
```

Start the jupyter notebook server:

```
jupyter notebook
```

Open the `TraderBacktest.ipynb` notebook and run the commands (feel free to do any necessary modifications).

You can also evaluate the trader over multiple time periods by setting the `evaluation_period_months` flag to a positive number, e.g.:

```
bazel run :trader_main -- \
  --input_price_history_delimited_proto_file="/tmp/bitstampUSD.dpb" \
  --trader="limit" \
  --start_date_utc="2016-01-01" \
  --end_date_utc="2017-01-01" \
  --sampling_rate_sec=300 \
  --evaluation_period_months=6 \
  --start_security_balance=1.0 \
  --start_cash_balance=0.0 \
  --evaluate_batch=false
```

The backtester now does not output any exchange states, as there are multiple time periods over which the trader is evaluated. You should get a similar output as follows:

```
Loaded 11026148 records in 5.279 seconds
[2016-01-01 - 2016-07-01): 1.12124
[2016-02-01 - 2016-08-01): 1.06402
[2016-03-01 - 2016-09-01): 1.19233
[2016-04-01 - 2016-10-01): 1.12148
[2016-05-01 - 2016-11-01): 1.12339
[2016-06-01 - 2016-12-01): 1.11982
[2016-07-01 - 2017-01-01): 1.11289
```

As you can see, the limit trader consistently beats the baseline *buy and hold* strategy when evaluated over 6 months.

Finally, you can find the optimal hyper-parameters by setting the flag `evaluate_batch` to `true`.

```
bazel run :trader_main -- \
  --input_price_history_delimited_proto_file="/tmp/bitstampUSD.dpb" \
  --trader="limit" \
  --start_date_utc="2016-01-01" \
  --end_date_utc="2017-01-01" \
  --sampling_rate_sec=300 \
  --evaluation_period_months=6 \
  --start_security_balance=1.0 \
  --start_cash_balance=0.0 \
  --evaluate_batch=true
```

What hyper-parameters are optimized and what values are tried is defined in the source code. By default, you should get a similar output as follows:

```
Loaded 11026148 records in 4.748 seconds
limit-trader [0.010 0.005]: 1.45073
limit-trader [0.020 0.005]: 1.35072
limit-trader [0.030 0.005]: 1.28322
limit-trader [0.010 0.015]: 1.15455
limit-trader [0.030 0.015]: 1.12659
limit-trader [0.030 0.010]: 1.12217
limit-trader [0.020 0.015]: 1.10204
limit-trader [0.010 0.010]: 1.07616
limit-trader [0.020 0.010]: 1.0702
```

## Caveats

Keep in mind that there are many risks associated with live trading, and backtesting itself does not guarantee the expected gains.

- The input price history might be damaged (it might contain gaps or wrong quotations). (To some degree it is possible to detect gaps and potentially wrong quotations.)
- There might be bugs in the backtesting algorithm / implementation.

In addition, it is not possible to perfectly simulate the trader over the past historical data, since the trader's actions might impact the market (price movements). We will discuss some caveats with different order types below.

### Limit Order Caveats

Limit order guarantees the trading price, but it does not guarantee its execution (nor the fill size). Imagine that you place a limit sell order of 10 BTC at 200 USD. Assume that the (historical) price jumps to 205 USD/BTC at some point, but the corresponding tick volume is only 5 BTC. It is clear that the order will be triggered, but it is not clear how much of the order size will be filled. For this reason, we have introduced a flag `limit_order_fill_volume_ratio`, which specifies the ratio of the (tick) volume used to fill the limit order. If it is set to 0.1 then only 0.5 BTC would be traded (and the remaining order would be canceled). If it is set to 0 then this problem is ignored, and the order would be fully filled.

### Stop and Market Order Caveats

Stop order is not exactly a real order, as it is not present in the exchange order book. Rather, it is a promise by the exchange that it will execute the corresponding market order if the price jumps above (or drops below) the specified target. However, it is not clear when exactly will the exchange execute the stop order, so the trading price is also not guaranteed. Similarly, the trading price of the market order might be different from its target price (defined as the open price of a tick over which the market order is being executed). The reason is that there might not be enough liquidity at the market for executing the market order at its target price. Again, imagine that you place a stop buy order of 10 BTC at 200 USD. If the (historical) price jumps to 205 USD/BTC, the stop buy order will be executed fully, but it is not clear what would be the trading price. It can be anything between 200 and 205 USD/BTC. Therefore, we have introduced a flag `order_execution_accuracy`, which specifies the accuracy of executing market / stop orders at their target price. If it is 1.0, the market / stop order will be executed exactly at its target price (in our case 200 USD/BTC). If it is 0.0, the market / stop order will be executed at the worst possible price w.r.t. the given tick (in our case 205 USD/BTC). Any value between 0.0 and 1.0 can be used. The execution price will be then interpolated between the target price and the worst possible price.

**Note**: Different transaction fees might apply to different types of orders. Fees can be configured directly in the source code.

## Implementation Guidlines

We follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html). All code must be [clang-formatted](https://clang.llvm.org/docs/ClangFormat.html) (with Google predefined style),  unit-tested and peer-reviewed (if possible).
