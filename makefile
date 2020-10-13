CPPX = g++
CPP_FLAG = -O2 -std=c++17 -DIS_2PC
OBJECTS = main.o board_x.o fen_x.o move_gen_x.o perft_x.o uci_x.o

ChessX : $(OBJECTS)
	$(CPPX) -o ChessX $(OBJECTS)

main.o : main.cpp chess_def_x.hpp board_x.hpp variant_x.hpp fen_x.hpp move_gen_x.hpp perft_x.hpp uci_x.hpp
	$(CPPX) -c main.cpp -o main.o $(CPP_FLAG)

board_x.o : board_x.cpp board_x.hpp chess_def_x.hpp
	$(CPPX) -c board_x.cpp -o board_x.o $(CPP_FLAG)

fen_x.o : fen_x.cpp fen_x.hpp board_x.hpp variant_x.hpp chess_def_x.hpp
	$(CPPX) -c fen_x.cpp -o fen_x.o $(CPP_FLAG)

move_gen_x.o : move_gen_x.cpp move_gen_x.hpp board_x.hpp variant_x.hpp chess_def_x.hpp
	$(CPPX) -c move_gen_x.cpp -o move_gen_x.o $(CPP_FLAG)

perft_x.o : perft_x.cpp perft_x.hpp move_gen_x.hpp board_x.hpp variant_x.hpp chess_def_x.hpp fen_x.hpp
	$(CPPX) -c perft_x.cpp -o perft_x.o $(CPP_FLAG)

uci_x.o : uci_x.cpp uci_x.hpp perft_x.hpp move_gen_x.hpp board_x.hpp variant_x.hpp chess_def_x.hpp fen_x.hpp
	$(CPPX) -c uci_x.cpp -o uci_x.o $(CPP_FLAG)

clean :
	rm ChessX $(OBJECTS)