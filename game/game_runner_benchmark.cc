// To run a benchmark:
//   $ bazel run -c opt game:game_runner_benchmark
//
// To profile a benchmark:
//   $ bazel build -c opt game:game_runner_benchmark
//   $ env CPUPROFILE=/tmp/benchmark.prof bazel-bin/game/game_runner_benchmark
//   $ pprof -http=":8000" bazel-bin/game/game_runner_benchmark \
//       /tmp/benchmark.prof

#include <memory>
#include <vector>

#include "ai/random.h"
#include "benchmark/benchmark.h"
#include "game/game_runner.h"

namespace santorini {
namespace {

// -----------------------------------------------------
// Benchmark           Time             CPU   Iterations
// -----------------------------------------------------
// BM_Rollout      77505 ns        77502 ns         9005 (initial)

static void BM_Rollout(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<std::unique_ptr<Player>> players;
    players.push_back(std::make_unique<RandomAI>());
    players.push_back(std::make_unique<RandomAI>());
    GameRunner game_runner(std::move(players));
    game_runner.Play();
  }
}
BENCHMARK(BM_Rollout);

}  // namespace
}  // namespace santorini

BENCHMARK_MAIN();