cc_binary(
    name = "trader",
    srcs = ["trader.cc"],
    deps = [
        "//base",
        "//base:side_input",
        "//eval",
        "//logging:csv_logger",
        "//traders:trader_factory",
        "//util:proto",
        "//util:time",
        "@com_github_gflags_gflags//:gflags",
    ],
)

cc_binary(
    name = "convert",
    srcs = ["convert.cc"],
    deps = [
        "//base",
        "//base:history",
        "//util:proto",
        "//util:time",
        "@com_github_gflags_gflags//:gflags",
    ],
)
