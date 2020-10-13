#ifndef CHESS_DEF_X_HPP
#define CHESS_DEF_X_HPP

#include <cstdint>

namespace chess_x {
  typedef unsigned player_size_type;
  typedef unsigned piece_size_type;
  typedef int board_size_type;
  typedef uint64_t bb_type;
  enum piece_type : piece_size_type;
  constexpr piece_size_type PIECE_NUMBER = 7;
  constexpr player_size_type NUM_PLAYER = 2;
  inline board_size_type bitScanForward(uint64_t bb);
  inline uint64_t MS1B(uint64_t x);
  const int index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
  };
  constexpr uint64_t debruijn64(0x03f79d71b4cb0a89);
}

chess_x::board_size_type chess_x::bitScanForward(uint64_t bb) {
  return index64[((bb ^ (bb - 1)) * debruijn64) >> 58];
}

uint64_t chess_x::MS1B(uint64_t x) {
  x |= x >> 32;
  x |= x >> 16;
  x |= x >>  8;
  x |= x >>  4;
  x |= x >>  2;
  x |= x >>  1;
  return (x >> 1) + 1;
}

enum chess_x::piece_type : chess_x::piece_size_type { P, R, N, B, K, Q, empty, unknown };

#endif