#ifndef CHESS_DEF_X_H
#define CHESS_DEF_X_H

#include <stdint.h>

#define PLAYER_N_X 2
#define PIECE_N_X 7
#define BOARD_WIDTH_X 8
#define BOARD_HEIGHT_X 8

typedef uint64_t bb_t_x;

enum { P_X, R_X, N_X, B_X, K_X, Q_X, EMPTY_X, UNKNOWN_X };
enum { WHITE_X, BLACK_X };

typedef struct {
  bb_t_x bb_arr[PLAYER_N_X][PIECE_N_X];
  int piece_set[BOARD_HEIGHT_X * BOARD_WIDTH_X];
  bb_t_x all_pieces, empty_sqrs, pieces_bb[PLAYER_N_X];
  bb_t_x capture, capture_emp;
  int side;
  int castle[PLAYER_N_X][2];
  bb_t_x enp_pos;
  int half_move_clock, full_move_counter;
} board_x;

#endif