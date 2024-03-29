// Copyright © 2021 Peter Cerno. All rights reserved.

#ifndef TRADERS_TRADER_FACTORY_H
#define TRADERS_TRADER_FACTORY_H

#include "absl/strings/string_view.h"
#include "base/trader.h"

namespace trader {

// Returns a new instance of TraderEmitter for the given trader_name.
std::unique_ptr<TraderEmitter> GetTrader(absl::string_view trader_name);

// Returns batch of TraderEmitters for the given trader_name.
std::vector<std::unique_ptr<TraderEmitter>> GetBatchOfTraders(
    absl::string_view trader_name);

}  // namespace trader

#endif  // TRADERS_TRADER_FACTORY_H
