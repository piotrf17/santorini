package(default_visibility = ["//visibility:public"])

cc_library(
    name = "imgui",
    srcs = [
        "imconfig.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_tables.cpp",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
    ],
    hdrs = ["imgui.h"],
)

cc_library(
    name = "imgui_impl_glfw",
    srcs = ["imgui_impl_glfw.cpp"],
    hdrs = ["imgui_impl_glfw.h"],
    deps = [":imgui"],
)

cc_library(
    name = "imgui_impl_opengl2",
    srcs = ["imgui_impl_opengl2.cpp"],
    hdrs = ["imgui_impl_opengl2.h"],
    deps = [":imgui"],
)
