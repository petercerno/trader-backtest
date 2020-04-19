cc_binary(
    name = "trader",
    srcs = ["trader.cc"],
    deps = [
        "//external:gflags",
        "//lib:trader_base",
        "//lib:trader_eval",
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
