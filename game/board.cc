#include "game/board.h"

#include <cstring>

#include "absl/log/log.h"

namespace santorini {
namespace {

constexpr int kMoveMap[8][2] = {
    {-1, -1}, {-1, 0}, {-1, +1}, {0, -1}, {0, +1}, {+1, -1}, {+1, 0}, {+1, +1},
};

}  // namespace

Board::Board()
    : current_player_(0),
      workers_{
          {{1, 1}, {1, 3}},
          {{3, 1}, {3, 3}},
      } {
  std::memset(heights_, 0, kNumRows * kNumCols * sizeof(int));
  std::memset(worker_map_, 0, kNumRows * kNumCols * sizeof(bool));
  for (int player : {0, 1}) {
    for (int worker : {0, 1}) {
      worker_map_[workers_[player][worker][0]][workers_[player][worker][1]] =
          true;
    }
  }
}

std::vector<bool> Board::PossibleMoves() const {
  std::vector<bool> moves(128, false);
  int move_idx = 0;
  for (int worker : {0, 1}) {
    for (int move = 0; move < 8; ++move) {
      for (int build = 0; build < 8; ++build) {
        moves[move_idx] = ValidMove(worker, move, build);
        ++move_idx;
      }
    }
  }
  return moves;
}

bool Board::MakeMove(Move move) { return false; }

bool Board::ValidMove(int worker, int move, int build) const {
  const int worker_row = workers_[current_player_][worker][0];
  const int new_worker_row = worker_row + kMoveMap[move][0];
  const int build_row = new_worker_row + kMoveMap[build][0];
  const int worker_col = workers_[current_player_][worker][1];
  const int new_worker_col = worker_col + kMoveMap[move][1];
  const int build_col = new_worker_col + kMoveMap[build][1];

  // First, let's do a simple check if the moves are on the board.
  if (new_worker_row < 0 || new_worker_row >= kNumRows) return false;
  if (new_worker_col < 0 || new_worker_col >= kNumCols) return false;
  if (build_row < 0 || build_row >= kNumRows) return false;
  if (build_col < 0 || build_col >= kNumCols) return false;

  // We can't move on top of another worker.
  if (worker_map_[new_worker_row][new_worker_col]) return false;
  // We can't build on top of another worker (unless it is our original
  // position).
  if (worker_map_[build_row][build_col] &&
      !(build_row == worker_row && build_col == worker_col))
    return false;

  // We can only move to a lower spot, or a spot one higher.
  // We also can't move on top of a finished spot.
  const int current_height = heights_[worker_row][worker_col];
  const int new_height = heights_[new_worker_row][new_worker_col];
  if (current_height + 1 < new_height) return false;
  if (new_height == 4) return false;

  // We can't build on top of a finished spot.
  if (heights_[build_row][build_col] == 4) return false;

  return true;
}

}  // namespace santorini