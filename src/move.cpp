#include "move.h"

char getPieceChar(int piece) {
	switch (piece) {
		case P: return 'P';
		case N:	return 'N';
		case B:	return 'B';
		case R:	return 'R';
		case Q:	return 'Q';
		case K:	return 'K';
		case p:	return 'p';
		case n:	return 'n';
		case b:	return 'b';
		case r:	return 'r';
		case q:	return 'q';
		case k:	return 'k';
		default: return '.';
	}
}

char getPromotionPieceChar(int piece) {
	switch (piece) {
		case P: case p:	return 'p';
		case N: case n:	return 'n';
		case B: case b:	return 'b';
		case R: case r:	return 'r';
		case Q: case q:	return 'q';
		case K: case k:	return 'k';
		default:		return ' ';
	}
}

int charToPiece(char piece) {
	switch (piece) {
		case 'P': return P;
		case 'N': return N;
		case 'B': return B;
		case 'R': return R;
		case 'Q': return Q;
		case 'K': return K;
		case 'p': return p;
		case 'n': return n;
		case 'b': return b;
		case 'r': return r;
		case 'q': return q;	
		case 'k': return k;
		default: return NOPIECE;
	}
}

Square squareFromName(const char* squareName) {
	// We expect square names to be in the form of "a1"
	// so we can just use the ASCII values to get the file and rank
	// and then multiply by 8 to get the square number
	U8 rank = 7 - (squareName[1] - '1');
	U8 file = squareName[0] - 'a';
	if (rank > 7 || file > 7) {
		return noSquare;
	}
	return rank * 8 + file;
}