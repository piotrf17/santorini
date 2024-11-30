#include "game/board.h"

#include "absl/flags/parse.h"
#include "absl/log/globals.h"
#include "absl/log/log.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace santorini {
namespace {

using ::testing::SizeIs;

int Id(int worker, int move, int build) {
  return (worker << 6) + (move << 3) + build;
}

bool CheckMoves(const std::vector<bool>& moves, int worker, int move,
                const std::vector<bool>& builds) {
  if (builds.size() != 8) return false;
  for (int build = 0; build < 8; ++build) {
    const int id = Id(worker, move, build);
    if (moves[id] != builds[build]) {
      LOG(ERROR) << "worker: " << worker << " move: " << move
                 << " build: " << build << " -- expected: " << builds[build]
                 << " actual: " << moves[id];
      return false;
    }
  }
  return true;
}

TEST(BoardTest, PossibleMoves_Start) {
  Board board;
  std::vector<bool> moves = board.PossibleMoves();
  ASSERT_THAT(moves, SizeIs(128));
  // Worker 0.
  EXPECT_TRUE(CheckMoves(moves, 0, 0, {0, 0, 0, 0, 1, 0, 1, 1}));
  EXPECT_TRUE(CheckMoves(moves, 0, 1, {0, 0, 0, 1, 1, 1, 1, 1}));
  EXPECT_TRUE(CheckMoves(moves, 0, 2, {0, 0, 0, 1, 1, 1, 1, 0}));
  EXPECT_TRUE(CheckMoves(moves, 0, 3, {0, 1, 1, 0, 1, 0, 1, 1}));
  EXPECT_TRUE(CheckMoves(moves, 0, 4, {1, 1, 1, 1, 0, 1, 1, 1}));
  EXPECT_TRUE(CheckMoves(moves, 0, 5, {0, 1, 1, 0, 1, 0, 1, 0}));
  EXPECT_TRUE(CheckMoves(moves, 0, 6, {1, 1, 1, 1, 1, 1, 0, 1}));
  EXPECT_TRUE(CheckMoves(moves, 0, 7, {1, 1, 0, 1, 1, 0, 1, 0}));
  // Worker 1.
  EXPECT_TRUE(CheckMoves(moves, 1, 0, {0, 0, 0, 1, 1, 0, 1, 1}));
  EXPECT_TRUE(CheckMoves(moves, 1, 1, {0, 0, 0, 1, 1, 1, 1, 1}));
  EXPECT_TRUE(CheckMoves(moves, 1, 2, {0, 0, 0, 1, 0, 1, 1, 0}));
  EXPECT_TRUE(CheckMoves(moves, 1, 3, {1, 1, 1, 0, 1, 1, 1, 1}));
  EXPECT_TRUE(CheckMoves(moves, 1, 4, {1, 1, 0, 1, 0, 1, 1, 0}));
  EXPECT_TRUE(CheckMoves(moves, 1, 5, {0, 1, 1, 1, 1, 0, 1, 0}));
  EXPECT_TRUE(CheckMoves(moves, 1, 6, {1, 1, 1, 1, 1, 1, 0, 1}));
  EXPECT_TRUE(CheckMoves(moves, 1, 7, {1, 1, 0, 1, 0, 0, 1, 0}));
}

TEST(BoardTest, PossibleMoves_MadeMoves) {
  Board board;

  ASSERT_TRUE(board.MakeMove(Id(0, 0, 4)));
  ASSERT_TRUE(board.MakeMove(Id(0, 5, 4)));

  // The state of the board:
  //
  // A  1  0  0  0
  // 0  0  0  A  0
  // 0  0  0  0  0
  // 0  0  0  B  0
  // B  1  0  0  0
  {
    std::vector<bool> moves = board.PossibleMoves();
    ASSERT_THAT(moves, SizeIs(128));
    EXPECT_TRUE(CheckMoves(moves, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(CheckMoves(moves, 0, 1, {0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(CheckMoves(moves, 0, 2, {0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(CheckMoves(moves, 0, 3, {0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(CheckMoves(moves, 0, 4, {0, 0, 0, 1, 1, 1, 1, 1}));
    EXPECT_TRUE(CheckMoves(moves, 0, 5, {0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(CheckMoves(moves, 0, 6, {0, 1, 1, 0, 1, 0, 1, 1}));
    EXPECT_TRUE(CheckMoves(moves, 0, 7, {1, 1, 1, 1, 1, 1, 1, 1}));
  }

  ASSERT_TRUE(board.MakeMove(Id(0, 6, 2)));
  ASSERT_TRUE(board.MakeMove(Id(0, 1, 7)));

  // The state of the board:
  //
  // 0  2  0  0  0
  // A  0  0  A  0
  // 0  0  0  0  0
  // B  0  0  B  0
  // 0  1  0  0  0
  {
    std::vector<bool> moves = board.PossibleMoves();
    EXPECT_TRUE(CheckMoves(moves, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(CheckMoves(moves, 0, 1, {0, 0, 0, 0, 1, 0, 1, 1}));
    EXPECT_TRUE(CheckMoves(moves, 0, 2, {0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(CheckMoves(moves, 0, 3, {0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(CheckMoves(moves, 0, 4, {1, 1, 1, 1, 1, 1, 1, 1}));
    EXPECT_TRUE(CheckMoves(moves, 0, 5, {0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(CheckMoves(moves, 0, 6, {0, 1, 1, 0, 1, 0, 0, 1}));
    EXPECT_TRUE(CheckMoves(moves, 0, 7, {1, 1, 1, 1, 1, 0, 1, 1}));
  }
}

}  // namespace
}  // namespace santorini

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  absl::SetGlobalVLogLevel(0);
  absl::ParseCommandLine(argc, argv);
  return RUN_ALL_TESTS();
}