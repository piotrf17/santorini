package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mcts",
    srcs = ["mcts.cc"],
    hdrs = ["mcts.h"],
    deps = [
        "//game:board",
        "//game:player",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/log:log",
        "@abseil-cpp//absl/strings:str_format",
    ],
)

cc_library(
    name = "random",
    srcs = ["random.cc"],
    hdrs = ["random.h"],
    deps = [
        "//game:player",
        "@abseil-cpp//absl/log:check",
    ],
)
