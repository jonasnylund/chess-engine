
#include <cstdint>
#include <optional>

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

enum class Castling : uint8_t {
  NO_CASTLING = 0,
  SHORT = 1,
  LONG = 2,
};

constexpr enum Castling operator |(const enum Castling lhs, const enum Castling rhs) {
  return static_cast<Castling>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator &(const enum Castling lhs, const enum Castling rhs) {
  return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}

struct SquareIndex {
  uint8_t file;
  uint8_t rank;
};

class Board {
 public:
  Board() = default;
  ~Board() = default;

  // Set up a position from a FEN notation string.
  static Board FromFEN(const std::string& fen);
  // Write the current position to FEN notation.
  std::string ToFEN();

 private:
  Piece squares[8][8];
  Castling castling[2];
  std::optional<SquareIndex> en_passent;
  bool white_to_move;
  int halfmove_clock = 0;
  int fullmove_clock = 0;
};
