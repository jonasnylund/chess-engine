#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H

#include <cstdint>
#include <optional>
#include <string>

#include "pieces.h"

enum class Castling : uint8_t {
  NO_CASTLING = 0,
  KINGSIDE = 1,
  QUEENSIDE = 2,
};

constexpr enum Castling operator |(const enum Castling lhs, const enum Castling rhs) {
  return static_cast<Castling>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator &(const enum Castling lhs, const enum Castling rhs) {
  return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}
constexpr enum Castling operator ~(const enum Castling v) {
  return static_cast<Castling>(~static_cast<uint8_t>(v));
}

struct SquareIndex {
  int8_t file;
  int8_t rank;
};

// Holds the state of the board in the current position.
class Board {
 public:
  Board() = default;
  ~Board() = default;

  Board(const Board& board) = default;
  Board(Board&& board) = default;
  Board& operator=(const Board& board) = default;
  Board& operator=(Board&& board) = default;

  void Move(SquareIndex from, SquareIndex to, Piece promotion, Castling castling);

  // Returns the next unoccupied square after 
  std::optional<SquareIndex> NextOccupied(
    SquareIndex square, bool white) const;

  // Set up a position from a FEN notation string.
  static Board FromFEN(const std::string& fen);
  // Write the current position to FEN notation.
  std::string ToFEN() const;

  // Prints the game board to e.g. std::cout.
  void Print(std::ostream& stream) const;

  inline Piece Get(int8_t file, int8_t rank) const {
    return this->squares[rank][file];
  }
  inline bool WhiteToMove() const {
    return this->white_to_move;
  }
  inline SquareIndex KingsPosition(bool white) const {
    return this->kings_position[!white];
  }
  inline std::optional<SquareIndex> EnPassantSquare() const {
    return this->en_passent;
  }
  inline Castling CastlingAllowed(bool white) const {
    return this->castling[!white];
  }
  inline int8_t RookStartingFile(Castling side) const {
    if (side == Castling::KINGSIDE)
      return this->kingside_rook_start_file;
    else
      return this->queenside_rook_start_file;
  }
  inline int HalfmoveClock() const {
    return this->halfmove_clock;
  }

 private:
  Piece squares[8][8];
  Castling castling[2];
  SquareIndex kings_position[2];
  int8_t queenside_rook_start_file = 0;
  int8_t kingside_rook_start_file = 7;
  std::optional<SquareIndex> en_passent;
  bool white_to_move;
  int halfmove_clock = 0;
  int fullmove_clock = 0;
};

#endif