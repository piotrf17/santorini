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

// A node in the game tree.
// Technically, this also includes edges going out from this node.
struct Node {
  std::string DebugString() const;

  // Whether or not this node has been expanded.
  bool expanded = false;

  // The move that caused us to arrive at this node, i.e. the incoming edge
  // to this node.
  int move = -1;

  // The player that played the above move.
  int player = -1;

  // The number of wins tracked for having made the above move.
  int wins = 0;

  // The number of times rollouts have visited the above move.
  int visits = 0;

  // The above move is a winning move. Note that in Santorini it is impossible
  // to make a move and lose immediately.
  bool terminal_win = false;

  Node* parent = nullptr;

  // Children are stored as shared_ptr to make it easier to make a copy of the
  // tree. This is done for expediency, unique_ptr would make this code less bug
  // prone.
  std::vector<std::shared_ptr<Node>> children;
};

// An AI player that uses Monte Carlo Tree Search (MCTS).
class MctsAI : public Player {
 public:
  MctsAI(int player_id, const MctsOptions& options = {});
  ~MctsAI();

  int SelectMove(const Board& board) override;

  std::shared_ptr<const Node> prev_tree() const { return prev_tree_; }
  int prev_move() const { return tree_->move; }

 private:
  void Iteration(Board board);

  int player_id_;
  MctsOptions options_;

  std::mutex tree_mutex_;
  std::shared_ptr<Node> tree_;
  std::shared_ptr<Node> prev_tree_;
};

}  // namespace santorini

#endif