#ifndef VARIANT_X_HPP
#define VARIANT_X_HPP

#include <iostream>
#include "chess_def_x.hpp"

namespace chess_x {
  template<player_size_type N> struct variant;
  template<player_size_type N> std::ostream& operator<<(std::ostream &out, const variant<N> &var);
  inline void variant_test();
}

template<chess_x::player_size_type N> struct chess_x::variant {
  friend std::ostream& operator<<<N>(std::ostream &out, const variant<N> &var);
  variant() noexcept : can_castle(), castle_pos(), king_pos(), enemy(), side(0), enp_x(), enp_y(), pawn_base(), pawn_prom(), half_move_clock(0), full_move(1), promotion(), prom_count(0) {}
  variant(const bool (&cc)[2][N], const board_size_type (&cp)[14][N], const bool (&ene)[N][N], const board_size_type (&pb)[N], const board_size_type (&pp)[N], piece_size_type pro_c, const piece_type (&pro_t)[PIECE_NUMBER], const board_size_type (&kp)[2][N]) noexcept;
  virtual ~variant() noexcept = default;
  bool can_castle[2][N];
  bool enemy[N][N];
  board_size_type castle_pos[14][N], king_pos[2][N];
  player_size_type side;
  board_size_type enp_x[N], enp_y[N], pawn_base[N], pawn_prom[N];
  unsigned half_move_clock, full_move;
  piece_type promotion[PIECE_NUMBER];
  piece_size_type prom_count;
};

template<chess_x::player_size_type N> chess_x::variant<N>::variant(const bool (&cc)[2][N], const board_size_type (&cp)[14][N], const bool (&ene)[N][N], const board_size_type (&pb)[N], const board_size_type (&pp)[N], piece_size_type pro_c, const piece_type (&pro_t)[PIECE_NUMBER], const board_size_type (&kp)[2][N]) noexcept : side(0), half_move_clock(0), full_move(1), prom_count(pro_c) {
  for (int i = 0; i < 2; ++i)
    for (player_size_type j = 0; j < N; ++j)
      can_castle[i][j] = cc[i][j];
  for (int i = 0; i < 14; ++i)
    for (player_size_type j = 0; j < N; ++j)
      castle_pos[i][j] = cp[i][j];
  for (player_size_type i = 0; i < N; ++i) {
    enp_x[i] = -1;
    enp_y[i] = -1;
  }
  for (player_size_type i = 0; i < N; ++i)
    for (player_size_type j = 0; j < N; ++j)
      enemy[i][j] = ene[i][j];
  for (player_size_type i = 0; i < N; ++i) {
    pawn_base[i] = pb[i];
    pawn_prom[i] = pp[i];
  }
  for (piece_size_type i = 0; i < PIECE_NUMBER; ++i)
    promotion[i] = pro_t[i];
  for (int i = 0; i < 2; ++i)
    for (player_size_type j = 0; j < N; ++j)
      king_pos[i][j] = kp[i][j];
}

template<chess_x::player_size_type N> std::ostream& chess_x::operator<<(std::ostream &out, const chess_x::variant<N> &var) {
  out << var.side << std::endl;
  for (int i = 0; i < 2; ++i)
    for (player_size_type j = 0; j < N; ++j)
      out << var.can_castle[i][j] << " ";
  out << std::endl;
  for (int i = 0; i < 14; ++i)
    for (player_size_type j = 0; j < N; ++j)
      out << var.castle_pos[i][j] << " ";
  out << std::endl;
  for (player_size_type i = 0; i < N; ++i)
    out << var.enp_x[i] << " " << var.enp_y[i] << " ";
  out << std::endl;
  out << var.half_move_clock << " " << var.full_move << std::endl;
  return out;
}

namespace chess_x {
  const bool can_castle_2pc[2][2] = { true, true, true, true };
  const board_size_type castle_pos_2pc[14][2] = { {4, 4}, {0, 7}, {6, 6}, {0, 7}, {2, 2}, {0, 7},
  {7, 7}, {0, 7}, {5, 5}, {0, 7}, {0, 0}, {0, 7}, {3, 3}, {0, 7} };
  const bool enemy_2pc[2][2] = { false, true, true, false };
  const board_size_type pawn_base_2pc[2] = { 3, 4 };
  const board_size_type pawn_prom_2pc[2] = { 7, 0 };
  const piece_size_type pro_n_2pc = 4;
  const piece_type prom_2pc[PIECE_NUMBER] = { Q, R, N, B, empty, empty, empty };
  const board_size_type king_pos_2pc[2][2] = { {4, 4}, {0, 7} };
  const variant<2> standard_2pc(can_castle_2pc, castle_pos_2pc, enemy_2pc, pawn_base_2pc, pawn_prom_2pc, pro_n_2pc, prom_2pc, king_pos_2pc);
}

void chess_x::variant_test() {
  variant<2> var;
  std::cout << var << std::endl;
  var = standard_2pc;
  std::cout << var << std::endl;
}

#endif