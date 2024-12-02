#include "ai/random.h"

#include "absl/log/check.h"

namespace santorini {

int RandomAI::SelectMove(const Board& board) {
  std::vector<Board::Move> moves = board.PossibleMoves();
  CHECK(!moves.empty());
  return moves[rand() % moves.size()].move_id;
}

}  // namespace santorini