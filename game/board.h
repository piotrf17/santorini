#ifndef SANTORINI_GAME_BOARD_H_
#define SANTORINI_GAME_BOARD_H_

#include <string>
#include <vector>

namespace santorini {

std::string MoveDebugString(int move_id);

// To simplify the implementation, the worker placements are fixed:
// _ _ _ _ _
// _ A _ A _
// _ _ _ _ _
// _ B _ B _
// _ _ _ _ _
// TODO(piotrf): support variable initial worker placement.
// TODO(piotrf): winning move doesn't need a valid build
class Board {
 public:
  static const int kNumRows = 5;
  static const int kNumCols = 5;

  Board();

  // Move a worker and build. Returns true if the move was valid.
  // An invalid move will not change the state of the board.
  //
  // A move is identified by an integer from 0 to 127.
  //
  // The bits of this identifier are (from highest to lowest)
  // 1 bit  -- identifiying the worker
  // 3 bits -- the move to make
  // 3 bits -- the location to build
  //
  // Both "move" and "build" are from 0-7, chosing an adjacent square:
  //  0  1  2
  //  3  x  4
  //  5  6  7
  //
  // Note that "build" is relative to the location after "move" is applied.
  bool MakeMove(int move_id);

  // Returns a vector of possible moves.
  struct Move {
    int move_id;
    bool is_winning;
  };
  std::vector<Move> PossibleMoves() const;

  // Returns a vector of 128 booleans, representing which of the 128
  // possible moves in any given turn are valid.
  std::vector<bool> PossibleMoveMask() const;

  // Print a colored view of the board to the console.
  void Print() const;

  // Returns -1 if there is no winner yet, else the index of the
  // winning player.
  int winner() const { return winner_; }

  int current_player() const { return current_player_; }

  const std::vector<int> past_moves() const { return past_moves_; }

  int height(int row, int col) const { return heights_[row][col]; }
  const int* worker(int player, int worker) const {
    return workers_[player][worker];
  }

 private:
  bool ValidMove(int worker, int move, int build, bool* is_winning) const;

  int current_player_;
  std::vector<int> past_moves_;
  int heights_[kNumRows][kNumCols];
  bool worker_map_[kNumRows][kNumCols];
  int workers_[2][2][2];  // (player, worker, row/column)
  int winner_ = -1;
};

}  // namespace santorini

#endif