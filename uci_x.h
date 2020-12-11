#ifndef UCI_X_H
#define UCI_X_H

#include <stdio.h>

void uci_init_x(void);
void uci_start_x(FILE *in, FILE *out);
int parse_cmd_x(const char *cmd, FILE *out);

#endif