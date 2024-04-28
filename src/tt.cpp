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
evalHashEntry* evalHash;

void initTT(){
    resizeTT(hashSize);
    if(evalHash != nullptr) delete[] evalHash;
    evalHash = new evalHashEntry[evalHashSize];
}

ttEntry* probeTT(HashKey key) {
    ttEntry *entry = &tt[key % (tt.size())];
    // Iterate through the entries.
    if (entry->hashKey == key) {
        entry->flags &= ~hashOLD;
        return entry;
    }
    return nullptr;
}

void writeTT(HashKey key, Score score, Score staticEval, Depth depth, U8 flags, Move move, Ply ply) {
    move = packMove(move); // pack to 2 bytes
    score -= ply * (score < -mateValue);
    score += ply * (score > mateValue);
    ttEntry *entry = &tt[key % (tt.size())];
    // Iterate through the entries.
    if (depth >= entry->depth) {
        entry->hashKey = key;
        entry->score = score;
        entry->eval = staticEval;
        entry->depth = depth;
        entry->flags = flags;
        entry->bestMove = move;
        return;
    }
    return;
}

Score getCachedEval(HashKey h){
    evalHashEntry &e = evalHash[h % (evalHashSize)];
    if (e.hashKey == h) return e.score;
    return noScore;
}

void cacheEval(HashKey h, Score s){
    evalHashEntry* e = &evalHash[h % (evalHashSize)];
    e->hashKey = h;
    e->score = s;
}

U16 hashfull() {
    U16 cnt = 0;
	for (int i = 0; i < 1000; i++) if (tt[i].hashKey && (!(tt[i].flags & hashOLD))) cnt++;
	return cnt;
}