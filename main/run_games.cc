#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

ABSL_FLAG(int, seed, -1, "Random number seed. If -1, use time.");
ABSL_FLAG(int, num_games, 10, "Number of games to play.");

int main(int argc, char **argv) {
  // Initialize command line flags and logging.
  absl::ParseCommandLine(argc, argv);

  return 0;
}