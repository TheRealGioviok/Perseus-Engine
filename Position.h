#pragma once
#include "move.h"
#include <algorithm>
#include "BBmacros.h"


struct Position{
    
    BitBoard bitboards[12];
    U8 side;
    U8 enPassant;
    U8 fiftyMove;
    U8 castle;
    HashKey hashKey;
    HashKey pawnHash;
    Move lastMove = 0;
    U8 gamePhase = 0;

    // The default constructor instantiates the position with the standard chess starting position.
    Position();

    // The FEN constructor instantiates the position with the FEN string.
    Position(const char* fen);

    /**
     * @brief The Position::generateHashKey function generates the hash key of the position from scratch.
     * @note This function is called by the constructors. Otherwise the hash gets incrementally updated.
     * @return The generated hashKey
     */
    HashKey generateHashKey();

    /**
     * @brief The Position::generatePawnHashKey function generates the hash key of the pawn structure from scratch.
     * @note This function is called by the constructors. Otherwise the hash gets incrementally updated.
     */
    HashKey generatePawnHashKey();

    /**
     * @brief The Position::print function prints the position to stdout.
     */
    void print();

    /**
     * @brief The wipe function wipes the position.
     */
    void wipe();

    /**
     * @brief The mayBeZugzwang function tries to detect positions where reductions may be unsafe.
     * @return True if the position is not to prune, false otherwise.
     */
    bool mayBeZugzwang();

    /**
     * @brief The parseFEN function parses a FEN string, and places the pieces on the board. It also sets the turn, castling rights, en passant square, half move clock and full move number.
     * It is filled with sanity checks, and will print an error message containing the error if it finds one.
     * @param fen The FEN string to parse.
     * @return True if the FEN string was parsed successfully, false otherwise (state of the position is undefined).
     */
    bool parseFEN(char* FEN);

    /**
     * @brief The isSquareAttacked function returns true if the given square is attacked by the given side.
     * @param square The square to check.
     * @param side The side to check.
     * @return True if the square is attacked, false otherwise.
     */
    bool isSquareAttacked(U8 square, U8 side);

    /**
     * @brief The isSquareAttackedPre function returns true if the given square is attacked by the given side.
     * @param square The square to check.
     * @param side The side to check.
     * @param occupancy An already calculated occupancy.
     * @return True if the square is attacked, false otherwise.
     */
    bool isSquareAttackedPre(U8 square, U8 side, BitBoard occupancy);

    /**
     * @brief The makeMove function makes a move on the board.
     * @param move The move to make.
     * @return True if the move was made, false otherwise (state of the position is undefined).
     */
    bool makeMove(Move move);

    /**
     * @brief The makeNullMove function makes a null move on the board.
     */
    void makeNullMove();

    /**
     * @brief The generateMoves function generates all pseudo legal moves for the current side.
     * @param moveList The moveList to fill with the generated moves.
     */
    void generateMoves(MoveList& moveList);

    /**
     * @brief The generateCaptures function generates all pseud legal captures for the current side.
     * @param moveList The moveList to fill with the generated moves.
     */
    void generateCaptures(MoveList &moveList);

    /**
     * @brief The pieceOn function returns the piece on the given square.
     * @param square The square to check.
     * @return The piece on the given square.
     */
    Piece pieceOn(Square square);

    /**
    * @brief The legalizeTTMove function verifies if the TT move is Legal
    * @param move The move to check
    * @returns 0 if move wasnt legal, a legal move otherwise
    */
    Move legalizeTTMove(Move move);
    /**
     * @brief The addMove function adds a move to the move list.
     * @param ml The move list to add the move to.
     * @param move The move to add.
     */
    inline void addMove(MoveList *ml, ScoredMove move);

    /**
	* @brief The getFEN function returns the FEN string of the current position
	* @return The FEN string of the current position
    */
	std::string getFEN();

    bool inCheck();

	bool insufficientMaterial();

    void reflect();

};
