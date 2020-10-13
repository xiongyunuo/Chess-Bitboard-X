#include "move_gen_x.hpp"
#include <cmath>

void chess_x::move_gen::initialize() {
  width = bd.get_width();
  height = bd.get_height();
  piece_bitboard = bd.get_piece_bitboard();
  piece_set = bd.get_piece_set();
  for (player_size_type i = 0; i < NUM_PLAYER; ++i) {
    all_pieces_arr[i] = bb_type(0);
    for (piece_size_type j = 0; j < PIECE_NUMBER; ++j)
      all_pieces_arr[i] |= piece_bitboard[i][j];
  }
  all_pieces = bb_type(0);
  for (player_size_type i = 0; i < NUM_PLAYER; ++i)
    all_pieces |= all_pieces_arr[i];
  empty_sqrs = ~all_pieces;
  for (player_size_type i = 0; i < NUM_PLAYER; ++i)
    if (var.king_pos[0][i] >= 0 && var.king_pos[1][i] >= 0)
      king_pos[i] = bb_type(1) << (var.king_pos[1][i] * width + var.king_pos[0][i]);
    else
      king_pos[i] = bb_type(0);
  for (player_size_type i = 0; i < NUM_PLAYER; ++i)
    if (var.enp_x[i] >= 0 && var.enp_y[i] >= 0)
      enp_pos[i] = bb_type(1) << (var.enp_y[i] * width + var.enp_x[i]);
    else
      enp_pos[i] = bb_type(0);
  for (player_size_type i = 0; i < NUM_PLAYER; ++i) {
    castle_bb[0][i] = bb_type(0);
    board_size_type sx = var.castle_pos[0][i], sy = var.castle_pos[1][i], ex = var.castle_pos[2][i], ey = var.castle_pos[3][i];
    board_size_type dx = ex - sx, dy = ey - sy;
    if (dx == 0 && dy == 0) continue;
    if (dx) dx /= std::abs(ex - sx);
    if (dy) dy /= std::abs(ey - sy);
    for (board_size_type x = sx, y = sy; x != ex || y != ey; x += dx, y += dy)
      castle_bb[0][i] |= bb_type(1) << (y * height + x);
    castle_bb[0][i] |= bb_type(1) << (ey * height + ex);
  }
  for (player_size_type i = 0; i < NUM_PLAYER; ++i) {
    castle_bb[1][i] = bb_type(0);
    board_size_type sx = var.castle_pos[0][i], sy = var.castle_pos[1][i], ex = var.castle_pos[4][i], ey = var.castle_pos[5][i];
    board_size_type dx = ex - sx, dy = ey - sy;
    if (dx == 0 && dy == 0) continue;
    if (dx) dx /= std::abs(ex - sx);
    if (dy) dy /= std::abs(ey - sy);
    for (board_size_type x = sx, y = sy; x != ex || y != ey; x += dx, y += dy)
      castle_bb[1][i] |= bb_type(1) << (y * height + x);
    castle_bb[1][i] |= bb_type(1) << (ey * height + ex);
  }
  for (player_size_type i = 0; i < NUM_PLAYER; ++i) {
    king_rook_bb[0][i] = bb_type(0);
    board_size_type sx = var.castle_pos[0][i], sy = var.castle_pos[1][i], ex = var.castle_pos[6][i], ey = var.castle_pos[7][i];
    board_size_type dx = ex - sx, dy = ey - sy;
    if (dx == 0 && dy == 0) continue;
    if (dx) dx /= std::abs(ex - sx);
    if (dy) dy /= std::abs(ey - sy);
    for (board_size_type x = sx + dx, y = sy + dy; x != ex || y != ey; x += dx, y += dy)
      king_rook_bb[0][i] |= bb_type(1) << (y * height + x);
  }
  for (player_size_type i = 0; i < NUM_PLAYER; ++i) {
    king_rook_bb[1][i] = bb_type(0);
    board_size_type sx = var.castle_pos[0][i], sy = var.castle_pos[1][i], ex = var.castle_pos[10][i], ey = var.castle_pos[11][i];
    board_size_type dx = ex - sx, dy = ey - sy;
    if (dx == 0 && dy == 0) continue;
    if (dx) dx /= std::abs(ex - sx);
    if (dy) dy /= std::abs(ey - sy);
    for (board_size_type x = sx + dx, y = sy + dy; x != ex || y != ey; x += dx, y += dy)
      king_rook_bb[1][i] |= bb_type(1) << (y * height + x);
  }
  for (player_size_type i = 0; i < NUM_PLAYER; ++i) {
    castle_king_bb[0][i] = bb_type(1) << (var.castle_pos[0][i] + var.castle_pos[1][i] * height);
    castle_king_bb[1][i] = bb_type(1) << (var.castle_pos[2][i] + var.castle_pos[3][i] * height);
    castle_king_bb[2][i] = bb_type(1) << (var.castle_pos[4][i] + var.castle_pos[5][i] * height);
    castle_rook_bb[0][i] = bb_type(1) << (var.castle_pos[6][i] + var.castle_pos[7][i] * height);
    castle_rook_bb[1][i] = bb_type(1) << (var.castle_pos[8][i] + var.castle_pos[9][i] * height);
    castle_rook_bb[2][i] = bb_type(1) << (var.castle_pos[10][i] + var.castle_pos[11][i] * height);
    castle_rook_bb[3][i] = bb_type(1) << (var.castle_pos[12][i] + var.castle_pos[13][i] * height);
    castle_king[0][i] = var.castle_pos[0][i] + var.castle_pos[1][i] * height;
    castle_king[1][i] = var.castle_pos[2][i] + var.castle_pos[3][i] * height;
    castle_king[2][i] = var.castle_pos[4][i] + var.castle_pos[5][i] * height;
    castle_rook[0][i] = var.castle_pos[6][i] + var.castle_pos[7][i] * height;
    castle_rook[1][i] = var.castle_pos[8][i] + var.castle_pos[9][i] * height;
    castle_rook[2][i] = var.castle_pos[10][i] + var.castle_pos[11][i] * height;
    castle_rook[3][i] = var.castle_pos[12][i] + var.castle_pos[13][i] * height;
  }
  for (player_size_type i = 0; i < NUM_PLAYER; ++i) {
    if (!(king_pos[i] & castle_king_bb[0][i])) {
      var.can_castle[0][i] = false;
      var.can_castle[1][i] = false;
    }
    if (!(piece_bitboard[i][R] & castle_rook_bb[0][i]))
      var.can_castle[0][i] = false;
    if (!(piece_bitboard[i][R] & castle_rook_bb[2][i]))
      var.can_castle[1][i] = false;
  }
  update_capture(var.side);
  precompute();
}

void chess_x::move_gen::precompute() {
  high = bb_type(1) << (width * height - 1);
  ranks = new bb_type[height]();
  for (board_size_type i = 0; i < height; ++i)
    for (board_size_type j = 0; j < width; ++j)
      ranks[i] |= bb_type(1) << (i * width + j);
  files = new bb_type[width]();
  for (board_size_type j = 0; j < width; ++j)
    for (board_size_type i = 0; i < height; ++i)
      files[j] |= bb_type(1) << (i * width + j);
  not_files = new bb_type[width]();
  for (board_size_type j = 0; j < width / 2; ++j) {
    for (board_size_type i = 0; i <= j; ++i)
      not_files[j] |= files[i];
    not_files[j] = ~not_files[j];
  }
  for (board_size_type j = width / 2; j < width; ++j) {
    for (board_size_type i = width - 1; i >= j; --i)
      not_files[j] |= files[i];
    not_files[j] = ~not_files[j];
  }
  knight_attack = new bb_type[width * height]();
  for (board_size_type i = 0; i < height; ++i)
    for (board_size_type j = 0; j < width; ++j) {
      bb_type knights = bb_type(1) << (i * width + j), east, west, attacks;
      east = (knights & not_files[width - 1]) << 1;
      west = (knights & not_files[0]) >> 1;
      attacks = (east | west) << (2 * width);
      attacks |= (east | west) >> (2 * width);
      east = (east & not_files[width - 1]) << 1;
      west = (west & not_files[0]) >> 1;
      attacks |= (east | west) << width;
      attacks |= (east | west) >> width;
      knight_attack[i * width + j] = attacks;
    }
  king_attack = new bb_type[width * height]();
  for (board_size_type i = 0; i < height; ++i)
    for (board_size_type j = 0; j < width; ++j) {
      bb_type kings = bb_type(1) << (i * width + j);
      bb_type attacks = ((kings & not_files[width - 1]) << 1) | ((kings & not_files[0]) >> 1);
      kings |= attacks;
      attacks |= (kings << width) | (kings >> width);
      king_attack[i * width + j] = attacks;
    }
  file_attack = new bb_type[width * height]();
  for (board_size_type i = 0; i < height; ++i)
    for (board_size_type j = 0; j < width; ++j) {
      for (board_size_type k = i + 1; k < height; ++k)
        file_attack[i * width + j] |= bb_type(1) << (k * width + j);
      for (board_size_type k = 0; k < i; ++k)
        file_attack[i * width + j] |= bb_type(1) << (k * width + j);
    }
  rank_attack = new bb_type[width * height]();
  for (board_size_type i = 0; i < height; ++i)
    for (board_size_type j = 0; j < width; ++j) {
      for (board_size_type k = j + 1; k < width; ++k)
        rank_attack[i * width + j] |= bb_type(1) << (i * width + k);
      for (board_size_type k = 0; k < j; ++k)
        rank_attack[i * width + j] |= bb_type(1) << (i * width + k);
    }
  diag_attack = new bb_type[width * height]();
  for (board_size_type i = 0; i < height; ++i)
    for (board_size_type j = 0; j < width; ++j) {
      board_size_type i2 = i + 1, j2 = j + 1;
      while (i2 < height && j2 < width) {
        diag_attack[i * width + j] |= bb_type(1) << (i2 * width + j2);
        ++i2; ++j2;
      }
      i2 = i - 1; j2 = j - 1;
      while (i2 >= 0 && j2 >= 0) {
        diag_attack[i * width + j] |= bb_type(1) << (i2 * width + j2);
        --i2; --j2;
      }
    }
  anti_diag_attack = new bb_type[width * height]();
  for (board_size_type i = 0; i < height; ++i)
    for (board_size_type j = 0; j < width; ++j) {
      board_size_type i2 = i + 1, j2 = j - 1;
      while (i2 < height && j2 >= 0) {
        anti_diag_attack[i * width + j] |= bb_type(1) << (i2 * width + j2);
        ++i2; --j2;
      }
      i2 = i - 1; j2 = j + 1;
      while (i2 >= 0 && j2 < width) {
        anti_diag_attack[i * width + j] |= bb_type(1) << (i2 * width + j2);
        --i2; ++j2;
      }
    }
  upper = new bb_type[width * height]();
  lower = new bb_type[width * height]();
  for (board_size_type i = 0; i < width * height; ++i) {
    for (board_size_type j = 0; j < i; ++j)
      lower[i] |= bb_type(1) << j;
    for (board_size_type j = i + 1; j < width * height; ++j)
      upper[i] |= bb_type(1) << j;
  }
}

void chess_x::move_gen::destroy() noexcept {
  if (ranks) delete [] ranks;
  if (files) delete [] files;
  if (not_files) delete [] not_files;
  if (knight_attack) delete [] knight_attack;
  if (king_attack) delete [] king_attack;
  if (file_attack) delete [] file_attack;
  if (rank_attack) delete [] rank_attack;
  if (diag_attack) delete [] diag_attack;
  if (anti_diag_attack) delete [] anti_diag_attack;
  if (upper) delete [] upper;
  if (lower) delete [] lower;
}

int chess_x::move_gen::generate_moves(move *moves) const {
  int index = 0;
  bb_type one(1);
  board_size_type pos, pos2;
  bb_type temp = ((piece_bitboard[var.side][P] << width) >> (var.side * 2 * width)) & empty_sqrs & ~ranks[var.pawn_prom[var.side]];
  bb_type temp2 = temp;
  while (temp) {
    pos = __builtin_ctzll(temp);
    moves[index] = move(P, var.side, one << (pos - width + var.side * 2 * width), one << (pos), empty, P);
    ++index;
    temp &= temp - one;
  }
  temp = ((temp2 << width) >> (var.side * 2 * width)) & empty_sqrs & ranks[var.pawn_base[var.side]] & ~ranks[var.pawn_prom[var.side]];
  while (temp) {
    pos = __builtin_ctzll(temp);
    moves[index] = move(P, var.side, one << (pos - 2 * width + var.side * 4 * width), one << (pos), empty, P, false, one << (pos - width + var.side * 2 * width));
    ++index;
    temp &= temp - one;
  }
  temp = ((temp2 << width) >> (var.side * 2 * width)) & empty_sqrs & ranks[var.pawn_base[var.side]] & ranks[var.pawn_prom[var.side]];
  while (temp) {
    pos = __builtin_ctzll(temp);
    for (piece_size_type i = 0; i < var.prom_count; ++i) {
      moves[index] = move(P, var.side, one << (pos - 2 * width + var.side * 4 * width), one << (pos), empty, var.promotion[i]);
      ++index;
    }
    temp &= temp - one;
  }
  temp = (((piece_bitboard[var.side][P] & not_files[width - 1]) << (width + 1)) >> (var.side * 2 * width)) & (capture | enp_pos[1 - var.side]) & ~ranks[var.pawn_prom[var.side]];
  while (temp) {
    pos = __builtin_ctzll(temp);
    moves[index] = move(P, var.side, one << (pos - width - 1 + var.side * 2 * width), one << (pos), piece_set[pos], P, (one << pos) & enp_pos[1 - var.side]);
    ++index;
    temp &= temp - one;
  }
  temp = (((piece_bitboard[var.side][P] & not_files[0]) << (width - 1)) >> (var.side * 2 * width)) & (capture | enp_pos[1 - var.side]) & ~ranks[var.pawn_prom[var.side]];
  while (temp) {
    pos = __builtin_ctzll(temp);
    moves[index] = move(P, var.side, one << (pos - width + 1 + var.side * 2 * width), one << (pos), piece_set[pos], P, (one << pos) & enp_pos[1 - var.side]);
    ++index;
    temp &= temp - one;
  }
  temp = ((piece_bitboard[var.side][P] << width) >> (var.side * 2 * width)) & empty_sqrs & ranks[var.pawn_prom[var.side]];
  while (temp) {
    pos = __builtin_ctzll(temp);
    for (piece_size_type i = 0; i < var.prom_count; ++i) {
      moves[index] = move(P, var.side, one << (pos - width + var.side * 2 * width), one << (pos), empty, var.promotion[i]);
      ++index;
    }
    temp &= temp - one;
  }
  temp = (((piece_bitboard[var.side][P] & not_files[width - 1]) << (width + 1)) >> (var.side * 2 * width)) & (capture | enp_pos[1 - var.side]) & ranks[var.pawn_prom[var.side]];
  while (temp) {
    pos = __builtin_ctzll(temp);
    for (piece_size_type i = 0; i < var.prom_count; ++i) {
      moves[index] = move(P, var.side, one << (pos - width - 1 + var.side * 2 * width), one << (pos), piece_set[pos], var.promotion[i], (one << pos) & enp_pos[1 - var.side]);
      ++index;
    }
    temp &= temp - one;
  }
  temp = (((piece_bitboard[var.side][P] & not_files[0]) << (width - 1)) >> (var.side * 2 * width)) & (capture | enp_pos[1 - var.side]) & ranks[var.pawn_prom[var.side]];
  while (temp) {
    pos = __builtin_ctzll(temp);
    for (piece_size_type i = 0; i < var.prom_count; ++i) {
      moves[index] = move(P, var.side, one << (pos - width + 1 + var.side * 2 * width), one << (pos), piece_set[pos], var.promotion[i], (one << pos) & enp_pos[1 - var.side]);
      ++index;
    }
    temp &= temp - one;
  }
  temp = piece_bitboard[var.side][N];
  while (temp) {
    pos = __builtin_ctzll(temp);
    temp2 = knight_attack[pos] & capture_empty;
    while (temp2) {
      pos2 = __builtin_ctzll(temp2);
      moves[index] = move(N, var.side, one << pos, one << pos2, piece_set[pos2], N);
      ++index;
      temp2 &= temp2 - one;
    }
    temp &= temp - one;
  }
  temp = piece_bitboard[var.side][K];
  while (temp) {
    pos = __builtin_ctzll(temp);
    temp2 = king_attack[pos] & capture_empty;
    while (temp2) {
      pos2 = __builtin_ctzll(temp2);
      moves[index] = move(K, var.side, one << pos, one << pos2, piece_set[pos2], K);
      ++index;
      temp2 &= temp2 - one;
    }
    temp &= temp - one;
  }
  bb_type occu, up, low;
  temp = piece_bitboard[var.side][R] | piece_bitboard[var.side][Q];
  while (temp) {
    pos = __builtin_ctzll(temp);
    occu = all_pieces & file_attack[pos];
    up = occu & upper[pos];
    up &= ~(up - one);
    low = high >> __builtin_clzll((occu & lower[pos]) | one);
    temp2 = ((up << 1) - low) & file_attack[pos] & capture_empty;
    while (temp2) {
      pos2 = __builtin_ctzll(temp2);
      moves[index] = move(piece_set[pos], var.side, one << pos, one << pos2, piece_set[pos2], piece_set[pos]);
      ++index;
      temp2 &= temp2 - one;
    }
    occu = all_pieces & rank_attack[pos];
    up = occu & upper[pos];
    up &= ~(up - one);
    low = high >> __builtin_clzll((occu & lower[pos]) | one);
    temp2 = ((up << 1) - low) & rank_attack[pos] & capture_empty;
    while (temp2) {
      pos2 = __builtin_ctzll(temp2);
      moves[index] = move(piece_set[pos], var.side, one << pos, one << pos2, piece_set[pos2], piece_set[pos]);
      ++index;
      temp2 &= temp2 - one;
    }
    temp &= temp - one;
  }
  temp = piece_bitboard[var.side][B] | piece_bitboard[var.side][Q];
  while (temp) {
    pos = __builtin_ctzll(temp);
    occu = all_pieces & diag_attack[pos];
    up = occu & upper[pos];
    up &= ~(up - one);
    low = high >> __builtin_clzll((occu & lower[pos]) | one);
    temp2 = ((up << 1) - low) & diag_attack[pos] & capture_empty;
    while (temp2) {
      pos2 = __builtin_ctzll(temp2);
      moves[index] = move(piece_set[pos], var.side, one << pos, one << pos2, piece_set[pos2], piece_set[pos]);
      ++index;
      temp2 &= temp2 - one;
    }
    occu = all_pieces & anti_diag_attack[pos];
    up = occu & upper[pos];
    up &= ~(up - one);
    low = high >> __builtin_clzll((occu & lower[pos]) | one);
    temp2 = ((up << 1) - low) & anti_diag_attack[pos] & capture_empty;
    while (temp2) {
      pos2 = __builtin_ctzll(temp2);
      moves[index] = move(piece_set[pos], var.side, one << pos, one << pos2, piece_set[pos2], piece_set[pos]);
      ++index;
      temp2 &= temp2 - one;
    }
    temp &= temp - one;
  }
  if (var.can_castle[0][var.side] && !(all_pieces & king_rook_bb[0][var.side])) {
    moves[index] = move(1, var.side);
    ++index;
  }
  if (var.can_castle[1][var.side] && !(all_pieces & king_rook_bb[1][var.side])) {
    moves[index] = move(2, var.side);
    ++index;
  }
  return index;
}

void chess_x::move_gen::update_attack_bb(player_size_type c) const {
  attack_bb |= ((piece_bitboard[c][P] & not_files[width - 1]) << (width + 1)) >> (c * 2 * width);
  attack_bb |= ((piece_bitboard[c][P] & not_files[0]) << (width - 1)) >> (c * 2 * width);
  bb_type knights = piece_bitboard[c][N], east, west, attacks;
  east = (knights & not_files[width - 1]) << 1;
  west = (knights & not_files[0]) >> 1;
  attacks = (east | west) << (2 * width);
  attacks |= (east | west) >> (2 * width);
  east = (east & not_files[width - 1]) << 1;
  west = (west & not_files[0]) >> 1;
  attacks |= (east | west) << width;
  attacks |= (east | west) >> width;
  attack_bb |= attacks;
  bb_type kings = piece_bitboard[c][K];
  attacks = ((kings & not_files[width - 1]) << 1) | ((kings & not_files[0]) >> 1);
  kings |= attacks;
  attacks |= (kings << width) | (kings >> width);
  attack_bb |= attacks;
  bb_type occu, up, low;
  bb_type temp = piece_bitboard[c][R] | piece_bitboard[c][Q], one(1);
  board_size_type pos;
  while (temp) {
    pos = __builtin_ctzll(temp);
    occu = all_pieces & file_attack[pos];
    up = occu & upper[pos];
    up &= ~(up - one);
    low = high >> __builtin_clzll((occu & lower[pos]) | one);
    attack_bb |= ((up << 1) - low) & file_attack[pos];
    occu = all_pieces & rank_attack[pos];
    up = occu & upper[pos];
    up &= ~(up - one);
    low = high >> __builtin_clzll((occu & lower[pos]) | one);
    attack_bb |= ((up << 1) - low) & rank_attack[pos];
    temp &= temp - one;
  }
  temp = piece_bitboard[c][B] | piece_bitboard[c][Q];
  while (temp) {
    pos = __builtin_ctzll(temp);
    occu = all_pieces & diag_attack[pos];
    up = occu & upper[pos];
    up &= ~(up - one);
    low = high >> __builtin_clzll((occu & lower[pos]) | one);
    attack_bb |= ((up << 1) - low) & diag_attack[pos];
    occu = all_pieces & anti_diag_attack[pos];
    up = occu & upper[pos];
    up &= ~(up - one);
    low = high >> __builtin_clzll((occu & lower[pos]) | one);
    attack_bb |= ((up << 1) - low) & anti_diag_attack[pos];
    temp &= temp - one;
  }
}

bool chess_x::move_gen::post_check(const move &m) const {
  attack_bb = bb_type(0);
  #ifdef IS_2PC
  update_attack_bb(var.side);
  #else
  for (player_size_type i = 0; i < NUM_PLAYER; ++i)
    if (var.enemy[m.turn][i])
      update_attack_bb(i);
  #endif
  if (m.castle)
    return !(attack_bb & castle_bb[m.castle - 1][m.turn]);
  return !(attack_bb & king_pos[m.turn]);
}

void chess_x::move_gen::make_move(move &m) {
  m.castle_index = var.side;
  m.pre_castle[0] = var.can_castle[0][var.side];
  m.pre_castle[1] = var.can_castle[1][var.side];
  if (m.castle) {
    piece_bitboard[m.turn][K] ^= castle_king_bb[0][m.turn];
    piece_bitboard[m.turn][K] ^= castle_king_bb[m.castle][m.turn];
    piece_bitboard[m.turn][R] ^= castle_rook_bb[2 * m.castle - 2][m.turn];
    piece_bitboard[m.turn][R] ^= castle_rook_bb[2 * m.castle - 1][m.turn];
    all_pieces_arr[m.turn] ^= castle_king_bb[0][m.turn];
    all_pieces_arr[m.turn] ^= castle_king_bb[m.castle][m.turn];
    all_pieces_arr[m.turn] ^= castle_rook_bb[2 * m.castle - 2][m.turn];
    all_pieces_arr[m.turn] ^= castle_rook_bb[2 * m.castle - 1][m.turn];
    all_pieces ^= castle_king_bb[0][m.turn];
    all_pieces ^= castle_king_bb[m.castle][m.turn];
    all_pieces ^= castle_rook_bb[2 * m.castle - 2][m.turn];
    all_pieces ^= castle_rook_bb[2 * m.castle - 1][m.turn];
    piece_set[castle_king[0][m.turn]] = empty;
    piece_set[castle_king[m.castle][m.turn]] = K;
    piece_set[castle_rook[2 * m.castle - 2][m.turn]] = empty;
    piece_set[castle_rook[2 * m.castle - 1][m.turn]] = R;
    king_pos[var.side] = castle_king_bb[m.castle][m.turn];
    m.old_enp = enp_pos[var.side];
    enp_pos[var.side] = bb_type(0);
    var.can_castle[0][var.side] = false;
    var.can_castle[1][var.side] = false;
    var.side = 1 - var.side;
    empty_sqrs = ~all_pieces;
    update_capture(var.side);
    return;
  }
  piece_bitboard[m.turn][m.piece] ^= m.from;
  piece_bitboard[m.turn][m.pro_p] ^= m.to;
  all_pieces_arr[m.turn] ^= m.from;
  all_pieces_arr[m.turn] ^= m.to;
  all_pieces ^= m.from;
  if (m.captured != empty) {
    all_pieces_arr[1 - m.turn] ^= m.to;
    piece_bitboard[1 - m.turn][m.captured] ^= m.to;
  }
  else all_pieces ^= m.to;
  if (m.from == king_pos[var.side]) {
    var.can_castle[0][var.side] = false;
    var.can_castle[1][var.side] = false;
    king_pos[var.side] = m.to;
  }
  if (var.can_castle[0][var.side] && m.from == castle_rook_bb[0][var.side])
    var.can_castle[0][var.side] = false;
  else if (var.can_castle[1][var.side] && m.from == castle_rook_bb[2][var.side])
    var.can_castle[1][var.side] = false;
  if (var.can_castle[0][1 - var.side] && m.to == castle_rook_bb[0][1 - var.side]) {
    m.castle_index = 1 - var.side;
    m.pre_castle[0] = var.can_castle[0][1 - var.side];
    m.pre_castle[1] = var.can_castle[1][1 - var.side];
    var.can_castle[0][1 - var.side] = false;
  }
  else if (var.can_castle[1][1 - var.side] && m.to == castle_rook_bb[2][1 - var.side]) {
    m.castle_index = 1 - var.side;
    m.pre_castle[0] = var.can_castle[0][1 - var.side];
    m.pre_castle[1] = var.can_castle[1][1 - var.side];
    var.can_castle[1][1 - var.side] = false;
  }
  piece_set[__builtin_ctzll(m.from)] = empty;
  piece_set[__builtin_ctzll(m.to)] = m.pro_p;
  if (m.is_enp) {
    bb_type enp_bb = (m.to >> width) << (2 * width * m.turn);
    piece_bitboard[1 - m.turn][P] ^= enp_bb;
    all_pieces_arr[1 - m.turn] ^= enp_bb;
    all_pieces ^= enp_bb;
    piece_set[__builtin_ctzll(enp_bb)] = empty;
  }
  m.old_enp = enp_pos[var.side];
  enp_pos[var.side] = m.new_enp;
  var.side = 1 - var.side;
  empty_sqrs = ~all_pieces;
  update_capture(var.side);
}

void chess_x::move_gen::unmake_move(const move &m) {
  var.can_castle[0][m.castle_index] = m.pre_castle[0];
  var.can_castle[1][m.castle_index] = m.pre_castle[1];
  if (m.castle) {
    piece_bitboard[m.turn][K] ^= castle_king_bb[0][m.turn];
    piece_bitboard[m.turn][K] ^= castle_king_bb[m.castle][m.turn];
    piece_bitboard[m.turn][R] ^= castle_rook_bb[2 * m.castle - 2][m.turn];
    piece_bitboard[m.turn][R] ^= castle_rook_bb[2 * m.castle - 1][m.turn];
    all_pieces_arr[m.turn] ^= castle_king_bb[0][m.turn];
    all_pieces_arr[m.turn] ^= castle_king_bb[m.castle][m.turn];
    all_pieces_arr[m.turn] ^= castle_rook_bb[2 * m.castle - 2][m.turn];
    all_pieces_arr[m.turn] ^= castle_rook_bb[2 * m.castle - 1][m.turn];
    all_pieces ^= castle_king_bb[0][m.turn];
    all_pieces ^= castle_king_bb[m.castle][m.turn];
    all_pieces ^= castle_rook_bb[2 * m.castle - 2][m.turn];
    all_pieces ^= castle_rook_bb[2 * m.castle - 1][m.turn];
    piece_set[castle_king[0][m.turn]] = K;
    piece_set[castle_king[m.castle][m.turn]] = empty;
    piece_set[castle_rook[2 * m.castle - 2][m.turn]] = R;
    piece_set[castle_rook[2 * m.castle - 1][m.turn]] = empty;
    var.side = m.turn;
    enp_pos[var.side] = m.old_enp;
    empty_sqrs = ~all_pieces;
    king_pos[var.side] = castle_king_bb[0][m.turn];
    update_capture(var.side);
    return;
  }
  piece_bitboard[m.turn][m.piece] ^= m.from;
  piece_bitboard[m.turn][m.pro_p] ^= m.to;
  all_pieces_arr[m.turn] ^= m.from;
  all_pieces_arr[m.turn] ^= m.to;
  all_pieces ^= m.from;
  piece_set[__builtin_ctzll(m.from)] = m.piece;
  piece_set[__builtin_ctzll(m.to)] = m.captured;
  if (m.captured != empty) {
    all_pieces_arr[1 - m.turn] ^= m.to;
    piece_bitboard[1 - m.turn][m.captured] ^= m.to;
  }
  else all_pieces ^= m.to;
  if (m.is_enp) {
    bb_type enp_bb = (m.to >> width) << (2 * width * m.turn);
    piece_bitboard[1 - m.turn][P] ^= enp_bb;
    all_pieces_arr[1 - m.turn] ^= enp_bb;
    all_pieces ^= enp_bb;
    piece_set[__builtin_ctzll(enp_bb)] = P;
  }
  var.side = m.turn;
  enp_pos[var.side] = m.old_enp;
  if (m.to == king_pos[var.side]) king_pos[var.side] = m.from;
  empty_sqrs = ~all_pieces;
  update_capture(var.side);
}

void chess_x::move_gen::print_board(std::ostream &out) const {
  for (board_size_type i = 0; i < height; ++i) {
    for (board_size_type j = 0; j < width; ++j)
      std::cout << piece_to_str(piece_set[bd.coor_to_n(i, j)]) << " ";
    std::cout << std::endl;
  }
}