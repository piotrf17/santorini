cc_library(
    name = "board",
    srcs = ["board.cc"],
    hdrs = ["board.h"],
    deps = [
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "board_test",
    srcs = ["board_test.cc"],
    deps = [
        ":board",
        "@com_google_absl//absl/log",
        "@com_google_googletest//:gtest_main",
    ],
)
