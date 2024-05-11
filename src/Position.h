#pragma once
#include "move.h"
#include <algorithm>
#include "BBmacros.h"

#define KILLER1SCORE 9999999ULL
#define KILLER2SCORE 9999998ULL
#define COUNTERSCORE 9999997ULL
#define LASTMOVEKILLERSCORE 9999996ULL
#define PROMOTIONBONUS 2000000000ULL
#define GOODCAPTURESCORE 1000000000ULL
extern unsigned long long BADCAPTURESCORE;


struct Position{
    
    BitBoard bitboards[12];
    BitBoard occupancies[3];
    U8 side;
    U8 enPassant;
    U8 fiftyMove; // 50 move rule counter, number of plies since the last irreversible move
    Ply totalPly; // Total number of plies since the start of the game
    Ply plyFromNull; // Ply from the last null move (see Null Move Pruning for more information)
    U8 castle;
    HashKey hashKey;
    Move lastMove = 0;
    Score psqtScore[2] = {0,0}; // PSQT score, incrementally updated

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
     * @brief The hasNonPawns function checks if the current side to move has non-pawn material.
     * @return true if the position has non-pawn material, false otherwise.
     */
    bool hasNonPawns();

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
     * @brief The attacksToPre function returns a bitboard with all the attackers to a square.
     * @param square The square to check.
     * @param occupancy an already calculated occupancy
     * @param diagonalAttackers an already calculated diagonal attackers
     * @param orthogonalAttackers an already calculated orthogonal attackers
     * @return A bitboard with all the attackers to the square.
     * @TODO: diagonal and orthogonal attackers can be passed as parameters as they are already calculated
     */
    inline BitBoard attacksToPre(Square square, BitBoard occupancy, BitBoard diagonalAttackers, BitBoard orthogonalAttackers);

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
     * @param killer1 The first killer move.
     * @param killer2 The second killer move.
     * @param counterMove The counter move.
     */
    void generateMoves(MoveList& moveList, Move killer1, Move killer2, Move counterMove);

    /**
     * @brief The generateCaptures function generates all pseud legal captures for the current side.
     * @param moveList The moveList to fill with the generated moves.
     */
    void generateCaptures(MoveList &moveList);

    /**
     * @brief The generateUnsortedMoves function generates all pseudo legal moves for the current side. Used for perft testing (when we don't care about the move ordering).
     * @param moveList The moveList to fill with the generated moves.
     */
    void generateUnsortedMoves(MoveList &moveList);

    /**
     * @brief The pieceOn function returns the piece on a square. If the square is empty, NOPIECE is returned.
     * @param square The square to check.
     * @return The piece on the square.
     */
    inline Piece pieceOn(Square square) {
        // We will do a branchless lookup
        // We will have a "result" variable that will contain the piece on the square
        Piece result = p + testBit(occupancies[BLACK], square) * 6; // If the piece is black, we add 6 to the result. Also, if the square is empty, the result will be 6 which added to p is 6+6=12 (NOPIECE)
        // We will subtract the piece code once we find it
        result -= (p - P) * testBit(bitboards[P] | bitboards[p], square);
        result -= (p - N) * testBit(bitboards[N] | bitboards[n], square);
        result -= (p - B) * testBit(bitboards[B] | bitboards[b], square);
        result -= (p - R) * testBit(bitboards[R] | bitboards[r], square);
        result -= (p - Q) * testBit(bitboards[Q] | bitboards[q], square); 
        result -= (p - K) * testBit(bitboards[K] | bitboards[k], square);
        return result; // If no piece was found, the result will remain NOPIECE
    }

    /**
    * @brief The legalizeTTMove function verifies if the TT move is Legal
    * @param move The move to check
    * @returns 0 if move wasnt legal, a legal move otherwise
    */
    Move legalizeTTMove(Move move);

    inline void addEp(MoveList *ml, ScoredMove move);

    inline void addPromoCapture(MoveList *ml, ScoredMove move, Piece movedPiece, Piece capturedPiece, Piece promotion);

    inline void addCapture(MoveList *ml, ScoredMove move, Piece movedPiece, Piece capturedPiece);

    inline void addPromotion(MoveList *ml, ScoredMove move, Piece piece);

    inline void addQuiet(MoveList *ml, ScoredMove move, Square source, Square target, Move killer1, Move killer2, Move counterMove);

    inline void addUnsorted(MoveList *ml, ScoredMove move);

    /**
	* @brief The getFEN function returns the FEN string of the current position
	* @return The FEN string of the current position
    */
	std::string getFEN();
    
    /**
     * @brief The SEE function returns whether the move is a good capture or not.
     * @tparam threshold The threshold to check against.
     * @param move The move to check.
     * @return true if the move is a good capture, false otherwise.
     */
    template <Score threshold>
    bool SEE(Move move);

    bool inCheck();

	bool insufficientMaterial();

    void reflect();
};

/**
 * @brief The UndoInfo class contains all the information needed to undo a move.
 * @note This class is used by the search to store the information needed to undo a move.
 */
struct UndoInfo {
    // Irreversible information
    HashKey hashKey;
    // HashKey pawnHashKey;
    Square enPassant;
    U8 castle;
    U8 fiftyMove;
    Ply totalPly;
    Ply plyFromNull;
    Move lastMove;
    Score psqtScore[2];
    U8 side;
    // Reversible information
    BitBoard bitboards[12];
    BitBoard occupancies[3];

    /**
     * @brief The constructor of the UndoInfo class.
     * @param position The position to store the information from.
     */
    UndoInfo(Position& position);

    /**
     * @brief The undoMove function undoes a move.
     * @param position The position to undo the move on.
     * @param move The move to undo.
     */
    void undoMove(Position& position, Move move);

    /**
     * @brief The undoNullMove function undoes a null move.
     * @param position The position to undo the null move on.
     */
    void undoNullMove(Position& position);

};