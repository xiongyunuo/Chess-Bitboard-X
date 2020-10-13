#ifndef FEN_X_HPP
#define FEN_X_HPP

#include <string>
#include <utility>
#include <stdexcept>
#include "board_x.hpp"
#include "variant_x.hpp"
#include "chess_def_x.hpp"

namespace chess_x {
  class fen_2pc_parser;
  class fen_error : public std::runtime_error { using std::runtime_error::runtime_error; };
  const std::string fen_2pc = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  void fen_test();
}

class chess_x::fen_2pc_parser {
  mutable int pos;
  mutable std::string str;
  board_size_type width, height;
  void read_next(const std::string &fen) const;
public:
  fen_2pc_parser(board_size_type w = 8, board_size_type h = 8) noexcept : width(w), height(h), pos(0) {}
  virtual ~fen_2pc_parser() noexcept = default;
  std::pair<board, variant<2>> parse(const std::string &fen, bool debug = false) const;
};

#endif