package(default_visibility = ["//:__pkg__"])

load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "trader_config_proto",
    srcs = ["trader_config.proto"],
)

cc_proto_library(
    name = "trader_config_cc_proto",
    deps = [":trader_config_proto"],
)

cc_library(
    name = "traders",
    srcs = [
        "limit_trader.cc",
        "stop_trader.cc",
    ],
    hdrs = [
        "limit_trader.h",
        "stop_trader.h",
    ],
    deps = [
        ":trader_config_cc_proto",
        "//lib:trader_base",
    ],
)
