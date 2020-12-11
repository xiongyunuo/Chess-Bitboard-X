#ifndef CHESS_UTIL_X_H
#define CHESS_UTIL_X_H

#include <stdio.h>
#include "chess_def_x.h"

int char_to_piece_x(char p);
char piece_to_char_x(int p);
void print_board_set_x(FILE *out, const board_x *bd);
void print_ufm_bb_x(FILE *out, bb_t_x bb);
void print_fm_bb_x(FILE *out, bb_t_x bb);
void print_board_info_x(FILE *out, const board_x *bd);

#endif