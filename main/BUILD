cc_binary(
    name = "debug_gui",
    srcs = ["debug_gui.cc"],
    linkopts = [
        "-lglfw",
        "-lGL",
    ],
    deps = [
        "//ai:mcts",
        "//game:game_runner",
        "//imgui",
        "//imgui:imgui_impl_glfw",
        "//imgui:imgui_impl_opengl2",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/log:flags",
        "@abseil-cpp//absl/log:initialize",
        "@abseil-cpp//absl/strings",
    ],
)

cc_binary(
    name = "run_games",
    srcs = ["run_games.cc"],
    linkopts = ["-lprofiler"],
    deps = [
        "//ai:mcts",
        "//ai:random",
        "//game:game_runner",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log:flags",
        "@abseil-cpp//absl/log:initialize",
    ],
)
