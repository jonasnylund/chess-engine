#include "board.h"

#include <cassert>
#include <cctype>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <optional>

Board Board::FromFEN(const std::string& fen) {
  Board board;
  memset(board.squares, 0, sizeof(board.squares));
  memset(board.castling, 0, sizeof(board.castling));

  int chars_read = 0;
  uint8_t rank = 7;
  uint8_t file = 0;

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
      board.castling[0] = board.castling[0] | Castling::SHORT;
    }
    else if (fen.at(chars_read) == 'Q') {
      board.castling[0] = board.castling[0] | Castling::LONG;
    }
    else if (fen.at(chars_read) == 'k') {
      board.castling[1] = board.castling[1] | Castling::SHORT;
    }
    else if (fen.at(chars_read) == 'q') {
      board.castling[1] = board.castling[1] | Castling::LONG;
    }
    else if (fen.at(chars_read) == ' ') {
      break;
    }
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

std::string Board::ToFEN() {
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

      constexpr int upper = 'A' - 'a';
      const bool is_white = this->squares[rank][file] & Piece::IS_WHITE;
      const int add_case = is_white ? upper : 0;

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
  output.push_back(' ');
  if (this->castling[0] & Castling::SHORT) {
    output.push_back('K');
  }
  if (this->castling[0] & Castling::LONG) {
    output.push_back('Q');
  }
  if (this->castling[1] & Castling::SHORT) {
    output.push_back('k');
  }
  if (this->castling[1] & Castling::LONG) {
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
