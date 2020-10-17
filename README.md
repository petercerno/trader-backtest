# Trader Backtest

High-performance backtesting engine written in C++ for evaluating trading strategies restricted to a single trading pair (e.g. BTC/USD) and finding their optimal hyper-parameters. This software gives you a full control over every single bit that goes into backtesting. Arbitrage and margin trading are not supported. Licensed under [MIT](http://opensource.org/licenses/MIT).

 * **Dependencies**: [Protocol Buffers](https://github.com/protocolbuffers/protobuf), [GFlags](https://github.com/gflags/gflags), [Google Test](https://github.com/google/googletest), [Bazel](https://bazel.build/).
 * **Data Analysis Tools**: [Python3](https://www.python.org/), [Jupyter](https://jupyter.org/), [matplotlib](https://matplotlib.org/), [NumPy](http://www.numpy.org/), [Pandas](http://pandas.pydata.org/).

[Visual Studio Code](https://code.visualstudio.com/) is highly recommended for code editing.

**DISCLAIMER**: ALL OPINIONS EXPRESSED HERE ARE MY OWN AND DO NOT EXPRESS THE VIEWS OR OPINIONS OF MY EMPLOYER.

**DISCLAIMER**: I DO NOT PROVIDE ANY FINANCIAL, INVESTMENT, LEGAL, TAX OR ANY OTHER PROFESSIONAL ADVICE. I AM NOT A BROKER, FINANCIAL ADVISOR, INVESTMENT ADVISOR, PORTFOLIO MANAGER OR TAX ADVISOR. YOU ACKNOWLEDGE AND AGREE THAT ONLY YOU ARE RESPONSIBLE FOR YOUR USE OF ANY INFORMATION THAT YOU OBTAIN FROM THIS REPOSITORY OR SOFTWARE. YOUR DECISIONS MADE IN RELIANCE ON THE SOFTWARE OR YOUR INTERPRETATIONS OF THE DATA ARE YOUR OWN FOR WHICH YOU HAVE FULL RESPONSIBILITY. YOU EXPRESSLY AGREE THAT YOUR USE OF THE SOFTWARE IS AT YOUR SOLE RISK.

## Contributing

Before contributing to this repository I would encourage you to write a short design doc (with [Google Docs](https://docs.google.com/)) and share it with me for a pre-review. Once approved, you will have to follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html). All code must be [clang-formatted](https://clang.llvm.org/docs/ClangFormat.html) (with Google predefined style) and unit-tested.

## Motivation

People sometimes get seduced by (day-)trading stocks or crypto-currencies only to end up making poor decisions and loosing money. This software tries to prevent that by providing a tool to scrutinize one's trading ideas. 

**Warning**: If you ever decide to expose yourself to the highly volatile waters of crypto-currencies then stick only to the most reputable exchanges like [Coinbase](https://www.coinbase.com/) or [Kraken](https://www.kraken.com/). Always do your own research before investing and never invest more than you can afford to loose.

**Note**: It turns out that when it comes to [Bitcoin](https://bitcoin.org/en/) one strategy that worked well historically also happens to be the simplest one: Buy Bitcoin and [HODL](https://www.investopedia.com/terms/h/hodl.asp). For this reason we compare the performance of all trading strategies against this so-called Buy-And-HODL strategy. If you decide to HODL, you should keep your crypto-currencies in a hardware wallet like [Ledger](https://www.ledger.com/) or [Trezor](https://trezor.io/) rather than trusting an exchange. (There is an old saying in the crypto community that if you don’t own your keys, you don’t own your crypto.) I would also recommend writing down the [mnemonic phrase](https://en.bitcoinwiki.org/wiki/Mnemonic_phrase) into a steel capsule like [Cryptosteel](https://cryptosteel.com/?) and put it into a bank safe deposit box.

So what makes (day-)trading so seductive? If you look at a price chart like the one shown below it might occur to you: "Oh, if I had bought Bitcoin at the end of March 2017 and then sold it in December 2017 I would have made more than 2000%." Now let's break down why this is not that easy.

![BTC/USD 2017 Bull Market](./BTC_USD_2017.png)

BTC/USD 2017 Bull Market, provided by [TradingView](https://www.tradingview.com/).

First of all, in order for anyone to commit to do any concrete action (such as buying Bitcoin) one needs to define under which circumstances they would be willing to do that. Once these buying / selling conditions are defined, it should be possible to evaluate their performance over historical data. Keep in mind, however, that the estimated historical performance is not guaranteed to persist in the future (especially when [overfitting](https://en.wikipedia.org/wiki/Overfitting)). Therefore, evaluating the trading strategy over many historical periods is always a good idea.

It turns out that it is not that easy to define buying / selling rules that would capture that 2000% Bitcoin gain in 2017. One would experience at least five 30% - 40% price corrections during this period often happening in a matter of few days. Such corrections could easily wipe you out for good if you did margin trading (even with moderate leverage). For this reason we do not (and will not) support margin trading.

## Exchanges

In this section we provide a brief (but over-simplified) overview of how crypto-currency exchanges work and make some assumptions for our use case. We focus solely on a single trading pair (e.g. BTC/USD) on a single centralized exchange like [Coinbase Pro](https://pro.coinbase.com/) or [Kraken](https://www.kraken.com/). Contrary to popular belief, interacting with a centralized crypto-currency exchange almost never involves interacting with a blockchain. The only exceptions are crypto-currency deposits and withdrawals, which always require a blockchain transaction. Centralized exchanges have internal databases with all account balances and orders of all customers. Thus trading a crypto-currency is often a very fast operation (as only the internal database needs to be updated). Moreover, for security reasons the centralized exchanges often store most of their customers' funds in offline (cold) storages. Exchanges typically offer [REST](https://en.wikipedia.org/wiki/Representational_state_transfer) / [FIX](https://en.wikipedia.org/wiki/Financial_Information_eXchange) API for trading (and sometimes also websocket feed for real-time market data updates). It is possible to restrict the API Key Permissions so that only certain operations are allowed with that API Key (e.g. it is possible to restrict the API Key to trading and disable deposits / withdrawals).

**Note**: There are many (for the most part [Ethereum](https://ethereum.org/en/)-based) decentralized exchanges (DEXes) like [Uniswap](https://app.uniswap.org/), [Curve Finance](https://www.curve.fi/), etc. and decentralized lending platforms like [Aave](https://aave.com/), [Maker](https://makerdao.com/en/), [Compound](https://compound.finance/), etc. now very popular in the [DeFi](https://www.coindesk.com/what-is-defi) ecosystem. (You can see how much value is locked in these protocols e.g. in [defipulse.com](https://defipulse.com/).) One notable difference (w.r.t. the centralized exchanges) is that interacting with these DEXes (and other protocols) requires interacting with the Ethereum blockchain (via a web3 wallet like [MetaMask](https://metamask.io/) or web3 API like [web3.js](https://web3js.readthedocs.io/)) and is subject to [gas fees](https://etherscan.io/gastracker) (although there are several ongoing Ethereum scaling projects and also [Eth2.0](https://ethereum.org/en/eth2/) that might reduce these fees significantly). You can connect [MetaMask](https://metamask.io/) to [Ledger](https://www.ledger.com/metamask) or [Trezor](https://wiki.trezor.io/Apps:MetaMask), which makes all these transactions highly secure. Interestingly, it is possible to execute arbitrage trades over multiple DEXes, as one can write a smart contract that interacts with all these DEXes / lending platforms in a single Ethereum transaction (see e.g. this [article from Dec 2019](https://medium.com/cambrial-capital/exploring-defi-trading-strategies-arbitrage-in-defi-7736dc5dd77a)). On the other hand, when doing arbitrage trading on centralized exchanges one needs to move their funds (crypto currencies / fiat currencies) between exchanges, which is a non-trivial (and perhaps risky) process. For this reason we do not support arbitrage trading.

A crypto-currency exchange can be viewed as a (market)place where buyers meet with sellers in order to exchange their crypto-currencies for fiat (or other crypto-currencies) and vice versa. Having a centralized exchange as a facilitator of trades is beneficial, as the participants do not need to trust (or even know) each other in order to safely execute their trades. They only need to trust the exchange itself. (The DEXes go even further as the participants do not even need to trust the exchange. They only need to trust the (byte)code behind the smart contract implementing the exchange (deployed at a specific contract address), see e.g. the [source code](https://github.com/Uniswap) for [Uniswap](https://app.uniswap.org/). They also need to trust the correctness of the compiler (e.g. [Solidity](https://en.wikipedia.org/wiki/Solidity)) to produce correct and secure bytecode, and also the correctness and security of the whole [Ethereum](https://ethereum.org/en/) blockchain and their [EVM](https://ethereum.org/en/developers/docs/evm/).)

When restricting to a single trading pair (e.g. BTC/USD), the main data structure of a (centralized) exchange is the so-called [order book](https://en.wikipedia.org/wiki/Order_book_(trading)), which contains the list of all buy orders (bids) and the list of all sell orders (asks). The exchange participants can provide the so-called liquidity into the exchange by locking their funds into these bids or asks via the so-called limit orders (free of charge). (Side note: It is also possible to provide liquidity into DEXes by locking [ETH](https://en.bitcoinwiki.org/wiki/Ethereum) or other supported [ERC-20](https://en.bitcoinwiki.org/wiki/ERC20) tokens like [DAI](https://makerdao.com/en/) (stable coin pegged to USD) or Wrapped Bitcoin [WBTC](https://wbtc.network/) (token pegged to Bitcoin) into the so-called liquidity pools. In this way it is possible to earn fees collected by the DEX or even earn governance tokens like UNI from [Uniswap](https://app.uniswap.org/). However, keep in mind that there are some [caveats](https://medium.com/@pintail/uniswap-a-good-deal-for-liquidity-providers-104c0b6816f2), not to mention smart contract risks.)

On the other hand, when a trader wants to buy a crypto-currency then they can issue the so-called market buy order with the desired amount. Market buy order is executed immediately and liquidates the lowest ask(s) in the order book in order to fulfill the desired amount. Similarly, when a trader wants to sell some amount of crypto-currency they can issue a market sell order with the desired amount and the highest bid(s) will be immediately liquidated from the exchange. The trader initiating the market order is said to demand liquidity, and the counter-party to the transaction supplies liquidity. The exchange usually extracts fees from all parties involved in these transactions, although the (counter-)party that is providing liquidity is usually subjected to smaller (or sometimes even zero) fees in order to incentivize traders to provide liquidity. Note that the order book (or the corresponding depth chart) is usually shown to all exchange participants (although the identities of the participants are hidden).

In addition to limit orders and market orders, exchanges typically provide also the so-called stop orders (or even other more exotic orders not discussed here). Stop buy order at a particular target price can be thought of as a promise by the exchange that it will execute the corresponding market buy order as soon as the price (e.g. the lowest ask) rises to (or above) the provided target price. Similarly, stop sell order at a particular target price is a promise that the exchange will execute the corresponding market sell order as soon as the price (e.g. the highest bid) drops to (or below) the provided target price. The important thing to keep in mind is that (contrary to limit orders) the target price for stop orders is not guaranteed (as the price jumps can be sudden and non-continuous). Moreover, stop orders are not visible to other exchange participants.

## Simplifications

Unfortunately, we do not have access to (the historical updates to) the order book and all orders that happened on the exchange. Instead, we have access only to the so-called price history. The price history is defined as follows: Whenever a transaction (market order) was executed, in which some party and counter-party exchanged some amount `V` of crypto-currency at price `P` and at time `T`, the triple `(T,P,V)` would be added to the price history. (In our case we do not know whether this was a buy or sell order. Thus sometimes we observe sequences of rather jumpy prices, especially when there was a wide [bid-ask spread](https://en.wikipedia.org/wiki/Bid%E2%80%93ask_spread).) In general, it is not possible to reconstruct the order book based only on the price history. The price (e.g. if defined as the highest bid) might change suddenly without any market orders being executed. (Imagine that all participants withdrew all their bids. That would essentially crash the price to zero without a single market sell order being submitted.) To complicate matters even more, the trader's actions might impact the actions of other market participants (and the price itself).

The price history is still a rather inconvenient data structure for fast processing. Therefore, we aggregate (re-sample) it into the so-called [OHLC (Open High Low Close) history](https://en.wikipedia.org/wiki/Open-high-low-close_chart) with some fixed aggregation (sampling) period (e.g. 5 minutes per OHLC tick). An OHLC tick covering the time-period `[T, T+dT)` is a tuple `(T,O,H,L,C,V)` defined as follows: Let `(T[i],P[i],V[i])` be all price history triples from the time-period `[T, T+dT)` for `i = 0 ... k-1`, such that `T <= T[0] <= ... <= T[k-1] < T + dT`. Then:

* `O := P[0]`
* `H := max(P[i])`
* `L := min(P[i])`
* `C := P[k-1]`
* `V := sum(V[i])`

(Where `i` is taken over all: `0 ... k-1`.) If there were no price history triples within the period `[T, T+dT)` then we define `V := 0` and `O = H = L = C := ` the previous close price (of a previous OHLC tick). Note that the sampling period `dT` is implicitly assumed (and is not part of the tuple). For simplicity, the time `T` is represented as a [UNIX time](https://en.wikipedia.org/wiki/Unix_time) (in seconds) divisible by the period `dT` (also in seconds). This makes it easy to quickly find the corresponding OHLC tick for any UNIX timestamp (e.g. using a simple integer division). The trader is executed over the OHLC history one OHLC tick at a time, and emits orders at the end of each OHLC tick. The emitted orders are then executed on the next OHLC tick(s) (to avoid peeking into the future). In the following sub-sections we discuss how we deal with different order types.

### Limit Orders

Limit order guarantees the target price, but it does not guarantee fulfillment (full execution). Imagine that you placed a limit sell order (ask) of 10 BTC at 10'000 USD. Assume that the (historical) price jumped to 10'005 USD/BTC at some point, but the corresponding historical (traded) volume was only 5 BTC. It is clear that your limit order would have been at least partially filled (since all the historical asks at or above your order's target price were liquidated), but it is not clear how much of your order's target amount would have been filled (as there might have been other asks at or below you order's target price that had to be filled as well). For this reason, we have introduced `max_volume_ratio`, which specifies the ratio of the (OHLC tick's) volume that would have been used to fill your limit order. For example, if it was set to 0.1 then only 0.5 BTC would have been used to partially fill your limit order (on the given OHLC tick).

### Stop and Market Orders

As discussed before, a stop order can be thought of as a promise by the exchange that the corresponding market order will be executed if the price jumps above (or drops below) the specified target price. It is not clear, however, when exactly the exchange would execute your stop order (as the price jumps can be sudden and non-continuous) so the target price is not guaranteed. Similarly, the effective price of a market order might be different from the current market price (which is defined in our algorithm as the opening price of the OHLC tick over which the market order is being executed). The reason is that there might not be enough liquidity for executing the market order at the opening price. Again, imagine that you placed a stop buy order of 10 BTC at 10'000 USD. If the (historical) price jumped to 10'005 USD/BTC, the stop buy order would have been executed fully, but it is not clear what would have been the effective price. It can be anything between 10'000 and 10'005 USD/BTC (or even more). Therefore, we have introduced `market_liquidity`, which specifies the accuracy of executing market / stop orders at their target price. If it was set to 1.0, the market / stop order would have been executed exactly at its target price (in our case 10'000 USD/BTC). If it was set to 0.0, the market / stop order would have been executed at the worst possible price w.r.t. the given OHLC tick (in our case 10'005 USD/BTC). Any value between 0.0 and 1.0 can be used. The price is then interpolated between the target price and the worst possible price. Since we have no information about market depth (or liquidity) we cannot really predict the effective price of market / stop orders, especially for large volumes.

## Installation

First, install [Bazel](https://bazel.build/). Build everything (inside the project source code directory) by running:

``` 
bazel build ... 
```

Test everything by running:

``` 
bazel test ... 
```

## Project Structure

There are two main binaries you can run:

* `convert.cc` : Converts a CSV file into a more compact delimited protocol buffer file. Allows re-sampling price history into OHLC (Open High Low Close) history with fixed sampling rate.
* `trader.cc` : Loads OHLC history (in delimited protocol buffer file format) and evaluates one or more traders over it.

The source code is structured as follows:

* `util/` : General utilities (not related to trading).
* `lib/` : Core trading data structures, interfaces, trader execution and evaluation logic.
* `indicators/` : Simple technical indicators that can be re-used by traders.
* `traders/` : Specific trader implementations.

If you want to backtest your own trader (trading strategy), you need to add and implement your trader in the `traders/` directory. That means implementing `TraderInterface` (for the trading logic) and `TraderFactoryInterface` (for emitting new instances of your trader based on the corresponding config in the `trader_config.proto`). Then you need to update the `trader.cc` binary so that it can initialize and use your trader for backtesting.

## Trader Execution

Every trader is executed (backtested) as follows:

* At every step the trader receives the latest OHLC tick `T[i]`, current account balances, and updates its internal state. The current time is at the end of the OHLC tick `T[i]` time period. (The trader is not updated on zero volume OHLC ticks. These OHLC ticks indicate a gap in a price history, which could have been caused by an unresponsive exchange or its API.)
* Then the trader needs to decide what orders to emit. There are no other active orders on the exchange at this moment (see the explanation below).
* Once the trader decides what orders to emit, the exchange will execute (or cancel) all these orders on the follow-up OHLC tick `T[i+1]` . The trader does not see the follow-up OHLC tick `T[i+1]` , so there is no peeking into the future.
* Once all orders are executed (or canceled) by the exchange, the trader receives the follow-up OHLC tick `T[i+1]` and the whole process repeats.

Note that at every step every order gets either executed or canceled by the exchange. This is a design simplification so that there are no active orders that the trader needs to maintain over time. In practice, however, we would not cancel orders if they would be re-emitted again. We would simply modify the existing orders (from the previous iteration) based on the updated state.

Also note that the OHLC history sampling rate defines the frequency at which the trader is updated and emits orders. In general, traders should be designed in a frequency-agnostic way. In other words, they should have similar behavior and performance characteristics regardless of how frequently they are called. Traders should not assume anything about how often and when exactly they are called. One reason for that is that exchanges (or their APIs) sometimes become unresponsive for random periods of time (and we see that e.g. in the gaps in the price histories). Therefore, we encourage to test the traders on OHLC histories with various sampling rates.

## Trader Evaluation

When evaluating trader performance, we compare it to the following benchmark:

* **Buy and HODL**: Invest everything into the crypto-currency and hold it.

The *Buy and HODL* strategy is surprisingly hard to beat (especially for BTC/USD).

There are several options how to evaluate a trader:

* Evaluate a trader over a specific time period. We can log exchange states and/or trader's internal states into a CSV file and later analyze them e.g. using [Jupyter Lab](https://jupyter.org/).
* Evaluate a trader over multiple time periods. This lets us to see the trader's performance across many different time periods. Here, however, we do not log anything.
* Evaluate a batch of traders (i.e. do a [grid search](https://en.wikipedia.org/wiki/Hyperparameter_optimization) over trader's hyper-parameters) over a single or multiple time periods (in parallel). This helps us to find an interesting sub-space of trader's hyper-parameters with good performance.

## Example

Download BTC/USD historical prices from [bitcoincharts](http://bitcoincharts.com/):

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
  --output_price_history_delimited_proto_file="/$(pwd)/data/bitstampUSD.dpb" \
  --start_date_utc="2017-01-01" \
  --end_date_utc="2020-10-01"
```

The `start_date_utc` and `end_date_utc` dates are optional. We use them to make the output smaller.

For a trader evaluation we need an OHLC history. It is possible to re-sample the original CSV file, but the delimited protocol buffer file will be much faster to read:

```
bazel run :convert -- \
  --input_price_history_delimited_proto_file="/$(pwd)/data/bitstampUSD.dpb" \
  --output_ohlc_history_delimited_proto_file="/$(pwd)/data/bitstampUSD_5min.dpb" \
  --start_date_utc="2017-01-01" \
  --end_date_utc="2020-10-01" \
  --sampling_rate_sec=300
```

Now we can evaluate a simple *stop trader* over a single time period and log its internal state:

``` 
bazel run :trader -- \
  --input_ohlc_history_delimited_proto_file="/$(pwd)/data/bitstampUSD_5min.dpb" \
  --trader="stop" \
  --output_exchange_log_file="/$(pwd)/data/bitstampUSD.out.csv" \
  --output_trader_log_file="/$(pwd)/data/stop_trader_log.csv" \
  --start_date_utc="2017-01-01" \
  --end_date_utc="2020-10-01" \
  --start_base_balance=1.0 \
  --start_quote_balance=0.0
```

The output:

```
Loaded 394272 records in 0.523 seconds
Selected 394272 OHLC ticks within the period: [2017-01-01 00:00:00 - 2020-10-01 00:00:00)

Trader evaluation:
[2017-01-01 00:00:00 - 2020-10-01 00:00:00): 1.1067
```

The evaluation output is the ratio of the trader's performance and the performance of the *Buy and HODL* strategy.

It is better, however, to evaluate the trader over multiple time periods:

```
bazel run :trader -- \
  --input_ohlc_history_delimited_proto_file="/$(pwd)/data/bitstampUSD_5min.dpb" \
  --trader="stop" \
  --start_date_utc="2017-01-01" \
  --end_date_utc="2020-10-01" \
  --evaluation_period_months=6 \
  --start_base_balance=1.0 \
  --start_quote_balance=0.0
```

The output:

```
Loaded 394272 records in 0.522 seconds
Selected 394272 OHLC ticks within the period: [2017-01-01 00:00:00 - 2020-10-01 00:00:00)

Trader evaluation:
[2017-01-01 00:00:00 - 2017-07-01 00:00:00): 0.745228
[2017-02-01 00:00:00 - 2017-08-01 00:00:00): 0.876014
[2017-03-01 00:00:00 - 2017-09-01 00:00:00): 0.893903
[2017-04-01 00:00:00 - 2017-10-01 00:00:00): 1.02772
[2017-05-01 00:00:00 - 2017-11-01 00:00:00): 1.03472
...
[2019-12-01 00:00:00 - 2020-06-01 00:00:00): 1.39337
[2020-01-01 00:00:00 - 2020-07-01 00:00:00): 1.43864
[2020-02-01 00:00:00 - 2020-08-01 00:00:00): 1.33628
[2020-03-01 00:00:00 - 2020-09-01 00:00:00): 1.17149
[2020-04-01 00:00:00 - 2020-10-01 00:00:00): 0.860107
```

To evaluate a batch of traders (i.e. to do a grid search) one can run:

```
bazel run :trader -- \
  --input_ohlc_history_delimited_proto_file="/$(pwd)/data/bitstampUSD_5min.dpb" \
  --trader="stop" \
  --start_date_utc="2017-01-01" \
  --end_date_utc="2020-10-01" \
  --evaluation_period_months=6 \
  --start_base_balance=1.0 \
  --start_quote_balance=0.0 \
  --evaluate_batch=true
```

The output:

```
Loaded 394272 records in 0.805 seconds
Selected 394272 OHLC ticks within the period: [2017-01-01 00:00:00 - 2020-10-01 00:00:00)

Batch evaluation:
stop-trader[0.100|0.100|0.010|0.100]: 1.05953
stop-trader[0.150|0.100|0.010|0.050]: 1.0426
stop-trader[0.200|0.050|0.010|0.010]: 1.03798
stop-trader[0.150|0.100|0.010|0.010]: 1.02781
stop-trader[0.150|0.100|0.010|0.100]: 1.02206
...
```
