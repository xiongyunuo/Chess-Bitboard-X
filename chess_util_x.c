#include "chess_util_x.h"

int char_to_piece_x(char p) {
  if (p == 'P') return P_X;
  else if (p == 'Q') return Q_X;
  else if (p == 'R') return R_X;
  else if (p == 'N') return N_X;
  else if (p == 'B') return B_X;
  else if (p == 'K') return K_X;
  return UNKNOWN_X;
}

char piece_to_char_x(int p) {
  if (p == P_X) return 'P';
  else if (p == Q_X) return 'Q';
  else if (p == R_X) return 'R';
  else if (p == N_X) return 'N';
  else if (p == B_X) return 'B';
  else if (p == K_X) return 'K';
  return '-';
}

void print_board_set_x(FILE *out, const board_x *bd) {
  int i, j;
  for (i = BOARD_HEIGHT_X - 1; i >= 0; --i)
    for (j = 0; j < BOARD_WIDTH_X; ++j)
      fprintf(out, "%c%s", piece_to_char_x(bd->piece_set[i * BOARD_WIDTH_X + j]), (j == BOARD_WIDTH_X - 1) ? "\n" : " ");
}

void print_ufm_bb_x(FILE *out, bb_t_x bb) {
  int i;
  for (i = 0; i < BOARD_HEIGHT_X * BOARD_WIDTH_X; ++i)
    fprintf(out, "%s", (bb & (((bb_t_x)1) << i)) ? "1" : "0");
}

void print_fm_bb_x(FILE *out, bb_t_x bb) {
  int i, j;
  for (i = 0; i < BOARD_HEIGHT_X; ++i)
    for (j = 0; j < BOARD_WIDTH_X; ++j)
      fprintf(out, "%s%s", (bb & (((bb_t_x)1) << ((BOARD_HEIGHT_X - 1 - i) * BOARD_WIDTH_X + j))) ? "1" : "0", (j == BOARD_WIDTH_X - 1) ? "\n" : "");
}

void print_board_info_x(FILE *out, const board_x *bd) {
  fprintf(out, "%d\n", bd->side);
  fprintf(out, "%d %d\n", bd->castle[WHITE_X][0], bd->castle[WHITE_X][1]);
  fprintf(out, "%d %d\n", bd->castle[BLACK_X][0], bd->castle[BLACK_X][1]);
  print_ufm_bb_x(out, bd->enp_pos);
  fprintf(out, "\n%d %d\n", bd->half_move_clock, bd->full_move_counter);
}