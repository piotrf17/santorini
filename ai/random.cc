#include "ai/random.h"

#include "absl/log/check.h"

namespace santorini {

int RandomAI::SelectMove(const Board& board) {
  std::vector<bool> moves = board.PossibleMoves();
  std::vector<int> valid_moves;
  valid_moves.reserve(moves.size());
  for (int i = 0; i < moves.size(); ++i) {
    if (moves[i]) {
      valid_moves.push_back(i);
    }
  }
  CHECK(!valid_moves.empty()) << "oops";
  return valid_moves[rand() % valid_moves.size()];
}

}  // namespace santorini