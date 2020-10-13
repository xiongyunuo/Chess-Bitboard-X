# Chess Bitboard X

This is a bitboard based move generator for standard 2 player chess.

To compile, just type `make` in Terminal.

The executable is run as an interactive commandline tool. Currently supported commands:

1. `setpos [FEN]`

Set current board position.

2. `perft (-d) [n]`

Run perft to depth `n` of the current position. Flag `-d` is optional, specifying `-d` and the program will output additional data of the total number of captures, promotions, EnPassant and castles.

3. `allmv`

Output all legal moves of the current position.

4. `quit`

Quit the program.

For an example, see `screenshot.png`.