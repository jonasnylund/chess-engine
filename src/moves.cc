#include "moves.h"

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

void PawnMove(const Board& board, Piece piece, SquareIndex from, std::vector<Move>& output) {
	const bool is_white = piece & Piece::IS_WHITE;
	const int8_t direction = is_white ? 1 : -1;
	const int8_t target_rank = from.rank + direction;
	const bool is_promotional_square = target_rank == (is_white ? 7 : 0);
	const bool is_starting_rank = from.rank == (is_white ? 1 : 6);

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
}

void KnightMove(const Board& board, Piece piece, SquareIndex from, std::vector<Move>& output) {
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
}

void BishopMove(const Board& board, Piece piece, SquareIndex from, std::vector<Move>& output) {
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
}

void RookMove(const Board& board, Piece piece, SquareIndex from, std::vector<Move>& output) {
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
}

void QueenMove(const Board& board, Piece piece, SquareIndex from, std::vector<Move>& output) {
	BishopMove(board, piece, from, output);
	RookMove(board, piece, from, output);
}

void KingMove(const Board& board, Piece piece, SquareIndex from, std::vector<Move>& output) {
	const bool is_white = piece & Piece::IS_WHITE;

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
}

std::optional<SquareIndex> CastlingMove(const Board& board,
																				Piece piece,
																				SquareIndex from,
																				Castling side) {
	const bool is_white = piece & Piece::IS_WHITE;
	Castling castling = board.CastlingAllowed(is_white);

	if (!(castling & side)) {
		return std::nullopt;
	}

	SquareIndex king_target;
	int8_t rook_target_file;
	if (side == Castling::KINGSIDE) {
		king_target = {.file = 6, .rank = from.rank};
		rook_target_file = king_target.file - 1;
	}
	else {
		king_target = {.file = 2, .rank = from.rank};
		rook_target_file = king_target.file + 1;
	}
	const int8_t rook_file = board.RookStartingFile(side);

	int8_t start = std::min(from.file, king_target.file);
	int8_t stop = std::max(from.file, king_target.file);
	for (int8_t file = start; file <= stop; file++) {
		if (IsAttacked(board, {.file = file, .rank = from.rank}, !is_white)) {
			// The king cannot castle from, through or into check.
			return std::nullopt;
		}
		else if (!IsEmpty(board, file, from.rank) && file != from.file && file != rook_file) {
			// The squares that the king travel through must be empty,
			// apart from by the king itself and the castling rook.
			return std::nullopt;
		}
	}
	start = std::min(rook_file, rook_target_file);
	stop = std::max(rook_file, rook_target_file);
	for (int8_t file = start; file <= stop; file++) {
		// The squares that the rook travel through must be also empty,
		// apart from by the king itself and the castling rook. These
		// squares must not necessarily be the same as the kings in chess960.
		if (!IsEmpty(board, file, from.rank) && file != from.file && file != rook_file) {
			return std::nullopt;
		}
	}

	return king_target;
}

void PossibleMoves(const Board& board,
									 const SquareIndex from,
									 std::vector<Move>& output) {
	const Piece piece = board.Get(from.file, from.rank);

	if (piece & Piece::PAWN) {
		PawnMove(board, piece, from, output);
	}
	else if (piece & Piece::KNIGHT) {
		KnightMove(board, piece, from, output);
	}
	else if (piece & Piece::BISHOP) {
		BishopMove(board, piece, from, output);
	}
	else if (piece & Piece::ROOK) {
		RookMove(board, piece, from, output);
	}
	else if (piece & Piece::QUEEN) {
		QueenMove(board, piece, from, output);
	}
	else if (piece & Piece::KING) {
		KingMove(board, piece, from, output);
		// Castling moves are handled separately.
		std::optional<SquareIndex> castling;
		castling = CastlingMove(board, piece, from, Castling::KINGSIDE);
		if (castling.has_value()) {
			output.push_back({.from = from, .to = *castling, .castling=Castling::KINGSIDE});
		}
		castling = CastlingMove(board, piece, from, Castling::QUEENSIDE);
		if (castling.has_value()) {
			output.push_back({.from = from, .to = *castling, .castling=Castling::QUEENSIDE});
		}
	}
}


}  // namespace

bool IsAttacked(const Board& board, SquareIndex square, bool by_white) {
	std::vector<Move> moves;
	// Pretend that a piece of the opposite color stands on the given square
	// and compute the available moves. Then check if one of the moves leads
	// to a piece of that type.
	const Piece white = by_white ? Piece::EMPTY : Piece::IS_WHITE;

	PawnMove(board, white, square, moves);
	for (const Move& move : moves) {
		if (move.to.file == square.file) {
			// Pawns move forward but don't capture, ignore these moves.
			continue;
		}
		if (board.Get(move.to.file, move.to.rank) & Piece::PAWN)
			return true;
	}

	moves.clear();
	KnightMove(board, white, square, moves);
	for (const Move& move : moves) {
		if (board.Get(move.to.file, move.to.rank) & Piece::KNIGHT)
			return true;
	}

	moves.clear();
	BishopMove(board, white, square, moves);
	for (const Move& move : moves) {
		if (board.Get(move.to.file, move.to.rank) & Piece::BISHOP)
			return true;
		// The queen can also move as a bishop, check for both.
		if (board.Get(move.to.file, move.to.rank) & Piece::QUEEN)
			return true;
	}

	moves.clear();
	RookMove(board, white, square, moves);
	for (const Move& move : moves) {
		if (board.Get(move.to.file, move.to.rank) & Piece::ROOK)
			return true;
		// The queen can also move as a rook, check for both.
		if (board.Get(move.to.file, move.to.rank) & Piece::QUEEN)
			return true;
	}

	moves.clear();
	KingMove(board, white, square, moves);
	for (const Move& move : moves) {
		if (board.Get(move.to.file, move.to.rank) & Piece::KING)
			return true;
	}

	return false;
}

MoveIterator::MoveIterator(const Board& board)
	: source_position(board) {
	// Find where the king is ones.
	std::optional<SquareIndex> square = SquareIndex({.file = -1, .rank = 0});
	while (square.has_value()) {
		if (board.Get(square->file, square->rank) & Piece::KING) {
			this->kings_position = *square;
			break;
		}
		square = board.NextOccupied(*square, board.WhiteToMove());
	}
	Reset();
}

MoveIterator::MoveIterator(const Board& board, SquareIndex kings_position)
  : source_position(board),
	  kings_position(kings_position) {
	Reset();
}

std::optional<Move> MoveIterator::Next() {
	// Iterate until we find a legal move, or runs out of squares.
	while (true) {
		// If there are still moves to consider on the current square, check those first.
		if (this->current_index < this->moves.size()) {
			const Move move = this->moves[this->current_index++];
			this->yielded_position = this->source_position;
			this->yielded_position.Move(move.from, move.to, move.promotion, move.castling);

			// Check if the king is in check after the move. If so, the move is illegal.
			if (move.from.file == this->kings_position.file &&
					move.from.rank == this->kings_position.rank) {
				// If the king moved, check for checks on the new square.
				if (IsAttacked(
						this->yielded_position, move.to, this->yielded_position.WhiteToMove())) {
					continue;
				}
			}
			else if (IsAttacked(
					this->yielded_position, this->kings_position, this->yielded_position.WhiteToMove())) {
				// Else check on the kings previous square.
				continue;
			}
			// King not in check, the move is valid.
			return move;
		}
		// When all moves are considered, continue to the next piece.
		std::optional<SquareIndex> next_square = this->source_position.NextOccupied(
			this->current_square, this->source_position.WhiteToMove());
		if (!next_square.has_value()) {
			// If there are no more piece to consider, we have covered all moves.
			return std::nullopt;
		}
		this->current_square = *next_square;
		
		this->moves.clear();
		this->current_index = 0;
		// Fetch available moves from the new position.
		PossibleMoves(this->source_position, this->current_square, this->moves);
	}
}
