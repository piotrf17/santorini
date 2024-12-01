package(default_visibility = ["//visibility:public"])

cc_library(
    name = "board",
    srcs = ["board.cc"],
    hdrs = ["board.h"],
    deps = [
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/strings",
    ],
)

cc_test(
    name = "board_test",
    srcs = ["board_test.cc"],
    deps = [
        ":board",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:vlog_is_on",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "game_runner",
    srcs = ["game_runner.cc"],
    hdrs = ["game_runner.h"],
    deps = [
        ":board",
        ":player",
        "@abseil-cpp//absl/log:check",
    ],
)

cc_library(
    name = "player",
    hdrs = ["player.h"],
    deps = [
        ":board",
    ],
)
