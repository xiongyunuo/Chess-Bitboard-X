#ifndef BOARD_X_HPP
#define BOARD_X_HPP

#include <iostream>
#include <string>
#include "chess_def_x.hpp"

namespace chess_x {
  class board;
  struct piece_struct;
  std::ostream& operator<<(std::ostream &out, const board &b);
  std::ostream& operator<<(std::ostream &out, const piece_struct &p);
  piece_type str_to_piece(const std::string &str);
  std::string piece_to_str(piece_type p);
  void board_test();
}

struct chess_x::piece_struct {
  friend std::ostream& operator<<(std::ostream &out, const piece_struct &p);
  piece_type piece;
  player_size_type color;
  piece_struct(piece_type p = empty, player_size_type c = 0) noexcept : piece(p), color(c) {}
  virtual ~piece_struct() noexcept = default;
};

class chess_x::board {
  friend std::ostream& operator<<(std::ostream &out, const board &b);
public:
  typedef board_size_type size_type;
  typedef bb_type bitboard_type;
private:
  bitboard_type **piece_bitboard;
  piece_type *piece_set;
  size_type width, height;
  player_size_type types;
  void destroy() noexcept;
  void copy(const board &b);
public:
  board() noexcept : width(0), height(0), types(0), piece_bitboard(nullptr), piece_set(nullptr) {}
  board(size_type w, size_type h, player_size_type ts, piece_struct *pieces);
  board(const board &b) { copy(b); }
  board(board &&b) noexcept;
  virtual ~board() noexcept { destroy(); }
  board& operator=(const board &rhs);
  board& operator=(board &&rhs) noexcept;
  size_type coor_to_n(size_type i, size_type j) const noexcept { return (height - 1 - i) * width + j; }
  void print_ufm_bb(std::ostream &out, const bitboard_type &bb) const;
  void print_fm_bb(std::ostream &out, const bitboard_type &bb) const;
  bitboard_type** get_piece_bitboard() const noexcept { return piece_bitboard; }
  piece_type* get_piece_set() const noexcept { return piece_set; }
  size_type get_width() const noexcept { return width; }
  size_type get_height() const noexcept { return height; }
};

inline chess_x::board::board(board &&b) noexcept : width(b.width), height(b.height), types(b.types), piece_bitboard(b.piece_bitboard), piece_set(b.piece_set) {
  b.piece_bitboard = nullptr;
  b.piece_set = nullptr;
}

inline chess_x::board& chess_x::board::operator=(const board &rhs) {
  if (this != &rhs) {
    destroy();
    copy(rhs);
  }
  return *this;
}

#endif