#ifndef FEN_X_H
#define FEN_X_H

#include "chess_def_x.h"

#define STANDARD_FEN_X "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

int parse_fen_x(const char *fen, board_x *bd);

#endif