#include "game/game_runner.h"

#include "absl/log/check.h"
#include "game/board.h"
#include "game/player.h"

namespace santorini {

GameRunner::GameRunner(std::vector<std::unique_ptr<Player>> players)
    : players_(std::move(players)) {
  CHECK_EQ(players_.size(), 2);
}

int GameRunner::Play() {
  Board board;
  int winner = -1;
  while (winner == -1) {
    const int move = players_[board.current_player()]->SelectMove(board);
    CHECK(board.MakeMove(move));
    for (auto& observer : observers_) {
      observer(board, move);
    }
    winner = board.winner();

    // If a player is out of moves, they lose.
    if (winner == -1 && board.PossibleMoveIds().empty()) {
      winner = !board.current_player();
    }
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