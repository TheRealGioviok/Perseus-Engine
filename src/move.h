#pragma once
#include <string>
#include "BBmacros.h"
#include <sstream>
#include <iostream>
/*
0000 0000 0000 0000 0000 00xx xxxx  source square		0x3f
0000 0000 0000 0000 xxxx xx00 0000  target square		0xfc0
0000 0000 0000 xxxx 0000 0000 0000  piece				0xf000
0000 0000 xxxx 0000 0000 0000 0000  captured piece		0xf0000
0000 xxxx 0000 0000 0000 0000 0000  promoted piece		0xf00000

0001 0000 0000 0000 0000 0000 0000  double push flag	0x200000
0010 0000 0000 0000 0000 0000 0000  enpassant flag		0x400000
0100 0000 0000 0000 0000 0000 0000  castling flag		0x800000
1000 0000 0000 0000 0000 0000 0000  check flag			0x1000000
*/


#define encodeMove(source, target, piece, capture, promotion, doubleP, enpassant, castle, check) \
		((source)			|	\
		((target) << 6)		|	\
		((piece) << 12)		|	\
		((capture) << 16)	|	\
		((promotion) << 20)	|	\
		((doubleP) << 24)	|	\
		((enpassant) << 25)	|	\
		((castle) << 26)	|	\
		((check) << 27))

#define moveSource(move)	((move) & 0x3f)
#define moveTarget(move)	(((move) & 0xfc0) >> 6)
#define movePiece(move)		(((move) & 0xf000) >> 12)
#define moveCapture(move)	(((move) & 0xf0000) >> 16)
#define movePromotion(move)	(((move) & 0xf00000) >> 20)
#define isDoublePush(move)	(((move) >> 24) & 1)
#define isEnPassant(move)	(((move) >> 25) & 1)
#define isCastling(move)	(((move) >> 26) & 1)
#define isCheck(move)		(((move) >> 27) & 1)
#define setCheck(move)		((move) |= (1 << 27))

#define isCapture(move)		((((move) & 0xf0000) >> 16) != NOPIECE)
#define isPromotion(move)	((((move) & 0xf00000) >> 20) != NOPIECE)
#define isReversible(move)	((((move) & 0x20ff0000) == 0xcc0000) && (((move) & 0xf000) % 6 != 0)) // This means captures, promotions, castlings, pawn moves
#define okToReduce(move)	(! (isCapture(move) || isPromotion(move)))
#define onlyMove(move)		((move) & 0x2fffffff)
#define getScore(move)		((move) >> 32)

#define NULLMOVE encodeMove(a8,a8,NOPIECE,NOPIECE,NOPIECE,false,false,false,false)
#define isOk(move) ((move & 0xfff) != 0)

#define sameMovePos(move1,move2) (((move1) & 0xffff) == ((move2) & 0xffff)) // To compare two pseudolegal moves in the same position, it is enough to compare the source, target and piece

/**
 * @brief The getPieceChar function returns the character of a piece.
 * @param piece The piece to get the character of.
 * @return The character of the piece.
 */
char getPieceChar(int piece);

/**
 * @brief The getPromotionPieceChar function returns the character of a promotion piece.
 * @param piece The piece to get the character of.
 * @return The character of the piece.
 * @note This function is used for the promotion piece in the UCI protocol, since pieces are always in lowercase, despite the side to move.
 */
char getPromotionPieceChar(int piece);

/**
 * @brief The charToPiece function returns the piece of a character.
 * @param piece The character to get the piece of.
 * @return The piece of the character.
 */
int charToPiece(char piece);

/**
 * @brief The squareFromName function returns the square of a square name.
 * @param squareName The pointer to a string containing the square name.
 * @return The square of the square name.
 */
Square squareFromName(const char* squareName);

struct MoveList {
	//moves
	ScoredMove moves[128] = { 0 };
	//move count
	U8 count = 1;
	// clear function
	inline void clear() {
		count = 1;
	}

};

inline void printMove(ScoredMove move){
	if (isPromotion(move)){
		std::cout << coords[moveSource(move)] << coords[moveTarget(move)] << getPromotionPieceChar(movePromotion(move));
	}
	else {
		std::cout << coords[moveSource(move)] << coords[moveTarget(move)];
	}
}

inline std::string getMoveString(ScoredMove move){
	std::stringstream ss;
	if (isPromotion(move)){
		ss << coords[moveSource(move)] << coords[moveTarget(move)] << getPromotionPieceChar(movePromotion(move));
	}
	else {
		ss << coords[moveSource(move)] << coords[moveTarget(move)];
	}
	return ss.str();
}

inline PackedMove packMove(Move move) {
	return (move & 0xfff) | (movePromotion(move) << 12);
}

inline bool packedMoveCompare(PackedMove m1, Move m2) {
	return packMove(m2) == m1;
}
