package(default_visibility = [
    "//:__pkg__",
    "//indicators:__pkg__",
    "//traders:__pkg__",
])

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
    name = "trader_account",
    srcs = ["trader_account.cc"],
    hdrs = ["trader_account.h"],
    deps = [":trader_cc_proto"],
)

cc_test(
    name = "trader_account_test",
    srcs = ["trader_account_test.cc"],
    deps = [
        ":trader_account",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trader_base",
    hdrs = ["trader_base.h"],
    deps = [":trader_cc_proto"],
)

cc_test(
    name = "trader_base_test",
    srcs = ["trader_base_test.cc"],
    deps = [
        ":trader_base",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trader_interface",
    hdrs = ["trader_interface.h"],
    deps = [":trader_base"],
)

cc_library(
    name = "trader_eval",
    srcs = ["trader_eval.cc"],
    hdrs = ["trader_eval.h"],
    deps = [
        ":trader_account",
        ":trader_interface",
        "//util:util_time",
    ],
)

cc_test(
    name = "trader_eval_test",
    srcs = ["trader_eval_test.cc"],
    deps = [
        ":trader_eval",
        "//util:util_time",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trader_history",
    srcs = ["trader_history.cc"],
    hdrs = ["trader_history.h"],
    deps = [":trader_base"],
)

cc_test(
    name = "trader_history_test",
    srcs = ["trader_history_test.cc"],
    deps = [
        ":trader_history",
        "@googletest//:gtest_main",
    ],
)
