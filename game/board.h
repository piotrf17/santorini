#ifndef SANTORINI_GAME_BOARD_H_
#define SANTORINI_GAME_BOARD_H_

#include <string>
#include <vector>

namespace santorini {

// To simplify the implementation, the worker placements are fixed:
// _ _ _ _ _
// _ A _ A _
// _ _ _ _ _
// _ B _ B _
// _ _ _ _ _
// TODO(piotrf): support variable initial worker placement.
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

  // Returns a vector of 128 booleans, representing which of the 128
  // possible moves in any given turn are valid.
  std::vector<bool> PossibleMoves() const;

  // Print a colored view of the board to the console.
  void Print() const;

 private:
  bool ValidMove(int worker, int move, int build) const;

  int current_player_;
  int heights_[kNumRows][kNumCols];
  bool worker_map_[kNumRows][kNumCols];
  int workers_[2][2][2];  // (player, worker, row/column)
};

}  // namespace santorini

#endif