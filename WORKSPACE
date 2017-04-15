git_repository(
    name   = "com_github_gflags_gflags",
    commit = "30dbc81fb5ffdc98ea9b14b1918bfe4e8779b26e", # v2.2.0 + fix
    remote = "https://github.com/gflags/gflags.git"
)

bind(
    name   = "gflags",
    actual = "@com_github_gflags_gflags//:gflags",
)

bind(
    name   = "gflags_nothreads",
    actual = "@com_github_gflags_gflags//:gflags_nothreads",
)

new_http_archive(
    name = "gtest",
    url = "https://github.com/google/googletest/archive/release-1.8.0.zip",
    sha256 = "f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
    build_file = "gtest.BUILD",
    strip_prefix = "googletest-release-1.8.0",
)

http_archive(
    name = "protobuf",
    url = "https://github.com/google/protobuf/releases/download/v3.2.0/protobuf-cpp-3.2.0.zip",
    sha256 = "6b43d1c4dbf9be95da4b29b92d30d0e7e08ec498b5f2fb022afd960458d9222a",
    strip_prefix = "protobuf-3.2.0",
)
