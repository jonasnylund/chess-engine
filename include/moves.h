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
bool IsAttacked(
	const Board& board, SquareIndex square, bool by_white);

// Creates an iterator over all legal moves in the current position.
class MoveIterator {
 public:
  explicit MoveIterator(const Board& board);

	// Returns the next legal move, or nullopt if all moves have been
	// considered.
	std::optional<Move> Next();

	// Returns a view of the board before any move.
	const Board* SourcePosition() const {
		return &this->source_position;
	}
	// Returns a view of the board at after the last move returned by `Next`.
	const Board* CurrentPosition() const {
		return &this->yielded_position;
	}
	// Continue the position after the last move returned by `Next`,
	// iterating the responsive moves of the other side.
	MoveIterator ContinuePosition() const {
		return MoveIterator(this->yielded_position);
	}

	// Resets the iterator to the initial state.
	void Reset() {
		this->current_square = {.file = -1, .rank = 0};
		this->moves.clear();
		this->current_index = 0;
	}

 private:
  const Board source_position;
	Board yielded_position;
	SquareIndex current_square;
	std::vector<Move> moves;
	int current_index = 0;
};

#endif