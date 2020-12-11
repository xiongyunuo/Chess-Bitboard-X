#ifndef MOVE_GEN_X_H
#define MOVE_GEN_X_H

#include "chess_def_x.h"

extern bb_t_x knight_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x king_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x file_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x rank_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x diagonal_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x anti_diagonal_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x upper_bb_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x lower_bb_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern int castle_pos_x[PLAYER_N_X][2];
extern bb_t_x castle_king_bb_x[PLAYER_N_X][3];
extern bb_t_x castle_rook_bb_x[PLAYER_N_X][4];
extern int castle_king_pos_x[PLAYER_N_X][3];
extern int castle_rook_pos_x[PLAYER_N_X][4];
extern bb_t_x can_castle_bb_x[PLAYER_N_X][2];
extern bb_t_x can_castle_attack_bb_x[PLAYER_N_X][2];

extern int r_shift_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern int b_shift_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x r_mask_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x b_mask_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x r_magic_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x b_magic_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x *r_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
extern bb_t_x *b_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];

typedef struct {
  int piece, c_piece, p_piece;
  int from, to;
  bb_t_x enp_pos;
  bb_t_x pre_enp, new_enp;
  int pre_castle[PLAYER_N_X][2];
  int castle, enp_tar;
} move_x;

void move_gen_init_x(void);
int move_gen_x(move_x *moves, board_x *bd);
void make_move_x(board_x *bd, move_x *mv);
void unmake_move_x(board_x *bd, move_x *mv);
int post_check_x(board_x *bd);
int post_check_2_x(board_x *bd);

#endif