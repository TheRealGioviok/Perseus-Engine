#include "evaluation.h"
#include "tables.h"
#include "tt.h"
#include "movegen.h"
#include "zobrist.h"
#include "types.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <cmath>

PScore PSQTs[2][12][64];

PawnEvalHashEntry pawnEvalHash[PAWNHASHSIZE];


void initTables() {
    for(Square square = a8; square < noSquare; square++) {
        // White
        PSQTs[0][P][square] = pawnTable[0][square] ; // + materialValues[P];
        PSQTs[1][P][square] = pawnTable[1][square] ; // + materialValues[P];
        PSQTs[0][N][square] = knightTable[0][square] ; // + materialValues[N];
        PSQTs[1][N][square] = knightTable[1][square] ; // + materialValues[N];
        PSQTs[0][B][square] = bishopTable[0][square] ; // + materialValues[B];
        PSQTs[1][B][square] = bishopTable[1][square] ; // + materialValues[B];
        PSQTs[0][R][square] = rookTable[0][square] ; // + materialValues[R];
        PSQTs[1][R][square] = rookTable[1][square] ; // + materialValues[R];
        PSQTs[0][Q][square] = queenTable[0][square] ; // + materialValues[Q];
        PSQTs[1][Q][square] = queenTable[1][square] ; // + materialValues[Q];
        PSQTs[0][K][square] = kingTable[0][square];
        PSQTs[1][K][square] = kingTable[1][square];
        // Black
        PSQTs[0][p][flipSquare(square)] = -PSQTs[0][P][square];
        PSQTs[1][p][flipSquare(square)] = -PSQTs[1][P][square];
        PSQTs[0][n][flipSquare(square)] = -PSQTs[0][N][square];
        PSQTs[1][n][flipSquare(square)] = -PSQTs[1][N][square];
        PSQTs[0][b][flipSquare(square)] = -PSQTs[0][B][square];
        PSQTs[1][b][flipSquare(square)] = -PSQTs[1][B][square];
        PSQTs[0][r][flipSquare(square)] = -PSQTs[0][R][square];
        PSQTs[1][r][flipSquare(square)] = -PSQTs[1][R][square];
        PSQTs[0][q][flipSquare(square)] = -PSQTs[0][Q][square];
        PSQTs[1][q][flipSquare(square)] = -PSQTs[1][Q][square];
        PSQTs[0][k][flipSquare(square)] = -PSQTs[0][K][square];
        PSQTs[1][k][flipSquare(square)] = -PSQTs[1][K][square];
    }
}

constexpr PScore DOUBLEISOLATEDPEN = S(14, 47);
constexpr PScore ISOLATEDPEN = S(17, 22);
constexpr PScore BACKWARDPEN = S(3, 16);
constexpr PScore DOUBLEDPEN = S(15, 21);
constexpr PScore SUPPORTEDPHALANX = S(0, 4);
constexpr PScore ADVANCABLEPHALANX = S(7, 27);
constexpr PScore R_SUPPORTEDPHALANX = S(3, 10);
constexpr PScore R_ADVANCABLEPHALANX = S(1, 19);
constexpr PScore passedRankBonus[7] = {S(0, 0), S(11, -88), S(-9, -60), S(-3, -5), S(29, 47), S(49, 149), S(144, 242), };
constexpr PScore PASSEDPATHBONUS = S(-5, 24);
constexpr PScore SUPPORTEDPASSER = S(27, 1);
constexpr PScore INNERSHELTER = S(2, -6);
constexpr PScore OUTERSHELTER = S(9, 4);
constexpr PScore BISHOPPAIR = S(19, 127);
constexpr PScore ROOKONOPENFILE = S(22, 12);
constexpr PScore ROOKONSEMIOPENFILE = S(20, 10);
constexpr PScore KNIGHTONEXTOUTPOST = S(23, 36);
constexpr PScore BISHOPONEXTOUTPOST = S(27, 0);
constexpr PScore KNIGHTONINTOUTPOST = S(20, 39);
constexpr PScore BISHOPONINTOUTPOST = S(31, -7);
constexpr PScore KNIGHTPROTECTOR = S(-6, 0);
constexpr PScore BISHOPPROTECTOR = S(-4, 0);
constexpr PScore BISHOPPAWNS = S(1, -4);
constexpr PScore THREATSAFEPAWN = S(49, 63);
constexpr PScore THREATPAWNPUSH = S(20, 37);
constexpr PScore PAWNHANGING = S(-3, -62);
constexpr PScore NONPAWNHANGING = S(-24, -37);
constexpr PScore KINGTHREAT = S(-2, 1);
constexpr PScore QUEENINFILTRATION = S(-2, 1);
constexpr PScore RESTRICTEDSQUARES = S(4, 4);
constexpr PScore TEMPO = S(14, 28);

constexpr PScore PAWNATTACKINNERRING = S(-61, 104);
constexpr PScore KNIGHTATTACKINNERRING = S(95, 48);
constexpr PScore BISHOPATTACKINNERRING = S(102, -5);
constexpr PScore ROOKATTACKINNERRING = S(107, -44);
constexpr PScore QUEENATTACKINNERRING = S(95, -165);
constexpr PScore PAWNATTACKOUTERRING = S(73, -62);
constexpr PScore KNIGHTATTACKOUTERRING = S(72, 271);
constexpr PScore BISHOPATTACKOUTERRING = S(90, 96);
constexpr PScore ROOKATTACKOUTERRING = S(40, 48);
constexpr PScore QUEENATTACKOUTERRING = S(82, 55);
constexpr PScore NOQUEENDANGER = S(-629, -8734);
constexpr PScore PINNEDSHELTERDANGER = S(113, -41);
constexpr PScore SAFECHECK[4] = {S(299, 484), S(131, -237), S(255, -31), S(215, -7), };
constexpr PScore ALLCHECKS[4] = {S(68, 190), S(35, 140), S(88, -46), S(7, 56), };
constexpr PScore SAFETYINNERSHELTER = S(-48, 41);
constexpr PScore SAFETYOUTERSHELTER = S(-45, -20);
constexpr PScore INNERWEAKNESS = S(79, 51);
constexpr PScore OUTERWEAKNESS = S(14, 17);
constexpr PScore KSTEMPO = S(34, -4);

constexpr Score COMPLEXITYPASSERS = 220;
constexpr Score COMPLEXITYPAWNS = 971;
constexpr Score COMPLEXITYBLOCKEDPAIRS = -338;
constexpr Score COMPLEXITYPAWNTENSION = -873;
constexpr Score COMPLEXITYOUTFLANKING = -68;
constexpr Score COMPLEXITYINFILTRATION = -928;
constexpr Score COMPLEXITYPAWNBOTHFLANKS = 10343;
constexpr Score COMPLEXITYPAWNENDING = 12682;
constexpr Score COMPLEXITYALMOSTUNWINNABLE = -3350;
constexpr Score COMPLEXITYBIAS = -19960;

// Function to access the table values
static inline S32 getKingSafetyFromTable(const std::array<int, KSTABLESIZE>& table, int x) {
    // Map x to the table index range
    S32 index = ((x - MIN_X) * (KSTABLESIZE - 1) + (MAX_X - MIN_X)/2) / (MAX_X - MIN_X);
    return table[std::clamp(index, 0, KSTABLESIZE-1)];
}

int getKingSafetyMg(int x) {
    return getKingSafetyFromTable(kingSafetyMgTable, x);
}

int getKingSafetyEg(int x) {
    return getKingSafetyFromTable(kingSafetyEgTable, x);
}

static inline constexpr BitBoard centralFiles = files(2) | files(3) | files(4) | files(5);

template <Piece pt>
//(bb, occ, pinned, mobilityArea, mobilityScore);
static inline void getMobilityFeat(const BitBoard (&bb)[12], BitBoard occCheck, const BitBoard pinned, const BitBoard mob, BitBoard &attackedByUs, BitBoard &ourMultiAttacks, BitBoard &ptAttacks, S8 *features, const Square kingSquare, const BitBoard kingRing, const BitBoard kingOuter, S32 &innerAttacks, S32 &outerAttacks, S32 &kingDist) {
    constexpr Side us = pt < p ? WHITE : BLACK;
    // constexpr Side them = us ? BLACK : WHITE;
    BitBoard pieces = bb[pt] & ~pinned;

    // Consider diagonal xrays through our queens
    if constexpr (pt == B || pt == Q)                   occCheck ^= bb[Q];
    else if constexpr (pt == b || pt == q)              occCheck ^= bb[q];

    // Consider cross xrays through our rooks
    if constexpr (pt == R || pt == Q)                   occCheck ^= bb[R];
    else if constexpr (pt == r || pt == q)              occCheck ^= bb[r];
    
    // Iterate over all pieces of the given type
    
    while (pieces) {
        BitBoard moves;
        BitBoard mobMoves;
        Square sq = popLsb(pieces);
        if constexpr (pt == N || pt == n) {
            moves = knightAttacks[sq];
            mobMoves = moves & mob;
            U8 moveCount = popcount(mobMoves);
            features[moveCount] += us == WHITE ? 1 : -1;
            kingDist += chebyshevDistance[kingSquare][sq] * (us == WHITE ? 1 : -1);
        }
        else if constexpr (pt == B || pt == b) { // X-ray through our queens
            moves = getBishopAttack(sq, occCheck);
            mobMoves = moves & mob;
            U8 moveCount = popcount(mobMoves);
            features[moveCount] += us == WHITE ? 1 : -1;
            kingDist += chebyshevDistance[kingSquare][sq] * (us == WHITE ? 1 : -1);
        }
        else if constexpr (pt == R || pt == r) { // X-ray through our queens and rooks
            moves = getRookAttack(sq, occCheck);
            mobMoves = moves & mob;
            U8 moveCount = popcount(mobMoves);
            features[moveCount] += us == WHITE ? 1 : -1;
        }
        else if constexpr (pt == Q || pt == q) { // X-ray through our queens
            moves = getQueenAttack(sq, occCheck);
            mobMoves = moves & mob;
            U8 moveCount = popcount(mobMoves);
            features[moveCount] += us == WHITE ? 1 : -1;
        }
        innerAttacks += popcount(mobMoves & kingRing);
        outerAttacks += popcount(mobMoves & kingOuter);
        ourMultiAttacks |= attackedByUs & moves;
        attackedByUs |= moves;
        ptAttacks |= moves;
    }
} 

template <Piece pt>
//(bb, occ, pinned, mobilityArea, mobilityScore);
static inline void mobility(const BitBoard *bb, BitBoard occCheck, const BitBoard pinned, const BitBoard mobilityArea, PScore &score, BitBoard &attackedByUs, BitBoard &ourMultiAttacks, BitBoard &ptAttacks, const Square kingSquare, const BitBoard kingRing, const BitBoard kingOuter, PScore &innerAttacks, PScore &outerAttacks){
    // constexpr Side them = us ? BLACK : WHITE;
    BitBoard pieces = bb[pt] & ~pinned; // TODO: try to exclude xray through pinned pieces

    // Consider diagonal xrays through our queens
    if constexpr (pt == B || pt == Q)                   occCheck ^= bb[Q];

    // Consider cross xrays through our rooks
    if constexpr (pt == R || pt == Q)                   occCheck ^= bb[R];
    
    // Iterate over all pieces of the given type
    
    while (pieces) {
        BitBoard moves;
        BitBoard mobMoves;
        Square sq = popLsb(pieces);
        if constexpr (pt == N) {
            moves = knightAttacks[sq];
            mobMoves = moves & mobilityArea;
            U8 moveCount = popcount(mobMoves);
            // Add the mobility score
            score += knightMob[moveCount] + KNIGHTPROTECTOR * chebyshevDistance[kingSquare][sq];
            innerAttacks += KNIGHTATTACKINNERRING * popcount(mobMoves & kingRing);
            outerAttacks += KNIGHTATTACKOUTERRING * popcount(mobMoves & kingOuter);
        }
        else if constexpr (pt == B) {
            moves = getBishopAttack(sq, occCheck);
            mobMoves = moves & mobilityArea;
            U8 moveCount = popcount(mobMoves); // X-ray through our queens
            // Add the mobility score
            score += bishopMob[moveCount] + BISHOPPROTECTOR * chebyshevDistance[kingSquare][sq];
            innerAttacks += BISHOPATTACKINNERRING * popcount(mobMoves & kingRing);
            outerAttacks += BISHOPATTACKOUTERRING * popcount(mobMoves & kingOuter);
        }
        else if constexpr (pt == R) {
            moves = getRookAttack(sq, occCheck);
            mobMoves = moves & mobilityArea;
            U8 moveCount = popcount(mobMoves); // X-ray through our queens and rooks
            // Add the mobility score
            score += rookMob[moveCount];
            innerAttacks += ROOKATTACKINNERRING * popcount(mobMoves & kingRing);
            outerAttacks += ROOKATTACKOUTERRING * popcount(mobMoves & kingOuter);
        }
        else if constexpr (pt == Q) {
            moves = getQueenAttack(sq, occCheck);
            mobMoves = moves & mobilityArea;
            U8 moveCount = popcount(mobMoves); // X-ray through our queens
            // Add the mobility score
            score += queenMob[moveCount];
            innerAttacks += QUEENATTACKINNERRING * popcount(mobMoves & kingRing);
            outerAttacks += QUEENATTACKOUTERRING * popcount(mobMoves & kingOuter);
        }
        ourMultiAttacks |= attackedByUs & moves;
        attackedByUs |= moves;
        ptAttacks |= moves;
    }
} 

static inline BitBoard filesFromBB(BitBoard bb){
    bb |= bb >> 8 | bb << 8;
    bb |= bb >> 16 | bb << 16;
    bb |= bb >> 32 | bb << 32;
    return bb;
}

template <bool side>
static inline BitBoard advancePathMasked(BitBoard bb, BitBoard mask){
    if constexpr (side == WHITE){
        bb |= (bb >> 8) & mask;
        bb |= (bb >> 16) & mask;
        bb |= (bb >> 32) & mask;
    }
    else {
        bb |= (bb << 8) & mask;
        bb |= (bb << 16) & mask;
        bb |= (bb << 32) & mask;
    }
    return bb;
}

template <bool side>
static inline BitBoard pawnSpanPawns(BitBoard movers, BitBoard blockers){
    if constexpr (side == WHITE){
        return makePawnAttacks<WHITE>(advancePathMasked<WHITE>(movers, ~blockers)) & ~blockers;
    }
    else {
        return makePawnAttacks<BLACK>(advancePathMasked<BLACK>(movers, ~blockers)) & ~blockers;
    }
}

inline PScore pawnEval(const HashKey hashKey, const BitBoard (&bb)[12], const BitBoard (&doubledPawns)[2], const BitBoard (&pawnFiles)[2], const BitBoard (&protectedPawns)[2], const BitBoard (&pawnBlockage)[2], const BitBoard (&occ)[3], const BitBoard(&attackedBy)[2], const BitBoard (multiAttacks)[2], const BitBoard (defendedByPawn)[2], S32& passedCount){
    // Try probing for the correct side here
    PawnEvalHashEntry& entry = pawnEvalHash[hashKey & 0x3FFFF];
    // Init the score
    PScore score = PScore(0,0);
    // Check
    if (entry.hash == hashKey){
        BitBoard whitePassers = entry.passers & bb[P];
        BitBoard blackPassers = entry.passers & bb[p];
        passedCount += popcount(entry.passers); // May already be nonzero
        score = entry.score;
        BitBoard block = occ[BOTH] | (~defendedByPawn[WHITE] & ((~attackedBy[WHITE] & attackedBy[BLACK]) | (multiAttacks[BLACK]))); 
        while (whitePassers){
            Square sq = popLsb(whitePassers);
            BitBoard sqb = squareBB(sq);
            // Give bonus for how many squares the pawn can advance
            BitBoard passedPath = advancePathMasked<WHITE>(sqb, ~block);
            score += PASSEDPATHBONUS * popcount(passedPath);
        }
        block = occ[BOTH] | (~defendedByPawn[BLACK] & ((~attackedBy[BLACK] & attackedBy[WHITE]) | (multiAttacks[WHITE]))); 
        while (blackPassers){
            Square sq = popLsb(blackPassers);
            BitBoard sqb = squareBB(sq);
            // Give bonus for how many squares the pawn can advance
            BitBoard passedPath = advancePathMasked<BLACK>(sqb, ~block);
            score -= PASSEDPATHBONUS * popcount(passedPath);
        }
        return score;
    }
    else {
        PScore extraScore = PScore(0,0);
        BitBoard passers = 0ULL;
        BitBoard pieces = bb[P];
        BitBoard block = occ[BOTH] | (~defendedByPawn[WHITE] & ((~attackedBy[WHITE] & attackedBy[BLACK]) | (multiAttacks[BLACK])));
        while (pieces){
            // Evaluate isolated, backward, doubled pawns and connected pawns
            const Square sq = popLsb(pieces);
            const BitBoard sqb = squareBB(sq);
            const U8 rank = 7 - rankOf(sq);
            bool doubled = doubledPawns[WHITE] & sqb;
            bool isolated = !(isolatedPawnMask[sq] & bb[P]);
            bool pawnOpposed = !(pawnFiles[BLACK] & sqb);
            bool supported = protectedPawns[WHITE] & sqb;
            bool advancable = !(pawnBlockage[WHITE] & north(sqb));
            bool phal = phalanx[sq] & bb[P];
            bool candidatePassed = !(passedPawnMask[WHITE][sq] & bb[p]);
            
            bool backward = !( // If the pawn
                (backwardPawnMask[WHITE][sq] & bb[P]) // If the pawn is doesn't have behind or adjacent pawns in adjacent files
                || advancable // If the pawn can't advance (blocked by enemy pawn or enemy pawn capture square)
            );
            // Check if the pawn is doubled
            if (isolated){
                if (doubled && pawnOpposed) score -= DOUBLEISOLATEDPEN;
                else score -= ISOLATEDPEN;
            }
            else if (backward) score -= BACKWARDPEN;

            if (doubled && !supported) score -= DOUBLEDPEN;
            else if (supported && phal){
                score += SUPPORTEDPHALANX;
                score += R_SUPPORTEDPHALANX * (rank - 2);
            }
            else if (advancable && phal){
                score += ADVANCABLEPHALANX;
                score += R_ADVANCABLEPHALANX * (rank - 2);
            }
            if (candidatePassed){
                // Give bonus for how close the pawn is to the promotion square
                score += passedRankBonus[rank];
                // Bonus for connected or supported passed pawns
                if (supported) score += SUPPORTEDPASSER;
                ++passedCount;
                passers |= sqb;
                // Give bonus for how many squares the pawn can advance. This can't be stored into the entry, since it depends on the rest of the position.
                BitBoard passedPath = advancePathMasked<WHITE>(sqb, ~block);
                extraScore += PASSEDPATHBONUS * popcount(passedPath);
            }
        }

        // Black pawns
        pieces = bb[p];
        block = occ[BOTH] | (~defendedByPawn[BLACK] & ((~attackedBy[BLACK] & attackedBy[WHITE]) | (multiAttacks[WHITE])));
        while (pieces){
            // Evaluate isolated, backward, doubled pawns and connected pawns
            const Square sq = popLsb(pieces);
            const BitBoard sqb = squareBB(sq);
            const U8 rank = rankOf(sq);
            bool doubled = doubledPawns[BLACK] & sqb;
            bool isolated = !(isolatedPawnMask[sq] & bb[p]);
            bool pawnOpposed = !(pawnFiles[WHITE] & sqb);
            bool supported = protectedPawns[BLACK] & sqb;
            bool advancable = !(pawnBlockage[BLACK] & south(sqb));
            bool phal = phalanx[sq] & bb[p];
            bool candidatePassed = !(passedPawnMask[BLACK][sq] & bb[P]);
            
            bool backward = !( // If the pawn
                (backwardPawnMask[BLACK][sq] & bb[p]) // If the pawn is doesn't have behind or adjacent pawns in adjacent files
                || advancable // If the pawn can't advance (blocked by enemy pawn or enemy pawn capture square)
            );
            // Check if the pawn is doubled
            if (isolated){
                if (doubled && pawnOpposed) score += DOUBLEISOLATEDPEN;
                else score += ISOLATEDPEN;
            }
            else if (backward) score += BACKWARDPEN;

            if (doubled && !supported) score += DOUBLEDPEN;
            else if (supported && phal){
                score -= SUPPORTEDPHALANX;
                score -= R_SUPPORTEDPHALANX * (rank - 2);
            }
            else if (advancable && phal){
                score -= ADVANCABLEPHALANX;
                score -= R_ADVANCABLEPHALANX * (rank - 2);
            }
            if (candidatePassed){
                // Give bonus for how close the pawn is to the promotion square
                score -= passedRankBonus[rank];
                // Bonus for connected or supported passed pawns
                if (supported) score -= SUPPORTEDPASSER;
                ++passedCount;
                passers |= sqb;
                // Give bonus for how many squares the pawn can advance. This can't be stored into the entry, since it depends on the rest of the position.
                BitBoard passedPath = advancePathMasked<BLACK>(sqb, ~block);
                extraScore -= PASSEDPATHBONUS * popcount(passedPath);
            }
        }

        // Save
        entry.hash = hashKey;
        entry.score = score;
        entry.passers = passers;
        return score + extraScore;
    }
    
}

Score pestoEval(Position *pos){
    auto const& bb = pos->bitboards;
    auto const& occ = pos->occupancies;
    HashKey pawnHashKey = pos->pawnHashKey ^ enPassantKeysTable[pos->enPassant];
    prefetch(&pawnEvalHash[pawnHashKey & 0x3FFFF]);
    // Setup the game phase
    S32 gamePhase = gamephaseInc[P] * popcount(bb[P] | bb[p]) +
                    gamephaseInc[N] * popcount(bb[N] | bb[n]) +
                    gamephaseInc[B] * popcount(bb[B] | bb[b]) +
                    gamephaseInc[R] * popcount(bb[R] | bb[r]) +
                    gamephaseInc[Q] * popcount(bb[Q] | bb[q]) +
                    gamephaseInc[K] * popcount(bb[K] | bb[k]);
    gamePhase = std::min(gamePhase, (S32)24); // If we have a lot of pieces, we don't want to go over 24

    const Square whiteKing = lsb(bb[K]);
    const Square blackKing = lsb(bb[k]);

    // Mobility calculations

    // Pin, mobility and threat calculations
    BitBoard attackedBy[2] = {
        kingAttacks[whiteKing],
        kingAttacks[blackKing]
    };

    BitBoard pawnAttackedSquares[2] = {
        0,
        0
    };

    BitBoard multiAttacks[2] = {
        0ULL,
        0ULL
    };
    
    BitBoard doublePawnAttackedSquares[2] = {
        0ULL,
        0ULL
    };
    
    {
        // Left Attacks
        BitBoard sidePawnAttacks[2] = {
            ((bb[P] & notFile(0)) >> 9),
            ((bb[p] & notFile(0)) << 7)
        }; 
        doublePawnAttackedSquares[WHITE] = sidePawnAttacks[WHITE];
        doublePawnAttackedSquares[BLACK] = sidePawnAttacks[BLACK];
        // Update Multi attacks
        multiAttacks[WHITE] |= attackedBy[WHITE] & sidePawnAttacks[WHITE];
        multiAttacks[BLACK] |= attackedBy[BLACK] & sidePawnAttacks[BLACK];
        // Update Normal attacks
        attackedBy[WHITE] |= sidePawnAttacks[WHITE];
        attackedBy[BLACK] |= sidePawnAttacks[BLACK];
        // Update exclusive pawn attacks
        pawnAttackedSquares[WHITE] |= sidePawnAttacks[WHITE];
        pawnAttackedSquares[BLACK] |= sidePawnAttacks[BLACK];
        // Right Attacks
        sidePawnAttacks[WHITE] = ((bb[P] & notFile(7)) >> 7); 
        sidePawnAttacks[BLACK] = ((bb[p] & notFile(7)) << 9);
        doublePawnAttackedSquares[WHITE] &= sidePawnAttacks[WHITE];
        doublePawnAttackedSquares[BLACK] &= sidePawnAttacks[BLACK];
        // Update multi attacks
        multiAttacks[WHITE] |= attackedBy[WHITE] & sidePawnAttacks[WHITE];
        multiAttacks[BLACK] |= attackedBy[BLACK] & sidePawnAttacks[BLACK];
        // Update Normal attacks
        attackedBy[WHITE] |= sidePawnAttacks[WHITE];
        attackedBy[BLACK] |= sidePawnAttacks[BLACK];
        // Update exclusive pawn attacks
        pawnAttackedSquares[WHITE] |= sidePawnAttacks[WHITE];
        pawnAttackedSquares[BLACK] |= sidePawnAttacks[BLACK];
    }

    // Pinned mask
    BitBoard RQmask[2] = {
        bb[R] | bb[Q],
        bb[r] | bb[q]
    };
    BitBoard BQmask[2] = {
        bb[B] | bb[Q],
        bb[b] | bb[q]
    };
    BitBoard pinned[2] = {
        getPinnedPieces(occ[BOTH], occ[WHITE], whiteKing, RQmask[BLACK], BQmask[BLACK]),
        getPinnedPieces(occ[BOTH], occ[BLACK], blackKing, RQmask[WHITE], BQmask[WHITE])
    };

    const BitBoard blockedPawns[2] = {
        bb[P] & (occ[BOTH] >> 8),
        bb[p] & (occ[BOTH] << 8)
    };

    BitBoard underDevelopedPawns[2] = {
        bb[P] & (ranks(6) | ranks(5)),
        bb[p] & (ranks(1) | ranks(2))
    };

    const BitBoard mobilityArea[2] = {
        // Mobility area.
        // Squares in the mobility area are:
        // 1. Free of our pieces
        // 2. Not defended by enemy pawns

        // Calculate a negative mask for the mobility area, and then invert it
        (blockedPawns[WHITE] | underDevelopedPawns[WHITE] | pawnAttackedSquares[BLACK] | pinned[WHITE] | bb[K] | bb[Q]) ^ 0xFFFFFFFFFFFFFFFF,
        (blockedPawns[BLACK] | underDevelopedPawns[BLACK] | pawnAttackedSquares[WHITE] | pinned[BLACK] | bb[k] | bb[q]) ^ 0xFFFFFFFFFFFFFFFF
    };

    // Calculate the mobility scores (index by phase and color)
    const BitBoard pawnSpan[2] = {
        advancePathMasked<WHITE>(bb[P], ~(bb[p] | pawnAttackedSquares[BLACK])),
        advancePathMasked<BLACK>(bb[p], ~(bb[P] | pawnAttackedSquares[WHITE]))
    };

    const BitBoard outpostSquares[2] = {
        (ranks(2) | ranks(3) | ranks(4)) & pawnAttackedSquares[WHITE] & ~(makePawnAttacks<BLACK>(pawnSpan[BLACK]) | bb[p]) & notFile(0) & notFile(7),
        (ranks(5) | ranks(4) | ranks(3)) & pawnAttackedSquares[BLACK] & ~(makePawnAttacks<WHITE>(pawnSpan[WHITE]) | bb[P]) & notFile(0) & notFile(7)
    };

    const BitBoard nonPawns[2] = {
        bb[N] | bb[B] | RQmask[WHITE],
        bb[n] | bb[b] | RQmask[BLACK],
    };

    const BitBoard kingRing[2] = {
        kingAttacks[whiteKing] & ~doublePawnAttackedSquares[WHITE],
        kingAttacks[blackKing] & ~doublePawnAttackedSquares[BLACK]
    };

    const BitBoard kingOuter[2] = {
        fiveSquare[whiteKing] & ~doublePawnAttackedSquares[WHITE],
        fiveSquare[blackKing] & ~doublePawnAttackedSquares[BLACK]
    };
    BitBoard kingCheckers[2][4] = {
        {
            knightAttacks[whiteKing],
            getBishopAttack(whiteKing, occ[BOTH]),
            getRookAttack(whiteKing, occ[BOTH]),
            0
        },
        {
            knightAttacks[blackKing],
            getBishopAttack(blackKing, occ[BOTH]),
            getRookAttack(blackKing, occ[BOTH]),
            0
        }
    };
    kingCheckers[WHITE][Q-1] = kingCheckers[WHITE][B-1] | kingCheckers[WHITE][R-1];
    kingCheckers[BLACK][Q-1] = kingCheckers[BLACK][B-1] | kingCheckers[BLACK][R-1];

    BitBoard ptAttacks[2][4] = { {0} };

    PScore innerAttacks[2] = {PScore(0,0), PScore(0,0)};
    PScore outerAttacks[2] = {PScore(0,0), PScore(0,0)};

    PScore score = PScore(0,0);

    // std::cout << "PSQT scores are :\t"<<-score.mg()<<"\t"<<-score.eg()<<std::endl;
    
    // Black mobility
    mobility<N>(bb+6, occ[BOTH], pinned[BLACK], mobilityArea[BLACK], score, attackedBy[BLACK], multiAttacks[BLACK], ptAttacks[BLACK][N-1], blackKing, kingRing[WHITE], kingOuter[WHITE], innerAttacks[BLACK], outerAttacks[BLACK]);
    mobility<B>(bb+6, occ[BOTH], pinned[BLACK], mobilityArea[BLACK], score, attackedBy[BLACK], multiAttacks[BLACK], ptAttacks[BLACK][B-1], blackKing, kingRing[WHITE], kingOuter[WHITE], innerAttacks[BLACK], outerAttacks[BLACK]);
    mobility<R>(bb+6, occ[BOTH], pinned[BLACK], mobilityArea[BLACK], score, attackedBy[BLACK], multiAttacks[BLACK], ptAttacks[BLACK][R-1], blackKing, kingRing[WHITE], kingOuter[WHITE], innerAttacks[BLACK], outerAttacks[BLACK]);
    mobility<Q>(bb+6, occ[BOTH], pinned[BLACK], mobilityArea[BLACK], score, attackedBy[BLACK], multiAttacks[BLACK], ptAttacks[BLACK][Q-1], blackKing, kingRing[WHITE], kingOuter[WHITE], innerAttacks[BLACK], outerAttacks[BLACK]);

    // Flip
    score = -score;

    // White mobility
    mobility<N>(bb, occ[BOTH], pinned[WHITE], mobilityArea[WHITE], score, attackedBy[WHITE], multiAttacks[WHITE], ptAttacks[WHITE][N-1], whiteKing, kingRing[BLACK], kingOuter[BLACK], innerAttacks[WHITE], outerAttacks[WHITE]);
    mobility<B>(bb, occ[BOTH], pinned[WHITE], mobilityArea[WHITE], score, attackedBy[WHITE], multiAttacks[WHITE], ptAttacks[WHITE][B-1], whiteKing, kingRing[BLACK], kingOuter[BLACK], innerAttacks[WHITE], outerAttacks[WHITE]);
    mobility<R>(bb, occ[BOTH], pinned[WHITE], mobilityArea[WHITE], score, attackedBy[WHITE], multiAttacks[WHITE], ptAttacks[WHITE][R-1], whiteKing, kingRing[BLACK], kingOuter[BLACK], innerAttacks[WHITE], outerAttacks[WHITE]);
    mobility<Q>(bb, occ[BOTH], pinned[WHITE], mobilityArea[WHITE], score, attackedBy[WHITE], multiAttacks[WHITE], ptAttacks[WHITE][Q-1], whiteKing, kingRing[BLACK], kingOuter[BLACK], innerAttacks[WHITE], outerAttacks[WHITE]);
    
    // std::cout << "PSQT scores are :\t"<<score.mg()<<"\t"<<score.eg()<<std::endl;
    score += pos->psqtScores[indexColorSide(WHITE,(fileOf(whiteKing) <= 3))] + pos->psqtScores[indexColorSide(BLACK,(fileOf(blackKing) <= 3))];

    // Weak pieces
    const BitBoard weakPieces[2] = {
        occ[WHITE] & ~pawnAttackedSquares[WHITE] & ((attackedBy[BLACK] & ~attackedBy[WHITE]) | (multiAttacks[BLACK] & ~multiAttacks[WHITE])),
        occ[BLACK] & ~pawnAttackedSquares[BLACK] & ((attackedBy[WHITE] & ~attackedBy[BLACK]) | (multiAttacks[WHITE] & ~multiAttacks[BLACK]))
    };

    // Hanging enemies
    const BitBoard hanging[2] = {
        weakPieces[WHITE] & (~attackedBy[WHITE] | (nonPawns[WHITE] & multiAttacks[BLACK])),
        weakPieces[BLACK] & (~attackedBy[BLACK] | (nonPawns[BLACK] & multiAttacks[WHITE]))
    };

    // King Threats (eg op-ness hopefully) (also, cap to 1)
    const BitBoard kingThreats[2] = {
        weakPieces[BLACK] & kingAttacks[WHITE],
        weakPieces[WHITE] & kingAttacks[BLACK]
    };

    // Malus for directly doubled, undefended pawns
    const BitBoard protectedPawns[2] = {
        bb[P] & pawnAttackedSquares[WHITE],
        bb[p] & pawnAttackedSquares[BLACK]
    };

    const BitBoard pawnBlockage[2] = {
        bb[p] | bb[P] | pawnAttackedSquares[BLACK],
        bb[P] | bb[p] | pawnAttackedSquares[WHITE]
    };

    S32 passedCount = 0;

    BitBoard doubledPawns[2] = { bb[P] & (bb[P] << 8), bb[p] & (bb[p] >> 8) };
    BitBoard pawnFiles[2] = { filesFromBB(bb[P]), filesFromBB(bb[p]) };

    score += pawnEval(pawnHashKey, bb, doubledPawns, pawnFiles, protectedPawns, pawnBlockage, occ, attackedBy, multiAttacks, pawnAttackedSquares, passedCount);
    
    // Calculate king safety
    // King shield. The inner shield is direcly in front of the king so it should be at least supported by the king itself
    BitBoard innerShelters[2] = {
        kingShelter[WHITE][whiteKing],
        kingShelter[BLACK][blackKing]
    };
    // The outer shield is the squares in front of the inner shield. We only consider supported squares
    BitBoard outerShelters[2] = {
        north(innerShelters[WHITE]) & pawnAttackedSquares[WHITE],
        south(innerShelters[BLACK]) & pawnAttackedSquares[BLACK]
    };

    // Get pawns
    innerShelters[WHITE] &= bb[P];
    outerShelters[WHITE] &= bb[P];
    innerShelters[BLACK] &= bb[p];
    outerShelters[BLACK] &= bb[p];

    // Add the shelter bonus
    const Score innerShelterDiff = popcount(innerShelters[WHITE]) - popcount(innerShelters[BLACK]);
    const Score outerShelterDiff = popcount(outerShelters[WHITE]) - popcount(outerShelters[BLACK]);
    score += INNERSHELTER * innerShelterDiff;
    score += OUTERSHELTER * outerShelterDiff;

    // Add bonus for bishop pairs
    score += BISHOPPAIR * ((popcount(bb[B])>=2) - (popcount(bb[b])>=2));

    // Add bonus for rooks on open files
    const BitBoard openFiles = ~(pawnFiles[WHITE] | pawnFiles[BLACK]);
    const BitBoard semiOpenFor[2] = {
        ~pawnFiles[WHITE],
        ~pawnFiles[BLACK]
    };
    const Score openDiff = popcount(bb[R] & openFiles) - popcount(bb[r] & openFiles);
    const Score semiOpenDiff = popcount(bb[R] & semiOpenFor[WHITE]) - popcount(bb[r] & semiOpenFor[BLACK]);
    score += ROOKONOPENFILE * openDiff;
    score += ROOKONSEMIOPENFILE * semiOpenDiff;

    // Add outpost squares bonus for knights and bishops
    const Score extKnightOutpostDiff = popcount(outpostSquares[WHITE] & (files(1) | files(6)) & bb[N]) - popcount(outpostSquares[BLACK] & (files(1) | files(6)) & bb[n]);
    score += KNIGHTONEXTOUTPOST * extKnightOutpostDiff;
    const Score extBishopOutpostDiff = popcount(outpostSquares[WHITE] & (files(1) | files(6)) & bb[B]) - popcount(outpostSquares[BLACK] & (files(1) | files(6)) & bb[b]);
    score += BISHOPONEXTOUTPOST * extBishopOutpostDiff;
    const Score intKnightOutpostDiff = popcount(outpostSquares[WHITE] & (notFile(1) & notFile(6)) & bb[N]) - popcount(outpostSquares[BLACK] & (notFile(1) & notFile(6)) & bb[n]);
    score += KNIGHTONINTOUTPOST * intKnightOutpostDiff;
    const Score intBishopOutpostDiff = popcount(outpostSquares[WHITE] & (notFile(1) & notFile(6)) & bb[B]) - popcount(outpostSquares[BLACK] & (notFile(1) & notFile(6)) & bb[b]);
    score += BISHOPONINTOUTPOST * intBishopOutpostDiff;

    // Add bonus for bishop-pawn concordance
    const Score bishopPawnsDiff =
        popcount(bb[B] & squaresOfColor[WHITE]) * popcount(squaresOfColor[WHITE] & bb[P]) * (popcount(blockedPawns[WHITE] & centralFiles) + !(pawnAttackedSquares[WHITE] & squaresOfColor[WHITE] & bb[B])) -
        popcount(bb[b] & squaresOfColor[WHITE]) * popcount(squaresOfColor[WHITE] & bb[p]) * (popcount(blockedPawns[BLACK] & centralFiles) + !(pawnAttackedSquares[BLACK] & squaresOfColor[WHITE] & bb[b])) +
        popcount(bb[B] & squaresOfColor[BLACK]) * popcount(squaresOfColor[BLACK] & bb[P]) * (popcount(blockedPawns[WHITE] & centralFiles) + !(pawnAttackedSquares[WHITE] & squaresOfColor[BLACK] & bb[B])) -
        popcount(bb[b] & squaresOfColor[BLACK]) * popcount(squaresOfColor[BLACK] & bb[p]) * (popcount(blockedPawns[BLACK] & centralFiles) + !(pawnAttackedSquares[BLACK] & squaresOfColor[BLACK] & bb[b])) ;
    
    score += BISHOPPAWNS * bishopPawnsDiff;

    // Threats
    const BitBoard threatSafePawns[2] = {
        makePawnAttacks<WHITE>(bb[P] & (attackedBy[WHITE] | ~attackedBy[BLACK])) & nonPawns[BLACK],
        makePawnAttacks<BLACK>(bb[p] & (attackedBy[BLACK] | ~attackedBy[WHITE])) & nonPawns[WHITE],
    };

    BitBoard pushes[2] = {
        north(bb[P]) & ~(occ[BOTH] | pawnAttackedSquares[BLACK]),
        south(bb[p]) & ~(occ[BOTH] | pawnAttackedSquares[WHITE]),
    };
    pushes[WHITE] |= north(pushes[WHITE] & ranks(5)) & ~(occ[BOTH] | pawnAttackedSquares[BLACK]);
    pushes[BLACK] |= south(pushes[BLACK] & ranks(2)) & ~(occ[BOTH] | pawnAttackedSquares[WHITE]);

    const BitBoard threatPawnPush[2] = {
        makePawnAttacks<WHITE>(pushes[WHITE] & (attackedBy[WHITE] | ~attackedBy[BLACK])) & nonPawns[BLACK],
        makePawnAttacks<BLACK>(pushes[BLACK] & (attackedBy[BLACK] | ~attackedBy[WHITE])) & nonPawns[WHITE],
    };

    const Score safePawnThreatDiff = popcount(threatSafePawns[WHITE]) - popcount(threatSafePawns[BLACK]);
    const Score pawnPushThreatDiff = popcount(threatPawnPush[WHITE]) - popcount(threatPawnPush[BLACK]);

    score += THREATSAFEPAWN * safePawnThreatDiff;
    score += THREATPAWNPUSH * pawnPushThreatDiff;

    const Score pawnHangingDiff = popcount(hanging[WHITE] & bb[P]) - popcount(hanging[BLACK] & bb[p]);
    const Score nonPawnHangingDiff = popcount(hanging[WHITE] & nonPawns[WHITE]) - popcount(hanging[BLACK] & nonPawns[BLACK]);
    score += PAWNHANGING * pawnHangingDiff;
    score += NONPAWNHANGING * nonPawnHangingDiff;

    const Score kingThreatsDiff = (!!kingThreats[WHITE]) - (!!kingThreats[BLACK]);
    score += KINGTHREAT * kingThreatsDiff;

    // Queen infiltration
    const Score queenInfiltrationDiff = popcount(~pawnSpan[BLACK] & (ranks(0) | ranks(1) | ranks(2) | ranks(3)) & bb[Q]) - popcount(~pawnSpan[WHITE] & (ranks(7) | ranks(6) | ranks(5) | ranks(4)) & bb[q]);
    score += QUEENINFILTRATION * queenInfiltrationDiff;

    // Square restriction
    const Score restrictedSquaresDiff = popcount(multiAttacks[WHITE] & attackedBy[BLACK] & ~multiAttacks[BLACK]) - popcount(multiAttacks[BLACK] & attackedBy[WHITE] & ~multiAttacks[WHITE]);
    score += RESTRICTEDSQUARES * restrictedSquaresDiff;

    // Calculate safe checks
    const BitBoard weakSquares[2] = {
        attackedBy[BLACK] & (~attackedBy[WHITE] | ((kingAttacks[whiteKing] | ptAttacks[WHITE][Q-1]) & ~multiAttacks[WHITE])),
        attackedBy[WHITE] & (~attackedBy[BLACK] | ((kingAttacks[blackKing] | ptAttacks[BLACK][Q-1]) & ~multiAttacks[BLACK])),
    };

    const BitBoard checks[2][4] = {
        {
            ptAttacks[WHITE][N-1] & kingCheckers[BLACK][N-1],
            ptAttacks[WHITE][B-1] & kingCheckers[BLACK][B-1],
            ptAttacks[WHITE][R-1] & kingCheckers[BLACK][R-1],
            ptAttacks[WHITE][Q-1] & kingCheckers[BLACK][Q-1],
        },
        {
            ptAttacks[BLACK][N-1] & kingCheckers[WHITE][N-1],
            ptAttacks[BLACK][B-1] & kingCheckers[WHITE][B-1],
            ptAttacks[BLACK][R-1] & kingCheckers[WHITE][R-1],
            ptAttacks[BLACK][Q-1] & kingCheckers[WHITE][Q-1],
        }
    };

    BitBoard safeChecks[2][4] = {
        {
            checks[WHITE][N-1] & (~attackedBy[BLACK] | (weakSquares[BLACK] & multiAttacks[WHITE])),
            checks[WHITE][B-1] & (~attackedBy[BLACK] | (weakSquares[BLACK] & multiAttacks[WHITE])),
            checks[WHITE][R-1] & (~attackedBy[BLACK] | (weakSquares[BLACK] & multiAttacks[WHITE])),
            checks[WHITE][Q-1] & (~attackedBy[BLACK] | (weakSquares[BLACK] & multiAttacks[WHITE])),
        },
        {
            checks[BLACK][N-1] & (~attackedBy[WHITE] | (weakSquares[WHITE] & multiAttacks[BLACK])),
            checks[BLACK][B-1] & (~attackedBy[WHITE] | (weakSquares[WHITE] & multiAttacks[BLACK])),
            checks[BLACK][R-1] & (~attackedBy[WHITE] | (weakSquares[WHITE] & multiAttacks[BLACK])),
            checks[BLACK][Q-1] & (~attackedBy[WHITE] | (weakSquares[WHITE] & multiAttacks[BLACK])),
        }
    };

    safeChecks[WHITE][B-1] &= ~safeChecks[WHITE][Q-1];
    safeChecks[BLACK][B-1] &= ~safeChecks[BLACK][Q-1];

    safeChecks[WHITE][Q-1] &= ~safeChecks[WHITE][R-1];
    safeChecks[BLACK][Q-1] &= ~safeChecks[BLACK][R-1];
    
    // Add kingDanger index eval
    // Calcuate danger score
    innerAttacks[WHITE] += PAWNATTACKINNERRING * popcount(pawnAttackedSquares[WHITE] & kingRing[BLACK]);
    outerAttacks[WHITE] += PAWNATTACKOUTERRING * popcount(pawnAttackedSquares[WHITE] & kingOuter[BLACK]);
    innerAttacks[BLACK] += PAWNATTACKINNERRING * popcount(pawnAttackedSquares[BLACK] & kingRing[WHITE]);
    outerAttacks[BLACK] += PAWNATTACKOUTERRING * popcount(pawnAttackedSquares[BLACK] & kingOuter[WHITE]);

    PScore dangerIndex[2] = {
        innerAttacks[WHITE] + outerAttacks[WHITE],
        innerAttacks[BLACK] + outerAttacks[BLACK]
    };

    dangerIndex[WHITE] += NOQUEENDANGER * (!bb[Q]);
    dangerIndex[BLACK] += NOQUEENDANGER * (!bb[q]);

    dangerIndex[WHITE] += PINNEDSHELTERDANGER * popcount(pinned[BLACK] & innerShelters[BLACK]);
    dangerIndex[BLACK] += PINNEDSHELTERDANGER * popcount(pinned[WHITE] & innerShelters[WHITE]);

    // Safe checks
    dangerIndex[WHITE] += SAFECHECK[N-1] * popcount(safeChecks[WHITE][N-1]);
    dangerIndex[WHITE] += SAFECHECK[B-1] * popcount(safeChecks[WHITE][B-1]);
    dangerIndex[WHITE] += SAFECHECK[R-1] * popcount(safeChecks[WHITE][R-1]);
    dangerIndex[WHITE] += SAFECHECK[Q-1] * popcount(safeChecks[WHITE][Q-1]);
    dangerIndex[BLACK] += SAFECHECK[N-1] * popcount(safeChecks[BLACK][N-1]);
    dangerIndex[BLACK] += SAFECHECK[B-1] * popcount(safeChecks[BLACK][B-1]);
    dangerIndex[BLACK] += SAFECHECK[R-1] * popcount(safeChecks[BLACK][R-1]);
    dangerIndex[BLACK] += SAFECHECK[Q-1] * popcount(safeChecks[BLACK][Q-1]);

    // Ring weakness attacks
    dangerIndex[WHITE] += INNERWEAKNESS * popcount(kingRing[BLACK] & weakSquares[BLACK]);
    dangerIndex[WHITE] += OUTERWEAKNESS * popcount(outerRing[BLACK] & weakSquares[BLACK]);
    dangerIndex[BLACK] += INNERWEAKNESS * popcount(kingRing[WHITE] & weakSquares[WHITE]);
    dangerIndex[BLACK] += OUTERWEAKNESS * popcount(outerRing[WHITE] & weakSquares[WHITE]);

    // All checks
    dangerIndex[WHITE] += ALLCHECKS[N-1] * popcount(checks[WHITE][N-1]);
    dangerIndex[WHITE] += ALLCHECKS[B-1] * popcount(checks[WHITE][B-1]);
    dangerIndex[WHITE] += ALLCHECKS[R-1] * popcount(checks[WHITE][R-1]);
    dangerIndex[WHITE] += ALLCHECKS[Q-1] * popcount(checks[WHITE][Q-1]);
    dangerIndex[BLACK] += ALLCHECKS[N-1] * popcount(checks[BLACK][N-1]);
    dangerIndex[BLACK] += ALLCHECKS[B-1] * popcount(checks[BLACK][B-1]);
    dangerIndex[BLACK] += ALLCHECKS[R-1] * popcount(checks[BLACK][R-1]);
    dangerIndex[BLACK] += ALLCHECKS[Q-1] * popcount(checks[BLACK][Q-1]);

    dangerIndex[WHITE] += SAFETYINNERSHELTER * popcount(innerShelters[BLACK]);
    dangerIndex[WHITE] += SAFETYOUTERSHELTER * popcount(outerShelters[BLACK]);
    dangerIndex[BLACK] += SAFETYINNERSHELTER * popcount(innerShelters[WHITE]);
    dangerIndex[BLACK] += SAFETYOUTERSHELTER * popcount(outerShelters[WHITE]);
    const S32 sign = 1 - 2 * pos->side;
    dangerIndex[pos->side] += KSTEMPO;


    const S32 mgWhiteDanger = getKingSafetyMg(dangerIndex[WHITE].mg());
    const S32 egWhiteDanger = getKingSafetyEg(dangerIndex[WHITE].eg());
    const S32 mgBlackDanger = getKingSafetyMg(dangerIndex[BLACK].mg());
    const S32 egBlackDanger = getKingSafetyEg(dangerIndex[BLACK].eg());
    
    const PScore safety = PScore(mgWhiteDanger - mgBlackDanger, egWhiteDanger - egBlackDanger);
    score += safety;

    // Calculate mg and eg scores
    score += TEMPO * sign;

    S32 mgScore = score.mg();
    S32 egScore = score.eg();

    // Complexity adjustment, so we avoid going into drawish barely better endgames
    const S32 outflanking = std::abs(fileOf(whiteKing)- fileOf(blackKing)) - std::abs(rankOf(whiteKing)- rankOf(blackKing));
    const S32 blockedPairs = popcount(north(bb[P]) & bb[p]) * 2;
    const S32 pawnTension = popcount(pawnAttackedSquares[WHITE] & bb[p]) + popcount(pawnAttackedSquares[BLACK] & bb[P]);
    const BitBoard pawns = bb[P] | bb[p];
    const bool pawnsOnBothFlanks = (boardSide[0] & pawns) && (boardSide[1] & pawns);
    const bool almostUnwinnable = outflanking < 0 && !pawnsOnBothFlanks;
    const bool infiltration = rankOf(whiteKing) <= 3 || rankOf(blackKing) >= 4;
    const Score complexity = (COMPLEXITYPASSERS * passedCount
                        +  COMPLEXITYPAWNS * popcount(pawns)
                        +  COMPLEXITYBLOCKEDPAIRS * blockedPairs
                        +  COMPLEXITYPAWNTENSION * pawnTension
                        +  COMPLEXITYOUTFLANKING * outflanking
                        +  COMPLEXITYINFILTRATION * infiltration
                        +  COMPLEXITYPAWNBOTHFLANKS * pawnsOnBothFlanks
                        +  COMPLEXITYPAWNENDING * !(nonPawns[WHITE] | nonPawns[BLACK])
                        +  COMPLEXITYALMOSTUNWINNABLE * almostUnwinnable
                        +  COMPLEXITYBIAS) / 100;
    
    const Score v = ((egScore > 0) - (egScore < 0)) * std::max(complexity, Score(-std::abs(egScore)));
    egScore += v;

    return Score(
        sign * 
        (
            mgScore * gamePhase +
            egScore * (24 - gamePhase)
        ) / 24
    );
}

std::vector<Score> getCurrentEvalWeights(){
    std::vector<Score> weights;
    // Add the material weights
    for (Piece piece = P; piece <= Q; piece++){
        weights.push_back(materialValues[piece].mg());
    }
    // PSQT weights handled later bc of king bucket
    // Add the mobility weights
    for (U8 mobcount = 0; mobcount < 9; mobcount++){
        weights.push_back(knightMob[mobcount].mg());
    }
    for (U8 mobcount = 0; mobcount < 14; mobcount++){
        weights.push_back(bishopMob[mobcount].mg());
    }
    for (U8 mobcount = 0; mobcount < 15; mobcount++){
        weights.push_back(rookMob[mobcount].mg());
    }
    for (U8 mobcount = 0; mobcount < 28; mobcount++){
        weights.push_back(queenMob[mobcount].mg());
    }
    // Add the pawn evaluation weights
    weights.push_back(DOUBLEISOLATEDPEN.mg());
    weights.push_back(ISOLATEDPEN.mg());
    weights.push_back(BACKWARDPEN.mg());
    weights.push_back(DOUBLEDPEN.mg());
    weights.push_back(SUPPORTEDPHALANX.mg());
    weights.push_back(R_SUPPORTEDPHALANX.mg());
    weights.push_back(ADVANCABLEPHALANX.mg());
    weights.push_back(R_ADVANCABLEPHALANX.mg());
    // Add the passed pawn bonus list
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(passedRankBonus[rank].mg());
    }
    weights.push_back(PASSEDPATHBONUS.mg());
    weights.push_back(SUPPORTEDPASSER.mg());

    // Add the shelter weights
    weights.push_back(INNERSHELTER.mg());
    weights.push_back(OUTERSHELTER.mg());
    
    // Add the bishop pair bonus
    weights.push_back(BISHOPPAIR.mg());

    // Add the rook and queen on open file bonus
    weights.push_back(ROOKONOPENFILE.mg());
    weights.push_back(ROOKONSEMIOPENFILE.mg());

    // Now, outpost
    weights.push_back(KNIGHTONEXTOUTPOST.mg());
    weights.push_back(BISHOPONEXTOUTPOST.mg());
    weights.push_back(KNIGHTONINTOUTPOST.mg());
    weights.push_back(BISHOPONINTOUTPOST.mg());

    // Now, king protector
    weights.push_back(KNIGHTPROTECTOR.mg());
    weights.push_back(BISHOPPROTECTOR.mg());

    // Now, bishop pawns
    weights.push_back(BISHOPPAWNS.mg());

    // Now, threats
    weights.push_back(THREATSAFEPAWN.mg());
    weights.push_back(THREATPAWNPUSH.mg());
    weights.push_back(PAWNHANGING.mg());
    weights.push_back(NONPAWNHANGING.mg());
    weights.push_back(KINGTHREAT.mg());
    weights.push_back(QUEENINFILTRATION.mg());
    weights.push_back(RESTRICTEDSQUARES.mg());

    // Add the tempo bonus
    weights.push_back(TEMPO.mg());

    // Now, EG
    // Add the material weights
    for (Piece piece = P; piece <= Q; piece++){
        weights.push_back(materialValues[piece].eg());
    }
    // Same thing abt PSQTs
    // Add the mobility weights
    for (U8 mobcount = 0; mobcount < 9; mobcount++){
        weights.push_back(knightMob[mobcount].eg());
    }
    for (U8 mobcount = 0; mobcount < 14; mobcount++){
        weights.push_back(bishopMob[mobcount].eg());
    }
    for (U8 mobcount = 0; mobcount < 15; mobcount++){
        weights.push_back(rookMob[mobcount].eg());
    }
    for (U8 mobcount = 0; mobcount < 28; mobcount++){
        weights.push_back(queenMob[mobcount].eg());
    }
    // Add the pawn evaluation weights
    weights.push_back(DOUBLEISOLATEDPEN.eg());
    weights.push_back(ISOLATEDPEN.eg());
    weights.push_back(BACKWARDPEN.eg());
    weights.push_back(DOUBLEDPEN.eg());
    weights.push_back(SUPPORTEDPHALANX.eg());
    weights.push_back(R_SUPPORTEDPHALANX.eg());
    weights.push_back(ADVANCABLEPHALANX.eg());
    weights.push_back(R_ADVANCABLEPHALANX.eg());
    // Add the passed pawn bonus list
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(passedRankBonus[rank].eg());
    }
    weights.push_back(PASSEDPATHBONUS.eg());
    weights.push_back(SUPPORTEDPASSER.eg());

    // Add the shelter weights
    weights.push_back(INNERSHELTER.eg());
    weights.push_back(OUTERSHELTER.eg());
    
    // Add the bishop pair bonus
    weights.push_back(BISHOPPAIR.eg());

    // Add the rook and queen on open file bonus
    weights.push_back(ROOKONOPENFILE.eg());
    weights.push_back(ROOKONSEMIOPENFILE.eg());

    // Now, outpost
    weights.push_back(KNIGHTONEXTOUTPOST.eg());
    weights.push_back(BISHOPONEXTOUTPOST.eg());
    weights.push_back(KNIGHTONINTOUTPOST.eg());
    weights.push_back(BISHOPONINTOUTPOST.eg());

    // Now, king protector
    weights.push_back(KNIGHTPROTECTOR.eg());
    weights.push_back(BISHOPPROTECTOR.eg());

    // Now, bishop pawns
    weights.push_back(BISHOPPAWNS.eg());

    // Now, threats
    weights.push_back(THREATSAFEPAWN.eg());
    weights.push_back(THREATPAWNPUSH.eg());
    weights.push_back(PAWNHANGING.eg());
    weights.push_back(NONPAWNHANGING.eg());
    weights.push_back(KINGTHREAT.eg());
    weights.push_back(QUEENINFILTRATION.eg());
    weights.push_back(RESTRICTEDSQUARES.eg());

    // Add the tempo bonus
    weights.push_back(TEMPO.eg());

    return weights;
}
    
// Returns the evaluation features tensor
// This will be used to tune the evaluation function
void getEvalFeaturesTensor(Position *pos, S8* tensor){

    auto const& bb = pos->bitboards;
    auto const& occ = pos->occupancies;
    auto us = pos->side;
    Square whiteKing = lsb(bb[K]);
    Square blackKing = lsb(bb[k]);
    bool wkhside = fileOf(whiteKing) <= 3;
    bool bkhside = fileOf(blackKing) <= 3;
    

    // Calculate the game phase
    S32 gamePhase = gamephaseInc[P] * popcount(bb[P] | bb[p]) +
                    gamephaseInc[N] * popcount(bb[N] | bb[n]) +
                    gamephaseInc[B] * popcount(bb[B] | bb[b]) +
                    gamephaseInc[R] * popcount(bb[R] | bb[r]) +
                    gamephaseInc[Q] * popcount(bb[Q] | bb[q]) +
                    gamephaseInc[K] * popcount(bb[K] | bb[k]);
    
    gamePhase = std::min(gamePhase, (S32)24); // If we have a lot of pieces, we don't want to go over 24
    // Add the game phase
    tensor[0] = gamePhase;
    ++tensor;

    // Add the psqt weights. Only add them once, no need to bloat the data.
    for (Square square = a8; square < noSquare; square++){
        Piece piece = pos->pieceOn(square);
        if (piece == NOPIECE) continue;
        Square sq = piece < p ? square : flipSquare(square);
        bool pieceColor = piece >= p;
        tensor[64 * (piece % 6) + sq] += (pieceColor ? 0b0100 : 0b0001) << (pieceColor ? bkhside : wkhside);
    }
    tensor += 64 * 6;

    // Add the material weights
    for (Piece piece = P; piece <= Q; piece++){
        tensor[piece] = popcount(pos->bitboards[piece]) - popcount(pos->bitboards[piece + 6]);
    }
    tensor += 5;

    // Pin, mobility and threat calculations
    BitBoard attackedBy[2] = {
        kingAttacks[whiteKing],
        kingAttacks[blackKing]
    };

    BitBoard pawnAttackedSquares[2] = {
        0,
        0
    };


    BitBoard multiAttacks[2] = {
        0ULL,
        0ULL
    };

    BitBoard doublePawnAttackedSquares[2] = {
        0ULL,
        0ULL
    };
    
    {
        // Left Attacks
        BitBoard sidePawnAttacks[2] = {
            ((bb[P] & notFile(0)) >> 9),
            ((bb[p] & notFile(0)) << 7)
        }; 
        doublePawnAttackedSquares[WHITE] = sidePawnAttacks[WHITE];
        doublePawnAttackedSquares[BLACK] = sidePawnAttacks[BLACK];
        // Update Multi attacks
        multiAttacks[WHITE] |= attackedBy[WHITE] & sidePawnAttacks[WHITE];
        multiAttacks[BLACK] |= attackedBy[BLACK] & sidePawnAttacks[BLACK];
        // Update Normal attacks
        attackedBy[WHITE] |= sidePawnAttacks[WHITE];
        attackedBy[BLACK] |= sidePawnAttacks[BLACK];
        // Update exclusive pawn attacks
        pawnAttackedSquares[WHITE] |= sidePawnAttacks[WHITE];
        pawnAttackedSquares[BLACK] |= sidePawnAttacks[BLACK];
        // Right Attacks
        sidePawnAttacks[WHITE] = ((bb[P] & notFile(7)) >> 7); 
        sidePawnAttacks[BLACK] = ((bb[p] & notFile(7)) << 9);
        doublePawnAttackedSquares[WHITE] &= sidePawnAttacks[WHITE];
        doublePawnAttackedSquares[BLACK] &= sidePawnAttacks[BLACK];
        // Update multi attacks
        multiAttacks[WHITE] |= attackedBy[WHITE] & sidePawnAttacks[WHITE];
        multiAttacks[BLACK] |= attackedBy[BLACK] & sidePawnAttacks[BLACK];
        // Update Normal attacks
        attackedBy[WHITE] |= sidePawnAttacks[WHITE];
        attackedBy[BLACK] |= sidePawnAttacks[BLACK];
        // Update exclusive pawn attacks
        pawnAttackedSquares[WHITE] |= sidePawnAttacks[WHITE];
        pawnAttackedSquares[BLACK] |= sidePawnAttacks[BLACK];
    }

    // Pinned mask
    BitBoard RQmask[2] = {
        bb[R] | bb[Q],
        bb[r] | bb[q]
    };
    BitBoard BQmask[2] = {
        bb[B] | bb[Q],
        bb[b] | bb[q]
    };
    BitBoard pinned[2] = {
        getPinnedPieces(occ[BOTH], occ[WHITE], whiteKing, RQmask[BLACK], BQmask[BLACK]),
        getPinnedPieces(occ[BOTH], occ[BLACK], blackKing, RQmask[WHITE], BQmask[WHITE])
    };

    BitBoard blockedPawns[2] = {
        bb[P] & (occ[BOTH] >> 8),
        bb[p] & (occ[BOTH] << 8)
    };

    BitBoard underDevelopedPawns[2] = {
        bb[P] & (ranks(6) | ranks(5)),
        bb[p] & (ranks(1) | ranks(2))
    };

    BitBoard mobilityArea[2] = {
        // Mobility area.
        // Squares in the mobility area are:
        // 1. Free of our pieces
        // 2. Not defended by enemy pawns

        // Calculate a negative mask for the mobility area, and then invert it
        (blockedPawns[WHITE] | underDevelopedPawns[WHITE] | pawnAttackedSquares[BLACK] | pinned[WHITE] | bb[K] | bb[Q]) ^ 0xFFFFFFFFFFFFFFFF,
        (blockedPawns[BLACK] | underDevelopedPawns[BLACK] | pawnAttackedSquares[WHITE] | pinned[BLACK] | bb[k] | bb[q]) ^ 0xFFFFFFFFFFFFFFFF
    };
    
    BitBoard nonPawns[2] = {
        bb[N] | bb[B] | bb[R] | bb[Q],
        bb[n] | bb[b] | bb[r] | bb[q],
    };

    const BitBoard kingRing[2] = {
        kingAttacks[whiteKing] & ~doublePawnAttackedSquares[WHITE],
        kingAttacks[blackKing] & ~doublePawnAttackedSquares[BLACK]
    };

    const BitBoard kingOuter[2] = {
        fiveSquare[whiteKing] & ~doublePawnAttackedSquares[WHITE],
        fiveSquare[blackKing] & ~doublePawnAttackedSquares[BLACK]
    };

    S32 innerAttacks[2][5] = { {0} }; 
    S32 outerAttacks[2][5] = { {0} };
    S32 kingDist[2] = {0};

    BitBoard kingCheckers[2][4] = {
        {
            knightAttacks[whiteKing],
            getBishopAttack(whiteKing, occ[BOTH]),
            getRookAttack(whiteKing, occ[BOTH]),
            0
        },
        {
            knightAttacks[blackKing],
            getBishopAttack(blackKing, occ[BOTH]),
            getRookAttack(blackKing, occ[BOTH]),
            0
        }
    };
    kingCheckers[WHITE][Q-1] = kingCheckers[WHITE][B-1] | kingCheckers[WHITE][R-1];
    kingCheckers[BLACK][Q-1] = kingCheckers[BLACK][B-1] | kingCheckers[BLACK][R-1];

    BitBoard ptAttacks[2][4] = { {0} };

    innerAttacks[WHITE][P] = popcount(pawnAttackedSquares[WHITE] & kingRing[BLACK]);
    innerAttacks[BLACK][P] = popcount(pawnAttackedSquares[BLACK] & kingRing[WHITE]);
    outerAttacks[WHITE][P] = popcount(pawnAttackedSquares[WHITE] & kingOuter[BLACK]);
    outerAttacks[BLACK][P] = popcount(pawnAttackedSquares[BLACK] & kingOuter[WHITE]);

    // Calculate the mobility scores (index by phase and color)
    getMobilityFeat<N>(bb, occ[BOTH], pinned[WHITE], mobilityArea[WHITE], attackedBy[WHITE], multiAttacks[WHITE], ptAttacks[WHITE][N-1], tensor, whiteKing, kingRing[BLACK], kingOuter[BLACK], innerAttacks[WHITE][N], outerAttacks[WHITE][N], kingDist[0]);
    getMobilityFeat<n>(bb, occ[BOTH], pinned[BLACK], mobilityArea[BLACK], attackedBy[BLACK], multiAttacks[BLACK], ptAttacks[BLACK][N-1], tensor, blackKing, kingRing[WHITE], kingOuter[WHITE], innerAttacks[BLACK][N], outerAttacks[BLACK][N], kingDist[0]);
    tensor += 9;
    getMobilityFeat<B>(bb, occ[BOTH], pinned[WHITE], mobilityArea[WHITE], attackedBy[WHITE], multiAttacks[WHITE], ptAttacks[WHITE][B-1], tensor, whiteKing, kingRing[BLACK], kingOuter[BLACK], innerAttacks[WHITE][B], outerAttacks[WHITE][B], kingDist[1]);
    getMobilityFeat<b>(bb, occ[BOTH], pinned[BLACK], mobilityArea[BLACK], attackedBy[BLACK], multiAttacks[BLACK], ptAttacks[BLACK][B-1], tensor, blackKing, kingRing[WHITE], kingOuter[WHITE], innerAttacks[BLACK][B], outerAttacks[BLACK][B], kingDist[1]);
    tensor += 14;
    getMobilityFeat<R>(bb, occ[BOTH], pinned[WHITE], mobilityArea[WHITE], attackedBy[WHITE], multiAttacks[WHITE], ptAttacks[WHITE][R-1], tensor, whiteKing, kingRing[BLACK], kingOuter[BLACK], innerAttacks[WHITE][R], outerAttacks[WHITE][R], kingDist[0]);
    getMobilityFeat<r>(bb, occ[BOTH], pinned[BLACK], mobilityArea[BLACK], attackedBy[BLACK], multiAttacks[BLACK], ptAttacks[BLACK][R-1], tensor, blackKing, kingRing[WHITE], kingOuter[WHITE], innerAttacks[BLACK][R], outerAttacks[BLACK][R], kingDist[0]);
    tensor += 15;
    getMobilityFeat<Q>(bb, occ[BOTH], pinned[WHITE], mobilityArea[WHITE], attackedBy[WHITE], multiAttacks[WHITE], ptAttacks[WHITE][Q-1], tensor, whiteKing, kingRing[BLACK], kingOuter[BLACK], innerAttacks[WHITE][Q], outerAttacks[WHITE][Q], kingDist[0]);
    getMobilityFeat<q>(bb, occ[BOTH], pinned[BLACK], mobilityArea[BLACK], attackedBy[BLACK], multiAttacks[BLACK], ptAttacks[BLACK][Q-1], tensor, blackKing, kingRing[WHITE], kingOuter[WHITE], innerAttacks[BLACK][Q], outerAttacks[BLACK][Q], kingDist[0]);
    tensor += 28;

    // Weak pieces
    BitBoard weakPieces[2] = {
        occ[WHITE] & ~pawnAttackedSquares[WHITE] & ((attackedBy[BLACK] & ~attackedBy[WHITE]) | (multiAttacks[BLACK] & ~multiAttacks[WHITE])),
        occ[BLACK] & ~pawnAttackedSquares[BLACK] & ((attackedBy[WHITE] & ~attackedBy[BLACK]) | (multiAttacks[WHITE] & ~multiAttacks[BLACK]))
    };

    // Hanging enemies
    BitBoard hanging[2] = {
        weakPieces[WHITE] & (~attackedBy[WHITE] | (nonPawns[WHITE] & multiAttacks[BLACK])),
        weakPieces[BLACK] & (~attackedBy[BLACK] | (nonPawns[BLACK] & multiAttacks[WHITE]))
    };

    // King Threats (eg op-ness hopefully) (also, cap to 1)
    BitBoard kingThreats[2] = {
        weakPieces[BLACK] & kingAttacks[WHITE],
        weakPieces[WHITE] & kingAttacks[BLACK]
    };

    // Add the pawn evaluation weights
    // Malus for directly doubled, undefended pawns
    BitBoard protectedPawns[2] = {
        bb[P] & pawnAttackedSquares[WHITE],
        bb[p] & pawnAttackedSquares[BLACK]
    };

    BitBoard pawnBlockage[2] = {
        bb[p] | bb[P] | pawnAttackedSquares[BLACK],
        bb[P] | bb[p] | pawnAttackedSquares[WHITE]
    };

    S32 passedCount = 0;

    BitBoard doubledPawns[2] = { bb[P] & (bb[P] << 8), bb[p] & (bb[p] >> 8) };
    BitBoard pawnFiles[2] = { filesFromBB(bb[P]), filesFromBB(bb[p]) };
    // White pawns
    BitBoard pieces = bb[P];
    BitBoard block = occ[BOTH] | (~pawnAttackedSquares[WHITE] & ((~attackedBy[WHITE] & attackedBy[BLACK]) | (multiAttacks[BLACK])));
    while (pieces){
        // Evaluate isolated, backward, doubled pawns and connected pawns
        Square sq = popLsb(pieces);
        BitBoard sqb = squareBB(sq);
        U8 rank = 7 - rankOf(sq);
        bool doubled = doubledPawns[WHITE] & squareBB(sq);
        bool isolated = !(isolatedPawnMask[sq] & bb[P]);
        bool pawnOpposed = !(pawnFiles[BLACK] & sqb);
        bool supported = protectedPawns[WHITE] & sqb;
        bool advancable = !(pawnBlockage[WHITE] & north(sqb));
        bool phal = phalanx[sq] & bb[P];
        bool candidatePassed = !(passedPawnMask[WHITE][sq] & bb[p]);
        
        bool backward = !( // If the pawn
            (backwardPawnMask[WHITE][sq] & bb[P]) // If the pawn is doesn't have behind or adjacent pawns in adjacent files
            || advancable // If the pawn can't advance (blocked by enemy pawn or enemy pawn capture square)
        );
        // Check if the pawn is doubled
        if (doubled && isolated && pawnOpposed) tensor[0]--;

        else if (isolated) tensor[1]--;
        else if (backward) tensor[2]--;
        if (doubled && !supported) tensor[3]--;
        else if (supported && phal) {
            tensor[4]++;
            tensor[5] += rank - 2;
        }
        else if (advancable && phal) {
            tensor[6]++;
            tensor[7] += rank - 2;
        }
        if (candidatePassed){
            ++passedCount;
            BitBoard passedPath = advancePathMasked<WHITE>(sqb, ~block);
            // Give bonus for how close the pawn is to the promotion square
            tensor[8 + rank] += 1;
            tensor[8 + 7] += popcount(passedPath);
            // Bonus for connected or supported passed pawns
            if (supported) tensor[8 + 7 + 1]++;
        }
    }

    // Black pawns
    pieces = bb[p];
    block = occ[BOTH] | (~pawnAttackedSquares[BLACK] & ((~attackedBy[BLACK] & attackedBy[WHITE]) | (multiAttacks[WHITE])));
    while (pieces){
        // Evaluate isolated, backward, doubled pawns and connected pawns
        Square sq = popLsb(pieces);
        BitBoard sqb = squareBB(sq);
        U8 rank = rankOf(sq);
        bool doubled = doubledPawns[BLACK] & squareBB(sq);
        bool isolated = !(isolatedPawnMask[sq] & bb[p]);
        bool pawnOpposed = !(pawnFiles[WHITE] & sqb);
        bool supported = protectedPawns[BLACK] & sqb;
        bool advancable = !(pawnBlockage[BLACK] & south(sqb));
        bool phal = phalanx[sq] & bb[p];
        bool candidatePassed = !(passedPawnMask[BLACK][sq] & bb[P]);
        
        bool backward = !( // If the pawn
            (backwardPawnMask[BLACK][sq] & bb[p]) // If the pawn is doesn't have behind or adjacent pawns in adjacent files
            || advancable // If the pawn can't advance (blocked by enemy pawn or enemy pawn capture square)
        );
        // Check if the pawn is doubled
        if (doubled && isolated && pawnOpposed) tensor[0]++;
        else if (isolated) tensor[1]++;
        else if (backward) tensor[2]++;
        if (doubled && !supported) tensor[3]++;
        else if (supported && phal) {
            tensor[4]--;
            tensor[5] -= rank - 2;
        }
        else if (advancable && phal) {
            tensor[6]--;
            tensor[7] -= rank - 2;
        }
        if (candidatePassed){
            ++passedCount;
            BitBoard passedPath = advancePathMasked<BLACK>(sqb, ~block);
            // Give bonus for how close the pawn is to the promotion square
            tensor[8 + rank]--;
            tensor[8 + 7] -= popcount(passedPath);
            // Bonus for connected or supported passed pawns
            if (supported) tensor[8 + 7 + 1]--;
        }
    }
    tensor += 8 + 7 + 2;

    // Calculate king safety
    // King shield. The inner shield is direcly in front of the king so it should be at least supported by the king itself
    BitBoard innerShelters[2] = {
        kingShelter[WHITE][whiteKing],
        kingShelter[BLACK][blackKing]
    };
    // The outer shield is the squares in front of the inner shield. We only consider supported squares
    BitBoard outerShelters[2] = {
        north(innerShelters[WHITE]) & pawnAttackedSquares[WHITE],
        south(innerShelters[BLACK]) & pawnAttackedSquares[BLACK]
    };

    // Get pawns
    innerShelters[WHITE] &= bb[P];
    innerShelters[BLACK] &= bb[p];
    outerShelters[WHITE] &= bb[P];
    outerShelters[BLACK] &= bb[p];

    // Add the shelter bonus
    tensor[0] += popcount(innerShelters[WHITE]) - popcount(innerShelters[BLACK]);
    tensor[1] += popcount(outerShelters[WHITE]) - popcount(outerShelters[BLACK]);
    tensor += 2;

    // Add the bishop pairs
    tensor[0] += (popcount(bb[B])>=2) - (popcount(bb[b])>=2);
    tensor++;

    // Add the rooks on open files
    BitBoard openFiles = ~(pawnFiles[WHITE] | pawnFiles[BLACK]);
    BitBoard semiOpenFor[2] = {
        ~pawnFiles[WHITE],
        ~pawnFiles[BLACK]
    };
    tensor[0] += (popcount(bb[R] & openFiles) - popcount(bb[r] & openFiles));
    tensor[1] += (popcount(bb[R] & semiOpenFor[WHITE]) - popcount(bb[r] & semiOpenFor[BLACK]));
    tensor += 2;

    // Outpost calculations
    BitBoard pawnSpan[2] = {
        advancePathMasked<WHITE>(bb[P], ~(bb[p] | pawnAttackedSquares[BLACK])),
        advancePathMasked<BLACK>(bb[p], ~(bb[P] | pawnAttackedSquares[WHITE]))
    };

    BitBoard outpostSquares[2] = {
        (ranks(2) | ranks(3) | ranks(4)) & pawnAttackedSquares[WHITE] & ~(makePawnAttacks<BLACK>(pawnSpan[BLACK]) | bb[p]) & notFile(0) & notFile(7),
        (ranks(5) | ranks(4) | ranks(3)) & pawnAttackedSquares[BLACK] & ~(makePawnAttacks<WHITE>(pawnSpan[WHITE]) | bb[P]) & notFile(0) & notFile(7)
    };

    Score extKnightOutpostDiff = popcount(outpostSquares[WHITE] & (files(1) | files(6)) & bb[N]) - popcount(outpostSquares[BLACK] & (files(1) | files(6)) & bb[n]);
    tensor[0] += extKnightOutpostDiff;
    Score extBishopOutpostDiff = popcount(outpostSquares[WHITE] & (files(1) | files(6)) & bb[B]) - popcount(outpostSquares[BLACK] & (files(1) | files(6)) & bb[b]);
    tensor[1] += extBishopOutpostDiff;
    Score intKnightOutpostDiff = popcount(outpostSquares[WHITE] & (notFile(1) & notFile(6)) & bb[N]) - popcount(outpostSquares[BLACK] & (notFile(1) & notFile(6)) & bb[n]);
    tensor[2] += intKnightOutpostDiff;
    Score intBishopOutpostDiff = popcount(outpostSquares[WHITE] & (notFile(1) & notFile(6)) & bb[B]) - popcount(outpostSquares[BLACK] & (notFile(1) & notFile(6)) & bb[b]);
    tensor[3] += intBishopOutpostDiff;
    tensor += 4;

    tensor[0] = kingDist[0];
    tensor[1] = kingDist[1];
    tensor += 2;

    // Add bonus for bishop-pawn concordance (technically broken in two same colored bishop cases, but kek)
    Score bishopPawnsDiff =
        popcount(bb[B] & squaresOfColor[WHITE]) * popcount(squaresOfColor[WHITE] & bb[P]) * (popcount(blockedPawns[WHITE] & centralFiles) + !(pawnAttackedSquares[WHITE] & squaresOfColor[WHITE] & bb[B])) -
        popcount(bb[b] & squaresOfColor[WHITE]) * popcount(squaresOfColor[WHITE] & bb[p]) * (popcount(blockedPawns[BLACK] & centralFiles) + !(pawnAttackedSquares[BLACK] & squaresOfColor[WHITE] & bb[b])) +
        popcount(bb[B] & squaresOfColor[BLACK]) * popcount(squaresOfColor[BLACK] & bb[P]) * (popcount(blockedPawns[WHITE] & centralFiles) + !(pawnAttackedSquares[WHITE] & squaresOfColor[BLACK] & bb[B])) -
        popcount(bb[b] & squaresOfColor[BLACK]) * popcount(squaresOfColor[BLACK] & bb[p]) * (popcount(blockedPawns[BLACK] & centralFiles) + !(pawnAttackedSquares[BLACK] & squaresOfColor[BLACK] & bb[b])) ;
    
    tensor[0] += bishopPawnsDiff;

    tensor++;

    // Threats
    BitBoard threatSafePawns[2] = {
        makePawnAttacks<WHITE>(bb[P] & (attackedBy[WHITE] | ~attackedBy[BLACK])) & nonPawns[BLACK],
        makePawnAttacks<BLACK>(bb[p] & (attackedBy[BLACK] | ~attackedBy[WHITE])) & nonPawns[WHITE],
    };

    BitBoard pushes[2] = {
        north(bb[P]) & ~(occ[BOTH] | pawnAttackedSquares[BLACK]),
        south(bb[p]) & ~(occ[BOTH] | pawnAttackedSquares[WHITE]),
    };
    pushes[WHITE] |= north(pushes[WHITE] & ranks(5)) & ~(occ[BOTH] | pawnAttackedSquares[BLACK]);
    pushes[BLACK] |= south(pushes[BLACK] & ranks(2)) & ~(occ[BOTH] | pawnAttackedSquares[WHITE]);

    BitBoard threatPawnPush[2] = {
        makePawnAttacks<WHITE>(pushes[WHITE] & (attackedBy[WHITE] | ~attackedBy[BLACK])) & nonPawns[BLACK],
        makePawnAttacks<BLACK>(pushes[BLACK] & (attackedBy[BLACK] | ~attackedBy[WHITE])) & nonPawns[WHITE],
    };

    Score safePawnThreatDiff = popcount(threatSafePawns[WHITE]) - popcount(threatSafePawns[BLACK]);
    Score pawnPushThreatDiff = popcount(threatPawnPush[WHITE]) - popcount(threatPawnPush[BLACK]);

    tensor[0] = safePawnThreatDiff;
    tensor[1] = pawnPushThreatDiff;
    tensor += 2;

    const Score pawnHangingDiff = popcount(hanging[WHITE] & bb[P]) - popcount(hanging[BLACK] & bb[p]);
    const Score nonPawnHangingDiff = popcount(hanging[WHITE] & nonPawns[WHITE]) - popcount(hanging[BLACK] & nonPawns[BLACK]);
    tensor[0] = pawnHangingDiff;
    tensor[1] = nonPawnHangingDiff;
    tensor += 2;

    const Score kingThreatsDiff = (!!kingThreats[WHITE]) - (!!kingThreats[BLACK]);
    tensor[0] = kingThreatsDiff;
    ++tensor;

    // Queen infiltration
    const Score queenInfiltrationDiff = popcount(~pawnSpan[BLACK] & (ranks(0) | ranks(1) | ranks(2) | ranks(3)) & bb[Q]) - popcount(~pawnSpan[WHITE] & (ranks(7) | ranks(6) | ranks(5) | ranks(4)) & bb[q]);
    tensor[0] = queenInfiltrationDiff;
    ++tensor;

    // Square restriction
    const Score restrictedSquaresDiff = popcount(multiAttacks[WHITE] & attackedBy[BLACK] & ~multiAttacks[BLACK]) - popcount(multiAttacks[BLACK] & attackedBy[WHITE] & ~multiAttacks[WHITE]);
    tensor[0] = restrictedSquaresDiff;
    ++tensor;

    // Add the tempo bonus
    tensor[0] += us == WHITE ? 1 : -1;
    tensor++;

    // Now, king index eval.
    Score noQueenDanger[2] = {!bb[Q], !bb[q]};
    S32 pinnedShelter[2] = {popcount(pinned[BLACK] & innerShelters[BLACK]), popcount(pinned[WHITE] & innerShelters[WHITE])};
    // Calculate safe checks
    const BitBoard weakSquares[2] = {
        attackedBy[BLACK] & (~attackedBy[WHITE] | ((kingAttacks[whiteKing] | ptAttacks[WHITE][Q-1]) & ~multiAttacks[WHITE])),
        attackedBy[WHITE] & (~attackedBy[BLACK] | ((kingAttacks[blackKing] | ptAttacks[BLACK][Q-1]) & ~multiAttacks[BLACK])),
    };
    
    const BitBoard checks[2][4] = {
        {
            ptAttacks[WHITE][N-1] & kingCheckers[BLACK][N-1],
            ptAttacks[WHITE][B-1] & kingCheckers[BLACK][B-1],
            ptAttacks[WHITE][R-1] & kingCheckers[BLACK][R-1],
            ptAttacks[WHITE][Q-1] & kingCheckers[BLACK][Q-1],
        },
        {
            ptAttacks[BLACK][N-1] & kingCheckers[WHITE][N-1],
            ptAttacks[BLACK][B-1] & kingCheckers[WHITE][B-1],
            ptAttacks[BLACK][R-1] & kingCheckers[WHITE][R-1],
            ptAttacks[BLACK][Q-1] & kingCheckers[WHITE][Q-1],
        }
    };

    BitBoard safeChecks[2][4] = {
        {
            checks[WHITE][N-1] & (~attackedBy[BLACK] | (weakSquares[BLACK] & multiAttacks[WHITE])),
            checks[WHITE][B-1] & (~attackedBy[BLACK] | (weakSquares[BLACK] & multiAttacks[WHITE])),
            checks[WHITE][R-1] & (~attackedBy[BLACK] | (weakSquares[BLACK] & multiAttacks[WHITE])),
            checks[WHITE][Q-1] & (~attackedBy[BLACK] | (weakSquares[BLACK] & multiAttacks[WHITE])),
        },
        {
            checks[BLACK][N-1] & (~attackedBy[WHITE] | (weakSquares[WHITE] & multiAttacks[BLACK])),
            checks[BLACK][B-1] & (~attackedBy[WHITE] | (weakSquares[WHITE] & multiAttacks[BLACK])),
            checks[BLACK][R-1] & (~attackedBy[WHITE] | (weakSquares[WHITE] & multiAttacks[BLACK])),
            checks[BLACK][Q-1] & (~attackedBy[WHITE] | (weakSquares[WHITE] & multiAttacks[BLACK])),
        }
    };


    safeChecks[WHITE][B-1] &= ~safeChecks[WHITE][Q-1];
    safeChecks[BLACK][B-1] &= ~safeChecks[BLACK][Q-1];

    safeChecks[WHITE][Q-1] &= ~safeChecks[WHITE][R-1];
    safeChecks[BLACK][Q-1] &= ~safeChecks[BLACK][R-1];

#define COMPLEXITYFEATURES 9

    // Complexity adjustment, so we avoid going into drawish barely better endgames
    Score outflanking = std::abs(fileOf(whiteKing)- fileOf(blackKing)) - std::abs(rankOf(whiteKing)- rankOf(blackKing));
    Score blockedPairs = popcount(north(bb[P]) & bb[p]) * 2;
    Score pawnTension = popcount(pawnAttackedSquares[WHITE] & bb[p]) + popcount(pawnAttackedSquares[BLACK] & bb[P]);
    BitBoard pawns = bb[P] | bb[p];
    bool pawnsOnBothFlanks = (boardSide[0] & pawns) && (boardSide[1] & pawns);
    bool almostUnwinnable = outflanking < 0 && !pawnsOnBothFlanks;
    bool infiltration = rankOf(whiteKing) <= 3 || rankOf(blackKing) >= 4;
    
    tensor[0] = passedCount;
    tensor[1] = popcount(pawns);
    tensor[2] = blockedPairs;
    tensor[3] = pawnTension;
    tensor[4] = outflanking;
    tensor[5] = infiltration;
    tensor[6] = pawnsOnBothFlanks;
    tensor[7] = !(nonPawns[WHITE] | nonPawns[BLACK]);
    tensor[8] = almostUnwinnable;

    tensor += 9;


#define KINGSAFETYCOLOREDPARAMS 50
    tensor[P] = innerAttacks[WHITE][P];
    tensor[N] = innerAttacks[WHITE][N];
    tensor[B] = innerAttacks[WHITE][B];
    tensor[R] = innerAttacks[WHITE][R];
    tensor[Q] = innerAttacks[WHITE][Q];
    tensor += 5;
    tensor[P] = outerAttacks[WHITE][P];
    tensor[N] = outerAttacks[WHITE][N];
    tensor[B] = outerAttacks[WHITE][B];
    tensor[R] = outerAttacks[WHITE][R];
    tensor[Q] = outerAttacks[WHITE][Q];
    tensor += 5;
    tensor[0] = noQueenDanger[WHITE];
    tensor[1] = pinnedShelter[WHITE];
    tensor += 2;
    tensor[0] = popcount(safeChecks[WHITE][N-1]);
    tensor[1] = popcount(safeChecks[WHITE][B-1]);
    tensor[2] = popcount(safeChecks[WHITE][R-1]);
    tensor[3] = popcount(safeChecks[WHITE][Q-1]);
    tensor += 4;
    tensor[0] = popcount(checks[WHITE][N-1]);
    tensor[1] = popcount(checks[WHITE][B-1]);
    tensor[2] = popcount(checks[WHITE][R-1]);
    tensor[3] = popcount(checks[WHITE][Q-1]);
    tensor += 4;
    tensor[0] = popcount(innerShelters[BLACK]);
    tensor[1] = popcount(outerShelters[BLACK]);
    tensor += 2;
    // Ring weakness attacks
    tensor[0] += popcount(kingRing[BLACK] & weakSquares[BLACK]);
    tensor[1] += popcount(outerRing[BLACK] & weakSquares[BLACK]);
    tensor += 2;
    tensor[0] += us == WHITE ? 1 : -1;
    tensor++;

    tensor[P] = innerAttacks[BLACK][P];
    tensor[N] = innerAttacks[BLACK][N];
    tensor[B] = innerAttacks[BLACK][B];
    tensor[R] = innerAttacks[BLACK][R];
    tensor[Q] = innerAttacks[BLACK][Q];
    tensor += 5;
    tensor[P] = outerAttacks[BLACK][P];
    tensor[N] = outerAttacks[BLACK][N];
    tensor[B] = outerAttacks[BLACK][B];
    tensor[R] = outerAttacks[BLACK][R];
    tensor[Q] = outerAttacks[BLACK][Q];
    tensor += 5;
    tensor[0] = noQueenDanger[BLACK];
    tensor[1] = pinnedShelter[BLACK];
    tensor += 2;
    tensor[0] += popcount(safeChecks[BLACK][N-1]);
    tensor[1] += popcount(safeChecks[BLACK][B-1]);
    tensor[2] += popcount(safeChecks[BLACK][R-1]);
    tensor[3] += popcount(safeChecks[BLACK][Q-1]);
    tensor += 4;
    tensor[0] = popcount(checks[BLACK][N-1]);
    tensor[1] = popcount(checks[BLACK][B-1]);
    tensor[2] = popcount(checks[BLACK][R-1]);
    tensor[3] = popcount(checks[BLACK][Q-1]);
    tensor += 4;
    tensor[0] = popcount(innerShelters[WHITE]);
    tensor[1] = popcount(outerShelters[WHITE]);
    tensor += 2;
    // Ring weakness attacks
    tensor[0] += popcount(kingRing[WHITE] & weakSquares[WHITE]);
    tensor[1] += popcount(outerRing[WHITE] & weakSquares[WHITE]);
    tensor += 2;
    tensor[0] += us == BLACK ? 1 : -1;
    tensor++;

    // Also assert the last element we wrote is the penultimate element
    // assert(tensorStart + tensorSize - 2 == tensor);
}


/**
 * @brief The convertToFeatures function converts a set of positions to a set of features
 * @param filename The filename of the epd file
 * @param output The output file to write the features to
 */
void convertToFeatures(std::string filename, std::string output) {
    std::ifstream file(filename);
    // Check if the file is open
    if (!file.is_open()){
        std::cout << "Error: Could not open file " << filename << std::endl;
        return;
    }
    // Open output in binary mode
    std::ofstream out(output, std::ios::binary);
    // Get the count of weights by getting the current eval weights
    auto weights = getCurrentEvalWeights();
    U32 weightCount = weights.size();
    U32 PSQTweights = 6 * 64; // PSQTS + 2 king buckets
    U32 entrySize = 1 + PSQTweights + weightCount / 2 + 1; // The entry size is the 1 (gamephase) + number of weights divided by 2 + 1, since we add the result at the end, but we don't need to write the features twice for each side
    entrySize += KINGSAFETYCOLOREDPARAMS;
    entrySize += COMPLEXITYFEATURES;
    std::cout << "Allocating for:\n";
    std::cout << "\t1\tGame result\n";
    std::cout << "\t1\tGame phase\n";
    std::cout << "\t" << PSQTweights << "\tPsqt entries\n";
    std::cout << "\t" << weightCount / 2 << "\tLinear evaluation terms\n";
    std::cout << "\t" << KINGSAFETYCOLOREDPARAMS / 2 << "x2=" << KINGSAFETYCOLOREDPARAMS << "\tAdditional color-dependant king safety formula features\n";
    std::cout << "\t" << COMPLEXITYFEATURES << "\tAdditional complexity features\n";
    std::cout << "For a total of " << entrySize << " bytes per entry" << std::endl;
    // Create buffer to store the features
    S8* features = new S8[entrySize];
    // For each line in the file
    std::string line;
    Position pos;
    U64 counter = 0;
    while (std::getline(file, line)){
        // Clear the features
        memset(features, 0, entrySize);
        // Parse the line. It contains the FEN string and the result. They are separated by a ";", and the result is either w,d,b (white, draw, black)
        std::vector<std::string> tokens;

        std::string token;
        std::istringstream tokenStream(line);
        while (std::getline(tokenStream, token, ';')){
            tokens.push_back(token);
        }
        // Parse the position
        if (pos.parseFEN((char*)tokens[0].c_str())){
            // Get the features
            getEvalFeaturesTensor(&pos, features);

            // Assert we filled the features correctly. This is checked by checking the penultimate element of the features and checking if it is 1 or -1
            assert(features[entrySize - 2] == 1 || features[entrySize - 2] == -1);
            // Get the result
            if (tokens[1] == "w") features[entrySize - 1] = 1;
            else if (tokens[1] == "d") features[entrySize - 1] = 0;
            else features[entrySize - 1] = -1;
            // Write the features to the output file
            out.write((char*)features, entrySize);
        }
        // Increment the counter and report progress each 16384 positions
        if (++counter % 16384 == 0){
            std::cout << "Processed " << counter << " positions...\r";
        }
    }
    // Report the final progress
    std::cout << "Processed " << counter << " positions." << std::endl;
    // Close the file
    out.close();
    // Delete the features
    delete[] features;
}