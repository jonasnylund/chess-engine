#ifndef CHESSENGINE_MOVES_H
#define CHESSENGINE_MOVES_H

#include <optional>
#include <vector>

#include "board.h"


struct Move {
	SquareIndex from;
	SquareIndex to;
	Castling castling = Castling::NO_CASTLING;
	Piece promotion = Piece::EMPTY;
};

std::vector<Move> PossibleMoves(const Board& board, Piece piece, SquareIndex from);

#endif