cc_binary(
    name = "trader",
    srcs = ["trader.cc"],
    deps = [
        "//base",
        "//eval",
        "//external:gflags",
        "//logging:csv_logger",
        "//traders",
        "//util:proto",
        "//util:time",
    ],
)

cc_binary(
    name = "convert",
    srcs = ["convert.cc"],
    deps = [
        "//base",
        "//base:history",
        "//external:gflags",
        "//util:proto",
        "//util:time",
    ],
)
