#include "tt.h"
#include "constants.h"
#include "move.h"

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
    score = 0;
}

// Transposition table and evaluation hash table
ttBucket* tt;
evalHashEntry* evalHash;

void initTT(){
    if(tt != nullptr) delete[] tt;
    tt = new ttBucket[ttBucketCount];
    if(evalHash != nullptr) delete[] evalHash;
    evalHash = new evalHashEntry[evalHashSize];
}

ttEntry* probeTT(HashKey key) {
    ttBucket *entry = &tt[key % (ttBucketCount)];
    // Iterate through the entries. 
    for (U64 i = 0; i < ttBucketSize; i++) {
        ttEntry* sentry = &(entry->entries[i]);
        // Same hash key?
        if (sentry->hashKey == key) {
            sentry->flags &= ~hashOLD;
            return sentry;
        }
    }
    return nullptr;
}

void writeTT(HashKey key, Score score, Score staticEval, Depth depth, U8 flags, Move move, Ply ply) {
    move = packMove(move); // pack to 2 bytes
    score -= ply * (score < -mateValue);
    score += ply * (score > mateValue);
    ttBucket *entry = &tt[key % (ttBucketCount)];
    // Iterate through the entries. 
    // We will replace the first entry which has a lower or equal depth, or, in the case of same key, entry->depth < depth + (flags == hashEXACT) << 1.
    U64 i;
    for (i = 0; i < ttBucketSize; i++) {
        ttEntry* sentry = &(entry->entries[i]);
        // Same hash key?
        if (sentry->hashKey == key) {
            if (sentry->depth <= depth + ((flags == hashEXACT) * 2)) {
                sentry->score = score;
                sentry->eval = staticEval;
                sentry->depth = depth;
                sentry->flags = flags;
                sentry->bestMove = move;
                return;
            }
            return; // If we found an entry with the same key, we must return in any case, in order to not duplicate.
        }
    }
    for (; i > 0 && entry->entries[i].depth + (!!(entry->entries[i].flags & hashEXACT)) <= depth + (!!(flags & hashEXACT));)--i;
    
    ttEntry* sentry = &(entry->entries[i]);
    // Shift entries to the right, starting from this
    for (U64 j = ttBucketSize - 1; j > i; j--) entry->entries[j] = entry->entries[j - 1];
    sentry->hashKey = key;
    sentry->score = score;
    sentry->eval = staticEval;
    sentry->depth = depth;
    sentry->flags = flags;
    sentry->bestMove = move;
    return;
    
}

Score getCachedEval(HashKey h){
    evalHashEntry &e = evalHash[h % (evalHashSize)];
    if (e.hashKey == h) return e.score;
    return -infinity;
}

void cacheEval(HashKey h, Score s){
    evalHashEntry* e = &evalHash[h % (evalHashSize)];
    e->hashKey = h;
    e->score = s;
}

U16 hashfull() {
    U16 cnt = 0;
	for (int i = 0; i < 1000; i++) {
		if (tt[i].entries[0].hashKey && (!(tt[i].entries[0].hashKey & hashOLD))) cnt++;
	}
	return cnt;
}