#include "game/game_runner.h"

#include "absl/log/check.h"
#include "game/board.h"
#include "game/player.h"

namespace santorini {

GameRunner::GameRunner(std::vector<std::unique_ptr<Player>> players)
    : players_(std::move(players)) {
  CHECK_EQ(players_.size(), 2);
}

int GameRunner::Step() {
  int winner = -1;
  const int move = players_[board_.current_player()]->SelectMove(board_);
  CHECK(board_.MakeMove(move));
  for (auto& observer : observers_) {
    observer(board_, move);
  }
  winner = board_.winner();

  // If a player is out of moves, they lose.
  if (winner == -1 && board_.PossibleMoves().empty()) {
    winner = !board_.current_player();
  }

  ++turn_;

  return winner;
}

int GameRunner::Play() {
  int winner = -1;
  while (winner == -1) {
    winner = Step();
  }
  return winner;
}

ObserverFunc BoardPrintingObserver() {
  return [](const Board& board, int move) {
    printf("%s\n", MoveDebugString(move).c_str());
    board.Print();
  };
}

}  // namespace santorini