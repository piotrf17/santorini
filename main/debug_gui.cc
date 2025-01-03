#include <GLFW/glfw3.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/check.h"
#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "ai/mcts.h"
#include "game/game_runner.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"

ABSL_FLAG(int, seed, -1, "Random number seed. If -1, use time.");

namespace santorini {

void GlfwErrorCallback(int error, const char* description) {
  LOG(ERROR) << "GLFW Error [" << error << "]: " << description;
}

class GraphicsContext {
 public:
  GraphicsContext() {
    glfwSetErrorCallback(GlfwErrorCallback);
    CHECK(glfwInit());
    window_ = glfwCreateWindow(1700, 1000, "santorini", nullptr, nullptr);
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

  void RunLoop(std::function<void()> main_fn) {
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

      main_fn();

      // Rendering
      ImGui::Render();
      int display_w, display_h;
      glfwGetFramebufferSize(window_, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

      glfwMakeContextCurrent(window_);
      glfwSwapBuffers(window_);
    }
  }

 private:
  GLFWwindow* window_;
};

void DrawBoard(const Board& board, const double grid_size) {
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  const ImVec2 start_p = ImGui::GetCursorScreenPos();
  for (int row = 0; row < Board::kNumRows; ++row) {
    for (int col = 0; col < Board::kNumCols; ++col) {
      ImVec2 p(start_p.x + grid_size * col, start_p.y + grid_size * row);
      const int h = board.height(row, col);
      draw_list->AddRect(ImVec2(p.x, p.y),
                         ImVec2(p.x + grid_size, p.y + grid_size),
                         IM_COL32(255, 255, 255, 255));
      if (h >= 1) {
        draw_list->AddRectFilled(
            ImVec2(p.x + 0.05 * grid_size, p.y + 0.05 * grid_size),
            ImVec2(p.x + 0.95 * grid_size, p.y + 0.95 * grid_size),
            IM_COL32(80, 80, 80, 255));
      }
      if (h >= 2) {
        draw_list->AddRectFilled(
            ImVec2(p.x + 0.1 * grid_size, p.y + 0.1 * grid_size),
            ImVec2(p.x + 0.9 * grid_size, p.y + 0.9 * grid_size),
            IM_COL32(160, 160, 160, 255));
      }
      if (h >= 3) {
        draw_list->AddRectFilled(
            ImVec2(p.x + 0.15 * grid_size, p.y + 0.15 * grid_size),
            ImVec2(p.x + 0.85 * grid_size, p.y + 0.85 * grid_size),
            IM_COL32(255, 255, 255, 255));
      }
      if (h >= 4) {
        draw_list->AddCircleFilled(
            ImVec2(p.x + 0.5 * grid_size, p.y + 0.5 * grid_size),
            0.3 * grid_size, IM_COL32(0, 0, 255, 255));
      }
    }
  }
  for (int player = 0; player < 2; ++player) {
    for (int worker = 0; worker < 2; ++worker) {
      const int* r_c = board.worker(player, worker);
      const ImVec2 center(start_p.x + grid_size * r_c[1] + 0.5 * grid_size,
                          start_p.y + grid_size * r_c[0] + 0.5 * grid_size);
      draw_list->AddCircleFilled(
          center, grid_size * 0.25,
          player == 0 ? IM_COL32(255, 0, 0, 255) : IM_COL32(0, 255, 0, 255));
      draw_list->AddText(
          /*font=*/nullptr,
          /*font_size=*/grid_size * 0.25,
          ImVec2(center.x - 0.05 * grid_size, center.y - 0.15 * grid_size),
          IM_COL32(0, 0, 0, 255), worker == 0 ? "0" : "1");
    }
  }
}

void AddMctsNodes(const Node* node, int selected_move = -1) {
  if (node == nullptr) return;
  std::string label = node->DebugString();
  if (node->move == selected_move) {
    label = absl::StrCat("[SELECTED] ", label);
  }
  if (ImGui::TreeNode(label.c_str())) {
    for (const auto& child : node->children) {
      AddMctsNodes(child.get());
    }
    ImGui::TreePop();
  }
}

void AddMctsWindow(const std::string& title, const MctsAI& mcts_ai) {
  ImGui::Begin(title.c_str());
  const Node* tree = mcts_ai.prev_tree().get();
  if (tree != nullptr) {
    if (ImGui::TreeNodeEx(tree->DebugString().c_str(),
                          ImGuiTreeNodeFlags_DefaultOpen)) {
      for (const auto& child : tree->children) {
        AddMctsNodes(child.get(), mcts_ai.prev_move());
      }
      ImGui::TreePop();
    }
  }
  ImGui::End();
}

}  // namespace santorini

int main(int argc, char** argv) {
  // Initialize command line flags and logging.
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);
  if (absl::GetFlag(FLAGS_seed) != -1) {
    srand(absl::GetFlag(FLAGS_seed));
  } else {
    srand(time(NULL));
  }

  std::vector<std::unique_ptr<santorini::Player>> players;
  auto player1_owned = std::make_unique<santorini::MctsAI>(
      0, santorini::MctsOptions{.c = 1.3,
                                .num_iterations = 100000,
                                .num_rollouts_per_iteration = 1,
                                .num_threads = 1});
  const santorini::MctsAI* player1 = player1_owned.get();
  players.push_back(std::move(player1_owned));
  auto player2_owned = std::make_unique<santorini::MctsAI>(
      1, santorini::MctsOptions{.c = 1.3,
                                .num_iterations = 100000,
                                .num_rollouts_per_iteration = 1,
                                .num_threads = 1});
  const santorini::MctsAI* player2 = player2_owned.get();
  players.push_back(std::move(player2_owned));
  santorini::GameRunner game_runner(std::move(players));

  santorini::GraphicsContext window;
  int winner = -1;
  window.RunLoop([&]() {
    ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
    ImGui::SetNextWindowSize(ImVec2(300.0, 400.0));
    ImGui::Begin("Game");
    ImGui::Text("Current turn: %d", game_runner.current_turn());
    ImGui::Text("Winner: %d", winner);
    if (ImGui::Button("Next Turn") && winner == -1) {
      winner = game_runner.Step();
    }
    DrawBoard(game_runner.board(), 50.0);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(300.0, 0.0));
    ImGui::SetNextWindowSize(ImVec2(700.0, 1000.0));
    AddMctsWindow("Player 1", *player1);

    ImGui::SetNextWindowPos(ImVec2(1000.0, 0.0));
    ImGui::SetNextWindowSize(ImVec2(700.0, 1000.0));
    AddMctsWindow("Player 2", *player2);
  });

  return 0;
}