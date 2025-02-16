#pragma once
#include "types.h"
#include "constants.h"
#include <vector>

// Table sizes (TODO: Make these configurable at runtime)
constexpr U64 ttEntryCount = 1048576;                      // 2^20
constexpr U64 ttBucketSize = 4;                            // 2^2
constexpr U64 ttBucketCount = ttEntryCount / ttBucketSize; // 2^18

// The enum of hashFlags
enum HashFlag
{
    hashNONE = 0,
    hashUPPER = 1,
    hashLOWER = 2,
    hashEXACT = 3,
    hashINVALID = 8,
    hashOLD = 16,
    hashSINGULAR = 32,
    hashEVALONLY = 64,
    hashPVMove = 128
};

#ifdef _MSC_VER
__declspec(align(16)) struct ttEntry {
    ttEntry(HashKey h, U16 b, Depth d, U8 f, Score s);
    ttEntry();
    HashKey hashKey;         // 8
    PackedMove bestMove;     // 2
    Depth depth;             // 1
    U8 flags = hashINVALID;  // 1
    Score score = noScore; // 2
    Score eval = noScore;  // 2
};

__declspec(align(64))struct ttBucket {
    ttEntry entries[ttBucketSize];
    ttBucket(){
		for (U64 i = 0; i < ttBucketSize; i++){
			entries[i] = ttEntry(0, 0, 0, hashINVALID, 0);
			entries[i].eval = noScore;
		}
	}
};
#elif __GNUC__
struct ttEntry {
    ttEntry(HashKey h, U16 b, Depth d, U8 f, Score s);
    ttEntry();
    HashKey hashKey;         // 8
    PackedMove bestMove;     // 2
    Depth depth;             // 1
    U8 flags = hashINVALID;  // 1
    Score score = noScore; // 2
    Score eval = noScore;  // 2
} __attribute__((aligned(16)));

struct ttBucket {
    ttEntry entries[ttBucketSize];
    ttBucket(){
        for (U64 i = 0; i < ttBucketSize; i++){
            entries[i] = ttEntry(0, 0, 0, hashINVALID, 0);
            entries[i].eval = noScore;
        }
    }
} __attribute__((aligned(64)));
#else
// TODO: Add support for other compilers
#error "Compiler not supported"
#endif



// Transposition table and evaluation hash table
extern std::vector<ttEntry> tt;

inline U64 hashEntryFor(HashKey key) {
    return static_cast<U64>(
        (
            (static_cast<U128>(key) * static_cast<U128>(tt.size())) >> 64
        )
    );
}

static inline void resizeTT(U64 mbSize){
    // calculate the number of entries
    U64 ttEntryCount = 1024 * 1024 * mbSize / sizeof(ttEntry);
    // allocate the memory on the tt vector
    tt.resize(ttEntryCount);
    // clear the memory
    for (ttEntry &e : tt) e = ttEntry();
}

/**
 * @brief The initTT function initializes the transposition table and evaluation cache.
 * @note The size is not configurable at runtime yet.
 */
void initTT();

/**
 * @brief The probeTT function probes the transposition table. It checks if the entry exists and if it is valid.
 * @param key The key.
 * @return The entry, or nullptr if it does not exist.
 */
ttEntry *probeTT(HashKey key);

/**
 * @brief The writeTT function writes the entry to the transposition table.
 * @param key The key.
 * @param score The score.
 * @param depth The depth.
 * @param flags The flags.
 * @param move The move.
 * @param ply The ply.
 * @param isPv The isPv flag.
 * @param wasPv The wasPv flag.
 */
void writeTT(HashKey key, Score score, Score staticEval, Depth depth, U8 flags, Move move, Ply ply, bool isPv, bool wasPv);

U16 hashfull();