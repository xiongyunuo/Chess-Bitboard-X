#include "perft_x.h"
#include "move_gen_x.h"
#include "fen_x.h"
#include <time.h>
#include <stdio.h>

board_x perft_bd_x;

unsigned long long perft_x(register int depth) {
  move_x moves[256];
  register move_x *mv_pt = moves;
  register int num, i;
  register unsigned long long nodes = 0;
  num = move_gen_x(mv_pt, &perft_bd_x);
  if (depth == 1) return num;
  for (i = 0; i < num; ++i) {
    make_move_x(&perft_bd_x, mv_pt);
    nodes += perft_x(depth - 1);
    unmake_move_x(&perft_bd_x, mv_pt);
    ++mv_pt;
  }
  return nodes;
}

void perft_test_pos_x(void) {
  char *pos[TEST_POS_NUM_X] = { PERFT_TEST_POS_1, PERFT_TEST_POS_2, PERFT_TEST_POS_3, PERFT_TEST_POS_4, PERFT_TEST_POS_5, PERFT_TEST_POS_6 };
  int depth[TEST_POS_NUM_X] = { 7, 5, 8, 6, 5, 5 };
  unsigned long long res[TEST_POS_NUM_X] = { 3195901860, 193690690, 3009794393, 706045033, 89941194, 164075551 };
  unsigned long long nodes;
  for (int i = 0; i < TEST_POS_NUM_X; ++i) {
    parse_fen_x(pos[i], &perft_bd_x);
    clock_t t;
    t = clock();
    nodes = perft_x(depth[i]);
    t = clock() - t;
    if (nodes == res[i])
      printf("Test %d Passed. Time %dms\n", i + 1, (int)(((double)1000 * t) / CLOCKS_PER_SEC));
    else
      printf("Test %d Failed. Time %dms\n", i + 1, (int)(((double)1000 * t) / CLOCKS_PER_SEC));
  }
}