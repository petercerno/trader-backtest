cc_binary(
    name = "trader",
    srcs = ["trader.cc"],
    deps = [
        "//eval:trader_eval",
        "//external:gflags",
        "//lib:trader_base",
        "//logging:csv_logger",
        "//traders",
        "//util:util_proto",
        "//util:util_time",
    ],
)

cc_binary(
    name = "convert",
    srcs = ["convert.cc"],
    deps = [
        "//external:gflags",
        "//lib:trader_base",
        "//lib:trader_history",
        "//util:util_proto",
        "//util:util_time",
    ],
)
