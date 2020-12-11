#include "fen_x.h"
#include "chess_util_x.h"
#include <ctype.h>

static int next_num_or_char(const char *str, int *num, char *c) {
  int counter = 0;
  while (*str && isspace(*str)) {
    ++str;
    ++counter;
  }
  if (*str == '\0') return -1;
  if (!isdigit(*str)) {
    *c = *str;
    *num = -1;
    return counter + 1;
  }
  *num = *str++ - '0';
  ++counter;
  while (*str && isdigit(*str)) {
    *num = 10 * *num + (*str++ - '0');
    ++counter;
  }
  return counter;
}

int parse_fen_x(const char *fen, board_x *bd) {
  int index = BOARD_HEIGHT_X * BOARD_WIDTH_X - 1;
  int incre, emp, piece, i;
  char p;
  bb_t_x bb;
  for (i = 0; i <= index; ++i)
    bd->piece_set[i] = EMPTY_X;
  for (i = 0; i < PIECE_N_X; ++i)
    bd->bb_arr[WHITE_X][i] = 0;
  for (i = 0; i < PIECE_N_X; ++i)
    bd->bb_arr[BLACK_X][i] = 0;
  bd->all_pieces = 0;
  bd->pieces_bb[WHITE_X] = 0;
  bd->pieces_bb[BLACK_X] = 0;
  while (index >= 0) {
    incre = next_num_or_char(fen, &emp, &p);
    if (incre == -1) return -1;
    fen += incre;
    if (emp == -1) {
      if (p == '/') continue;
      piece = char_to_piece_x(toupper(p));
      if (piece == UNKNOWN_X) return -1;
      bd->piece_set[(index / BOARD_WIDTH_X) * BOARD_WIDTH_X + (BOARD_WIDTH_X - 1 - index % BOARD_WIDTH_X)] = piece;
      bb = ((bb_t_x)1) << ((index / BOARD_WIDTH_X) * BOARD_WIDTH_X + (BOARD_WIDTH_X - 1 - index % BOARD_WIDTH_X));
      bd->bb_arr[islower(p) ? BLACK_X : WHITE_X][piece] |= bb;
      bd->all_pieces |= bb;
      bd->pieces_bb[islower(p) ? BLACK_X : WHITE_X] |= bb;
      --index;
    }
    else
      index -= emp;
  }
  bd->empty_sqrs = ~bd->all_pieces;
  incre = next_num_or_char(fen, &emp, &p);
  if (incre == -1 || emp != -1) return -1;
  fen += incre;
  bd->side = (p == 'w') ? WHITE_X : BLACK_X;
  bd->capture = bd->pieces_bb[1 - bd->side];
  bd->capture_emp = bd->capture | bd->empty_sqrs;
  bd->castle[WHITE_X][0] = bd->castle[WHITE_X][1] = bd->castle[BLACK_X][0] = bd->castle[BLACK_X][1] = 0;
  incre = next_num_or_char(fen, &emp, &p);
  if (incre == -1 || emp != -1) return -1;
  fen += incre;
  if (p == '-') {
    incre = next_num_or_char(fen, &emp, &p);
    if (incre == -1) return -1;
    fen += incre;
  }
  else do {
    if (p == 'K') bd->castle[WHITE_X][0] = 1;
    else if (p == 'Q') bd->castle[WHITE_X][1] = 1;
    else if (p == 'k') bd->castle[BLACK_X][0] = 1;
    else if (p == 'q') bd->castle[BLACK_X][1] = 1;
    else break;
    incre = next_num_or_char(fen, &emp, &p);
    if (incre == -1) return -1;
    fen += incre;
  } while (1);
  if (emp != -1) return -1;
  bd->enp_pos = 0;
  if (p != '-') {
    incre = next_num_or_char(fen, &emp, &p);
    if (incre == -1 || emp == -1) return -1;
    fen += incre;
    int shift = p - 'a' + BOARD_WIDTH_X * (emp - 1);
    if (shift < 0 || shift >= BOARD_WIDTH_X * BOARD_HEIGHT_X) return -1;
    bd->enp_pos = ((bb_t_x)1) << shift;
  }
  bd->half_move_clock = 0;
  bd->full_move_counter = 1;
  incre = next_num_or_char(fen, &emp, &p);
  if (incre == -1 || emp == -1) return 0;
  fen += incre;
  bd->half_move_clock = emp;
  incre = next_num_or_char(fen, &emp, &p);
  if (incre == -1 || emp == -1) return 0;
  fen += incre;
  bd->full_move_counter = emp;
  return 0;
}