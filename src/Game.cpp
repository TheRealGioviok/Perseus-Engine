#include "Game.h"
#include "constants.h"
#include "uci.h"
#include "tt.h"
#include "history.h"
#include "movegen.h"
#include "tables.h"
#include "evaluation.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

/**
 * @brief The perft function calculates the number of leaf nodes in the subtree rooted at the current position.
 * @param depth The depth to calculate to.
 * @return The number of leaf nodes in the subtree rooted at the current position.
 */
U64 Game::perft(Depth depth){
    //std::cout << "Starting perft at depth " << depth << std::endl;
    nodes = 0;
    return _perft(depth);
}

/**
 * @brief The divide function calculates the number of leaf nodes in the subtree rooted at the current position.
 * It shows the number of nodes for each possible move in the current position.
 * @param depth The depth to calculate to.
 * @return The number of leaf nodes in the subtree rooted at the current position.
 */
U64 Game::divide(Depth depth) {
    std::cout << "Starting divide at depth " << (int)depth << std::endl;
    nodes = 0;
    MoveList moveList;
    pos.generateMoves(moveList);
    Position save = pos;

    for (int i = 0; i < moveList.count; i++) {
        if (pos.makeMove(moveList.moves[i])) {
            printMove(moveList.moves[i]);
            std::cout << ": " << std::flush;
            U64 res = _perft(depth - 1);
            nodes += res;
            std::cout << res;
            std::cout << std::endl;
            pos = save;
        }
        else {
            pos = save;
        }
    }
    std::cout << "Divide results: " << nodes << "\n";
    return nodes;
}

void Game::communicate() {
    UCICommunicate(this);
}

/**
 * @brief The _perft function calculates the number of leaf nodes in the subtree rooted at the current position.
 * @param depth The depth to calculate to.
 * @return The number of leaf nodes in the subtree rooted at the current position.
 * @note This function is used by the perft function.
 */
U64 Game::_perft(Depth depth) {
    if (depth <= 0) {
        return 1;
    }
    U64 n = 0;
    MoveList moveList;
    pos.generateMoves(moveList);
    Position save = pos;
    ++ply;
    for (int i = 0; i < moveList.count; i++) {
        if (pos.makeMove(moveList.moves[i])) {
            n += _perft(depth - 1);
            pos = save;
        }
        else {
            pos = save;
        }
    }
    --ply;
    return n;
}

/**
 * @brief The reset function resets the game.
 */
void Game::reset(){
    pos.parseFEN((char*)std::string(startPosition).c_str());
    depth = 0;
    moveTime = 0;
    wtime = 0;
    btime = 0;
    winc = 0;
    binc = 0;
    stopped = false;
    bestMove = 0;
    lastScore = 0;
    ply = 0;
    repetitionCount = 0;

    // Clear history, killer and counter move tables
#if ENABLEHISTORYHEURISTIC
    memset(historyTable, 0, sizeof(historyTable));
    // memset(pieceFromHistoryTable, 0, sizeof(pieceFromHistoryTable));
    // memset(pieceToHistoryTable, 0, sizeof(pieceToHistoryTable));
#endif

#if ENABLEKILLERHEURISTIC
    memset(killerTable, 0, sizeof(killerTable));
#endif

#if ENABLECOUNTERMOVEHEURISTIC
    memset(counterMoveTable, 0, sizeof(counterMoveTable));
#endif

    // Clear pv len and pv table
    memset(pvLen, 0, sizeof(pvLen));
    memset(pvTable, 0, sizeof(pvTable));

    // Clear hashTable
#if ENABLETTSCORING
    memset(tt, 0, ttEntryCount * sizeof(ttEntry));
#endif

    // Clear eval cache
#if USINGEVALCACHE
    memset(evalHash, 0, evalHashSize * sizeof(evalHashEntry));
#endif

}

/**
 * @brief The parseFEN function parses a FEN string, and places the pieces on the board. It also sets the turn, castling rights, en passant square, half move clock and full move number.
 * It is filled with sanity checks, and will print an error message containing the error if it finds one.
 * @param fen The FEN string to parse.
 * @return True if the FEN string was parsed successfully, false otherwise (state of the position is undefined).
 * @note This function is a call to the Position::parseFEN function.
 */
bool Game::parseFEN(char *FEN){
    return pos.parseFEN(FEN);
}

/**
 * @brief The executeMoveList function executes a list of moves.
 * @param moveList The char* to the list of moves to execute.
 * @return True if the move list was executed successfully, false otherwise (state of the position is undefined).
 */
bool Game::executeMoveList(char *moveList){
    std::stringstream ss(moveList);
    std::string move;
    while (ss >> move) {
        //std::cout << "Newmove is now: " << move << "\n";
       Move parsedMove = getLegal(move.c_str());
        if (!pos.makeMove(parsedMove)) return false;
        if (!isReversible(parsedMove))repetitionCount = 0;
        repetitionTable[repetitionCount++] = pos.hashKey;
    }
    return true;
}

/**
 * @brief The getLegal function returns a move from a string. It returns 0 if the move is not legal.
 * @param move The string to parse.
 * @return The move parsed from the string.
 */
Move Game::getLegal(std::string move){
    MoveList moveList;
    pos.generateMoves(moveList);
    for (int i = 0; i < moveList.count; i++) {
        if (move == getMoveString(moveList.moves[i])) {
            return (Move)moveList.moves[i];
        }
    }
    std::cout << "Not found!\n";
    return 0;
}

/**
 * @brief The print function prints the current position.
 * @note This function is a call to the internal Position::print function.
 */
void Game::print(){
    pos.print();
}

/**
 * @brief The generateMoves function generates all pseudo legal moves for the current position. It is a call to the internal Position::generateMoves function.
 * @param moves The MoveList object to store the moves in.
 */
void Game::generateMoves(MoveList &moves){
    pos.generateMoves(moves);
}

/**
 * @brief The generateCaptures function generates all pseud legal captures for the current position. It is a call to the internal Position::generateCaptures function.
 * @param moves The MoveList object to store the moves in.
 */
void Game::generateCaptures(MoveList &moves){
    pos.generateCaptures(moves);
}

/**
 * @brief The makeMove function makes a move on the board. It is a call to the internal Position::makeMove function.
 * @param move The move to make.
 * @return True if the move was made, false otherwise (state of the position is undefined).
 */
bool Game::makeMove(Move move){
    ++ply;
#if DETECTREPETITION
    repetitionTable[repetitionCount++] = pos.hashKey;
#endif
    bool k = pos.makeMove(move);
#if ENABLEPREFETCHING && ENABLETTSCORING
    prefetch(&evalHash[pos.hashKey % evalHashSize]);
#endif
    return k;
}

/**
 * @brief The isRepetition function returns true if the current position is a repetition.
 * @return True if the current position is a repetition, false otherwise.
 */
bool Game::isRepetition() {
    // Get the hash key of the current position
    HashKey hashKey = pos.hashKey;
    // Iterate over the repetition table
    for (int i = repetitionCount - 1 ; i >= repetitionCount - pos.fiftyMove - 1; i -= 2) {
        // If the hash key is found, return true
        if (hashKey == repetitionTable[i])
            return true;
    }
    // If the hash key is not found, return false
    return false;
}

/**
 * @brief The eval function evaluates the current position.
 * @return The static evaluation of the current position.
 */
Score Game::evaluate(){

#if USINGEVALCACHE && ENABLEPREFETCHING
    prefetch(&evalHash[pos.hashKey % (evalHashSize)]);
#endif

   // Before calling the static score evaluation, we must check for the interior node recognizer ( known endgames )
    bool side = pos.side;
    BitBoard* ourPiecesp = pos.bitboards + side * 6;
    BitBoard* theirPiecesp = pos.bitboards + (side ^ 1) * 6;
    BitBoard ourPawns = *ourPiecesp;
    BitBoard ourKnights = *(ourPiecesp + 1);
    BitBoard ourBishops = *(ourPiecesp + 2);
    BitBoard ourRooks = *(ourPiecesp + 3);
    BitBoard ourQueens = *(ourPiecesp + 4);
    BitBoard ourKings = *(ourPiecesp + 5);
    BitBoard theirPawns = *theirPiecesp;
    BitBoard theirKnights = *(theirPiecesp + 1);
    BitBoard theirBishops = *(theirPiecesp + 2);
    BitBoard theirRooks = *(theirPiecesp + 3);
    BitBoard theirQueens = *(theirPiecesp + 4);
    BitBoard theirKings = *(theirPiecesp + 5);
    Square ourKing = lsb(ourKings);
    Square theirKing = lsb(theirKings);
    BitBoard ourPieces = ourPawns | ourKnights | ourBishops | ourRooks | ourQueens | ourKings;
    BitBoard theirPieces = theirPawns | theirKnights | theirBishops | theirRooks | theirQueens | theirKings;
    BitBoard pieces = ourPieces | theirPieces;

    Score score = 0;
    switch (popcount(pieces)) {
    case 2:
        return 0;
    case 3:
        if (ourKnights | ourBishops | theirKnights | theirBishops) return 0;
        if (ourPawns) { // KPvK
            Square pawn = lsb(ourPawns);
            score += (side ? 7 - rankOf(pawn) : rankOf(pawn)) * 4;
            BitBoard promoPath = side ? squaresBehind(pawn) : squaresAhead(pawn);
            score += KNOWNWIN * (std::min(5, popcount(promoPath)) < (chebyshevDistance[theirKing][lsb(promoPath)]) - ((promoPath & ourKings) > 0));
        }
        else if (theirPawns) { //KvKP
            Square pawn = lsb(theirPawns);
            score -= (side ? rankOf(pawn) : 7 - rankOf(pawn)) * 4;
            BitBoard promoPath = side ? squaresAhead(pawn) : squaresBehind(pawn);
            score += -KNOWNWIN * (std::min(5, popcount(promoPath)) < (chebyshevDistance[ourKing][lsb(promoPath)]) - 1);
        }
        else if (ourPieces ^ ourKings) { //KQvK - KRvK
            score += KNOWNWIN;
            score += (ourRooks ? 477 : 1025) + centerDistance[theirKing] * 47 + ((7 - chebyshevDistance[theirKing][ourKing]) << 5);
        }
        else { //KvKQ - KvKR
            score -= KNOWNWIN;
            score -= (theirRooks ? 477 : 1025) + centerDistance[ourKing] * 47 + ((7 - chebyshevDistance[ourKing][theirKing]) << 5);
        }
        return score;
    case 4:
        switch (popcount(ourPieces)) {
        case 1:
            // We are losing in all endgames except K v KNN ans some difficult K v KPP
            if (popcount(theirKnights) == 2) {
                // We give a little bonus to avoid helpmate
                return -5 * centerDistance[ourKing];
            }
            else if (theirKnights && theirBishops) {
                // K v KBN
                bool bishopColor = theirBishops & squaresOfColor[BLACK];
                return -KNOWNWIN - 5 * (colorCornerDistance[bishopColor][ourKing]) + centerDistance[ourKing];
            }
            else if (popcount(theirPawns) == 2) {
                Square pawn = lsb(theirPawns);
                score -= side ? rankOf(pawn) : 7 - rankOf(pawn);
                score -= 2 * (Score)popcount(kingAttacks[theirKing] & (side ? squaresBehind(pawn) : squaresAhead(pawn))
                );
                Square pawn2 = lsb(theirPawns);
                score -= side ? rankOf(pawn2) : 7 - rankOf(pawn2);
                score -= 2 * (Score)popcount(kingAttacks[theirKing] & (side ? squaresBehind(pawn2) : squaresAhead(pawn2))
                );

                if (fileOf(pawn) != fileOf(pawn2)) score -= KNOWNWIN;
                return score;
            }
            else if (theirBishops && theirPawns) {
                // KBP v K is usually a win, but if the bishop is of the wrong colour (different from the promotion square's colour) it may be a draw
                // Check draw using Uri's rule (Gerd Isenberg's implementation)
                Square pawn = lsb(theirPawns);
                Square bishop = lsb(theirBishops);
                Square promotionSquare = lsb(side ? (squaresBehind(pawn) & ranks(7)) : (squaresAhead(pawn) & ranks(0)));
                if ((bishop & squaresOfColor[BLACK]) != (promotionSquare & squaresOfColor[BLACK])) {
                    S8 losingDistance = chebyshevDistance[ourKing][promotionSquare];
                    S8 winningDistance = chebyshevDistance[theirKing][promotionSquare];
                    S8 pawnDistance = std::min(chebyshevDistance[pawn][promotionSquare], (U8)5) + (fileOf(ourKing) == fileOf(pawn));
                    if (losingDistance < winningDistance && losingDistance < pawnDistance) {
                        return -chebyshevDistance[ourKing][pawn]; // Draw
                    }
                    else {
                        return -KNOWNWIN - 5 * chebyshevDistance[pawn][promotionSquare] - 10 * (7 - chebyshevDistance[ourKing][pawn]);
                    }
                }
                else {
                    return -KNOWNWIN - 5 * chebyshevDistance[pawn][promotionSquare] - 10 * (7 - chebyshevDistance[ourKing][pawn]);
                }
            }
            else {
                return -KNOWNWIN - 100 * (Score)popcount(theirPawns)
                    - 300 * popcount(theirBishops | theirKnights)
                    - 500 * popcount(theirRooks)
                    - 900 * popcount(theirQueens)
                    - 10 * (Score)centerDistance[ourKing]
                    - 5 * (Score)chebyshevDistance[ourKing][theirKing];
            }
            break;
        case 2: {
            // If no pawns, only KQvKX gets a known win (except KQvKQ)
            // KBvKP and KNvKP is drawish
            score += 100 * (Score)(popcount(ourPawns) - popcount(theirPawns))
                + 300 * (Score)(popcount(ourBishops | ourKnights) - popcount(theirBishops | theirKnights))
                + 500 * (Score)(popcount(ourRooks) - popcount(theirRooks))
                + 950 * (Score)(popcount(ourQueens) - popcount(theirQueens));
            score += KNOWNWIN * (score >= 450);
            score -= KNOWNWIN * (score <= -450);
            if (abs(score) <= 200) score >>= 5;
            else {
                Square theirPiece = lsb(theirPieces ^ theirKings);
                Square ourPiece = lsb(ourPieces ^ ourKings);
                (score > 0) ? score += 10 * (7 - chebyshevDistance[ourKing][theirPiece]) :
                    score -= 10 * (7 - chebyshevDistance[theirKing][ourPiece]);
                if (abs(score) > KNOWNWIN) {
                    if (score > 0) score += 15 * chebyshevDistance[theirKing][theirPiece];
                    else score -= 15 * chebyshevDistance[ourKing][ourPiece];
                }
            }
        }
            return score;
        case 3:
            // We are winning in all endgames except K v KNN ans some difficult K v KPP
            if (popcount(ourKnights) == 2) {
                // We give a little bonus to avoid helpmate
                return 5 * centerDistance[theirKing];
            }
            else if (ourKnights && ourBishops) {
                // K v KBN
                bool bishopColor = ourBishops & squaresOfColor[BLACK];
                return +KNOWNWIN + 5 * (colorCornerDistance[bishopColor][theirKing]) + centerDistance[theirKing];
            }
            else if (popcount(ourPawns) == 2) {
                Square pawn = lsb(ourPawns);
                score += side ? rankOf(pawn) : 7 - rankOf(pawn);
                score += 2 * (Score)popcount(kingAttacks[ourKing] &
                    (side ? squaresBehind(pawn) : squaresAhead(pawn))
                );
                Square pawn2 = lsb(ourPawns);
                score += side ? rankOf(pawn2) : 7 - rankOf(pawn2);
                score += 2 * (Score)popcount(kingAttacks[ourKing] &
                    (side ? squaresBehind(pawn2) : squaresAhead(pawn2))
                );

                if (fileOf(pawn) != fileOf(pawn2)) score += KNOWNWIN;
                return score;
            }
            else if (ourBishops && ourPawns) {
				// KBP v K is usually a win, but if the bishop is of the wrong colour (different from the promotion square's colour) it may be a draw
				// Check draw using Uri's rule (Gerd Isenberg's implementation)
				Square pawn = lsb(ourPawns);
				Square bishop = lsb(ourBishops);
				Square promotionSquare = lsb(side ? (squaresBehind(pawn) & ranks(7)) : (squaresAhead(pawn) & ranks(0)));
                if ((bishop & squaresOfColor[BLACK]) != (promotionSquare & squaresOfColor[BLACK])) {
                    S8 losingDistance = chebyshevDistance[theirKing][promotionSquare] + 1;
                    S8 winningDistance = chebyshevDistance[ourKing][promotionSquare];
                    S8 pawnDistance = std::min(chebyshevDistance[pawn][promotionSquare], (U8)5) + (fileOf(theirKing) == fileOf(pawn));
                    if (losingDistance < winningDistance && losingDistance < pawnDistance) {
                        return chebyshevDistance[theirKing][pawn]; // Draw
                    }
                    else {
                        return KNOWNWIN + 5 * chebyshevDistance[pawn][promotionSquare] + 10 * (7 - chebyshevDistance[theirKing][pawn]);
                    }
                }
                else {
					// Right colour bishop
                    return KNOWNWIN + 5 * chebyshevDistance[pawn][promotionSquare] + 10 * (7 - chebyshevDistance[theirKing][pawn]);
                }
            }
            else {
                return +KNOWNWIN + 100 * (Score)popcount(ourPawns)
                    + 300 * popcount(ourBishops | ourKnights)
                    + 500 * popcount(ourRooks)
                    + 900 * popcount(ourQueens)
                    + 10 * (Score)centerDistance[theirKing]
                    + 5 * (Score)chebyshevDistance[theirKing][ourKing];
            }
            break;
        }
    default:
        return pestoEval(&pos);
    }
    

}

void Game::makeNullMove() {
    ++ply;
#if DETECTREPETITION
    repetitionTable[++repetitionCount] = pos.hashKey;
#endif
    pos.makeNullMove();
}

void Game::restore(Position save) {
    --ply;
#if DETECTREPETITION
    --repetitionCount;
#endif
    pos = save;
}
