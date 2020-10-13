#include "perft_x.hpp"
#include "fen_x.hpp"
#include <chrono>
#include <string>
#include <iostream>

chess_x::perft_result chess_x::perft::do_perft(int dep, bool d) {
  res = perft_result();
  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  if (d)
    res.total = perft_recur_d(dep);
  else
    res.total = perft_recur(dep);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  res.time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  return res;
}

chess_x::perft_result::size_type chess_x::perft::perft_recur(int dep) {
  if (dep == 0) return 1;
  move moves[500];
  int count = gen.generate_moves(moves);
  perft_result::size_type nodes = 0;
  for (int i = 0; i < count; ++i) {
    gen.make_move(moves[i]);
    if (gen.post_check(moves[i]))
      nodes += perft_recur(dep - 1);
    gen.unmake_move(moves[i]);
  }
  return nodes;
}

chess_x::perft_result::size_type chess_x::perft::perft_recur_d(int dep) {
  if (dep == 0) return 1;
  move moves[500];
  int count = gen.generate_moves(moves);
  perft_result::size_type nodes = 0;
  for (int i = 0; i < count; ++i) {
    gen.make_move(moves[i]);
    if (gen.post_check(moves[i])) {
      if (moves[i].castle) ++res.castle;
      else {
        if (moves[i].captured != empty) ++res.capture;
        if (moves[i].is_enp) { ++res.enp; ++res.capture; }
        if (moves[i].pro_p != moves[i].piece) ++res.pro;
      }
      nodes += perft_recur_d(dep - 1);
    }
    gen.unmake_move(moves[i]);
  }
  return nodes;
}

void chess_x::perft_test() {
  std::string fen[6] = { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ", "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10" };
  int dep[6] = { 6, 5, 7, 6, 5, 5 };
  perft_result::size_type result[6] = { 119060324ULL, 193690690ULL, 178633661ULL, 706045033ULL, 89941194ULL, 164075551ULL };
  fen_2pc_parser parser;
  perft per;
  for (int i = 0; i < 6; ++i) {
    std::pair<board, variant<2>> temp = parser.parse(fen[i]);
    per.set_generator(move_gen(temp.first, temp.second));
    perft_result per_res = per.do_perft(dep[i]);
    if (per_res.total == result[i])
      std::cout << "Test " << i + 1 << ": Passed" << std::endl;
    else
      std::cout << "Test " << i + 1 << ": Failed" << std::endl;
    std::cout << "Time: " << per_res.time << std::endl;
  }
}