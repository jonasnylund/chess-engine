#include "moves.h"

#include <iostream>
#include <optional>

#include "board.h"
#include "pieces.h"

namespace {

inline bool IsEmpty(const Board& board, int8_t file, int8_t rank) {
	return board.Get(file, rank) == Piece::EMPTY;
}

inline bool CanCapture(const Board& board, bool white, int8_t file, int8_t rank) {
	return static_cast<bool>(board.Get(file, rank) & Piece::IS_WHITE) != white;
}

std::vector<Move> PawnMove(const Board& board, Piece piece, SquareIndex from) {
	const bool is_white = piece & Piece::IS_WHITE;
	const int8_t direction = is_white ? 1 : -1;
	const int8_t target_rank = from.rank + direction;
	const bool is_promotional_square = target_rank == (is_white ? 7 : 0);
	const bool is_starting_rank = from.rank == (is_white ? 1 : 6);

	std::vector<Move> output;
	if (IsEmpty(board, from.file, target_rank)) {
		SquareIndex to = {.file = from.file, .rank = target_rank};

		if (is_promotional_square) {
			Piece white = is_white ? Piece::IS_WHITE : Piece::EMPTY;
			output.push_back({.from = from, .to = to, .promotion = Piece::QUEEN | white});
			output.push_back({.from = from, .to = to, .promotion = Piece::KNIGHT | white});
			// Pointless promotions.
			output.push_back({.from = from, .to = to, .promotion = Piece::BISHOP | white});
			output.push_back({.from = from, .to = to, .promotion = Piece::ROOK | white});
		}
		else {
			output.push_back({.from = from, .to = to});
			// Check if we can move two steps forward on first move.
			if (is_starting_rank && IsEmpty(board, from.file, from.rank + direction * 2)) {
				to = {
					.file = from.file,
					.rank = static_cast<int8_t>(from.rank + 2 * direction),
				};
				output.push_back({.from = from, .to = to});
			} 
		}
	}
	for (int8_t i = -1; i <= 1; i += 2) {
		const int8_t target_file = from.file + i;
		SquareIndex to = {.file = target_file, .rank = target_rank};
		std::optional<SquareIndex> en_passent = board.EnPassantSquare();

		if (target_file < 0 || target_file >= 8) {
			continue;
		}
		if (
			!IsEmpty(board, target_file, target_rank) &&
			CanCapture(board, is_white, target_file, target_rank)
		) {
			if (is_promotional_square) {
			Piece white = is_white ? Piece::IS_WHITE : Piece::EMPTY;
			output.push_back({.from = from, .to = to, .promotion = Piece::QUEEN | white});
			output.push_back({.from = from, .to = to, .promotion = Piece::KNIGHT | white});
			// Pointless promotions.
			output.push_back({.from = from, .to = to, .promotion = Piece::BISHOP | white});
			output.push_back({.from = from, .to = to, .promotion = Piece::ROOK | white});
			} else {
				output.push_back({.from = from, .to = to});
			}
		}
		else if(
			IsEmpty(board, target_file, target_rank) &&
			en_passent.has_value()
			&& en_passent->file == target_file && en_passent->rank == target_rank
		) {
			output.push_back({.from = from, .to = to});
		}
	}

	return output;
}

std::vector<Move> KnightMove(const Board& board, Piece piece, SquareIndex from) {
	std::vector<Move> output;
	const bool is_white = piece & Piece::IS_WHITE;

	for (int step = 1; step <= 2; step++) {
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				const SquareIndex to = {
					.file = static_cast<int8_t>(from.file + j * (3 - step)),
					.rank = static_cast<int8_t>(from.rank + i * step),
				};
				if (to.rank < 0 || to.rank > 7)
					continue;
				if (to.file < 0 || to.file > 7)
					continue;
				if (IsEmpty(board, to.file, to.rank) ||
						CanCapture(board, is_white, to.file, to.rank))
					output.push_back({.from = from, .to = to});
			}
		}
	}
	return output;
}

std::vector<Move> BishopMove(const Board& board, Piece piece, SquareIndex from) {
	std::vector<Move> output;
	const bool is_white = piece & Piece::IS_WHITE;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			for (int step = 1; step < 8; step ++) {
				const SquareIndex to = {
					.file = static_cast<int8_t>(from.file + j * step),
					.rank = static_cast<int8_t>(from.rank + i * step),
				};
				if (to.rank < 0 || to.rank > 7)
					break;
				if (to.file < 0 || to.file > 7)
					break;
				if (IsEmpty(board, to.file, to.rank)) {
					output.push_back({.from = from, .to = to});
				} else if (CanCapture(board, is_white, to.file, to.rank)) {
					output.push_back({.from = from, .to = to});
					break;
				}
				else break;
			}
		}
	}

	return output;
}

std::vector<Move> RookMove(const Board& board, Piece piece, SquareIndex from) {
	std::vector<Move> output;
	const bool is_white = piece & Piece::IS_WHITE;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = 0; j <= 1; j++) {
			const int file = i * j;
			const int rank = i * (j - 1);
			for (int step = 1; step < 8; step++) {
				const SquareIndex to = {
					.file = static_cast<int8_t>(from.file + file * step),
					.rank = static_cast<int8_t>(from.rank + rank * step),
				};
				if (to.rank < 0 || to.rank > 7)
					break;
				if (to.file < 0 || to.file > 7)
					break;
				if (IsEmpty(board, to.file, to.rank)) {
					output.push_back({.from = from, .to = to});
				} else if (CanCapture(board, is_white, to.file, to.rank)) {
					output.push_back({.from = from, .to = to});
					break;
				}
				else break;
			}
		}
	}

	return output;
}

std::vector<Move> QueenMove(const Board& board, Piece piece, SquareIndex from) {
	std::vector<Move> moves1 = BishopMove(board, piece, from);
	std::vector<Move> moves2 = RookMove(board, piece, from);

	const bool one_longer = moves1.size() > moves2.size();
	std::vector<Move>* longest = (one_longer ? &moves1 : &moves2);
	std::vector<Move>* shorter = (one_longer ? &moves2 : &moves1);

	longest->reserve(longest->size() + shorter->size());
	longest->insert(longest->end(), shorter->begin(), shorter->end());
	return *longest;
}

std::vector<Move> KingMove(const Board& board, Piece piece, SquareIndex from) {
	std::vector<Move> output;
	const bool is_white = piece & Piece::IS_WHITE;

	// Normal moves.
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) continue;

			const SquareIndex to = {
				.file = static_cast<int8_t>(from.file + j),
				.rank = static_cast<int8_t>(from.rank + i),
			};
			if (to.rank < 0 || to.rank > 7)
				continue;
			if (to.file < 0 || to.file > 7)
				continue;
			if (IsEmpty(board, to.file, to.rank) ||
					CanCapture(board, is_white, to.file, to.rank))
				output.push_back({.from = from, .to = to});
		}
	}
	// Castling moves
	// ...

	return output;
}

}  // namespace

std::vector<Move> PossibleMoves(const Board& board, Piece piece, SquareIndex from) {
	if (piece & Piece::PAWN) {
		return PawnMove(board, piece, from);
	}
	else if (piece & Piece::KNIGHT) {
		return KnightMove(board, piece, from);
	}
	else if (piece & Piece::BISHOP) {
		return BishopMove(board, piece, from);
	}
	else if (piece & Piece::ROOK) {
		return RookMove(board, piece, from);
	}
	else if (piece & Piece::QUEEN) {
		return QueenMove(board, piece, from);
	}

	return {};
}