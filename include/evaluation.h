#ifndef CHESSENGINE_EVALUATION_H
#define CHESSENGINE_EVALUATION_H

#include "board.h"

// Returns a heuristic score for the value of the current position.
// Positive values indicate advantage for white, negative for black.
// Return value is in units of 1 / 100th pawn.
int Evaluate(const Board* board, int depth);

#endif