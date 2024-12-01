#ifndef SANTORINI_AI_BLOKUS_H_
#define SANTORINI_AI_BLOKUS_H_

#include <memory>
#include <mutex>

#include "game/board.h"
#include "game/player.h"

namespace santorini {

struct MctsOptions {
  // The exploration parameter for UCB1.
  // Setting this higher favors exploration more over exploitation.
  // Theory dictates that this should be approximately sqrt(2).
  double c = 1.4;

  // The number of iterations of MCTS to run per move, with each iteration
  // consisting of selection of a leaf node, expansion of that node, rollout,
  // and backpropogation of rollout results.
  int num_iterations = 10000;

  // The number of random rollouts to run per MCTS iteration.
  int num_rollouts_per_iteration = 1;

  // The number of parallel threads that are running iterations.
  int num_threads = 1;
};

struct Node;

// An AI player that uses Monte Carlo Tree Search (MCTS).
class MctsAI : public Player {
 public:
  MctsAI(int player_id, const MctsOptions& options = {});
  ~MctsAI();

  int SelectMove(const Board& board) override;

 private:
  void Iteration(Board board);

  int player_id_;
  MctsOptions options_;

  std::mutex tree_mutex_;
  std::unique_ptr<Node> tree_;
};

}  // namespace santorini

#endif