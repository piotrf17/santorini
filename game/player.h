#ifndef SANTORINI_PLAYER_H_
#define SANTORINI_PLAYER_H_

#include "game/board.h"

namespace santorini {

class Player {
 public:
  virtual int SelectMove(const Board& board) = 0;
};

}  // namespace santorini

#endif