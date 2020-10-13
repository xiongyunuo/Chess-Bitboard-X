#ifndef UCI_X_HPP
#define UCI_X_HPP

#include <string>
#include "move_gen_x.hpp"
#include "fen_x.hpp"

namespace chess_x {
  class uci;
}

class chess_x::uci {
  move_gen gen;
  fen_2pc_parser parser;
public:
  uci();
  virtual ~uci() noexcept = default;
  void start();
  bool parse_command(const std::string &cmd);
};

#endif