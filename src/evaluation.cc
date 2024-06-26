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

int CountPieces(const Board* board) {
  int value = 0;

  for (int8_t file = 0; file < 8; file++) {
    for (int8_t rank = 0; rank < 8; rank++) {
      const Piece piece = board->Get(file, rank);
      if (piece == Piece::EMPTY) {
        continue;
      }
      const int sign = piece & Piece::IS_WHITE ? 1 : -1;
      if (piece & Piece::PAWN) {
        value += sign * kPawn;
      }
      else if (piece & Piece::KNIGHT) {
        value += sign * kKnight;
      }
      else if (piece & Piece::BISHOP) {
        value += sign * kBishop;
      }
      else if (piece & Piece::ROOK) {
        value += sign * kRook;
      }
      else if (piece & Piece::QUEEN) {
        value += sign * kQueen;
      }
    }
  }

  return value;
}

int Qiecence(MoveIterator& iterator, const int depth, int alpha, int beta) {
  int num_moves = 0;
  const Board* source_position = iterator.SourcePosition();
  const bool white_to_move = source_position->WhiteToMove();

  if (source_position->HalfmoveClock() >= 50) {
    // Draw by 50-move rule.
    return 0;
  }

  int min_max;
  if (white_to_move) {
    min_max = std::numeric_limits<int>::min();
    while (const std::optional<Move> move = iterator.Next(false, true, depth < 4)) {
      MoveIterator next = iterator.ContinuePosition();
      const int eval = Qiecence(next, depth + 1, alpha, beta);
      min_max = eval > min_max ? eval : min_max;
      if (min_max >= beta)
        return min_max;
      alpha = min_max > alpha ? min_max : alpha;
      num_moves++;
    }
  }
  else {
    min_max = std::numeric_limits<int>::max();
    while (const std::optional<Move> move = iterator.Next(false, true, depth < 4)) {
      MoveIterator next = iterator.ContinuePosition();
      const int eval = Qiecence(next, depth + 1, alpha, beta);
      min_max = eval < min_max ? eval : min_max;
      if (min_max <= alpha)
        return min_max;
      beta = min_max < beta ? min_max : beta; 
      num_moves++;
    }
  }

  if (num_moves == 0) {
    const SquareIndex king = source_position->KingsPosition(white_to_move);
    const bool is_in_check = IsAttacked(*source_position, king, !white_to_move);

    if (is_in_check) {
      // King is in check, and we have no moves. This is checkmate
      return (white_to_move ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max());
    }
    else {
      return CountPieces(source_position);
    }
  }
  return min_max;
}


int Evaluate(MoveIterator& iterator, const int depth, int alpha, int beta) {
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
    return Qiecence(iterator, 0, alpha, beta);
  }

  int min_max;
  if (white_to_move) {
    min_max = std::numeric_limits<int>::min();
    while (const std::optional<Move> move = iterator.Next(true, true, true)) {
      MoveIterator next = iterator.ContinuePosition();
      const int eval = Evaluate(next, depth - 1, alpha, beta);
      min_max = eval > min_max ? eval : min_max;
      if (min_max >= beta)
        return min_max;
      alpha = min_max > alpha ? min_max : alpha;
      num_moves++;
    }
  }
  else {
    min_max = std::numeric_limits<int>::max();
    while (const std::optional<Move> move = iterator.Next(true, true, true)) {
      MoveIterator next = iterator.ContinuePosition();
      const int eval = Evaluate(next, depth - 1, alpha, beta);
      min_max = eval < min_max ? eval : min_max;
      if (min_max <= alpha)
        return min_max;
      beta = min_max < beta ? min_max : beta; 
      num_moves++;
    }
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
  const int alpha = std::numeric_limits<int>::min();
  const int beta = std::numeric_limits<int>::max();
  return Evaluate(iterator, depth, alpha, beta);
}

