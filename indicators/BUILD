package(default_visibility = ["//:__pkg__"])

load("@rules_proto//proto:defs.bzl", "proto_library")

cc_library(
    name = "last_n_ohlc_ticks",
    srcs = ["last_n_ohlc_ticks.cc"],
    hdrs = ["last_n_ohlc_ticks.h"],
    deps = ["//lib:trader_base"],
)

cc_test(
    name = "last_n_ohlc_ticks_test",
    srcs = ["last_n_ohlc_ticks_test.cc"],
    deps = [
        ":last_n_ohlc_ticks",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "simple_moving_average",
    srcs = ["simple_moving_average.cc"],
    hdrs = ["simple_moving_average.h"],
    deps = [
        ":last_n_ohlc_ticks",
        "//lib:trader_base",
    ],
)

cc_test(
    name = "simple_moving_average_test",
    srcs = ["simple_moving_average_test.cc"],
    deps = [
        ":simple_moving_average",
        "@googletest//:gtest_main",
    ],
)
