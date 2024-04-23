#ifndef CHESSENGINE_MOVES_H
#define CHESSENGINE_MOVES_H

#include <vector>

#include "board.h"


struct Move {
	SquareIndex from;
	SquareIndex to;
	Castling castling = Castling::NO_CASTLING;
	Piece promotion = Piece::EMPTY;
};

// Returns true if the square at `square` is attacked by any white piece
// if `by_white` is true, otherwise by black.
bool IsAttacked(const Board& board, SquareIndex square, bool by_white);

// Returns a vector of possible moves for a piece of typ `piece`, at the
// square `from` on `board`.
std::vector<Move> PossibleMoves(const Board& board, Piece piece, SquareIndex from);

#endif