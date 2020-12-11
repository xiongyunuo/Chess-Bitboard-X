#include "uci_x.h"
#include "fen_x.h"
#include "perft_x.h"
#include <ctype.h>
#include <string.h>
#include <time.h>

static int read_next_word(const char *str, char *wrd, int len) {
  int counter = 0;
  while (*str && isspace(*str)) {
    ++str;
    ++counter;
  }
  if (*str == '\0') return -1;
  while (--len > 0 && *str && !isspace(*str)) {
    *wrd++ = *str++;
    ++counter;
  }
  *wrd = '\0';
  return counter;
}

void uci_init_x(void) {
  parse_fen_x(STANDARD_FEN_X, &perft_bd_x);
}

void uci_start_x(FILE *in, FILE *out) {
  char cmd[256];
  while (fgets(cmd, 256, in) != NULL && parse_cmd_x(cmd, out)) ;
}

int parse_cmd_x(const char *cmd, FILE *out) {
  const char *temp = cmd;
  char wrd[100];
  int counter = read_next_word(cmd, wrd, 100);
  if (counter == -1) return 1;
  cmd += counter;
  if (strcmp(wrd, "quit") == 0) return 0;
  else if (strcmp(wrd, "setpos") == 0) {
    board_x bd = perft_bd_x;
    if (parse_fen_x(cmd, &perft_bd_x) == -1) {
      fprintf(out, "Invalid FEN\n");
      perft_bd_x = bd;
    }
  }
  else if (strcmp(wrd, "perft") == 0) {
    int depth;
    if (sscanf(cmd, "%d", &depth) != 1 || depth <= 0) {
      fprintf(out, "Invalid command: %s", temp);
      return 1;
    }
    unsigned long long nodes;
    clock_t t = clock();
    nodes = perft_x(depth);
    t = clock() - t;
    fprintf(out, "%llu nodes in %dms\n", nodes, (int)(((double)1000 * t) / CLOCKS_PER_SEC));
  }
  else
    fprintf(out, "Unknown command: %s", temp);
  return 1;
}