OBJECTS = main.o chess_util_x.o fen_x.o move_gen_x.o perft_x.o uci_x.o
CX = gcc
FLAGS = -O2

ChessX : $(OBJECTS)
	$(CX) -o ChessX $(OBJECTS)

main.o : main.c chess_def_x.h chess_util_x.h fen_x.h move_gen_x.h perft_x.h uci_x.h
	$(CX) -c main.c $(FLAGS)

chess_util_x.o : chess_util_x.c chess_util_x.h chess_def_x.h
	$(CX) -c chess_util_x.c $(FLAGS)

fen_x.o : fen_x.c fen_x.h chess_util_x.h chess_def_x.h
	$(CX) -c fen_x.c $(FLAGS)

move_gen_x.o : move_gen_x.c move_gen_x.h chess_def_x.h
	$(CX) -c move_gen_x.c $(FLAGS)

perft_x.o : perft_x.c perft_x.h move_gen_x.h chess_def_x.h fen_x.h chess_util_x.h
	$(CX) -c perft_x.c $(FLAGS)

uci_x.o : uci_x.c uci_x.h chess_def_x.h chess_util_x.h fen_x.h move_gen_x.h perft_x.h
	$(CX) -c uci_x.c $(FLAGS)

.PHONY : clean
clean :
	rm ChessX $(OBJECTS)