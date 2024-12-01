#include <memory>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "ai/mcts.h"
#include "ai/random.h"
#include "game/game_runner.h"

ABSL_FLAG(int, seed, -1, "Random number seed. If -1, use time.");
ABSL_FLAG(int, num_games, 10, "Number of games to play.");
ABSL_FLAG(bool, print_board, false, "Print the board during play.");

int main(int argc, char **argv) {
  // Initialize command line flags and logging.
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();

  absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);

  if (absl::GetFlag(FLAGS_seed) != -1) {
    srand(absl::GetFlag(FLAGS_seed));
  } else {
    srand(time(NULL));
  }

  int wins[2] = {0, 0};

  absl::Time start = absl::Now();
  const int num_games = absl::GetFlag(FLAGS_num_games);
  for (int i = 0; i < num_games; ++i) {
    std::vector<std::unique_ptr<santorini::Player>> players;
    players.push_back(std::make_unique<santorini::MctsAI>(
        0, santorini::MctsOptions{.c = 1.3,
                                  .num_iterations = 100000,
                                  .num_rollouts_per_iteration = 1,
                                  .num_threads = 1}));
    players.push_back(std::make_unique<santorini::MctsAI>(
        1, santorini::MctsOptions{.num_iterations = 200000,
                                  .num_rollouts_per_iteration = 1,
                                  .num_threads = 1}));
    santorini::GameRunner game_runner(std::move(players));

    if (absl::GetFlag(FLAGS_print_board)) {
      game_runner.AddObserver(santorini::BoardPrintingObserver());
    }

    const int winner = game_runner.Play();
    wins[winner] += 1;
  }
  absl::Time end = absl::Now();

  LOG(INFO) << "Played " << num_games << " games in " << (end - start);
  LOG(INFO) << "Wins: ";
  LOG(INFO) << "  player[0]=" << wins[0];
  LOG(INFO) << "  player[1]=" << wins[1];

  return 0;
}