#ifndef SANTORINI_AI_RANDOM_H_
#define SANTORINI_AI_RANDOM_H_

#include "game/board.h"
#include "game/player.h"

namespace santorini {

class RandomAI : public Player {
 public:
  int SelectMove(const Board& board) override;
};

}  // namespace santorini

#endif