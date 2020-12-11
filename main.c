#include <stdio.h>
#include "move_gen_x.h"
#include "uci_x.h"

int main() {
  move_gen_init_x();
  uci_init_x();
  uci_start_x(stdin, stdout);
  return 0;
}