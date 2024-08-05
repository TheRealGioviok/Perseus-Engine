#pragma once
#include "Position.h"

extern U64 seCandidates;
extern U64 seActivations;
extern U64 avgDist;

struct SStack {
    Move excludedMove = 0;
    Score staticEval = 0;
    Move move = 0;
    S16 doubleExtensions = 0;
    Move killers[2] = {0, 0};

    SStack() {
        excludedMove = noMove;
        staticEval = 0;
        move = noMove;
        doubleExtensions = 0;
        killers[0] = killers[1] = noMove;
    }

    void wipe() {
        excludedMove = noMove;
        staticEval = 0;
        move = noMove;
        doubleExtensions = 0;
        killers[0] = killers[1] = noMove;
    }
};

class Game {
public:
    U64 nodes = 0;
    Ply ply;
    Position pos;
    Depth depth;
    Depth currSearch;
    U16 seldepth = 0;
    U64 moveTime;
    U8 searchMode = 0;
    U32 wtime;
    U32 btime;
    U32 winc;
    U32 binc;
    S32 rootDelta = infinity;
    Score lastScore = 0;
    S16 nmpPlies = 0;
    bool stopped = false;

    /**
     * @brief The print function prints the position to stdout.
     */
    void print();

    /**
     * @brief The perft function calculates the number of leaf nodes in the subtree rooted at the current position.
     * @param depth The depth to calculate to.
     * @return The number of leaf nodes in the subtree rooted at the current position.
     */
    U64 perft(Depth _depth);

    /**
     * @brief The _perft function calculates the number of leaf nodes in the subtree rooted at the current position.
     * @param depth The depth to calculate to.
     * @return The number of leaf nodes in the subtree rooted at the current position.
     * @note This function is used by the perft function.
     */
    U64 _perft(Depth _depth);

    /**
     * @brief The divide function calculates the number of leaf nodes in the subtree rooted at the current position.
     * It shows the number of nodes for each possible move in the current position.
     * @param depth The depth to calculate to.
     * @return The number of leaf nodes in the subtree rooted at the current position.
     */
    U64 divide(Depth depth);

    /**
     * @brief The reset function resets the game.
     */
    void reset();

    /**
     * @brief The parseFEN function parses a FEN string, and places the pieces on the board. It also sets the turn, castling rights, en passant square, half move clock and full move number.
     * It is filled with sanity checks, and will print an error message containing the error if it finds one.
     * @param fen The FEN string to parse.
     * @return True if the FEN string was parsed successfully, false otherwise (state of the position is undefined).
     * @note This function is a call to the Position::parseFEN function.
     */
    bool parseFEN(char* FEN);

    /**
     * @brief The executeMoveList function executes a list of moves.
     * @param moveList The char* to the list of moves to execute.
     * @return True if the move list was executed successfully, false otherwise (state of the position is undefined).
     */
    bool executeMoveList(char* moveList);

    /**
     * @brief The getLegal function returns a move from a string. It returns 0 if the move is not legal.
     * @param move The string to parse.
     * @return The move parsed from the string.
     */
    Move getLegal(std::string move);

    /**
     * @brief The generateMoves function generates all pseudo legal moves for the current position. It is a call to the internal Position::generateMoves function.
     * @param moves The MoveList object to store the moves in.
     * @param killer1 The first killer move.
     * @param killer2 The second killer move.
     * @param counterMove The counter move.
     */
    void generateMoves(MoveList& moves, Move killer1, Move killer2, Move counterMove);

    /**
     * @brief The generateCaptures function generates all pseud legal captures for the current position. It is a call to the internal Position::generateCaptures function.
     * @param moves The MoveList object to store the moves in.
     */
    void generateCaptures(MoveList &move);

    /**
     * @brief The makeMove function makes a move on the board. It is a call to the internal Position::makeMove function, but it also updates the repetition table, as well as the ply.
     * @param move The move to make.
     * @return True if the move was made, false otherwise (state of the position is undefined).
     */
    bool makeMove(Move move);

    /**
     * @brief The isRepetition function returns true if the current position is a repetition.
     * @return True if the current position is a repetition, false otherwise.
     */
    bool isRepetition();

    /**
     * @brief The eval function evaluates the current position.
     * @return The static evaluation of the current position.
     */
    Score evaluate();

    /** 
     * @brief The search function is the main routine for the engine search
     * @param alpha The lower bound of the search
     * @param beta The upper bound of the search
     * @param depth The depth to the quiescence drop
     * @param cutnode If true, the node is expected to be a cut node
     * @param ss The SStack object to store the search information in
     * @return The score of the position
     */
    Score search(Score alpha, Score beta, Depth depth, bool cutnode, SStack* ss);

    /**
     * @brief The makeNullMove function makes a null move on the board.
     * @note This function is a call to the internal Position::makeNullMove function, but it will update repetition informatio, as well as the ply;
     */
    void makeNullMove();

    /**
     * @brief The undo function undoes a move on the board.
     * @param undoer The UndoInfo object to store the information in.
     * @param move The move to undo.
     * @note Do not use this function to undo a null move. 
     */
    void undo(UndoInfo &undoer, Move move);

    /**
     * @brief The undoNullMove function undoes a null move on the board.
     * @param undoer The UndoInfo object to store the information in.
     * @note This function is a call to the internal Position::undoNullMove function, but it will update repetition information, as well as the ply.
     */
    void undoNullMove(UndoInfo &undoer);

    /**
     * @brief The quiescence function takes the leaves of the negamax tree and expands them up until no immediate tactical moves are available.
     * @param alpha The lower bound of the search
     * @param beta The upper bound of the search
     * @return The score of the position.
     */
    Score quiescence(Score alpha, Score beta);

    /**
     * @brief The startSearch function starts the search. It uses the internal parameters set by the GUI.
     * @param halveTT If true, the transposition table depth will be halved before the search.
     */
    void startSearch(bool halveTT);

    /**
     * @brief The contempt function returns the contempt score. The contempt score is used to assign a non-zero score to draw positions, depending on the optimism of the engine.
     * @return The contempt score.
     */
    Score contempt();

    /**
     * @brief The communicate function communicates with the GUI.
     * @note This is a call to the uci communicate(Game) function.
     */
    void communicate();
};

