cc_binary(
    name = "run_games",
    srcs = ["run_games.cc"],
    linkopts = ["-lprofiler"],
    deps = [
        "//ai:random",
        "//game:game_runner",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log:flags",
        "@abseil-cpp//absl/log:initialize",
    ],
)
