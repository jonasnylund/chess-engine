#include "board.h"

#include <cassert>
#include <cstring>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>


namespace {

int8_t RookPosition(const Board& board, Castling side, int8_t rank) {
  if (side == Castling::QUEENSIDE) {
    for (int8_t i = 0; i < 8; i++) {
      if (board.Get(i, rank) & Piece::ROOK) {
        return i;
      }
    }
  }
  else {
    for (int8_t i = 7; i >=0; i--) {
      if (board.Get(i, rank) & Piece::ROOK) {
        return i;
      }
    }
  }
  return -1;
}

}  // namespace


void Board::Move(SquareIndex from, SquareIndex to, Piece promotion, Castling castling) {
  const bool is_pawn_move = this->squares[from.rank][from.file] & Piece::PAWN;
  const bool is_king_move = this->squares[from.rank][from.file] & Piece::KING;
  bool is_capturing_move = this->squares[to.rank][to.file] != Piece::EMPTY;

  if (
    is_pawn_move && 
    this->en_passent.has_value() &&
    to.file == this->en_passent->file && 
    to.rank == this->en_passent->rank
  ) {
    // En passent capture is complicated, as we don't capture on the
    // target square.
    const int8_t capture_file = to.file;
    const int8_t capture_rank = from.rank;
    is_capturing_move = true;
    this->squares[capture_rank][capture_file] = Piece::EMPTY;
  }
  else if (is_king_move && castling != Castling::NO_CASTLING) {
    is_capturing_move = false;
  }
  else if (
    is_capturing_move &&
    this->squares[to.rank][to.file] & Piece::ROOK &&
    this->castling[this->white_to_move] != Castling::NO_CASTLING
  ) {
    // Check if the captured piece revokes castling rights.
    // Starting rank of the captured rook.
    const int8_t starting_rank = this->white_to_move ? 7 : 0;
    if (
      to.rank == starting_rank &&
      to.file == this->kingside_rook_start_file
    ) {
      this->castling[this->white_to_move] = static_cast<Castling>(
        this->castling[this->white_to_move] & ~Castling::KINGSIDE);
    }
    else if (
      to.rank == starting_rank &&
      to.file == this->queenside_rook_start_file
    ) {
      this->castling[this->white_to_move] = static_cast<Castling>(
        this->castling[this->white_to_move] & ~Castling::QUEENSIDE);
    }
  }
  if (
    this->squares[from.rank][from.file] & Piece::ROOK &&
    this->castling[!this->white_to_move] != Castling::NO_CASTLING
  ) {
    // Rook move revokes castling rights to that side.
    const int8_t starting_rank = this->white_to_move ? 0 : 7;
    if (
      from.rank == starting_rank &&
      from.file == this->kingside_rook_start_file
    ) {
      this->castling[!this->white_to_move] = static_cast<Castling>(
        this->castling[!this->white_to_move] & ~Castling::KINGSIDE
      );
    }
    else if (
      from.rank == starting_rank &&
      from.file == this->queenside_rook_start_file
    ) {
      this->castling[!this->white_to_move] = static_cast<Castling>(
        this->castling[!this->white_to_move] & ~Castling::QUEENSIDE
      );
    }
  }

  // Move the piece. Castling requires special treatment.
  if (is_king_move && castling != Castling::NO_CASTLING) {
    const int8_t rook_file = (
      castling == Castling::KINGSIDE ? 
      this->kingside_rook_start_file : 
      this->queenside_rook_start_file
    );
    const int8_t rook_target_file = (
      castling == Castling::KINGSIDE ? to.file - 1 : to.file + 1
    );
    // Remove the rook
    Piece moving_rook = this->squares[from.rank][rook_file];
    this->squares[from.rank][rook_file] = Piece::EMPTY;
    // Move the king.
    this->squares[to.rank][to.file] = this->squares[from.rank][from.file];
    this->squares[from.rank][from.file] = Piece::EMPTY;
    // Place the rook after moving the king.
    this->squares[to.rank][rook_target_file] = moving_rook;
  }
  else {
    // Just move the piece to the new file.
    this->squares[to.rank][to.file] = this->squares[from.rank][from.file];
    this->squares[from.rank][from.file] = Piece::EMPTY;
  }

  if (is_pawn_move && promotion != Piece::EMPTY) {
    this->squares[to.rank][to.file] = promotion;
  }
  if (
    is_pawn_move &&
    to.file == from.file &&
    std::abs(to.rank - from.rank) == 2
  ) {
    // Pawn move 2 steps forward, need to set en passent flags.
    const SquareIndex ep = {.file = to.file, .rank = static_cast<int8_t>((to.rank + from.rank) / 2)};
    this->en_passent = ep;
  }
  else {
    this->en_passent = std::nullopt;
  }

  if (is_king_move) {
    // Castling no longer allowed.
    this->castling[!this->white_to_move] = Castling::NO_CASTLING;
  }

  if (is_capturing_move || is_pawn_move) {
    this->halfmove_clock = 0;
  }
  else {
    this->halfmove_clock++;
  }
  this->white_to_move = !this->white_to_move;
  if (this->white_to_move) {
    this->fullmove_clock++;
  }
}

Board Board::FromFEN(const std::string& fen) {
  Board board;
  memset(board.squares, 0, sizeof(board.squares));
  memset(board.castling, 0, sizeof(board.castling));

  int chars_read = 0;
  int8_t rank = 7;
  int8_t file = 0;

  // Read position of pieces.
  for (char c : fen) {
    chars_read++;

    if (c == '/') {
      rank--;
      file = 0;
      continue;
    }
    else if(std::isdigit(c)) {
      file += c - '0';
      continue;
    }
    else if (c == ' ') {
      // Position should be fully read, left is special state information.
      break;
    }
    assert(0 <= file && file < 8);
    assert(0 <= rank && rank < 8);

    const char ch = std::tolower(c);
    if (ch == 'p') {
      board.squares[rank][file] = Piece::PAWN;
    }
    else if (ch == 'n') {
      board.squares[rank][file] = Piece::KNIGHT;
    }
    else if(ch == 'b') {
      board.squares[rank][file] = Piece::BISHOP;
    }
    else if (ch == 'r') {
      board.squares[rank][file] = Piece::ROOK;
    }
    else if (ch == 'q') {
      board.squares[rank][file] = Piece::QUEEN;
    }
    else if (ch == 'k') {
      board.squares[rank][file] = Piece::KING;
    }
    if (std::isupper(c)) {
      board.squares[rank][file] = board.squares[rank][file] | Piece::IS_WHITE;
    }
    file++;
  }

  // Size to move
  if (fen.at(chars_read++) == 'w') {
    board.white_to_move = true;
  }
  else {
    board.white_to_move = false;
  }

  // Castling priviliges.
  while (++chars_read < fen.size()) {
    if (fen.at(chars_read) == 'K') {
      board.castling[0] = board.castling[0] | Castling::KINGSIDE;
    }
    else if (fen.at(chars_read) == 'Q') {
      board.castling[0] = board.castling[0] | Castling::QUEENSIDE;
    }
    else if (fen.at(chars_read) == 'k') {
      board.castling[1] = board.castling[1] | Castling::KINGSIDE;
    }
    else if (fen.at(chars_read) == 'q') {
      board.castling[1] = board.castling[1] | Castling::QUEENSIDE;
    }
    else if (fen.at(chars_read) == ' ') {
      break;
    }
    else {
      // Already at en passent, rewind.
      chars_read--;
      break;
    }
  }
  if (board.castling[0] & Castling::KINGSIDE) {
    board.kingside_rook_start_file = RookPosition(board, Castling::KINGSIDE, 0);
  }
  else if(board.castling[1] & Castling::KINGSIDE) {
    board.kingside_rook_start_file = RookPosition(board, Castling::KINGSIDE, 7);
  }
  if (board.castling[0] & Castling::QUEENSIDE) {
    board.queenside_rook_start_file = RookPosition(board, Castling::QUEENSIDE, 0);
  }
  else if(board.castling[1] & Castling::QUEENSIDE) {
    board.queenside_rook_start_file = RookPosition(board, Castling::QUEENSIDE, 7);
  }

  // En passént
  SquareIndex en_passent;
  chars_read++;
  if (fen.at(chars_read) == '-') {
    board.en_passent = std::nullopt;
    chars_read++;
  }
  else {
    SquareIndex en_passent;
    en_passent.file = fen.at(chars_read++) - 'a';
    en_passent.rank = fen.at(chars_read++) - '0';
    board.en_passent = en_passent;
  }

  // Move clocks.
  std::string clock;
  while (fen.at(++chars_read) != ' ') {
    clock.push_back(fen.at(chars_read));
  }
  board.halfmove_clock = stoi(clock);

  clock.clear();
  while (++chars_read < fen.size()) {
    clock.push_back(fen.at(chars_read));
  }
  board.fullmove_clock = stoi(clock);

  return board;
}

std::string Board::ToFEN() const {
  std::string output;
  output.reserve(64);

  // Write piece positions
  for (int rank = 7; rank >= 0; rank--) {
    int empty_count = 0;
    for (int file = 0; file < 8; file++) {
      if (this->squares[rank][file] == Piece::EMPTY) {
        empty_count++;
      }
      else if (empty_count > 0) {
        output.push_back('0' + empty_count);
        empty_count = 0;
      }

      const bool is_white = this->squares[rank][file] & Piece::IS_WHITE;
      const int add_case = is_white ? 'A' - 'a' : 0;

      if (this->squares[rank][file] & Piece::PAWN) {
        output.push_back('p' + add_case);
      }
      else if (this->squares[rank][file] & Piece::KNIGHT) {
        output.push_back('n' + add_case);
      }
      else if (this->squares[rank][file] & Piece::BISHOP) {
        output.push_back('b' + add_case);
      }
      else if (this->squares[rank][file] & Piece::ROOK) {
        output.push_back('r' + add_case);
      }
      else if (this->squares[rank][file] & Piece::QUEEN) {
        output.push_back('q' + add_case);
      }
      else if (this->squares[rank][file] & Piece::KING) {
        output.push_back('k' + add_case);
      }
    }
    if (empty_count > 0) {
      output.push_back('0' + empty_count);
    }
    if (rank > 0) {
      output.push_back('/');
    }
  }

  // Side to move.
  output.push_back(' ');
  if (this->white_to_move) {
    output.push_back('w');
  }
  else {
    output.push_back('b');
  }

  // Castling
  if (
    this->castling[0] != Castling::NO_CASTLING ||
    this->castling[1] != Castling::NO_CASTLING
  ) {
    output.push_back(' ');
  }
  if (this->castling[0] & Castling::KINGSIDE) {
    output.push_back('K');
  }
  if (this->castling[0] & Castling::QUEENSIDE) {
    output.push_back('Q');
  }
  if (this->castling[1] & Castling::KINGSIDE) {
    output.push_back('k');
  }
  if (this->castling[1] & Castling::QUEENSIDE) {
    output.push_back('q');
  }

  // En passent
  output.push_back(' ');
  if (this->en_passent.has_value()) {
    output.push_back('a' + this->en_passent->file);
    output.push_back('0' + this->en_passent->rank);
  }
  else{
    output.push_back('-');
  }

  // Clocks
  std::string clock;
  output.push_back(' ');
  clock = std::to_string(this->halfmove_clock);
  for (const char ch : clock) output.push_back(ch);

  output.push_back(' ');
  clock = std::to_string(this->fullmove_clock);
  for (const char ch : clock) output.push_back(ch);

  return output;
}

void Board::Print(std::ostream& stream) const {
  stream << " +-a-+-b-+-c-+-d-+-e-+-f-+-g-+-h-+" << std::endl;
  for (int8_t rank = 7; rank >= 0; rank--) {
    stream << rank + 1 << "| ";
    for (int8_t file = 0; file < 8; file++) {
      const bool is_white = this->squares[rank][file] & Piece::IS_WHITE;
      const int add_case = is_white ? 'A' - 'a' : 0;

      if (this->squares[rank][file] & Piece::PAWN) {
        stream << static_cast<char>('p' + add_case);
      }
      else if (this->squares[rank][file] & Piece::KNIGHT) {
        stream << static_cast<char>('n' + add_case);
      }
      else if (this->squares[rank][file] & Piece::BISHOP) {
        stream << static_cast<char>('b' + add_case);
      }
      else if (this->squares[rank][file] & Piece::ROOK) {
        stream << static_cast<char>('r' + add_case);
      }
      else if (this->squares[rank][file] & Piece::QUEEN) {
        stream << static_cast<char>('q' + add_case);
      }
      else if (this->squares[rank][file] & Piece::KING) {
        stream << static_cast<char>('k' + add_case);
      }
      else {
        stream << ' ';
      }
      stream << " | ";
    }
    stream << std::endl;
    stream << " +---+---+---+---+---+---+---+---+" << std::endl;
  }
  if (this->en_passent.has_value()) {
    stream << static_cast<char>(this->en_passent->file + 'a') << this->en_passent->rank + 1 << " ";
  }
  else {
    stream << "- ";
  }
  if (this->castling[0] & Castling::KINGSIDE) {
    stream << 'K';
  }
  if (this->castling[0] & Castling::QUEENSIDE) {
    stream << 'Q';
  }
  if (this->castling[1] & Castling::KINGSIDE) {
    stream << 'k';
  }
  if (this->castling[1] & Castling::QUEENSIDE) {
    stream << 'q';
  }
  stream << " " <<  this->halfmove_clock << ", " << this->fullmove_clock << std::endl;
  if (this->white_to_move) {
    stream << "White";
  }
  else {
    stream << "Black";
  }
  stream << " to move" << std::endl;
}