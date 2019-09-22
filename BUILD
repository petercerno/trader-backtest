load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "trader_proto",
    srcs = ["trader.proto"],
)

cc_proto_library(
    name = "trader_cc_proto",
    deps = [":trader_proto"],
)

cc_library(
    name = "util_time",
    srcs = ["util_time.cc"],
    hdrs = ["util_time.h"],
)

cc_test(
    name = "util_time_test",
    srcs = ["util_time_test.cc"],
    deps = [
        ":util_time",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "util_proto",
    srcs = ["util_proto.cc"],
    hdrs = ["util_proto.h"],
    deps = [":trader_cc_proto"],
)

cc_test(
    name = "util_proto_test",
    srcs = ["util_proto_test.cc"],
    deps = [
        ":trader_cc_proto",
        ":util_proto",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trader_base",
    hdrs = ["trader_base.h"],
    deps = [":trader_cc_proto"],
)

cc_library(
    name = "util_test",
    srcs = ["util_test.cc"],
    hdrs = ["util_test.h"],
    deps = [
        ":trader_base",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "trader_base_test",
    srcs = ["trader_base_test.cc"],
    deps = [
        ":trader_base",
        ":util_test",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trader_util",
    srcs = ["trader_util.cc"],
    hdrs = ["trader_util.h"],
    deps = [
        ":trader_base",
        ":trader_cc_proto",
    ],
)

cc_test(
    name = "trader_util_test",
    srcs = ["trader_util_test.cc"],
    deps = [
        ":trader_util",
        ":util_test",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trader_io",
    srcs = ["trader_io.cc"],
    hdrs = ["trader_io.h"],
    deps = [":trader_base"],
)

cc_library(
    name = "trader_impls",
    srcs = [
        "limit_trader.cc",
        "limit_trader_v2.cc",
        "stop_trader.cc",
    ],
    hdrs = [
        "limit_trader.h",
        "limit_trader_v2.h",
        "stop_trader.h",
    ],
    deps = [":trader_base"],
)

cc_library(
    name = "exchange_account",
    srcs = ["exchange_account.cc"],
    hdrs = ["exchange_account.h"],
    deps = [":trader_base"],
)

cc_test(
    name = "exchange_account_test",
    srcs = ["exchange_account_test.cc"],
    deps = [
        ":exchange_account",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trader_eval",
    srcs = ["trader_eval.cc"],
    hdrs = ["trader_eval.h"],
    deps = [
        ":exchange_account",
        ":trader_base",
        ":util_time",
    ],
)

cc_binary(
    name = "trader_main",
    srcs = ["trader_main.cc"],
    deps = [
        ":trader_base",
        ":trader_eval",
        ":trader_impls",
        ":trader_io",
        ":trader_util",
        ":util_proto",
        ":util_time",
        "//external:gflags",
    ],
)

cc_binary(
    name = "csv_convert_main",
    srcs = ["csv_convert_main.cc"],
    deps = [
        ":trader_base",
        ":trader_io",
        ":trader_util",
        ":util_proto",
        ":util_time",
        "//external:gflags",
    ],
)
