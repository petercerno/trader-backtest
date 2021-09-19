package(default_visibility = [
    "//:__pkg__",
    "//eval:__pkg__",
])

load("@rules_proto//proto:defs.bzl", "proto_library")

cc_library(
    name = "logger",
    hdrs = ["logger.h"],
    deps = [
        "//base",
        "//base:account",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "csv_logger",
    srcs = ["csv_logger.cc"],
    hdrs = ["csv_logger.h"],
    deps = [":logger"],
)

cc_test(
    name = "csv_logger_test",
    srcs = ["csv_logger_test.cc"],
    deps = [
        ":csv_logger",
        "@com_google_googletest//:gtest_main",
    ],
)
