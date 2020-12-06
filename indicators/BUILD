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
    name = "util",
    srcs = ["util.cc"],
    hdrs = ["util.h"],
)

cc_test(
    name = "util_test",
    srcs = ["util_test.cc"],
    deps = [
        ":util",
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

cc_library(
    name = "exponential_moving_average",
    srcs = ["exponential_moving_average.cc"],
    hdrs = ["exponential_moving_average.h"],
    deps = [
        ":last_n_ohlc_ticks",
        ":util",
        "//lib:trader_base",
    ],
)

cc_test(
    name = "exponential_moving_average_test",
    srcs = ["exponential_moving_average_test.cc"],
    deps = [
        ":exponential_moving_average",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "moving_average_convergence_divergence",
    srcs = ["moving_average_convergence_divergence.cc"],
    hdrs = ["moving_average_convergence_divergence.h"],
    deps = [
        ":last_n_ohlc_ticks",
        ":util",
        "//lib:trader_base",
    ],
)

cc_test(
    name = "moving_average_convergence_divergence_test",
    srcs = ["moving_average_convergence_divergence_test.cc"],
    deps = [
        ":moving_average_convergence_divergence",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "relative_strength_index",
    srcs = ["relative_strength_index.cc"],
    hdrs = ["relative_strength_index.h"],
    deps = [
        ":last_n_ohlc_ticks",
        ":util",
        "//lib:trader_base",
    ],
)

cc_test(
    name = "relative_strength_index_test",
    srcs = ["relative_strength_index_test.cc"],
    deps = [
        ":relative_strength_index",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "stochastic_oscillator",
    srcs = ["stochastic_oscillator.cc"],
    hdrs = ["stochastic_oscillator.h"],
    deps = [
        ":last_n_ohlc_ticks",
        ":util",
        "//lib:trader_base",
    ],
)

cc_test(
    name = "stochastic_oscillator_test",
    srcs = ["stochastic_oscillator_test.cc"],
    deps = [
        ":stochastic_oscillator",
        "@googletest//:gtest_main",
    ],
)
