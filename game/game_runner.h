#ifndef SANTORINI_GAME_GAME_RUNNER_H_
#define SANTORINI_GAME_GAME_RUNNER_H_

#include <functional>
#include <memory>

#include "game/player.h"

namespace santorini {

// Interface for observers. After every move, the observer will be called
// with the latest move as well as the current game state.
typedef std::function<void(const Board& board, int move)> ObserverFunc;

class GameRunner {
 public:
  explicit GameRunner(std::vector<std::unique_ptr<Player>> players);

  // Step the game by one move.
  // If there is a winner, returns the index of the winning player.
  // Otherwise, returns -1.
  int Step();

  // Play the game until completion.
  // Returns the index of the winning player.
  int Play();

  // Add an observer, will be called after every move.
  void AddObserver(ObserverFunc observer) { observers_.push_back(observer); }

  int current_turn() const { return turn_; }
  const Board& board() const { return board_; }

 private:
  Board board_;
  int turn_ = 0;
  std::vector<std::unique_ptr<Player>> players_;
  std::vector<ObserverFunc> observers_;
};

// Observer that prints the board to stdout.
ObserverFunc BoardPrintingObserver();

}  // namespace santorini

#endif