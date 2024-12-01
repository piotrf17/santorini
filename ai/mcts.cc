#include "ai/mcts.h"

#include <atomic>
#include <thread>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"
#include "game/board.h"

namespace santorini {

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

  Node* parent = nullptr;
  std::vector<std::unique_ptr<Node>> children;
};

std::string Node::DebugString() const {
  if (move == -1) {
    return "uninitialized";
  }

  const double win_rate = visits > 0 ? static_cast<float>(wins) / visits : 0.0;
  return absl::StrFormat("(%.3f %d/%d), %d children, %s", win_rate, wins,
                         visits, children.size(), MoveDebugString(move));
}

namespace {

bool ShouldExpand(const Board& board, const Node& node) {
  if (board.winner() != -1) return false;
  CHECK(node.parent != nullptr);
  CHECK(!node.parent->children.empty());
  return node.parent->visits >= node.parent->children.size();
}

void ExpandNode(const Board& board, Node* node) {
  CHECK(!node->expanded) << "Expanding a non-leaf node: "
                         << node->DebugString();
  node->expanded = true;

  // Look for possible moves, and if found, create a child for each move.
  const std::vector<int> possible_moves = board.PossibleMoveIds();
  node->children.reserve(possible_moves.size());
  for (int move : possible_moves) {
    auto child_node = std::make_unique<Node>();
    child_node->move = move;
    child_node->player = board.current_player();
    child_node->parent = node;
    node->children.push_back(std::move(child_node));
  }
}

// Returns a pointer to a leaf-node in the game tree starting from `node`.
// The `board` is modified to reflect the state as moves are made following
// the nodes recursively down.
//
// A leaf node is defined as a node that has no children.
//
// Note that this function also includes the "expansion" phase in usual
// MCTS terminology. A leaf node is only expanded if all siblings have been
// visited at least once. After expansion, we return a child.
Node* SelectNode(Node* node, Board* board, double c) {
  // If a leaf node, possibly expand it and continue selection.
  if (node->children.empty()) {
    if (!ShouldExpand(*board, *node)) {
      return node;
    }
    ExpandNode(*board, node);
  }
  // xxx handle this condition
  CHECK_GT(node->children.size(), 0);

  // Pick the best child by UCB1 and recurse.
  std::vector<double> ucb1(node->children.size(),
                           std::numeric_limits<double>::infinity());
  const double logN = std::log(node->visits);
  for (size_t i = 0; i < node->children.size(); ++i) {
    const Node& child = *node->children[i];
    if (child.visits == 0) continue;
    ucb1[i] =
        1.0 * child.wins / child.visits + c * std::sqrt(logN / child.visits);
  }

  // If there are multiple children with the max UCB1, then select randomly.
  // This prevents biasing towards certain moves at the start of expansion.
  double max_ucb1 = *std::max_element(ucb1.begin(), ucb1.end());
  std::vector<int> children_with_max;
  for (size_t i = 0; i < node->children.size(); ++i) {
    if (ucb1[i] == max_ucb1) {
      children_with_max.push_back(i);
    }
  }
  int selected_child = children_with_max[rand() % children_with_max.size()];

  CHECK(board->MakeMove(node->children[selected_child]->move))
      << "SelectNode tried "
      << MoveDebugString(node->children[selected_child]->move);

  return SelectNode(node->children[selected_child].get(), board, c);
}

int Rollout(Board board) {
  while (board.winner() == -1) {
    const std::vector<int> possible_moves = board.PossibleMoveIds();
    if (possible_moves.empty()) {
      return !board.current_player();
    }
    const int move = possible_moves[rand() % possible_moves.size()];
    CHECK(board.MakeMove(move));
  }
  return board.winner();
}

}  // namespace

MctsAI::MctsAI(int player_id, const MctsOptions& options)
    : player_id_(player_id),
      options_(options),
      tree_(std::make_unique<Node>()) {}

MctsAI::~MctsAI() {}

void MctsAI::Iteration(Board board) {
  // Select and possibly expand a node.
  Node* node = nullptr;
  {
    std::lock_guard<std::mutex> lock(tree_mutex_);
    node = SelectNode(tree_.get(), &board, options_.c);
  }

  // Run rollouts on the selected node.
  for (int i = 0; i < options_.num_rollouts_per_iteration; ++i) {
    VLOG(3) << "  MCTS running rollout " << i;
    int winner = Rollout(board);
    VLOG(3) << "   rollout winner is " << winner;

    // Bookkeeping on the winner.
    std::lock_guard<std::mutex> lock(tree_mutex_);
    Node* update_node = node;
    CHECK(update_node->parent != nullptr);
    while (update_node != nullptr) {
      update_node->visits++;
      if (update_node->player == winner) {
        update_node->wins++;
      }
      update_node = update_node->parent;
    }
  }
}

int MctsAI::SelectMove(const Board& board) {
  // Unless this is the first move, update tree based on the opponent's move.
  if (!board.past_moves().empty()) {
    const int last_move = board.past_moves().back();
    VLOG(1) << "MCTS updating tree for move " << MoveDebugString(last_move);
    VLOG(1) << " previous tree_: " << tree_->DebugString();
    CHECK(!tree_->children.empty());
    bool found_match = false;
    for (std::unique_ptr<Node>& child : tree_->children) {
      VLOG(2) << "  child: " << child->DebugString();
      if (child->move == last_move) {
        VLOG(2) << "    Match found, stopping.";
        found_match = true;
        tree_ = std::move(child);
        tree_->parent = nullptr;
        break;
      }
    }
    CHECK(found_match);
  }

  // Expand out the root, in case we didn't find it above.
  if (!tree_->expanded) {
    ExpandNode(board, tree_.get());
  }
  CHECK_GT(tree_->children.size(), 0);
  VLOG(1) << "current tree_: " << tree_->DebugString();

  // If there is only a single move available, take it. In theory, we could
  // spend some time planning for future moves, but:
  //   1) we're not playing in a timed environment.
  //   2) it's rare that a single move will lead to many future moves.
  if (tree_->children.size() == 1) {
    tree_ = std::move(tree_->children[0]);
    return tree_->move;
  }

  // Run MCTS iterations.
  {
    // Launch all worker threads.
    std::vector<std::thread> workers;
    std::atomic<int> counter(0);
    for (int i = 0; i < options_.num_threads; ++i) {
      workers.emplace_back([&]() {
        while (true) {
          if (counter.fetch_add(1) >= options_.num_iterations) return;
          Iteration(board);
        }
      });
    }
    // Join worker threads.
    for (std::thread& worker : workers) {
      worker.join();
    }
  }

  // Pick the best move.
  VLOG(1) << "MCTS picking from " << tree_->children.size() << " moves.";
  int max_visits = 0;
  std::unique_ptr<Node>* best_child = nullptr;
  for (std::unique_ptr<Node>& child : tree_->children) {
    VLOG(2) << child->DebugString();
    if (child->visits > max_visits) {
      max_visits = child->visits;
      best_child = &child;
    }
  }
  CHECK(best_child != nullptr);
  VLOG(0) << "player " << player_id_ << " estimate of winning = "
          << static_cast<double>((*best_child)->wins) / (*best_child)->visits;
  tree_ = std::move(*best_child);
  return tree_->move;
}

}  // namespace santorini