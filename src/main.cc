
#include <iostream>
#include <cstring>
#include "board.h"

int main(int argc, char** argv){
  const std::string default_start_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  Board b = Board::FromFEN(default_start_pos);
  std::cout << b.ToFEN() << std::endl;
  return 0;
}