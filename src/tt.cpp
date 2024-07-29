#include "tt.h"
#include "constants.h"
#include "move.h"
#include "uci.h"
#include <vector>

ttEntry::ttEntry(HashKey h, U16 b, Depth d, U8 f, Score s) {
    hashKey = h;
    bestMove = b;
    depth = d;
    flags = f;
    score = s;
}

ttEntry::ttEntry() {
    hashKey = 0;
    bestMove = 0;
    depth = 0;
    flags = hashINVALID;
    score = noScore;
    eval = noScore;
}

// Transposition table and evaluation hash table
std::vector<ttEntry> tt;

void initTT(){
    resizeTT(hashSize);
}

ttEntry* probeTT(HashKey key) {
    ttEntry *entry = &tt[hashEntryFor(key)];
    // Iterate through the entries.
    if (entry->hashKey == key) {
        entry->flags &= ~hashOLD;
        return entry;
    }
    return nullptr;
}

void writeTT(HashKey key, Score score, Score staticEval, Depth depth, U8 flags, Move move, Ply ply, bool isPv) {

    ttEntry *entry = &tt[hashEntryFor(key)];
    move = move ? packMove(move) : entry->bestMove; // pack to 2 bytes
    if (entry->hashKey == key) {
        if (depth + isPv >= entry->depth + !!(entry->flags & hashPVMove)) {
            

            score -= ply * (score < -mateValue);
            score += ply * (score > mateValue);

            // Iterate through the entries.
            entry->hashKey = key;
            entry->score = score;
            entry->eval = staticEval;
            entry->depth = depth;
            entry->flags = flags;
        }
    }
    // Check if the entry can be overwritten.
    else if (!!(entry->flags & hashOLD) * 8 + depth + 2 * isPv + (flags & hashEXACT) >= entry->depth + !!(entry->flags & hashPVMove)) {

        move = move ? packMove(move) : entry->bestMove; // pack to 2 bytes

        score -= ply * (score < -mateValue);
        score += ply * (score > mateValue);
        
        // Iterate through the entries.
        entry->hashKey = key;
        entry->score = score;
        entry->eval = staticEval;
        entry->depth = depth;
        entry->flags = flags;
    }
    entry->bestMove = move;
}

// void writeTT(HashKey key, Score score, Score staticEval, Depth depth, U8 flags, Move move, Ply ply, bool isPv) {

//     ttEntry *entry = &tt[hashEntryFor(key)];
//     move = move ? packMove(move) : entry->bestMove; // pack to 2 bytes
//     if (
//         (flags & hashEXACT) > (entry->flags & hashEXACT)
//         || entry->hashKey != key
//         || depth + isPv >= entry->depth + !!(entry->flags & hashPVMove)
//         || (entry->flags & hashOLD)) {
//             score -= ply * (score < -mateValue);
//             score += ply * (score > mateValue);

//             // Iterate through the entries.
//             entry->hashKey = key;
//             entry->score = score;
//             entry->eval = staticEval;
//             entry->depth = depth;
//             entry->flags = flags;
//     }
//     entry->bestMove = move;
//     // Check if the entry can be overwritten.
//     // else if (!!(entry->flags & hashOLD) * 8 + depth + 2 * isPv + (flags & hashEXACT) >= entry->depth + !!(entry->flags & hashPVMove)) {

//     //     move = move ? packMove(move) : entry->bestMove; // pack to 2 bytes

//     //     score -= ply * (score < -mateValue);
//     //     score += ply * (score > mateValue);
        
//     //     // Iterate through the entries.
//     //     entry->hashKey = key;
//     //     entry->score = score;
//     //     entry->eval = staticEval;
//     //     entry->depth = depth;
//     //     entry->flags = flags;
//     // }
//     // entry->bestMove = move;
// }

U16 hashfull() {
    U16 cnt = 0;
	for (int i = 0; i < 1000; i++) if (tt[i].hashKey && (!(tt[i].flags & hashOLD))) cnt++;
	return cnt;
}