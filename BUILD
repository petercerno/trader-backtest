cc_binary(
    name = "trader_main",
    srcs = ["trader_main.cc"],
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
    name = "csv_convert_main",
    srcs = ["csv_convert_main.cc"],
    deps = [
        "//external:gflags",
        "//lib:trader_base",
        "//lib:trader_history",
        "//util:util_proto",
        "//util:util_time",
    ],
)
