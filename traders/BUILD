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
        "rebalancing_trader.cc",
        "stop_trader.cc",
    ],
    hdrs = [
        "rebalancing_trader.h",
        "stop_trader.h",
    ],
    deps = [
        ":trader_config_cc_proto",
        "//base:trader",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "trader_factory",
    srcs = ["trader_factory.cc"],
    hdrs = ["trader_factory.h"],
    deps = [
        ":traders",
        "@com_google_absl//absl/strings",
    ],
)
