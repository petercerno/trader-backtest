package(default_visibility = ["//visibility:public"])

load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "example_proto",
    srcs = ["example.proto"],
)

cc_proto_library(
    name = "example_cc_proto",
    deps = [":example_proto"],
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
    deps = ["@com_google_protobuf//:protobuf_lite"],
)

cc_test(
    name = "util_proto_test",
    srcs = ["util_proto_test.cc"],
    deps = [
        ":example_cc_proto",
        ":util_proto",
        "@googletest//:gtest_main",
    ],
)
