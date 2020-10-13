#ifndef PERFT_X_HPP
#define PERFT_X_HPP

#include "move_gen_x.hpp"

namespace chess_x {
  struct perft_result;
  class perft;
  void perft_test();
}

struct chess_x::perft_result {
  typedef unsigned long long size_type;
  size_type total, capture, castle, enp, pro, time;
  perft_result() noexcept : total(0), capture(0), castle(0), time(0), enp(0), pro(0) {}
};

class chess_x::perft {
  move_gen gen;
  perft_result res;
  perft_result::size_type perft_recur_d(int dep);
  perft_result::size_type perft_recur(int dep);
public:
  perft() noexcept = default;
  perft(const move_gen &g) : gen(g) {}
  virtual ~perft() noexcept = default;
  perft_result do_perft(int dep, bool d = false);
  void set_generator(const move_gen &g) { gen = g; }
};

#endif