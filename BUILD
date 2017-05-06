load(':protobuf.bzl', 'cc_proto')

cc_proto(
    name = "trader",
    src = "trader.proto"
)

cc_library(
    name = "util_time",
    srcs = ["util_time.cc"],
    hdrs = ["util_time.h"],
)

cc_test(
    name = "util_time_test",
    srcs = ["util_time_test.cc"],
    copts = ["-Iexternal/gtest/googletest/include"],
    deps = [
        ":util_time",
        "@gtest//:main"
    ],
)

cc_library(
    name = "util_proto",
    srcs = ["util_proto.cc"],
    hdrs = ["util_proto.h"],
    deps = ["@protobuf//:protobuf"],
    copts = ["-Iprotobuf/src"],
)

cc_test(
    name = "util_proto_test",
    srcs = ["util_proto_test.cc"],
    copts = ["-Iexternal/gtest/googletest/include"],
    deps = [
        ":trader_proto",
        ":util_proto",
        "@gtest//:main"
    ],
)

cc_library(
    name = "trader_base",
    srcs = ["trader_base.cc"],
    hdrs = ["trader_base.h"],
    deps = [":trader_proto"],
)

cc_test(
    name = "trader_base_test",
    srcs = ["trader_base_test.cc"],
    copts = ["-Iexternal/gtest/googletest/include"],
    deps = [
        ":trader_base",
        "@gtest//:main"
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
        ":util_proto",
        ":util_time",
        "//external:gflags",
    ],
)

cc_binary(
    name = "csv_convert_main",
    srcs = ["csv_convert_main.cc"],
    deps = [
        ":trader_io",
        ":util_proto",
        ":util_time",
        "//external:gflags",
    ],
)
