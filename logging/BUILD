package(default_visibility = [
    "//:__pkg__",
    "//lib:__pkg__",
])

load("@rules_proto//proto:defs.bzl", "proto_library")

cc_library(
    name = "logger_interface",
    hdrs = ["logger_interface.h"],
    deps = [
        "//lib:trader_account",
        "//lib:trader_base",
    ],
)

cc_library(
    name = "csv_logger",
    srcs = ["csv_logger.cc"],
    hdrs = ["csv_logger.h"],
    deps = [":logger_interface"],
)
