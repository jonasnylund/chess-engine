#ifndef CHESSENGINE_PIECE_H
#define CHESSENGINE_PIECE_H

#include <cstdint>
#include <optional>
#include <string>

enum class Piece : uint8_t {
  EMPTY = 0,
  PAWN = 1,
  KNIGHT = 2,
  BISHOP = 4,
  ROOK = 8,
  QUEEN = 16,
  KING = 32,
  IS_WHITE = 64,
};

constexpr enum Piece operator |(const enum Piece lhs, const enum Piece rhs) {
  return static_cast<Piece>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator &(const enum Piece lhs, const enum Piece rhs) {
  return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}

#endif