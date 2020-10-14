#include "fen_x.hpp"
#include <iostream>
#include <cctype>

namespace {
  std::pair<std::string, int> next_num_or_char(const std::string &str, int pos = 0) {
    std::string res = "";
    bool dig = false;
    while (pos < str.length())
      if (str[pos] == ' ') ++pos;
      else break;
    while (pos < str.length()) {
      if (isdigit(str[pos])) {
        res += str[pos];
        ++pos;
        dig = true;
      }
      else {
        if (dig) break;
        res += str[pos];
        ++pos;
        break;
      }
    }
    return std::pair<std::string, int>(res, pos);
  }
}

void chess_x::fen_2pc_parser::read_next(const std::string &fen) const {
  std::pair<std::string, int> temp = next_num_or_char(fen, pos);
  str = temp.first;
  pos = temp.second;
}

std::pair<chess_x::board, chess_x::variant<2>> chess_x::fen_2pc_parser::parse(const std::string &fen, bool debug) const {
  pos = 0;
  str = "";
  board_size_type cur = 0;
  piece_struct *pieces = new piece_struct[width * height];
  while (cur < width * height) {
    read_next(fen);
    if (!str.length()) {
      delete [] pieces;
      throw fen_error("Bad FEN");
    }
    if (str == "/")
      continue;
    else if (isdigit(str[0]))
      cur += std::stoi(str);
    else {
      piece_struct p(str_to_piece(std::string(1, (char)toupper(str[0]))), 0);
      if (p.piece == unknown) {
        delete [] pieces;
        throw fen_error("Bad FEN");
      }
      if (islower(str[0]))
        p.color = 1;
      pieces[cur] = p;
      ++cur;
    }
  }
  board bd(width, height, 2, pieces);
  variant<2> var(standard_2pc);
  if (debug)
    for (board_size_type i = 0; i < height; ++i) {
      for (board_size_type j = 0; j < width; ++j)
        std::cout << pieces[i * width + j] << " ";
      std::cout << std::endl;
    }
  for (int i = 0; i < 2; ++i)
    for (player_size_type j = 0; j < 2; ++j)
      var.king_pos[i][j] = -1;
  for (board_size_type i = 0; i < height; ++i)
    for (board_size_type j = 0; j < width; ++j)
      if (pieces[i * width + j].piece == K) {
        var.king_pos[0][pieces[i * width + j].color] = j;
        var.king_pos[1][pieces[i * width + j].color] = height - 1 - i;
      }
  delete [] pieces;
  read_next(fen);
  if (!str.length())
    throw fen_error("Bad FEN");
  if (str == "b")
    var.side = 1;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j)
      var.can_castle[i][j] = false;
  int counter = 0;
  while (true) {
    read_next(fen);
    if (!str.length())
      throw fen_error("Bad FEN");
    if (str == "-") {
      if (counter == 0) read_next(fen);
      break;
    }
    else if (str == "K") var.can_castle[0][0] = true;
    else if (str == "Q") var.can_castle[1][0] = true;
    else if (str == "k") var.can_castle[0][1] = true;
    else if (str == "q") var.can_castle[1][1] = true;
    else break;
    ++counter;
  }
  if (!str.length()) throw fen_error("Bad FEN");
  if (str != "-") {
    var.enp_x[var.side] = str[0] - 'a';
    read_next(fen);
    if (!str.length() || !isdigit(str[0])) throw fen_error("Bad FEN");
    var.enp_y[var.side] = std::stoi(str);
  }
  var.half_move_clock = 0;
  var.full_move = 1;
  read_next(fen);
  if (!str.length() || !isdigit(str[0])) return std::pair<board, variant<2>>(bd, var);
  var.half_move_clock = std::stoi(str);
  read_next(fen);
  if (!str.length() || !isdigit(str[0])) return std::pair<board, variant<2>>(bd, var);
  var.full_move = std::stoi(str);
  if (debug)
    std::cout << var;
  return std::pair<board, variant<2>>(bd, var);
}

void chess_x::fen_test() {
  fen_2pc_parser parser;
  parser.parse("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b Qk - 0 1", true);
  parser.parse("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b - - 3 5", true);
  parser.parse("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - e3 3 5", true);
  parser.parse("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b Qk e3 3 5", true);
}