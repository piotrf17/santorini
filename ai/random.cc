#include "ai/random.h"

#include "absl/log/check.h"

namespace santorini {

int RandomAI::SelectMove(const Board& board) {
  std::vector<int> moves = board.PossibleMoveIds();
  CHECK(!moves.empty());
  return moves[rand() % moves.size()];
}

}  // namespace santorini