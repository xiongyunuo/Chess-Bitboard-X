#include "move_gen_x.h"
#include <stdlib.h>

#define bsf_x(bb) __builtin_ctzll((bb))
#define bsr_x(bb) __builtin_clzll((bb))
#define pop_count_x(bb) __builtin_popcountll((bb))
#define RANK_4_X 0X00000000FF000000ULL
#define RANK_5_X 0X000000FF00000000ULL
#define RANK_8_X 0XFF00000000000000ULL
#define RANK_1_X 0X00000000000000FFULL
#define NOT_RANK_8_X 0X00FFFFFFFFFFFFFFULL
#define NOT_RANK_1_X 0XFFFFFFFFFFFFFF00ULL
#define NOT_FILE_H_X 0X7F7F7F7F7F7F7F7FULL
#define NOT_FILE_A_X 0XFEFEFEFEFEFEFEFEULL
#define HIGH_BB_X 0X8000000000000000ULL

#define add_move_x(p, c_p, f, t, e_p, n_p) moves->p_piece = moves->piece = p;\
moves->c_piece = c_p;\
moves->from = f;\
moves->to = t;\
moves->enp_pos = e_p;\
moves->new_enp = n_p;\
moves->castle = 0;\
++moves;\
++index;

#define add_pro_move_x(p, p_p, c_p, f, t, e_p, n_p) moves->piece = p;\
moves->p_piece = p_p;\
moves->c_piece = c_p;\
moves->from = f;\
moves->to = t;\
moves->enp_pos = e_p;\
moves->new_enp = n_p;\
moves->castle = 0;\
++moves;\
++index;

bb_t_x knight_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x king_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x file_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x rank_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x diagonal_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x anti_diagonal_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x upper_bb_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x lower_bb_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
int castle_pos_x[PLAYER_N_X][2] = { {7, 0}, {63, 56} };
bb_t_x castle_king_bb_x[PLAYER_N_X][3];
bb_t_x castle_rook_bb_x[PLAYER_N_X][4];
int castle_king_pos_x[PLAYER_N_X][3];
int castle_rook_pos_x[PLAYER_N_X][4];
bb_t_x can_castle_bb_x[PLAYER_N_X][2];
bb_t_x can_castle_attack_bb_x[PLAYER_N_X][2];

int r_shift_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
int b_shift_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x r_mask_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x b_mask_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x r_magic_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x b_magic_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x *r_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];
bb_t_x *b_attack_x[BOARD_WIDTH_X * BOARD_HEIGHT_X];

static bb_t_x random_bb() {
  bb_t_x u1, u2, u3, u4;
  u1 = (bb_t_x)(random()) & 0xFFFF; u2 = (bb_t_x)(random()) & 0xFFFF;
  u3 = (bb_t_x)(random()) & 0xFFFF; u4 = (bb_t_x)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

static bb_t_x random_bb_fewbits() {
  return random_bb() & random_bb() & random_bb();
}

static bb_t_x index_to_bb(int index, int bits, bb_t_x m) {
  int i, j;
  bb_t_x result = 0ULL;
  for (i = 0; i < bits; ++i) {
    j = bsf_x(m);
    m &= m - 1;
    if (index & (1 << i)) result |= (1ULL << j);
  }
  return result;
}

void move_gen_init_x(void) {
  int i, j, k, i2, j2;
  for (i = 0; i < BOARD_WIDTH_X * BOARD_HEIGHT_X; ++i) {
    bb_t_x knights = ((bb_t_x)1) << i;
    bb_t_x l1 = (knights >> 1) & 0x7f7f7f7f7f7f7f7full;
    bb_t_x l2 = (knights >> 2) & 0x3f3f3f3f3f3f3f3full;
    bb_t_x r1 = (knights << 1) & 0xfefefefefefefefeull;
    bb_t_x r2 = (knights << 2) & 0xfcfcfcfcfcfcfcfcull;
    bb_t_x h1 = l1 | r1;
    bb_t_x h2 = l2 | r2;
    knight_attack_x[i] = (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
  }
  for (i = 0; i < BOARD_WIDTH_X * BOARD_HEIGHT_X; ++i) {
    bb_t_x kings = ((bb_t_x)1) << i;
    bb_t_x attacks = ((kings & NOT_FILE_H_X) << 1) | ((kings & NOT_FILE_A_X) >> 1);
    kings |= attacks;
    attacks |= (kings << BOARD_WIDTH_X) | (kings >> BOARD_WIDTH_X);
    king_attack_x[i] = attacks;
  }
  for (i = 0; i < BOARD_HEIGHT_X; ++i)
    for (j = 0; j < BOARD_WIDTH_X; ++j) {
      for (k = i + 1; k < BOARD_HEIGHT_X; ++k)
        file_attack_x[i * BOARD_WIDTH_X + j] |= ((bb_t_x)1) << (k * BOARD_WIDTH_X + j);
      for (k = i - 1; k >= 0; --k)
        file_attack_x[i * BOARD_WIDTH_X + j] |= ((bb_t_x)1) << (k * BOARD_WIDTH_X + j);
    }
  for (i = 0; i < BOARD_HEIGHT_X; ++i)
    for (j = 0; j < BOARD_WIDTH_X; ++j) {
      for (k = j + 1; k < BOARD_WIDTH_X; ++k)
        rank_attack_x[i * BOARD_WIDTH_X + j] |= ((bb_t_x)1) << (i * BOARD_WIDTH_X + k);
      for (k = j - 1; k >= 0; --k)
        rank_attack_x[i * BOARD_WIDTH_X + j] |= ((bb_t_x)1) << (i * BOARD_WIDTH_X + k);
    }
  for (i = 0; i < BOARD_HEIGHT_X; ++i)
    for (j = 0; j < BOARD_WIDTH_X; ++j) {
      i2 = i + 1; j2 = j + 1;
      while (i2 < BOARD_HEIGHT_X && j2 < BOARD_WIDTH_X) {
        diagonal_attack_x[i * BOARD_WIDTH_X + j] |= ((bb_t_x)1) << (i2 * BOARD_WIDTH_X + j2);
        ++i2; ++j2;
      }
      i2 = i - 1; j2 = j - 1;
      while (i2 >= 0 && j2 >= 0) {
        diagonal_attack_x[i * BOARD_WIDTH_X + j] |= ((bb_t_x)1) << (i2 * BOARD_WIDTH_X + j2);
        --i2; --j2;
      }
      i2 = i + 1; j2 = j - 1;
      while (i2 < BOARD_HEIGHT_X && j2 >= 0) {
        anti_diagonal_attack_x[i * BOARD_WIDTH_X + j] |= ((bb_t_x)1) << (i2 * BOARD_WIDTH_X + j2);
        ++i2; --j2;
      }
      i2 = i - 1; j2 = j + 1;
      while (i2 >= 0 && j2 < BOARD_WIDTH_X) {
        anti_diagonal_attack_x[i * BOARD_WIDTH_X + j] |= ((bb_t_x)1) << (i2 * BOARD_WIDTH_X + j2);
        --i2; ++j2;
      }
    }
  for (i = 0; i < BOARD_WIDTH_X * BOARD_HEIGHT_X; ++i) {
    for (j = 0; j < i; ++j)
      lower_bb_x[i] |= ((bb_t_x)1) << j;
    for (j = i + 1; j < BOARD_WIDTH_X * BOARD_HEIGHT_X; ++j)
      upper_bb_x[i] |= ((bb_t_x)1) << j;
  }
  castle_king_bb_x[WHITE_X][0] = ((bb_t_x)1) << 4;
  castle_king_bb_x[WHITE_X][1] = ((bb_t_x)1) << 6;
  castle_king_bb_x[WHITE_X][2] = ((bb_t_x)1) << 2;
  castle_king_bb_x[BLACK_X][0] = ((bb_t_x)1) << 60;
  castle_king_bb_x[BLACK_X][1] = ((bb_t_x)1) << 62;
  castle_king_bb_x[BLACK_X][2] = ((bb_t_x)1) << 58;
  castle_rook_bb_x[WHITE_X][0] = ((bb_t_x)1) << 7;
  castle_rook_bb_x[WHITE_X][1] = ((bb_t_x)1) << 5;
  castle_rook_bb_x[WHITE_X][2] = ((bb_t_x)1) << 0;
  castle_rook_bb_x[WHITE_X][3] = ((bb_t_x)1) << 3;
  castle_rook_bb_x[BLACK_X][0] = ((bb_t_x)1) << 63;
  castle_rook_bb_x[BLACK_X][1] = ((bb_t_x)1) << 61;
  castle_rook_bb_x[BLACK_X][2] = ((bb_t_x)1) << 56;
  castle_rook_bb_x[BLACK_X][3] = ((bb_t_x)1) << 59;
  castle_king_pos_x[WHITE_X][0] = 4;
  castle_king_pos_x[WHITE_X][1] = 6;
  castle_king_pos_x[WHITE_X][2] = 2;
  castle_king_pos_x[BLACK_X][0] = 60;
  castle_king_pos_x[BLACK_X][1] = 62;
  castle_king_pos_x[BLACK_X][2] = 58;
  castle_rook_pos_x[WHITE_X][0] = 7;
  castle_rook_pos_x[WHITE_X][1] = 5;
  castle_rook_pos_x[WHITE_X][2] = 0;
  castle_rook_pos_x[WHITE_X][3] = 3;
  castle_rook_pos_x[BLACK_X][0] = 63;
  castle_rook_pos_x[BLACK_X][1] = 61;
  castle_rook_pos_x[BLACK_X][2] = 56;
  castle_rook_pos_x[BLACK_X][3] = 59;
  can_castle_bb_x[WHITE_X][0] = castle_rook_bb_x[WHITE_X][0] - 2 * castle_king_bb_x[WHITE_X][0];
  can_castle_bb_x[WHITE_X][1] = castle_king_bb_x[WHITE_X][0] - 2 * castle_rook_bb_x[WHITE_X][2];
  can_castle_bb_x[BLACK_X][0] = castle_rook_bb_x[BLACK_X][0] - 2 * castle_king_bb_x[BLACK_X][0];
  can_castle_bb_x[BLACK_X][1] = castle_king_bb_x[BLACK_X][0] - 2 * castle_rook_bb_x[BLACK_X][2];
  can_castle_attack_bb_x[WHITE_X][0] = 2 * castle_king_bb_x[WHITE_X][1] - castle_king_bb_x[WHITE_X][0];
  can_castle_attack_bb_x[WHITE_X][1] = 2 * castle_king_bb_x[WHITE_X][0] - castle_king_bb_x[WHITE_X][2];
  can_castle_attack_bb_x[BLACK_X][0] = 2 * castle_king_bb_x[BLACK_X][1] - castle_king_bb_x[BLACK_X][0];
  can_castle_attack_bb_x[BLACK_X][1] = 2 * castle_king_bb_x[BLACK_X][0] - castle_king_bb_x[BLACK_X][2];
  int file, rank;
  for (i = 0; i < BOARD_WIDTH_X * BOARD_HEIGHT_X; ++i) {
    file = i % BOARD_WIDTH_X;
    rank = i / BOARD_WIDTH_X;
    r_mask_x[i] = file_attack_x[i] | rank_attack_x[i];
    b_mask_x[i] = diagonal_attack_x[i] | anti_diagonal_attack_x[i];
    if (file != 0) {
      r_mask_x[i] &= NOT_FILE_A_X;
      b_mask_x[i] &= NOT_FILE_A_X;
    }
    if (file != BOARD_WIDTH_X - 1) {
      r_mask_x[i] &= NOT_FILE_H_X;
      b_mask_x[i] &= NOT_FILE_H_X;
    }
    if (rank != 0) {
      r_mask_x[i] &= NOT_RANK_1_X;
      b_mask_x[i] &= NOT_RANK_1_X;
    }
    if (rank != BOARD_HEIGHT_X - 1) {
      r_mask_x[i] &= NOT_RANK_8_X;
      b_mask_x[i] &= NOT_RANK_8_X;
    }
    r_shift_x[i] = pop_count_x(r_mask_x[i]);
    b_shift_x[i] = pop_count_x(b_mask_x[i]);
  }
  bb_t_x used[4096], block[4096], att[4096];
  bb_t_x occu, up, low, magic;
  int fail, fail2, index;
  for (i = 0; i < BOARD_WIDTH_X * BOARD_HEIGHT_X; ++i) {
    for (j = 0; j < (1 << r_shift_x[i]); ++j) {
      block[j] = index_to_bb(j, r_shift_x[i], r_mask_x[i]);
      occu = block[j] & file_attack_x[i];
      up = occu & upper_bb_x[i];
      up = up & -up;
      low = HIGH_BB_X >> bsr_x((occu & lower_bb_x[i]) | 1ULL);
      att[j] = ((up * 2) - low) & file_attack_x[i];
      occu = block[j] & rank_attack_x[i];
      up = occu & upper_bb_x[i];
      up = up & -up;
      low = HIGH_BB_X >> bsr_x((occu & lower_bb_x[i]) | 1ULL);
      att[j] |= ((up * 2) - low) & rank_attack_x[i];
    }
    r_attack_x[i] = (bb_t_x *)calloc(1 << r_shift_x[i], sizeof(bb_t_x));
    fail = 1;
    while (fail) {
      magic = random_bb_fewbits();
      for (k = 0; k < 4096; ++k) used[k] = 0;
      fail2 = 0;
      for (j = 0; j < (1 << r_shift_x[i]); ++j) {
        index = (block[j] * magic) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - r_shift_x[i]);
        r_attack_x[i][index] = att[j];
        if (used[index] == 0) used[index] = att[j];
        else if (used[index] != att[j]) {
          fail2 = 1;
          break;
        }
      }
      if (!fail2) {
        r_magic_x[i] = magic;
        fail = 0;
      }
    }
    for (j = 0; j < (1 << b_shift_x[i]); ++j) {
      block[j] = index_to_bb(j, b_shift_x[i], b_mask_x[i]);
      occu = block[j] & diagonal_attack_x[i];
      up = occu & upper_bb_x[i];
      up = up & -up;
      low = HIGH_BB_X >> bsr_x((occu & lower_bb_x[i]) | 1ULL);
      att[j] = ((up * 2) - low) & diagonal_attack_x[i];
      occu = block[j] & anti_diagonal_attack_x[i];
      up = occu & upper_bb_x[i];
      up = up & -up;
      low = HIGH_BB_X >> bsr_x((occu & lower_bb_x[i]) | 1ULL);
      att[j] |= ((up * 2) - low) & anti_diagonal_attack_x[i];
    }
    b_attack_x[i] = (bb_t_x *)calloc(1 << b_shift_x[i], sizeof(bb_t_x));
    fail = 1;
    while (fail) {
      magic = random_bb_fewbits();
      for (k = 0; k < 4096; ++k) used[k] = 0;
      fail2 = 0;
      for (j = 0; j < (1 << b_shift_x[i]); ++j) {
        index = (block[j] * magic) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - b_shift_x[i]);
        b_attack_x[i][index] = att[j];
        if (used[index] == 0) used[index] = att[j];
        else if (used[index] != att[j]) {
          fail2 = 1;
          break;
        }
      }
      if (!fail2) {
        b_magic_x[i] = magic;
        fail = 0;
      }
    }
  }
}

int move_gen_x(register move_x *moves, register board_x *bd) {
  register int pos, pos2, index = 0;
  register int check = 0, s = 1 - bd->side;
  register bb_t_x attack_bb = 0, att, check_bb, temp, temp2;
  bb_t_x pinned_bb[PIECE_N_X - 1][4] = {0};
  if (s == WHITE_X) {
    att = (bd->bb_arr[WHITE_X][P_X] << (BOARD_WIDTH_X + 1)) & NOT_FILE_A_X;
    if ((temp = (att & bd->bb_arr[bd->side][K_X]) >> (BOARD_WIDTH_X + 1))) {
      check_bb = temp;
      ++check;
    }
    attack_bb |= att;
    att = (bd->bb_arr[WHITE_X][P_X] << (BOARD_WIDTH_X - 1)) & NOT_FILE_H_X;
    if ((temp = (att & bd->bb_arr[bd->side][K_X]) >> (BOARD_WIDTH_X - 1))) {
      check_bb = temp;
      ++check;
    }
    attack_bb |= att;
  }
  else {
    att = (bd->bb_arr[BLACK_X][P_X] >> (BOARD_WIDTH_X - 1)) & NOT_FILE_A_X;
    if ((temp = (att & bd->bb_arr[bd->side][K_X]) << (BOARD_WIDTH_X - 1))) {
      check_bb = temp;
      ++check;
    }
    attack_bb |= att;
    att = (bd->bb_arr[BLACK_X][P_X] >> (BOARD_WIDTH_X + 1)) & NOT_FILE_H_X;
    if ((temp = (att & bd->bb_arr[bd->side][K_X]) << (BOARD_WIDTH_X + 1))) {
      check_bb = temp;
      ++check;
    }
    attack_bb |= att;
  }
  temp = bd->bb_arr[s][N_X];
  while (temp) {
    pos = bsf_x(temp);
    if (bd->bb_arr[bd->side][K_X] & knight_attack_x[pos]) {
      check_bb = ((bb_t_x)1) << pos;
      ++check;
    }
    attack_bb |= knight_attack_x[pos];
    temp &= temp - 1;
  }
  register bb_t_x kings = bd->bb_arr[s][K_X];
  register bb_t_x attacks = ((kings & NOT_FILE_H_X) << 1) | ((kings & NOT_FILE_A_X) >> 1);
  kings |= attacks;
  attacks |= (kings << BOARD_WIDTH_X) | (kings >> BOARD_WIDTH_X);
  attack_bb |= attacks;
  temp = bd->bb_arr[s][R_X] | bd->bb_arr[s][Q_X];
  register bb_t_x occu, up;
  while (temp) {
    pos = bsf_x(temp);
    attack_bb |= r_attack_x[pos][(((bd->all_pieces ^ bd->bb_arr[bd->side][K_X]) & r_mask_x[pos]) * r_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - r_shift_x[pos])];
    att = r_attack_x[pos][((bd->all_pieces & r_mask_x[pos]) * r_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - r_shift_x[pos])];
    if (file_attack_x[pos] & att & upper_bb_x[pos] & bd->bb_arr[bd->side][K_X]) {
      ++check;
      check_bb = (file_attack_x[pos] & att & upper_bb_x[pos]) | (((bb_t_x)1) << pos);
    }
    else if (file_attack_x[pos] & att & lower_bb_x[pos] & bd->bb_arr[bd->side][K_X]) {
      ++check;
      check_bb = (file_attack_x[pos] & att & lower_bb_x[pos]) | (((bb_t_x)1) << pos);
    }
    if (rank_attack_x[pos] & att & upper_bb_x[pos] & bd->bb_arr[bd->side][K_X]) {
      ++check;
      check_bb = (rank_attack_x[pos] & att & upper_bb_x[pos]) | (((bb_t_x)1) << pos);
    }
    else if (rank_attack_x[pos] & att & lower_bb_x[pos] & bd->bb_arr[bd->side][K_X]) {
      ++check;
      check_bb = (rank_attack_x[pos] & att & lower_bb_x[pos]) | (((bb_t_x)1) << pos);
    }
    occu = bd->all_pieces & file_attack_x[pos];
    up = occu & upper_bb_x[pos];
    if (up && (up & bd->bb_arr[bd->side][K_X]) && (up & -up & bd->pieces_bb[bd->side])) {
      pos2 = bsf_x(up);
      up &= up - 1;
      if (up && (up & -up & bd->bb_arr[bd->side][K_X]))
        pinned_bb[bd->piece_set[pos2]][0] |= ((bb_t_x)1) << pos2;
    }
    occu = bd->all_pieces & rank_attack_x[pos];
    up = occu & upper_bb_x[pos];
    if (up && (up & bd->bb_arr[bd->side][K_X]) && (up & -up & bd->pieces_bb[bd->side])) {
      pos2 = bsf_x(up);
      up &= up - 1;
      if (up && (up & -up & bd->bb_arr[bd->side][K_X]))
        pinned_bb[bd->piece_set[pos2]][1] |= ((bb_t_x)1) << pos2;
    }
    temp &= temp - 1;
  }
  temp = bd->bb_arr[s][B_X] | bd->bb_arr[s][Q_X];
  while (temp) {
    pos = bsf_x(temp);
    attack_bb |= b_attack_x[pos][(((bd->all_pieces ^ bd->bb_arr[bd->side][K_X]) & b_mask_x[pos]) * b_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - b_shift_x[pos])];
    att = b_attack_x[pos][((bd->all_pieces & b_mask_x[pos]) * b_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - b_shift_x[pos])];
    if (diagonal_attack_x[pos] & att & upper_bb_x[pos] & bd->bb_arr[bd->side][K_X]) {
      ++check;
      check_bb = (diagonal_attack_x[pos] & att & upper_bb_x[pos]) | (((bb_t_x)1) << pos);
    }
    else if (diagonal_attack_x[pos] & att & lower_bb_x[pos] & bd->bb_arr[bd->side][K_X]) {
      ++check;
      check_bb = (diagonal_attack_x[pos] & att & lower_bb_x[pos]) | (((bb_t_x)1) << pos);
    }
    if (anti_diagonal_attack_x[pos] & att & upper_bb_x[pos] & bd->bb_arr[bd->side][K_X]) {
      ++check;
      check_bb = (anti_diagonal_attack_x[pos] & att & upper_bb_x[pos]) | (((bb_t_x)1) << pos);
    }
    else if (anti_diagonal_attack_x[pos] & att & lower_bb_x[pos] & bd->bb_arr[bd->side][K_X]) {
      ++check;
      check_bb = (anti_diagonal_attack_x[pos] & att & lower_bb_x[pos]) | (((bb_t_x)1) << pos);
    }
    occu = bd->all_pieces & diagonal_attack_x[pos];
    up = occu & upper_bb_x[pos];
    if (up && (up & bd->bb_arr[bd->side][K_X]) && (up & -up & bd->pieces_bb[bd->side])) {
      pos2 = bsf_x(up);
      up &= up - 1;
      if (up && (up & -up & bd->bb_arr[bd->side][K_X]))
        pinned_bb[bd->piece_set[pos2]][2] |= ((bb_t_x)1) << pos2;
    }
    occu = bd->all_pieces & anti_diagonal_attack_x[pos];
    up = occu & upper_bb_x[pos];
    if (up && (up & bd->bb_arr[bd->side][K_X]) && (up & -up & bd->pieces_bb[bd->side])) {
      pos2 = bsf_x(up);
      up &= up - 1;
      if (up && (up & -up & bd->bb_arr[bd->side][K_X]))
        pinned_bb[bd->piece_set[pos2]][3] |= ((bb_t_x)1) << pos2;
    }
    temp &= temp - 1;
  }
  temp = bd->bb_arr[bd->side][K_X];
  while (temp) {
    pos = bsf_x(temp);
    occu = bd->all_pieces & file_attack_x[pos];
    up = occu & upper_bb_x[pos];
    if (up && (up & (bd->bb_arr[s][R_X] | bd->bb_arr[s][Q_X])) && (up & -up & bd->pieces_bb[bd->side])) {
      pos2 = bsf_x(up);
      up &= up - 1;
      if (up && (up & -up & (bd->bb_arr[s][R_X] | bd->bb_arr[s][Q_X])))
        pinned_bb[bd->piece_set[pos2]][0] |= ((bb_t_x)1) << pos2;
    }
    occu = bd->all_pieces & rank_attack_x[pos];
    up = occu & upper_bb_x[pos];
    if (up && (up & (bd->bb_arr[s][R_X] | bd->bb_arr[s][Q_X])) && (up & -up & bd->pieces_bb[bd->side])) {
      pos2 = bsf_x(up);
      up &= up - 1;
      if (up && (up & -up & (bd->bb_arr[s][R_X] | bd->bb_arr[s][Q_X])))
        pinned_bb[bd->piece_set[pos2]][1] |= ((bb_t_x)1) << pos2;
    }
    occu = bd->all_pieces & diagonal_attack_x[pos];
    up = occu & upper_bb_x[pos];
    if (up && (up & (bd->bb_arr[s][B_X] | bd->bb_arr[s][Q_X])) && (up & -up & bd->pieces_bb[bd->side])) {
      pos2 = bsf_x(up);
      up &= up - 1;
      if (up && (up & -up & (bd->bb_arr[s][B_X] | bd->bb_arr[s][Q_X])))
        pinned_bb[bd->piece_set[pos2]][2] |= ((bb_t_x)1) << pos2;
    }
    occu = bd->all_pieces & anti_diagonal_attack_x[pos];
    up = occu & upper_bb_x[pos];
    if (up && (up & (bd->bb_arr[s][B_X] | bd->bb_arr[s][Q_X])) && (up & -up & bd->pieces_bb[bd->side])) {
      pos2 = bsf_x(up);
      up &= up - 1;
      if (up && (up & -up & (bd->bb_arr[s][B_X] | bd->bb_arr[s][Q_X])))
        pinned_bb[bd->piece_set[pos2]][3] |= ((bb_t_x)1) << pos2;
    }
    temp &= temp - 1;
  }
  if (check >= 2) {
    temp = bd->bb_arr[bd->side][K_X];
    while (temp) {
      pos = bsf_x(temp);
      temp2 = king_attack_x[pos] & bd->capture_emp & ~attack_bb;
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(K_X, bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
  }
  else if (check == 1) {
    if (bd->side == WHITE_X) {
      temp2 = ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][1] | pinned_bb[P_X][2] | pinned_bb[P_X][3])) << BOARD_WIDTH_X) & bd->empty_sqrs & NOT_RANK_8_X;
      temp = temp2 & check_bb;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = check_bb & (temp2 << BOARD_WIDTH_X) & bd->empty_sqrs & RANK_4_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, EMPTY_X, pos - 2 * BOARD_WIDTH_X, pos, 0, ((bb_t_x)1) << (pos - BOARD_WIDTH_X));
        temp &= temp - 1;
      }
      temp = check_bb & ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][1] | pinned_bb[P_X][2] | pinned_bb[P_X][3])) << BOARD_WIDTH_X) & bd->empty_sqrs & RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, R_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, B_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = check_bb & ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][3])) << (BOARD_WIDTH_X + 1)) & bd->capture & NOT_FILE_A_X & NOT_RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (bd->bb_arr[WHITE_X][P_X] << (BOARD_WIDTH_X + 1)) & bd->enp_pos & NOT_FILE_A_X;
      if (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, bd->enp_pos, 0);
        make_move_x(bd, --moves);
        if (post_check_x(bd)) {
          unmake_move_x(bd, moves);
          ++moves;
        }
        else {
          unmake_move_x(bd, moves);
          --index;
        }
      }
      temp = check_bb & ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][2])) << (BOARD_WIDTH_X - 1)) & bd->capture & NOT_FILE_H_X & NOT_RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (bd->bb_arr[WHITE_X][P_X] << (BOARD_WIDTH_X - 1)) & bd->enp_pos & NOT_FILE_H_X;
      if (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, bd->enp_pos, 0);
        make_move_x(bd, --moves);
        if (post_check_x(bd)) {
          unmake_move_x(bd, moves);
          ++moves;
        }
        else {
          unmake_move_x(bd, moves);
          --index;
        }
      }
      temp = check_bb & ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][3])) << (BOARD_WIDTH_X + 1)) & bd->capture & NOT_FILE_A_X & RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, R_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, B_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = check_bb & ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][2])) << (BOARD_WIDTH_X - 1)) & bd->capture & NOT_FILE_H_X & RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, R_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, B_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        temp &= temp - 1;
      }
    }
    else {
      temp2 = ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][1] | pinned_bb[P_X][2] | pinned_bb[P_X][3])) >> BOARD_WIDTH_X) & bd->empty_sqrs & NOT_RANK_1_X;
      temp = temp2 & check_bb;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = check_bb & (temp2 >> BOARD_WIDTH_X) & bd->empty_sqrs & RANK_5_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, EMPTY_X, pos + 2 * BOARD_WIDTH_X, pos, 0, ((bb_t_x)1) << (pos + BOARD_WIDTH_X));
        temp &= temp - 1;
      }
      temp = check_bb & ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][1] | pinned_bb[P_X][2] | pinned_bb[P_X][3])) >> BOARD_WIDTH_X) & bd->empty_sqrs & RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, R_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, B_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = check_bb & ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][3])) >> (BOARD_WIDTH_X + 1)) & bd->capture & NOT_FILE_H_X & NOT_RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (bd->bb_arr[BLACK_X][P_X] >> (BOARD_WIDTH_X + 1)) & bd->enp_pos & NOT_FILE_H_X;
      if (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, bd->enp_pos, 0);
        make_move_x(bd, --moves);
        if (post_check_x(bd)) {
          unmake_move_x(bd, moves);
          ++moves;
        }
        else {
          unmake_move_x(bd, moves);
          --index;
        }
      }
      temp = check_bb & ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][2])) >> (BOARD_WIDTH_X - 1)) & bd->capture & NOT_FILE_A_X & NOT_RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (bd->bb_arr[BLACK_X][P_X] >> (BOARD_WIDTH_X - 1)) & bd->enp_pos & NOT_FILE_A_X;
      if (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, bd->enp_pos, 0);
        make_move_x(bd, --moves);
        if (post_check_x(bd)) {
          unmake_move_x(bd, moves);
          ++moves;
        }
        else {
          unmake_move_x(bd, moves);
          --index;
        }
      }
      temp = check_bb & ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][3])) >> (BOARD_WIDTH_X + 1)) & bd->capture & NOT_FILE_H_X & RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, R_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, B_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = check_bb & ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][2])) >> (BOARD_WIDTH_X - 1)) & bd->capture & NOT_FILE_A_X & RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, R_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, B_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        temp &= temp - 1;
      }
    }
    temp = bd->bb_arr[bd->side][N_X] & ~(pinned_bb[N_X][0] | pinned_bb[N_X][1] | pinned_bb[N_X][2] | pinned_bb[N_X][3]);
    while (temp) {
      pos = bsf_x(temp);
      temp2 = check_bb & knight_attack_x[pos] & bd->capture_emp;
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(N_X, bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = bd->bb_arr[bd->side][K_X];
    while (temp) {
      pos = bsf_x(temp);
      temp2 = king_attack_x[pos] & bd->capture_emp & ~attack_bb;
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(K_X, bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = (bd->bb_arr[bd->side][R_X] & ~(pinned_bb[R_X][1] | pinned_bb[R_X][2] | pinned_bb[R_X][3])) | (bd->bb_arr[bd->side][Q_X] & ~(pinned_bb[Q_X][1] | pinned_bb[Q_X][2] | pinned_bb[Q_X][3]));
    while (temp) {
      pos = bsf_x(temp);
      temp2 = check_bb & file_attack_x[pos] & bd->capture_emp & r_attack_x[pos][((bd->all_pieces & r_mask_x[pos]) * r_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - r_shift_x[pos])];
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(bd->piece_set[pos], bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = (bd->bb_arr[bd->side][R_X] & ~(pinned_bb[R_X][0] | pinned_bb[R_X][2] | pinned_bb[R_X][3])) | (bd->bb_arr[bd->side][Q_X] & ~(pinned_bb[Q_X][0] | pinned_bb[Q_X][2] | pinned_bb[Q_X][3]));
    while (temp) {
      pos = bsf_x(temp);
      temp2 = check_bb & rank_attack_x[pos] & bd->capture_emp & r_attack_x[pos][((bd->all_pieces & r_mask_x[pos]) * r_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - r_shift_x[pos])];
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(bd->piece_set[pos], bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = (bd->bb_arr[bd->side][B_X] & ~(pinned_bb[B_X][0] | pinned_bb[B_X][1] | pinned_bb[B_X][3])) | (bd->bb_arr[bd->side][Q_X] & ~(pinned_bb[Q_X][0] | pinned_bb[Q_X][1] | pinned_bb[Q_X][3]));
    while (temp) {
      pos = bsf_x(temp);
      temp2 = check_bb & diagonal_attack_x[pos] & bd->capture_emp & b_attack_x[pos][((bd->all_pieces & b_mask_x[pos]) * b_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - b_shift_x[pos])];
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(bd->piece_set[pos], bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = (bd->bb_arr[bd->side][B_X] & ~(pinned_bb[B_X][0] | pinned_bb[B_X][1] | pinned_bb[B_X][2])) | (bd->bb_arr[bd->side][Q_X] & ~(pinned_bb[Q_X][0] | pinned_bb[Q_X][1] | pinned_bb[Q_X][2]));
    while (temp) {
      pos = bsf_x(temp);
      temp2 = check_bb & anti_diagonal_attack_x[pos] & bd->capture_emp & b_attack_x[pos][((bd->all_pieces & b_mask_x[pos]) * b_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - b_shift_x[pos])];
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(bd->piece_set[pos], bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
  }
  else {
    if (bd->side == WHITE_X) {
      temp2 = ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][1] | pinned_bb[P_X][2] | pinned_bb[P_X][3])) << BOARD_WIDTH_X) & bd->empty_sqrs & NOT_RANK_8_X;
      temp = temp2;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (temp2 << BOARD_WIDTH_X) & bd->empty_sqrs & RANK_4_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, EMPTY_X, pos - 2 * BOARD_WIDTH_X, pos, 0, ((bb_t_x)1) << (pos - BOARD_WIDTH_X));
        temp &= temp - 1;
      }
      temp = ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][1] | pinned_bb[P_X][2] | pinned_bb[P_X][3])) << BOARD_WIDTH_X) & bd->empty_sqrs & RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, R_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, B_X, EMPTY_X, pos - BOARD_WIDTH_X, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][3])) << (BOARD_WIDTH_X + 1)) & bd->capture & NOT_FILE_A_X & NOT_RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (bd->bb_arr[WHITE_X][P_X] << (BOARD_WIDTH_X + 1)) & bd->enp_pos & NOT_FILE_A_X;
      if (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, bd->enp_pos, 0);
        make_move_x(bd, --moves);
        if (post_check_2_x(bd)) {
          unmake_move_x(bd, moves);
          ++moves;
        }
        else {
          unmake_move_x(bd, moves);
          --index;
        }
      }
      temp = ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][2])) << (BOARD_WIDTH_X - 1)) & bd->capture & NOT_FILE_H_X & NOT_RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (bd->bb_arr[WHITE_X][P_X] << (BOARD_WIDTH_X - 1)) & bd->enp_pos & NOT_FILE_H_X;
      if (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, bd->enp_pos, 0);
        make_move_x(bd, --moves);
        if (post_check_2_x(bd)) {
          unmake_move_x(bd, moves);
          ++moves;
        }
        else {
          unmake_move_x(bd, moves);
          --index;
        }
      }
      temp = ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][3])) << (BOARD_WIDTH_X + 1)) & bd->capture & NOT_FILE_A_X & RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, R_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, B_X, bd->piece_set[pos], pos - BOARD_WIDTH_X - 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = ((bd->bb_arr[WHITE_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][2])) << (BOARD_WIDTH_X - 1)) & bd->capture & NOT_FILE_H_X & RANK_8_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, R_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, B_X, bd->piece_set[pos], pos - BOARD_WIDTH_X + 1, pos, 0, 0);
        temp &= temp - 1;
      }
    }
    else {
      temp2 = ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][1] | pinned_bb[P_X][2] | pinned_bb[P_X][3])) >> BOARD_WIDTH_X) & bd->empty_sqrs & NOT_RANK_1_X;
      temp = temp2;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (temp2 >> BOARD_WIDTH_X) & bd->empty_sqrs & RANK_5_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, EMPTY_X, pos + 2 * BOARD_WIDTH_X, pos, 0, ((bb_t_x)1) << (pos + BOARD_WIDTH_X));
        temp &= temp - 1;
      }
      temp = ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][1] | pinned_bb[P_X][2] | pinned_bb[P_X][3])) >> BOARD_WIDTH_X) & bd->empty_sqrs & RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, R_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        add_pro_move_x(P_X, B_X, EMPTY_X, pos + BOARD_WIDTH_X, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][3])) >> (BOARD_WIDTH_X + 1)) & bd->capture & NOT_FILE_H_X & NOT_RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (bd->bb_arr[BLACK_X][P_X] >> (BOARD_WIDTH_X + 1)) & bd->enp_pos & NOT_FILE_H_X;
      if (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, bd->enp_pos, 0);
        make_move_x(bd, --moves);
        if (post_check_2_x(bd)) {
          unmake_move_x(bd, moves);
          ++moves;
        }
        else {
          unmake_move_x(bd, moves);
          --index;
        }
      }
      temp = ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][2])) >> (BOARD_WIDTH_X - 1)) & bd->capture & NOT_FILE_A_X & NOT_RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = (bd->bb_arr[BLACK_X][P_X] >> (BOARD_WIDTH_X - 1)) & bd->enp_pos & NOT_FILE_A_X;
      if (temp) {
        pos = bsf_x(temp);
        add_move_x(P_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, bd->enp_pos, 0);
        make_move_x(bd, --moves);
        if (post_check_2_x(bd)) {
          unmake_move_x(bd, moves);
          ++moves;
        }
        else {
          unmake_move_x(bd, moves);
          --index;
        }
      }
      temp = ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][3])) >> (BOARD_WIDTH_X + 1)) & bd->capture & NOT_FILE_H_X & RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, R_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        add_pro_move_x(P_X, B_X, bd->piece_set[pos], pos + BOARD_WIDTH_X + 1, pos, 0, 0);
        temp &= temp - 1;
      }
      temp = ((bd->bb_arr[BLACK_X][P_X] & ~(pinned_bb[P_X][0] | pinned_bb[P_X][1] | pinned_bb[P_X][2])) >> (BOARD_WIDTH_X - 1)) & bd->capture & NOT_FILE_A_X & RANK_1_X;
      while (temp) {
        pos = bsf_x(temp);
        add_pro_move_x(P_X, N_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, Q_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, R_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        add_pro_move_x(P_X, B_X, bd->piece_set[pos], pos + BOARD_WIDTH_X - 1, pos, 0, 0);
        temp &= temp - 1;
      }
    }
    temp = bd->bb_arr[bd->side][N_X] & ~(pinned_bb[N_X][0] | pinned_bb[N_X][1] | pinned_bb[N_X][2] | pinned_bb[N_X][3]);
    while (temp) {
      pos = bsf_x(temp);
      temp2 = knight_attack_x[pos] & bd->capture_emp;
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(N_X, bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = bd->bb_arr[bd->side][K_X];
    while (temp) {
      pos = bsf_x(temp);
      temp2 = king_attack_x[pos] & bd->capture_emp & ~attack_bb;
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(K_X, bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = (bd->bb_arr[bd->side][R_X] & ~(pinned_bb[R_X][1] | pinned_bb[R_X][2] | pinned_bb[R_X][3])) | (bd->bb_arr[bd->side][Q_X] & ~(pinned_bb[Q_X][1] | pinned_bb[Q_X][2] | pinned_bb[Q_X][3]));
    while (temp) {
      pos = bsf_x(temp);
      temp2 = file_attack_x[pos] & bd->capture_emp & r_attack_x[pos][((bd->all_pieces & r_mask_x[pos]) * r_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - r_shift_x[pos])];
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(bd->piece_set[pos], bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = (bd->bb_arr[bd->side][R_X] & ~(pinned_bb[R_X][0] | pinned_bb[R_X][2] | pinned_bb[R_X][3])) | (bd->bb_arr[bd->side][Q_X] & ~(pinned_bb[Q_X][0] | pinned_bb[Q_X][2] | pinned_bb[Q_X][3]));
    while (temp) {
      pos = bsf_x(temp);
      temp2 = rank_attack_x[pos] & bd->capture_emp & r_attack_x[pos][((bd->all_pieces & r_mask_x[pos]) * r_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - r_shift_x[pos])];
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(bd->piece_set[pos], bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = (bd->bb_arr[bd->side][B_X] & ~(pinned_bb[B_X][0] | pinned_bb[B_X][1] | pinned_bb[B_X][3])) | (bd->bb_arr[bd->side][Q_X] & ~(pinned_bb[Q_X][0] | pinned_bb[Q_X][1] | pinned_bb[Q_X][3]));
    while (temp) {
      pos = bsf_x(temp);
      temp2 = diagonal_attack_x[pos] & bd->capture_emp & b_attack_x[pos][((bd->all_pieces & b_mask_x[pos]) * b_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - b_shift_x[pos])];
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(bd->piece_set[pos], bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    temp = (bd->bb_arr[bd->side][B_X] & ~(pinned_bb[B_X][0] | pinned_bb[B_X][1] | pinned_bb[B_X][2])) | (bd->bb_arr[bd->side][Q_X] & ~(pinned_bb[Q_X][0] | pinned_bb[Q_X][1] | pinned_bb[Q_X][2]));
    while (temp) {
      pos = bsf_x(temp);
      temp2 = anti_diagonal_attack_x[pos] & bd->capture_emp & b_attack_x[pos][((bd->all_pieces & b_mask_x[pos]) * b_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - b_shift_x[pos])];
      while (temp2) {
        pos2 = bsf_x(temp2);
        add_move_x(bd->piece_set[pos], bd->piece_set[pos2], pos, pos2, 0, 0);
        temp2 &= temp2 - 1;
      }
      temp &= temp - 1;
    }
    if (bd->castle[bd->side][0] && !(bd->all_pieces & can_castle_bb_x[bd->side][0]) && !(attack_bb & can_castle_attack_bb_x[bd->side][0])) {
      moves->castle = 1;
      moves->enp_pos = 0;
      moves->new_enp = 0;
      ++moves;
      ++index;
    }
    if (bd->castle[bd->side][1] && !(bd->all_pieces & can_castle_bb_x[bd->side][1]) && !(attack_bb & can_castle_attack_bb_x[bd->side][1])) {
      moves->castle = 2;
      moves->enp_pos = 0;
      moves->new_enp = 0;
      ++moves;
      ++index;
    }
  }
  return index;
}

int post_check_x(register board_x *bd) {
  register bb_t_x attack_bb = 0;
  if (bd->side == WHITE_X) {
    attack_bb |= (bd->bb_arr[WHITE_X][P_X] << (BOARD_WIDTH_X + 1)) & NOT_FILE_A_X;
    attack_bb |= (bd->bb_arr[WHITE_X][P_X] << (BOARD_WIDTH_X - 1)) & NOT_FILE_H_X;
  }
  else {
    attack_bb |= (bd->bb_arr[BLACK_X][P_X] >> (BOARD_WIDTH_X - 1)) & NOT_FILE_A_X;
    attack_bb |= (bd->bb_arr[BLACK_X][P_X] >> (BOARD_WIDTH_X + 1)) & NOT_FILE_H_X;
  }
  register bb_t_x knights = bd->bb_arr[bd->side][N_X];
  register bb_t_x l1 = (knights >> 1) & 0x7f7f7f7f7f7f7f7full;
  register bb_t_x l2 = (knights >> 2) & 0x3f3f3f3f3f3f3f3full;
  register bb_t_x r1 = (knights << 1) & 0xfefefefefefefefeull;
  register bb_t_x r2 = (knights << 2) & 0xfcfcfcfcfcfcfcfcull;
  register bb_t_x h1 = l1 | r1;
  register bb_t_x h2 = l2 | r2;
  attack_bb |= (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
  register bb_t_x temp = bd->bb_arr[bd->side][R_X] | bd->bb_arr[bd->side][Q_X];
  register int pos;
  while (temp) {
    pos = bsf_x(temp);
    attack_bb |= r_attack_x[pos][((bd->all_pieces & r_mask_x[pos]) * r_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - r_shift_x[pos])];
    temp &= temp - 1;
  }
  temp = bd->bb_arr[bd->side][B_X] | bd->bb_arr[bd->side][Q_X];
  while (temp) {
    pos = bsf_x(temp);
    attack_bb |= b_attack_x[pos][((bd->all_pieces & b_mask_x[pos]) * b_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - b_shift_x[pos])];
    temp &= temp - 1;
  }
  return !(attack_bb & bd->bb_arr[1 - bd->side][K_X]);
}

int post_check_2_x(register board_x *bd) {
  register bb_t_x attack_bb = 0;
  register bb_t_x temp = bd->bb_arr[bd->side][R_X] | bd->bb_arr[bd->side][Q_X];
  register int pos;
  while (temp) {
    pos = bsf_x(temp);
    attack_bb |= r_attack_x[pos][((bd->all_pieces & r_mask_x[pos]) * r_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - r_shift_x[pos])];
    temp &= temp - 1;
  }
  temp = bd->bb_arr[bd->side][B_X] | bd->bb_arr[bd->side][Q_X];
  while (temp) {
    pos = bsf_x(temp);
    attack_bb |= b_attack_x[pos][((bd->all_pieces & b_mask_x[pos]) * b_magic_x[pos]) >> (BOARD_WIDTH_X * BOARD_HEIGHT_X - b_shift_x[pos])];
    temp &= temp - 1;
  }
  return !(attack_bb & bd->bb_arr[1 - bd->side][K_X]);
}

void make_move_x(register board_x *bd, register move_x *mv) {
  mv->pre_castle[WHITE_X][0] = bd->castle[WHITE_X][0];
  mv->pre_castle[WHITE_X][1] = bd->castle[WHITE_X][1];
  mv->pre_castle[BLACK_X][0] = bd->castle[BLACK_X][0];
  mv->pre_castle[BLACK_X][1] = bd->castle[BLACK_X][1];
  mv->pre_enp = bd->enp_pos;
  bd->enp_pos = mv->new_enp;
  if (mv->castle) {
    bd->bb_arr[bd->side][K_X] ^= castle_king_bb_x[bd->side][0];
    bd->bb_arr[bd->side][K_X] ^= castle_king_bb_x[bd->side][mv->castle];
    bd->bb_arr[bd->side][R_X] ^= castle_rook_bb_x[bd->side][2 * mv->castle - 2];
    bd->bb_arr[bd->side][R_X] ^= castle_rook_bb_x[bd->side][2 * mv->castle - 1];
    bd->pieces_bb[bd->side] ^= castle_king_bb_x[bd->side][0];
    bd->pieces_bb[bd->side] ^= castle_king_bb_x[bd->side][mv->castle];
    bd->pieces_bb[bd->side] ^= castle_rook_bb_x[bd->side][2 * mv->castle - 2];
    bd->pieces_bb[bd->side] ^= castle_rook_bb_x[bd->side][2 * mv->castle - 1];
    bd->all_pieces ^= castle_king_bb_x[bd->side][0];
    bd->all_pieces ^= castle_king_bb_x[bd->side][mv->castle];
    bd->all_pieces ^= castle_rook_bb_x[bd->side][2 * mv->castle - 2];
    bd->all_pieces ^= castle_rook_bb_x[bd->side][2 * mv->castle - 1];
    bd->piece_set[castle_king_pos_x[bd->side][0]] = EMPTY_X;
    bd->piece_set[castle_king_pos_x[bd->side][mv->castle]] = K_X;
    bd->piece_set[castle_rook_pos_x[bd->side][2 * mv->castle - 2]] = EMPTY_X;
    bd->piece_set[castle_rook_pos_x[bd->side][2 * mv->castle - 1]] = R_X;
    bd->enp_pos = 0;
    bd->castle[bd->side][0] = 0;
    bd->castle[bd->side][1] = 0;
    bd->empty_sqrs = ~bd->all_pieces;
    bd->capture = bd->pieces_bb[bd->side];
    bd->capture_emp = bd->capture | bd->empty_sqrs;
    bd->side = 1 - bd->side;
    return;
  }
  register bb_t_x from_bb = ((bb_t_x)1) << mv->from, to_bb = ((bb_t_x)1) << mv->to;
  bd->piece_set[mv->from] = EMPTY_X;
  bd->piece_set[mv->to] = mv->p_piece;
  bd->bb_arr[bd->side][mv->piece] ^= from_bb;
  bd->bb_arr[bd->side][mv->p_piece] ^= to_bb;
  bd->pieces_bb[bd->side] ^= from_bb;
  bd->pieces_bb[bd->side] ^= to_bb;
  bd->all_pieces ^= from_bb;
  if (mv->c_piece != EMPTY_X) {
    bd->bb_arr[1 - bd->side][mv->c_piece] ^= to_bb;
    bd->pieces_bb[1 - bd->side] ^= to_bb;
  }
  else bd->all_pieces ^= to_bb;
  if (mv->piece == K_X) {
    bd->castle[bd->side][0] = 0;
    bd->castle[bd->side][1] = 0;
  }
  if (mv->from == castle_pos_x[bd->side][0])
    bd->castle[bd->side][0] = 0;
  else if (mv->from == castle_pos_x[bd->side][1])
    bd->castle[bd->side][1] = 0;
  if (mv->to == castle_pos_x[1 - bd->side][0])
    bd->castle[1 - bd->side][0] = 0;
  else if (mv->to == castle_pos_x[1 - bd->side][1])
    bd->castle[1 - bd->side][1] = 0;
  if (mv->enp_pos) {
    register bb_t_x enp_bb = (mv->enp_pos >> BOARD_WIDTH_X) << (2 * BOARD_WIDTH_X * bd->side);
    mv->enp_tar = bsf_x(enp_bb);
    bd->piece_set[mv->enp_tar] = EMPTY_X;
    bd->bb_arr[1 - bd->side][P_X] ^= enp_bb;
    bd->pieces_bb[1 - bd->side] ^= enp_bb;
    bd->all_pieces ^= enp_bb;
  }
  bd->empty_sqrs = ~bd->all_pieces;
  bd->capture = bd->pieces_bb[bd->side];
  bd->capture_emp = bd->capture | bd->empty_sqrs;
  bd->side = 1 - bd->side;
}

void unmake_move_x(register board_x *bd, register move_x *mv) {
  bd->castle[WHITE_X][0] = mv->pre_castle[WHITE_X][0];
  bd->castle[WHITE_X][1] = mv->pre_castle[WHITE_X][1];
  bd->castle[BLACK_X][0] = mv->pre_castle[BLACK_X][0];
  bd->castle[BLACK_X][1] = mv->pre_castle[BLACK_X][1];
  bd->enp_pos = mv->pre_enp;
  bd->side = 1 - bd->side;
  if (mv->castle) {
    bd->bb_arr[bd->side][K_X] ^= castle_king_bb_x[bd->side][0];
    bd->bb_arr[bd->side][K_X] ^= castle_king_bb_x[bd->side][mv->castle];
    bd->bb_arr[bd->side][R_X] ^= castle_rook_bb_x[bd->side][2 * mv->castle - 2];
    bd->bb_arr[bd->side][R_X] ^= castle_rook_bb_x[bd->side][2 * mv->castle - 1];
    bd->pieces_bb[bd->side] ^= castle_king_bb_x[bd->side][0];
    bd->pieces_bb[bd->side] ^= castle_king_bb_x[bd->side][mv->castle];
    bd->pieces_bb[bd->side] ^= castle_rook_bb_x[bd->side][2 * mv->castle - 2];
    bd->pieces_bb[bd->side] ^= castle_rook_bb_x[bd->side][2 * mv->castle - 1];
    bd->all_pieces ^= castle_king_bb_x[bd->side][0];
    bd->all_pieces ^= castle_king_bb_x[bd->side][mv->castle];
    bd->all_pieces ^= castle_rook_bb_x[bd->side][2 * mv->castle - 2];
    bd->all_pieces ^= castle_rook_bb_x[bd->side][2 * mv->castle - 1];
    bd->piece_set[castle_king_pos_x[bd->side][0]] = K_X;
    bd->piece_set[castle_king_pos_x[bd->side][mv->castle]] = EMPTY_X;
    bd->piece_set[castle_rook_pos_x[bd->side][2 * mv->castle - 2]] = R_X;
    bd->piece_set[castle_rook_pos_x[bd->side][2 * mv->castle - 1]] = EMPTY_X;
    bd->empty_sqrs = ~bd->all_pieces;
    bd->capture = bd->pieces_bb[1 - bd->side];
    bd->capture_emp = bd->capture | bd->empty_sqrs;
    return;
  }
  register bb_t_x from_bb = ((bb_t_x)1) << mv->from, to_bb = ((bb_t_x)1) << mv->to;
  bd->piece_set[mv->to] = mv->c_piece;
  bd->piece_set[mv->from] = mv->piece;
  bd->bb_arr[bd->side][mv->piece] ^= from_bb;
  bd->bb_arr[bd->side][mv->p_piece] ^= to_bb;
  bd->pieces_bb[bd->side] ^= from_bb;
  bd->pieces_bb[bd->side] ^= to_bb;
  bd->all_pieces ^= from_bb;
  if (mv->c_piece != EMPTY_X) {
    bd->bb_arr[1 - bd->side][mv->c_piece] ^= to_bb;
    bd->pieces_bb[1 - bd->side] ^= to_bb;
  }
  else bd->all_pieces ^= to_bb;
  if (mv->enp_pos) {
    register bb_t_x enp_bb = ((bb_t_x)1) << mv->enp_tar;
    bd->piece_set[mv->enp_tar] = P_X;
    bd->bb_arr[1 - bd->side][P_X] ^= enp_bb;
    bd->pieces_bb[1 - bd->side] ^= enp_bb;
    bd->all_pieces ^= enp_bb;
  }
  bd->empty_sqrs = ~bd->all_pieces;
  bd->capture = bd->pieces_bb[1 - bd->side];
  bd->capture_emp = bd->capture | bd->empty_sqrs;
}