package(default_visibility = ["//:__pkg__"])

load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "eval_proto",
    srcs = ["eval.proto"],
    deps = ["//base:base_proto"],
)

cc_proto_library(
    name = "eval_cc_proto",
    deps = [":eval_proto"],
)

cc_library(
    name = "eval",
    srcs = ["eval.cc"],
    hdrs = ["eval.h"],
    deps = [
        ":eval_cc_proto",
        "//base",
        "//base:account",
        "//base:side_input",
        "//base:trader",
        "//indicators:volatility",
        "//logging:logger",
        "//util:time",
    ],
)

cc_test(
    name = "eval_test",
    srcs = ["eval_test.cc"],
    deps = [
        ":eval",
        "//logging:csv_logger",
        "//util:time",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)
