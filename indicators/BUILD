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