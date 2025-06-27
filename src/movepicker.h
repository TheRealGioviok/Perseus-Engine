#pragma once
#include "types.h"
#include "position.h"
#include "move.h"
#include "movegen.h"

enum MovegenType{
    MovegenTypeQuiets = 0,
    MovegenTypeNoisy = 1,
    MovegenTypeAll = 3
};

static inline int sortTTUp(MoveList &ml, PackedMove ttMove)
{
    for (int i = 0; i < ml.count; i++)
    {
        if (packedMoveCompare(ttMove, (Move)ml.moves[i]))
        {
            ml.moves[0] = ml.moves[i];
            ml.moves[i] = 0;
            return 0;
        }
    }
    return 1;
}

class MovePicker
{
    private:
    MoveList movelist;
    Move excludedMove;
    PackedMove ttMove;

public:
    U16 currentIndex;

    MovePicker(Position &pos, Move excludedMove, PackedMove ttMove, SStack *ss, 
               MovegenType U = MovegenTypeAll)
        : excludedMove(excludedMove), ttMove(ttMove)
    {
        movelist.count = 1; // Reserve space for the TT move
        if (U == MovegenTypeNoisy){
            pos.generateCaptures(movelist);
        }
        else {
            pos.generateMoves(movelist, ss);
        }

        // We will sort the moves (TODO: change this later)
        std::sort(std::begin(movelist.moves) + 1, std::begin(movelist.moves) + movelist.count, std::greater<ScoredMove>());

        // TODO: change this with legality check
        if (ttMove) currentIndex = sortTTUp(movelist, ttMove);
        else currentIndex = 1;
    }

    bool hasNext()
    {
        return currentIndex < movelist.count;
    }

    ScoredMove next()
    {
        while (hasNext()) {
            ScoredMove move = movelist.moves[currentIndex++];
            if (move && !sameMovePos(move, excludedMove)) {
                return move;
            }
        }
        return noMove;
    }
};


