#ifndef MOVE_GEN_X_HPP
#define MOVE_GEN_X_HPP

#include <iostream>
#include "board_x.hpp"
#include "variant_x.hpp"
#include "chess_def_x.hpp"

namespace chess_x {
  class move_gen;
  struct move;
}

struct chess_x::move {
  move() noexcept = default;
  move(piece_type p, player_size_type tn, bb_type f, bb_type t, piece_type cp, piece_type pro, bool ep = false, bb_type n_ep = bb_type(0)) noexcept : piece(p), turn(tn), from(f), to(t), captured(cp), pro_p(pro), is_enp(ep), new_enp(n_ep), castle(0) {}
  move(unsigned ca, player_size_type tn) : castle(ca), turn(tn) {}
  virtual ~move() noexcept = default;
  bool is_enp, pre_castle[NUM_PLAYER];
  unsigned castle, castle_index;
  bb_type new_enp, old_enp;
  bb_type from, to;
  piece_type piece, captured, pro_p;
  player_size_type turn;
};

class chess_x::move_gen {
protected:
  board bd;
  variant<NUM_PLAYER> var;
  bb_type king_pos[NUM_PLAYER], enp_pos[NUM_PLAYER], castle_bb[2][NUM_PLAYER], castle_rook_bb[4][NUM_PLAYER], castle_king_bb[3][NUM_PLAYER], king_rook_bb[2][NUM_PLAYER];
  bb_type all_pieces, empty_sqrs, all_pieces_arr[NUM_PLAYER];
  bb_type *ranks, *files, *not_files, *knight_attack, *king_attack;
  bb_type *file_attack, *rank_attack, *diag_attack, *anti_diag_attack, *upper, *lower;
  bb_type **piece_bitboard;
  bb_type high;
  piece_type *piece_set;
  board_size_type width, height, castle_rook[4][NUM_PLAYER], castle_king[3][NUM_PLAYER];
  mutable bb_type capture, capture_empty, attack_bb;
private:
  void initialize();
  void precompute();
  void destroy() noexcept;
  inline void update_capture(player_size_type c) const noexcept;
  void update_attack_bb(player_size_type c) const;
public:
  move_gen() noexcept : ranks(nullptr), files(nullptr), not_files(nullptr), knight_attack(nullptr), king_attack(nullptr), file_attack(nullptr), rank_attack(nullptr), diag_attack(nullptr), anti_diag_attack(nullptr), upper(nullptr), lower(nullptr) {}
  move_gen(const board &b, const variant<NUM_PLAYER> &v) : bd(b), var(v) { initialize(); }
  move_gen(const move_gen &gen) : bd(gen.bd), var(gen.var) { initialize(); }
  virtual ~move_gen() noexcept { destroy(); }
  inline move_gen& operator=(const move_gen &rhs);
  int generate_moves(move *moves) const;
  bool post_check(const move &m) const;
  void make_move(move &m);
  void unmake_move(const move &m);
  void print_board(std::ostream &out) const;
  board_size_type get_width() const noexcept { return bd.get_width(); }
  board_size_type get_height() const noexcept { return bd.get_height(); }
};

chess_x::move_gen& chess_x::move_gen::operator=(const move_gen &rhs) {
  if (this != &rhs) {
    destroy();
    bd = rhs.bd;
    var = rhs.var;
    initialize();
  }
  return *this;
}

void chess_x::move_gen::update_capture(player_size_type c) const noexcept {
  #ifdef IS_2PC
  capture = all_pieces_arr[1 - c];
  #else
  capture = bb_type(0);
  for (player_size_type i = 0; i < NUM_PLAYER; ++i)
    if (var.enemy[c][i])
      capture |= all_pieces_arr[i];
  #endif
  capture_empty = capture | empty_sqrs;
}

#endif