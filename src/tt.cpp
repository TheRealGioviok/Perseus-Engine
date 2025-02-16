#include "tt.h"
#include "constants.h"
#include "move.h"
#include "uci.h"
#include <vector>

ttEntry::ttEntry(HashKey h, U16 b, Depth d, U8 f, Score s) {
    hashKey = h;
    bestMove = b;
    depth = d;
    ageFlags = f;
    score = s;
}

ttEntry::ttEntry() {
    hashKey = 0;
    bestMove = 0;
    depth = 0;
    ageFlags = hashNONE;
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
        // entry->flags &= ~hashOLD;
        return entry;
    }
    return nullptr;
}

void writeTT(HashKey key, Score score, Score staticEval, Depth depth, U8 flags, U8 age, Move move, Ply ply, bool isPv, bool wasPv) {

    ttEntry *entry = &tt[hashEntryFor(key)];
    
    if (((flags & hashEXACT) > (entry->ageFlags & hashEXACT)) || ((entry->ageFlags >> 3) == age) || depth + 2 * isPv + 3 * (entry->hashKey != key) >= entry->depth) {
        score -= ply * (score < -mateValue);
        score += ply * (score > mateValue);

        // Iterate through the entries.
        entry->score = score;
        entry->eval = staticEval;
        entry->depth = depth;
        entry->ageFlags = flags | (wasPv * hashPVMove) | (age << 3);
        entry->bestMove = move ? packMove(move) : (entry->hashKey == key) ? entry->bestMove : 0; // pack to 2 bytes, if we don't have a move, we don't want to overwrite the old move if usable
        entry->hashKey = key;
        return;
    }
}

U16 hashfull(U8 age) {
    U16 cnt = 0;
	for (int i = 0; i < 1000; i++) 
        if (tt[i].hashKey && ((tt[i].ageFlags >> 3) == age)) cnt++;
	return cnt;
}