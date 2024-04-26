#include "evaluation.h"

#include <optional>

#include "board.h"
#include "moves.h"
#include "pieces.h"

namespace {

constexpr int kPawn = 100;
constexpr int kKnight = 300;
constexpr int kBishop = 300;
constexpr int kRook = 500;
constexpr int kQueen = 800;

int EvalSide(const Board* board, bool white) {
  int value = 0;

  std::optional<SquareIndex> current_square = board->NextOccupied(
    {.file = -1, .rank = 0}, white);

  while (current_square.has_value()) {
    const Piece piece = board->Get(current_square->file, current_square->rank);

    if (piece & Piece::PAWN) {
      value += kPawn;
    }
    else if (piece & Piece::KNIGHT) {
      value += kKnight;
    }
    else if (piece & Piece::BISHOP) {
      value += kBishop;
    }
    else if (piece & Piece::ROOK) {
      value += kRook;
    }
    else if (piece & Piece::QUEEN) {
      value += kQueen;
    }
    current_square = board->NextOccupied(*current_square, white);
  }

  return value;
}

int Evaluate(MoveIterator& iterator, int depth) {
  int num_moves = 0;
  const Board* source_position = iterator.SourcePosition();
  const bool white_to_move = source_position->WhiteToMove();

  if (source_position->HalfmoveClock() >= 50) {
    // Draw by 50-move rule.
    return 0;
  }
  if (depth <= 0) {
    // TODO: We should do a search for a quiet position before counting up the
    // pieces, if there are forced lines continuing.
    return EvalSide(source_position, true) - EvalSide(source_position, false);
  }

  int min_max = (white_to_move ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max());
  while (const std::optional<Move> move = iterator.Next()) {
    MoveIterator next = iterator.ContinuePosition();
    const int eval = Evaluate(next, depth - 1);
    min_max = (eval > min_max) == white_to_move ? eval : min_max;
    num_moves++;
  }

  if (num_moves == 0) {
    const SquareIndex king = source_position->KingsPosition(white_to_move);
    const bool is_in_check = IsAttacked(*source_position, king, !white_to_move);

    if (is_in_check) {
      // King is in check, and we have no moves. This is checkmate
      return (white_to_move ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max());
    }
    else {
      // Stalemate, it's a draw.
      return 0;
    }
  }
  return min_max;
}

}  // namespace

int Evaluate(const Board* board, int depth) {
  MoveIterator iterator(*board);
  return Evaluate(iterator, depth);
}

