# Chess Bitboard X

This is a bitboard based fast legal move generator for standard 2 player chess.

To compile, just type `make` in Terminal.

The executable is run as an interactive commandline tool. Currently supported commands:

1. `setpos [FEN]`

Set current board position.

2. `perft [n]`

Run perft to depth `n` of the current position.

4. `quit`

Quit the program.