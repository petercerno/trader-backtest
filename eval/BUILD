package(default_visibility = ["//:__pkg__"])

cc_library(
    name = "trader_eval",
    srcs = ["trader_eval.cc"],
    hdrs = ["trader_eval.h"],
    deps = [
        "//lib:trader_account",
        "//lib:trader_interface",
        "//logging:logger_interface",
        "//util:util_time",
    ],
)

cc_test(
    name = "trader_eval_test",
    srcs = ["trader_eval_test.cc"],
    deps = [
        ":trader_eval",
        "//logging:csv_logger",
        "//util:util_time",
        "@googletest//:gtest_main",
    ],
)
