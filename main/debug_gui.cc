#include <GLFW/glfw3.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/check.h"
#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"

namespace {

void GlfwErrorCallback(int error, const char* description) {
  LOG(ERROR) << "GLFW Error [" << error << "]: " << description;
}

class GraphicsContext {
 public:
  GraphicsContext() {
    glfwSetErrorCallback(GlfwErrorCallback);
    CHECK(glfwInit());
    window_ = glfwCreateWindow(1280, 720, "santorini", nullptr, nullptr);
    CHECK(window_ != nullptr);
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);  // Enable vsync.

    // Setup Dear ImGui context.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends.
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL2_Init();
  }

  ~GraphicsContext() {
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  void RunLoop() {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window;    bool show_another_window;
    ImGuiIO& io = ImGui::GetIO();
    while (!glfwWindowShouldClose(window_)) {
      // Poll and handle events (inputs, window resize, etc.)
      glfwPollEvents();
      if (glfwGetWindowAttrib(window_, GLFW_ICONIFIED) != 0) {
        ImGui_ImplGlfw_Sleep(10);
        continue;
      }

      // Start the Dear ImGui frame
      ImGui_ImplOpenGL2_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // 2. Show a simple window that we create ourselves. We use a Begin/End
      // pair to create a named window.
      {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");  // Create a window called "Hello,
                                        // world!" and append into it.

        ImGui::Text("This is some useful text.");  // Display some text (you can
                                                   // use a format strings too)
        ImGui::Checkbox("Demo Window",
                        &show_demo_window);  // Edit bools storing our window
                                             // open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat(
            "float", &f, 0.0f,
            1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3(
            "clear color",
            (float*)&clear_color);  // Edit 3 floats representing a color

        if (ImGui::Button(
                "Button"))  // Buttons return true when clicked (most widgets
                            // return true when edited/activated)
          counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
      }

      // Rendering
      ImGui::Render();
      int display_w, display_h;
      glfwGetFramebufferSize(window_, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                   clear_color.z * clear_color.w, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

      glfwMakeContextCurrent(window_);
      glfwSwapBuffers(window_);
    }
  }

 private:
  GLFWwindow* window_;
};

}  // namespace

int main(int argc, char** argv) {
  // Initialize command line flags and logging.
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);

  GraphicsContext window;
  window.RunLoop();

  return 0;
}