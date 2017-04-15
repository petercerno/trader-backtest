cc_library(
    name = "main",
    srcs = glob(
        ["googletest/src/*.cc",
         "googletest/src/*.h"],
        exclude = ["googletest/src/gtest-all.cc"]
    ),
    hdrs = glob([
        "googletest/include/**/*.h",
        "googletest/**/*.h"]),
    copts = ["-Iexternal/gtest/googletest/include",
             "-Iexternal/gtest/googletest"],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)
