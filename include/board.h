
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

class Board {
 public:
  Board() = default;
  ~Board() = default;

  inline void Move(SquareIndex from, SquareIndex to) {
    Move(from, to, Piece::EMPTY, Castling::NO_CASTLING);
  }
  inline void Move(SquareIndex from, SquareIndex to, Piece promotion) {
    Move(from, to, promotion, Castling::NO_CASTLING);
  }
  inline void Move(SquareIndex from, SquareIndex to, Castling castling) {
    Move(from, to, Piece::EMPTY, castling);
  }

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

 private:
  void Move(SquareIndex from, SquareIndex to, Piece promotion, Castling castling);

  Piece squares[8][8];
  Castling castling[2];
  int8_t queenside_rook_start_file = 0;
  int8_t kingside_rook_start_file = 7;
  std::optional<SquareIndex> en_passent;
  bool white_to_move;
  int halfmove_clock = 0;
  int fullmove_clock = 0;
};
