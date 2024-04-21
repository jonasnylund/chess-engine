#include <cstring>

#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

#include "board.h"
#include "moves.h"
#include "pieces.h"

namespace {

SquareIndex GetUncastledKingPos(const Board& b, int8_t rank) {
  for (int8_t file = 0; file < 8; file++) {
    if (b.Get(file, rank) & Piece::KING) {
      return {.file = file, .rank = rank};
    }
  }
  return {-1, -1};
}

std::tuple<SquareIndex, SquareIndex, Castling, Piece> GetMove(const Board& b) {
  std::string input;
  getline(std::cin, input);

  std::transform(input.begin(), input.end(), input.begin(),
    [](signed char c){ return std::tolower(c); });

  SquareIndex from;
  SquareIndex to;
  Castling castling = Castling::NO_CASTLING;
  int8_t kings_rank = b.WhiteToMove() ? 0 : 7;
  if (input == "o-o") {
    castling = Castling::KINGSIDE;
    from = GetUncastledKingPos(b, kings_rank);
    to = {.file = 'g' - 'a', .rank = kings_rank};
  }
  else if(input == "o-o-o") {
    castling = Castling::QUEENSIDE;
    from = GetUncastledKingPos(b, kings_rank);
    to = {.file = 'c' - 'a', .rank = kings_rank};
  }
  else {
    from = {
      .file = static_cast<int8_t>(input[0] - 'a'),
      .rank = static_cast<int8_t>(input[1] - '1'),
    };
    to = {
      .file = static_cast<int8_t>(input[2] - 'a'),
      .rank = static_cast<int8_t>(input[3] - '1'),
    };
  }

  Piece promotion = Piece::EMPTY;
  if (castling == Castling::NO_CASTLING && input.size() > 4) {
    Piece is_white = b.WhiteToMove() ? Piece::IS_WHITE : Piece::EMPTY;
    if (input.at(4) == 'b') {
      promotion = Piece::BISHOP | is_white;
    }
    else if (input.at(4) == 'n') {
      promotion = Piece::KNIGHT | is_white;
    }
    else if (input.at(4) == 'r') {
      promotion = Piece::ROOK | is_white;
    }
    else if (input.at(4) == 'q') {
      promotion = Piece::QUEEN | is_white;
    }
  }

  return {from, to, castling, promotion};
}

// For debugging
void PrintAvailableMoves(const Board& board) {
  const bool white_to_move = board.WhiteToMove();
  std::optional<SquareIndex> current = board.NextOccupied(
    {.file = -1, .rank = 0}, white_to_move);
  std::cout << current.has_value() << ", " << white_to_move << std::endl;
  while (current.has_value()) {
    const Piece piece = board.Get(current->file, current->rank);
    std::vector<Move> moves = PossibleMoves(board, piece, *current);

    std::cout << static_cast<char>(current->file + 'a') << current->rank + 1 << " ->";
    for (const Move& move : moves) {
      std::cout << " " << static_cast<char>(move.to.file + 'a') << move.to.rank + 1;
      if (move.promotion != Piece::EMPTY) {
        if (move.promotion & Piece::QUEEN)
          std::cout << "=q";
        else if (move.promotion & Piece::KNIGHT)
          std::cout << "=n";
      }
    }
    std::cout << std::endl;

    current = board.NextOccupied(*current, white_to_move);
  }
}

}  // namespace

int main(int argc, char** argv){
  std::string starting_pos;
  std::cout << "Starting position (leave empty for default):" << std::endl;
  getline(std::cin, starting_pos);
  if (starting_pos.empty()) {
    starting_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  }

  Board b = Board::FromFEN(starting_pos);

  while (true) {
    b.Print(std::cout);
    PrintAvailableMoves(b);

    const auto [from, to, castling, promotion] = GetMove(b);

    if (castling != Castling::NO_CASTLING) {
      b.Move(from, to, castling);
    }
    else if(promotion != Piece::EMPTY) {
      b.Move(from, to, promotion);
    }
    else {
      b.Move(from, to);
    }
  }

  return 0;
}