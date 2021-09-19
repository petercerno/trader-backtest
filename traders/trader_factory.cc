// Copyright © 2021 Peter Cerno. All rights reserved.

#include "traders/trader_factory.h"

#include "traders/rebalancing_trader.h"
#include "traders/stop_trader.h"
#include "traders/trader_config.pb.h"

namespace trader {
namespace {
static constexpr char kRebalancingTraderName[] = "rebalancing";
static constexpr char kStopTraderName[] = "stop";

// Returns the default rebalancing trader emitter.
std::unique_ptr<TraderEmitter> GetDefaultRebalancingTraderEmitter() {
  RebalancingTraderConfig config;
  config.set_alpha(0.7f);
  config.set_epsilon(0.05f);
  return std::unique_ptr<TraderEmitter>(new RebalancingTraderEmitter(config));
}

// Returns the default batch of rebalancing traders.
std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfRebalancingTraders() {
  return RebalancingTraderEmitter::GetBatchOfTraders(
      /*alphas=*/{0.1f, 0.3f, 0.5f, 0.7f, 0.9f},
      /*epsilons=*/{0.01f, 0.05f, 0.1f, 0.2f});
}

// Returns the default stop trader emitter.
std::unique_ptr<TraderEmitter> GetDefaultStopTraderEmitter() {
  StopTraderConfig config;
  config.set_stop_order_margin(0.1f);
  config.set_stop_order_move_margin(0.1f);
  config.set_stop_order_increase_per_day(0.01f);
  config.set_stop_order_decrease_per_day(0.1f);
  return std::unique_ptr<TraderEmitter>(new StopTraderEmitter(config));
}

// Returns the default batch of stop traders.
std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfStopTraders() {
  return StopTraderEmitter::GetBatchOfTraders(
      /*stop_order_margins=*/{0.05, 0.1, 0.15, 0.2},
      /*stop_order_move_margins=*/{0.05, 0.1, 0.15, 0.2},
      /*stop_order_increases_per_day=*/{0.01, 0.05, 0.1},
      /*stop_order_decreases_per_day=*/{0.01, 0.05, 0.1});
}
}  // namespace

std::unique_ptr<TraderEmitter> GetTrader(const std::string& trader_name) {
  if (trader_name == kRebalancingTraderName) {
    return GetDefaultRebalancingTraderEmitter();
  } else {
    assert(trader_name == kStopTraderName);
    return GetDefaultStopTraderEmitter();
  }
}

std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfTraders(
    const std::string& trader_name) {
  if (trader_name == kRebalancingTraderName) {
    return GetBatchOfRebalancingTraders();
  } else {
    assert(trader_name == kStopTraderName);
    return GetBatchOfStopTraders();
  }
}

}  // namespace trader
