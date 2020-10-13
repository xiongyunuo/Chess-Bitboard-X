#include "uci_x.hpp"
#include "perft_x.hpp"
#include <iostream>
#include <sstream>

namespace {
  std::string n_to_coor(chess_x::board_size_type n, chess_x::board_size_type w) {
    std::string res = "";
    res += (char)('a' + (n % w));
    res += std::to_string(n / w);
    return res;
  }

  void print_move(std::ostream &out, const chess_x::move &m, chess_x::board_size_type w) {
    if (m.castle == 1) out << "O-O";
    else if (m.castle == 2) out << "O-O-O";
    else {
      out << chess_x::piece_to_str(m.piece);
      out << n_to_coor(chess_x::bitScanForward(m.from), w);
      if (m.captured == chess_x::empty && !m.is_enp) out << "-";
      else out << "x";
      out << n_to_coor(chess_x::bitScanForward(m.to), w);
      if (m.is_enp) out << " EnP";
    }
  }
}

chess_x::uci::uci() {
  std::pair<board, variant<2>> temp = parser.parse(fen_2pc);
  gen = move_gen(temp.first, temp.second);
}

void chess_x::uci::start() {
  std::string command;
  while (std::getline(std::cin, command))
    if (!parse_command(command))
      break;
}

bool chess_x::uci::parse_command(const std::string &cmd) {
  std::istringstream input(cmd);
  std::string word;
  input >> word;
  if (!input) {
    std::cout << "Quitting..." << std::endl;
    return false;
  }
  if (word == "quit") {
    std::cout << "Quitting..." << std::endl;
    return false;
  }
  else if (word == "setpos") {
    std::string fen = "";
    char c;
    while (input.get(c))
      fen += c;
    try {
      std::pair<board, variant<2>> temp = parser.parse(fen);
      gen = move_gen(temp.first, temp.second);
    }
    catch (fen_error err) {
      std::cout << "Invalid FEN" << std::endl;
      return true;
    }
  }
  else if (word == "perft") {
    unsigned depth;
    std::string str;
    bool d = false;
    input >> str;
    try {
      depth = std::stoi(str);
    }
    catch (...) {
      if (str == "-d")
        d = true;
      else {
        std::cout << "Invalid command: " << cmd << std::endl;
        return true;
      }
      input >> depth;
      if (!input) {
        std::cout << "Invalid command: " << cmd << std::endl;
        return true;
      }
    }
    perft per(gen);
    perft_result per_res = per.do_perft(depth, d);
    std::cout << per_res.total << " nodes" << std::endl;
    if (d) {
      std::cout << "Captures: " << per_res.capture << std::endl;
      std::cout << "Promotions: " << per_res.pro << std::endl;
      std::cout << "Castles: " << per_res.castle << std::endl;
      std::cout << "EnP: " << per_res.enp << std::endl;
    }
    std::cout << "Time: " << per_res.time / 1000.0 << "s" << std::endl;
  }
  else if (word == "allmv") {
    move moves[500];
    int count = gen.generate_moves(moves);
    int count2 = 0;
    perft_result::size_type nodes = 0;
    for (int i = 0; i < count; ++i) {
      gen.make_move(moves[i]);
      if (gen.post_check(moves[i])) {
        print_move(std::cout, moves[i], gen.get_width());
        std::cout << std::endl;
        ++count2;
      }
      gen.unmake_move(moves[i]);
    }
    std::cout << count2 << " moves" << std::endl;
  }
  else
    std::cout << "Unknown command: " << cmd << std::endl;
  return true;
}