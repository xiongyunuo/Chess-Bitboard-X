#include "board_x.hpp"
#include <utility>

chess_x::piece_type chess_x::str_to_piece(const std::string &str) {
  if (str == "P") return P;
  else if (str == "N") return N;
  else if (str == "R") return R;
  else if (str == "B") return B;
  else if (str == "Q") return Q;
  else if (str == "K") return K;
  return unknown;
}

std::string chess_x::piece_to_str(piece_type p) {
  if (p == P) return "P";
  else if (p == N) return "N";
  else if (p == R) return "R";
  else if (p == B) return "B";
  else if (p == Q) return "Q";
  else if (p == K) return "K";
  else if (p == empty) return "-";
  return "U";
}

chess_x::board::board(size_type w, size_type h, player_size_type ts, piece_struct *pieces) : width(w), height(h), types(ts) {
  piece_bitboard = new bitboard_type*[types];
  piece_set = new piece_type[width * height];
  for (player_size_type i = 0; i < types; ++i)
    piece_bitboard[i] = new bitboard_type[PIECE_NUMBER]();
  for (size_type i = 0; i < height; ++i)
    for (size_type j = 0; j < width; ++j) {
      piece_struct piece = pieces[i * width + j];
      piece_set[coor_to_n(i, j)] = piece.piece;
      if (piece.piece == empty)
        continue;
      piece_bitboard[piece.color][piece.piece] |= bitboard_type(1) << coor_to_n(i, j);
    }
}

chess_x::board& chess_x::board::operator=(board &&rhs) noexcept {
  width = rhs.width;
  height = rhs.height;
  types = rhs.types;
  piece_bitboard = rhs.piece_bitboard;
  piece_set = rhs.piece_set;
  rhs.piece_bitboard = nullptr;
  rhs.piece_set = nullptr;
  return *this;
}

void chess_x::board::destroy() noexcept {
  if (!piece_bitboard) return;
  if (!piece_set) return;
  for (player_size_type i = 0; i < types; ++i)
    delete [] piece_bitboard[i];
  delete [] piece_bitboard;
  delete [] piece_set;
}

void chess_x::board::copy(const board &b) {
  width = b.width;
  height = b.height;
  types = b.types;
  piece_bitboard = new bitboard_type*[types];
  piece_set = new piece_type[width * height];
  for (player_size_type i = 0; i < types; ++i) {
    piece_bitboard[i] = new bitboard_type[PIECE_NUMBER];
    for (piece_size_type j = 0; j < PIECE_NUMBER; ++j)
      piece_bitboard[i][j] = b.piece_bitboard[i][j];
  }
  for (board_size_type i = 0; i < width * height; ++i)
    piece_set[i] = b.piece_set[i];
}

void chess_x::board::print_ufm_bb(std::ostream &out, const bitboard_type &bb) const {
  for (size_type i = 0; i < width * height; ++i) {
    if (bb & (bitboard_type(1) << i))
      out << "1";
    else
      out << "0";
  }
}

void chess_x::board::print_fm_bb(std::ostream &out, const bitboard_type &bb) const {
  for (size_type i = 0; i < height; ++i) {
    for (size_type j = 0; j < width; ++j)
      if (bb & (bitboard_type(1) << coor_to_n(i, j)))
        out << "1";
      else
        out << "0";
    out << std::endl;
  }
}

std::ostream& chess_x::operator<<(std::ostream &out, const piece_struct &p) {
  out << piece_to_str(p.piece) << p.color;
  return out;
}

std::ostream& chess_x::operator<<(std::ostream &out, const board &b) {
  for (player_size_type i = 0; i < b.types; ++i) {
    for (piece_size_type j = 0; j < PIECE_NUMBER; ++j) {
      b.print_ufm_bb(out, b.piece_bitboard[i][j]);
      if (j != PIECE_NUMBER - 1)
        out << " ";
    }
    out << std::endl;
  }
  return out;
}

void chess_x::board_test() {
  chess_x::piece_struct pieces[4] = { chess_x::piece_struct(chess_x::K, 0),
  chess_x::piece_struct(chess_x::R, 1), chess_x::piece_struct(chess_x::N, 2),
  chess_x::piece_struct(chess_x::K, 3) };
  chess_x::board bd(2, 2, 4, pieces);
  std::cout << bd << std::endl;
  chess_x::board bd2(bd);
  std::cout << bd2 << std::endl;
  bd2 = bd;
  std::cout << bd2 << std::endl;
  chess_x::board bd3(std::move(bd2));
  std::cout << bd3 << std::endl;
  bd3 = std::move(bd);
  std::cout << bd3 << std::endl;
}