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
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
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
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)
