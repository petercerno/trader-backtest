package(default_visibility = [
    "//:__pkg__",
    "//eval:__pkg__",
    "//indicators:__pkg__",
    "//logging:__pkg__",
    "//traders:__pkg__",
])

load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "base_proto",
    srcs = ["base.proto"],
)

cc_proto_library(
    name = "base_cc_proto",
    deps = [":base_proto"],
)

cc_library(
    name = "base",
    hdrs = ["base.h"],
    deps = [":base_cc_proto"],
)

cc_test(
    name = "base_test",
    srcs = ["base_test.cc"],
    deps = [
        ":base",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "account",
    srcs = ["account.cc"],
    hdrs = ["account.h"],
    deps = [":base"],
)

cc_test(
    name = "account_test",
    srcs = ["account_test.cc"],
    deps = [
        ":account",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trader",
    hdrs = ["trader.h"],
    deps = [":base"],
)

cc_library(
    name = "history",
    srcs = ["history.cc"],
    hdrs = ["history.h"],
    deps = [":base"],
)

cc_test(
    name = "history_test",
    srcs = ["history_test.cc"],
    deps = [
        ":history",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "side_input",
    srcs = ["side_input.cc"],
    hdrs = ["side_input.h"],
    deps = [":base"],
)

cc_test(
    name = "side_input_test",
    srcs = ["side_input_test.cc"],
    deps = [
        ":side_input",
        "@googletest//:gtest_main",
    ],
)
