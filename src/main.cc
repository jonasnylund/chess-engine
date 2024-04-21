
#include <iostream>
#include <cstring>
#include "board.h"

int main(int argc, char** argv){
  const std::string default_start_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - 0 1";

  Board b = Board::FromFEN(default_start_pos);
  b.Print(std::cout);
  return 0;
}