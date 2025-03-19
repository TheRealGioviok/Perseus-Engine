#include "BBmacros.h"
#include <iostream>

void printBitBoard(BitBoard bitboard) {
	// loop over board ranks
	for (int rank = 0; rank < 8; rank++) {
		std::cout << 8 - rank << "  ";
		for (int file = 0; file < 8; file++) {
			Square square = rank * 8 + file;
			std::cout << (int)testBit(bitboard, square) << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n   a b c d e f g h\n\n";
}
