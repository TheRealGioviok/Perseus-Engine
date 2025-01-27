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
#include <cassert>

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
    // Get time
    auto start = getTime64();
    nodes = 0;
    MoveList moveList;
    pos.generateUnsortedMoves(moveList);
    UndoInfo undoer = UndoInfo(pos);

    for (int i = 0; i < moveList.count; i++) {
        if (pos.makeMove(moveList.moves[i])) {
            printMove(moveList.moves[i]);
            std::cout << moveList.moves[i] << "\t" << ": " << std::flush;
            U64 res = _perft(depth - 1);
            nodes += res;
            std::cout << res;
            std::cout << std::endl;
            undoer.undoMove(pos, moveList.moves[i]);
        }
        else {
            undoer.undoMove(pos, moveList.moves[i]);
        }
    }
    auto end = getTime64();

    std::cout << "Divide results: " << nodes << "\n"
        << "Time: " << (end - start) << " ms\n"
        << "nps: " << std::to_string((nodes * 1000) / (end - start)) << std::endl;
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
    pos.generateUnsortedMoves(moveList);
    UndoInfo undoer = UndoInfo(pos);
    ++ply;
    for (int i = 0; i < moveList.count; i++) {
        if (pos.makeMove(moveList.moves[i])) {
            n += _perft(depth - 1);
            undoer.undoMove(pos, moveList.moves[i]);
        }
        else {
            undoer.undoMove(pos, moveList.moves[i]);
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
    lastScore = 0;
    ply = 0;
    seldepth = 0;
    rootDelta = infinity;
    nmpPlies = 0;

    // Clear history, killer and counter move tables
    memset(historyTable, 0, sizeof(historyTable));
    memset(counterMoveTable, 0, sizeof(counterMoveTable));
    memset(captureHistoryTable, 0, sizeof(captureHistoryTable));
    memset(continuationHistoryTable, 0, sizeof(continuationHistoryTable));
    memset(pawnsCorrHist, 0, sizeof(pawnsCorrHist));
    memset(nonPawnsCorrHist, 0, sizeof(nonPawnsCorrHist));
    memset(tripletCorrHist, 0, sizeof(tripletCorrHist));


    // Clear pv len and pv table
    memset(pvLen, 0, sizeof(pvLen));
    memset(pvTable, 0, sizeof(pvTable));

    // Clear hashTable
#if ENABLETTSCORING
    // tt is a vector, so we can't use memset. We have to clear it manually.
    for (size_t i = 0; i < tt.size(); i++) {
        tt[i].hashKey = 0;
        tt[i].bestMove = 0;
        tt[i].depth = 0;
        tt[i].score = noScore;
        tt[i].eval = noScore;
        tt[i].flags = hashINVALID;
    }

    // Clear pawn hash table
    memset(pawnEvalHash, 0, sizeof(pawnEvalHash));
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
        Move parsedMove = getLegal(move.c_str());
        repetitionTable[pos.totalPly] = pos.hashKey;
        if (!pos.makeMove(parsedMove)) return false;
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
    pos.generateUnsortedMoves(moveList);
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
 * @param killer1 The first killer move.
 * @param killer2 The second killer move.
 * @param counterMove The counter move.
 */
void Game::generateMoves(MoveList &moves, SStack* ss){
    pos.generateMoves(moves, ss);
    // We will sort the moves
    std::sort(std::begin(moves.moves) + 1, std::begin(moves.moves) + moves.count, std::greater<ScoredMove>());
}

/**
 * @brief The generateCaptures function generates all pseud legal captures for the current position. It is a call to the internal Position::generateCaptures function.
 * @param moves The MoveList object to store the moves in.
 */
void Game::generateCaptures(MoveList &moves){
    pos.generateCaptures(moves);
    // We will sort the moves
    std::sort(std::begin(moves.moves) + 1, std::begin(moves.moves) + moves.count, std::greater<ScoredMove>());
}

/**
 * @brief The makeMove function makes a move on the board. It is a call to the internal Position::makeMove function.
 * @param move The move to make.
 * @return True if the move was made, false otherwise (state of the position is undefined).
 */
bool Game::makeMove(Move move){
    assert(pos.totalPly >= 0);
    assert(pos.totalPly >= pos.fiftyMove);
    assert(pos.totalPly <= std::numeric_limits<U16>::max());
    ++ply;
#if DETECTREPETITION
    repetitionTable[pos.totalPly] = pos.hashKey;
#endif
    bool k = pos.makeMove(move);
    return k;
}

/**
 * @brief The isRepetition function returns true if the current position is a repetition.
 * @return True if the current position is a repetition, false otherwise.
 */
bool Game::isRepetition() {
    assert(pos.totalPly >= pos.fiftyMove);
    // Get the hash key of the current position
    S32 dist = std::min((S32)pos.fiftyMove, (S32)pos.plyFromNull);
    HashKey hashKey = pos.hashKey;
    S32 startPoint = pos.totalPly;
    S32 counter = 0;
    for (int idx = 4; idx <= dist; idx += 2){
        if (repetitionTable[startPoint - idx] == hashKey) {
            if (idx < ply) return true;
            if (++counter >= 2) return true;
        }
    }
    return false;
}

/**
 * @brief The eval function evaluates the current position.
 * @return The static evaluation of the current position.
 */
Score Game::evaluate(){
    return pestoEval(&pos);
}

void Game::makeNullMove() {
    ++ply;
    repetitionTable[pos.totalPly] = pos.hashKey;
    pos.makeNullMove();
}

void Game::undo(UndoInfo& undoer, Move move) {
    --ply;
    undoer.undoMove(pos, move);
}

void Game::undoNullMove(UndoInfo& undoer) {
    --ply;
    undoer.undoNullMove(pos);
}