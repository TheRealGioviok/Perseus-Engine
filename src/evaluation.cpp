#include "evaluation.h"
#include "tables.h"
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

constexpr PScore DOUBLEISOLATEDPEN = S(-16, -35);
constexpr PScore ISOLATEDPEN = S(-18, -20);
constexpr PScore BACKWARDPEN = S(-6, 8);
constexpr PScore DOUBLEDPEN = S(-19, -24);
constexpr PScore SUPPORTEDPHALANX = S(3, 10);
constexpr PScore ADVANCABLEPHALANX = S(6, 8);
constexpr PScore R_SUPPORTEDPHALANX = S(3, -2);
constexpr PScore R_ADVANCABLEPHALANX = S(3, 1);
constexpr PScore passedRankBonus[2][7] = {
    {S(0, 0), S(14, -65), S(-2, -41), S(2, 11), S(21, 66), S(44, 157), S(118, 232)}, 
    {S(0, 0), S(0, 0), S(10, -57), S(12, -2), S(55, 66), S(155, 181), S(400, 253)}
};
constexpr PScore PASSEDPATHBONUS = S(-6, 23);
constexpr PScore candidateRankBonus[2][7] = {
    {S(0, 0), S(-34, 8), S(-23, 25), S(-12, 61), S(2, 128), S(28, 107), S(0, 0)}, 
    {S(0, 0), S(0, 0), S(-5, 33), S(1, 68), S(41, 144), S(119, 168), S(0, 0)}
};
constexpr PScore INNERSHELTER = S(3, -18);
constexpr PScore OUTERSHELTER = S(11, -12);
constexpr PScore BISHOPPAIR = S(18, 116);
constexpr PScore ROOKONOPENFILE = S(21, 7);
constexpr PScore ROOKONSEMIOPENFILE = S(22, 10);
constexpr PScore KNIGHTONEXTOUTPOST = S(31, 36);
constexpr PScore BISHOPONEXTOUTPOST = S(39, 3);
constexpr PScore KNIGHTONINTOUTPOST = S(22, 38);
constexpr PScore BISHOPONINTOUTPOST = S(33, -6);
constexpr PScore KNIGHTPROTECTOR = S(-6, -2);
constexpr PScore BISHOPPROTECTOR = S(-5, -1);
constexpr PScore BISHOPPAWNS = S(-1, -7);
constexpr PScore THREATSAFEPAWN = S(45, -40);
constexpr PScore THREATPAWNPUSH = S(20, 37);
constexpr PScore PAWNHANGING = S(-1, -53);
constexpr PScore NONPAWNHANGING = S(-7, -40);
constexpr PScore PAWNTHREAT[2][5] = {
    {S(11, -66), S(32, 76), S(27, 114), S(28, 39), S(9, 32)}, 
    {S(6, -25), S(12, 127), S(13, 162), S(23, 92), S(16, 92)}
};
constexpr PScore KNIGHTTHREAT[2][5] = {
    {S(7, 19), S(-1, -31), S(25, 18), S(59, -12), S(41, 15)}, 
    {S(-7, 6), S(2, 18), S(26, 71), S(54, 59), S(44, 45)}
};
constexpr PScore BISHOPTHREAT[2][5] = {
    {S(11, 17), S(32, 21), S(-13, -40), S(57, -9), S(55, 21)}, 
    {S(0, 7), S(33, 69), S(-7, 11), S(51, 65), S(50, 69)}
};
constexpr PScore ROOKTHREAT[2][5] = {
    {S(4, 18), S(11, 47), S(22, 36), S(-8, -60), S(62, -18)}, 
    {S(-10, 5), S(1, 24), S(6, 21), S(-1, -5), S(45, 38)}
};
constexpr PScore QUEENTHREAT[2][5] = {
    {S(12, -2), S(26, -4), S(27, 23), S(-2, -22), S(3, -139)}, 
    {S(-2, 9), S(6, -1), S(5, 14), S(1, 6), S(10, -120)}
};
constexpr PScore KINGTHREAT = S(-6, -4);
constexpr PScore QUEENINFILTRATION = S(8, 3);
constexpr PScore INVASIONSQUARES = S(6, 6);
constexpr PScore RESTRICTEDSQUARES = S(4, 4);
constexpr PScore TEMPO = S(21, 27);

constexpr PScore PAWNATTACKINNERRING = S(63, -133);
constexpr PScore KNIGHTATTACKINNERRING = S(82, -22);
constexpr PScore BISHOPATTACKINNERRING = S(118, 53);
constexpr PScore ROOKATTACKINNERRING = S(122, 49);
constexpr PScore QUEENATTACKINNERRING = S(78, -82);
constexpr PScore PAWNATTACKOUTERRING = S(73, -44);
constexpr PScore KNIGHTATTACKOUTERRING = S(82, 52);
constexpr PScore BISHOPATTACKOUTERRING = S(83, 50);
constexpr PScore ROOKATTACKOUTERRING = S(30, 13);
constexpr PScore QUEENATTACKOUTERRING = S(57, 110);
constexpr PScore NOQUEENDANGER = S(-898, -672);
constexpr PScore PINNEDSHELTERDANGER = S(106, -73);
constexpr PScore FLANKATTACKS[2] = {S(32, -42), S(43, -53), };
constexpr PScore FLANKDEFENSES[2] = {S(-35, 19), S(-10, 13), };
constexpr PScore SAFECHECK[4] = {S(323, -57), S(86, 36), S(226, 108), S(231, 33), };
constexpr PScore ALLCHECKS[4] = {S(77, 14), S(25, 99), S(69, 26), S(13, 39), };
constexpr PScore SAFETYINNERSHELTER = S(-48, -105);
constexpr PScore SAFETYOUTERSHELTER = S(3, -194);
constexpr PScore INNERWEAKNESS = S(105, -60);
constexpr PScore OUTERWEAKNESS = S(14, 16);
constexpr PScore KSTEMPO = S(29, 25);

constexpr Score COMPLEXITYPASSERS = 474;
constexpr Score COMPLEXITYPAWNS = 1136;
constexpr Score COMPLEXITYBLOCKEDPAIRS = -385;
constexpr Score COMPLEXITYPAWNTENSION = -782;
constexpr Score COMPLEXITYOUTFLANKING = -69;
constexpr Score COMPLEXITYINFILTRATION = -794;
constexpr Score COMPLEXITYPAWNBOTHFLANKS = 10192;
constexpr Score COMPLEXITYPAWNENDING = 13258;
constexpr Score COMPLEXITYALMOSTUNWINNABLE = -3193;
constexpr Score COMPLEXITYBIAS = -21453;

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

// Refactored getPtPtThreatsFeat with a lambda to process each threat type.
template <bool us>
static inline PScore getPtPtThreatsFeat(Position &pos,
                                        const BitBoard (&weakPieces)[2],
                                        BitBoard (&pawnAttacks)[2],
                                        BitBoard (&ptAttacks)[2][4],
                                        S8* features)
{
    constexpr int them = !us;
    auto &occ = pos.occupancies;
    PScore score = S(0, 0);

    // Lambda to process threats and update feature values.
    auto processFeatThreats = [&](BitBoard threats, int offset) {
        while (threats) {
            const Square sq = popLsb(threats);
            Piece piece = pos.pieceOn(sq) - 6 * them;
            if (piece == K)
                continue;
            bool weakTarget = (weakPieces[them] & squareBB(sq)) != 0;
            features[offset + 5 * (!weakTarget) + piece] += (us == WHITE ? 1 : -1);
        }
    };

    processFeatThreats(pawnAttacks[us] & occ[them], 0);
    processFeatThreats(ptAttacks[us][N - 1] & occ[them], 10);
    processFeatThreats(ptAttacks[us][B - 1] & occ[them], 20);
    processFeatThreats(ptAttacks[us][R - 1] & occ[them], 30);
    processFeatThreats(ptAttacks[us][Q - 1] & occ[them], 40);

    return score;
}

template <bool us>
static inline PScore evalPtPtThreats(Position &pos,
                                     const BitBoard (&weakPieces)[2],
                                     BitBoard (&pawnAttacks)[2],
                                     BitBoard (&ptAttacks)[2][4])
{
    constexpr int them = !us;
    auto &occ = pos.occupancies;
    PScore score = S(0, 0);

    // This lambda takes a threat bitboard and a threat evaluation table.
    // It uses the table internally to add the proper bonus to score.
    auto processEvalThreats = [&](BitBoard threats, const auto &threatTable) {
        while (threats) {
            const Square sq = popLsb(threats);
            Piece piece = pos.pieceOn(sq) - 6 * them;
            if (piece == K)
                continue;
            // Uses the passed table to compute the bonus.
            score += threatTable[!(weakPieces[them] & squareBB(sq))][piece];
        }
    };

    processEvalThreats(pawnAttacks[us] & occ[them], PAWNTHREAT);
    processEvalThreats(ptAttacks[us][N - 1] & occ[them], KNIGHTTHREAT);
    processEvalThreats(ptAttacks[us][B - 1] & occ[them], BISHOPTHREAT);
    processEvalThreats(ptAttacks[us][R - 1] & occ[them], ROOKTHREAT);
    processEvalThreats(ptAttacks[us][Q - 1] & occ[them], QUEENTHREAT);

    return score;
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

// A helper structure to bundle the evaluation results for one side.
struct PawnStructureResult {
    PScore structureScore; // The sum of structure bonuses/penalties (cacheable).
    PScore extraScore;     // The extra passed pawn advance bonus (non-cacheable).
    BitBoard passers;      // The bitboard of candidate passed pawns.
    S32 passedCount;       // The number of candidate passed pawns.
};

// Template function to evaluate pawn structure for a single side.
template <int us>
PawnStructureResult pawnStructureEval(
    const BitBoard (&bb)[12],
    const BitBoard (&doubledPawns)[2],
    const BitBoard (&pawnFiles)[2],
    const BitBoard (&protectedPawns)[2],
    const BitBoard (&pawnBlockage)[2],
    const BitBoard (&occ)[3],
    const BitBoard (&attackedBy)[2],
    const BitBoard (multiAttacks)[2],
    const BitBoard (defendedByPawn)[2])
{
    constexpr auto them = 1 - us;
    PawnStructureResult result = { PScore(0,0), PScore(0,0), 0ULL, 0 };
    // Use the appropriate pawn index: for WHITE we use P, for BLACK we use p.
    constexpr int ourPawnsIndex = (us == WHITE ? P : p);
    constexpr int theirPawnsIndex = (us == WHITE ? p : P);
    constexpr auto pushUp = us == WHITE ? north : south;
    constexpr auto ahead = us == WHITE ? squaresAhead : squaresBehind;
    BitBoard pieces = bb[ourPawnsIndex];

    // Compute "block" used for passed pawn bonus.
    // (For the side being evaluated, the block mask combines the overall occupancy with the enemy pawns’ defensive gaps.)
    BitBoard block = occ[BOTH] | (~defendedByPawn[us] & ((~attackedBy[us] & attackedBy[them]) | (multiAttacks[them])));
    PScore score = S(0,0);
    PScore extraScore = S(0,0);
    while (pieces) {
        Square sq = popLsb(pieces);
        BitBoard sqb = squareBB(sq);
        // For white, rank runs from 0 (promotion) to 7; for black the opposite.
        U8 rank = (us == WHITE ? 7 - rankOf(sq) : rankOf(sq));

        // Common pawn evaluation flags:
        bool doubled       = (doubledPawns[us] & sqb) != 0;
        bool isolated      = !(isolatedPawnMask[sq] & bb[ourPawnsIndex]);
        bool pawnOpposed   = !(pawnFiles[them] & sqb);
        bool supported     = (protectedPawns[us] & sqb) != 0;
        bool advancable    = pawnBlockage[us] & pushUp(sqb);
        S8 phal          = popcount(phalanx[sq] & bb[ourPawnsIndex]);
        // Fully passsed pawn check
        const BitBoard stoppers = passedPawnMask[us][sq] & bb[theirPawnsIndex];
        // Candidate passed logic
        const BitBoard levers = makePawnAttacks<us>(sqb) & bb[theirPawnsIndex];
        const BitBoard leverPushes = pushUp(makePawnAttacks<us>(sqb)) & bb[theirPawnsIndex];
        S8 supportCount = popcount(makePawnAttacks<them>(sqb) & bb[ourPawnsIndex]);
        const bool candidate = (stoppers == (levers | leverPushes)) // No extra pawns behind the blockade
            && !(popcount(levers) - supportCount > 1)         // No levers outnumbering our support pawns
            && !(popcount(leverPushes) - phal > 0)            // No lever pushes outnumbering our phalanx support
            && !(popcount(levers) && popcount(leverPushes));  // No lever AND lever pushes (both pushing and not pushing the pawn gets it captured)
        // Backward pawn test.
        bool backward = !( (backwardPawnMask[us][sq] & bb[ourPawnsIndex]) || advancable );
        
        // For white, structure penalties are subtracted and bonuses added.
        if (isolated) {
            if (doubled && pawnOpposed)
                score += DOUBLEISOLATEDPEN;
            else
                score += ISOLATEDPEN;
        }
        else if (backward) {
            score += BACKWARDPEN;
        }

        if (doubled && !supported)
            score += DOUBLEDPEN;
        else if (supported && phal) {
            score += SUPPORTEDPHALANX;
            score += R_SUPPORTEDPHALANX * (rank - 2);
        }
        else if (advancable && phal) {
            score += ADVANCABLEPHALANX;
            score += R_ADVANCABLEPHALANX * (rank - 2);
        }
        // Only consider most advanced pawn in a file
        if (!(ahead(sq) & bb[ourPawnsIndex])){
            // Fully passed pawn
            if (!stoppers) {
                score += passedRankBonus[supported][rank];
                result.passedCount++;
                result.passers |= sqb;
                // The passed–pawn path bonus.
                extraScore += PASSEDPATHBONUS * popcount(advancePathMasked<us>(sqb, ~block));
            }
            // Candidate passed pawn
            else if (candidate){
                score += candidateRankBonus[supported][rank];
            }
        }
    }
    result.structureScore += us == WHITE ? score : -score;
    result.extraScore += us == WHITE ? extraScore : -extraScore;
    return result;
}

// The overall pawn evaluation function.
inline PScore pawnEval(const HashKey hashKey,
                         const BitBoard (&bb)[12],
                         const BitBoard (&doubledPawns)[2],
                         const BitBoard (&pawnFiles)[2],
                         const BitBoard (&protectedPawns)[2],
                         const BitBoard (&pawnBlockage)[2],
                         const BitBoard (&occ)[3],
                         const BitBoard (&attackedBy)[2],
                         const BitBoard (multiAttacks)[2],
                         const BitBoard (defendedByPawn)[2],
                         S32& passedCount)
{
    PawnEvalHashEntry& entry = pawnEvalHash[hashKey & 0x3FFFF];
    PScore score = PScore(0, 0);

    // If the pawn evaluation hash entry is valid, use cached structure score and then recompute the non-cacheable passed–pawn bonus.
    if (entry.hash == hashKey) {
        score = entry.score;
        // For white passers (cached by storing all passers together; white pawns are in bb[P]).
        BitBoard whitePassers = entry.passers & bb[P];
        {
            BitBoard block = occ[BOTH] | (~defendedByPawn[WHITE] & ((~attackedBy[WHITE] & attackedBy[BLACK]) | (multiAttacks[BLACK])));
            while (whitePassers) {
                Square sq = popLsb(whitePassers);
                BitBoard sqb = squareBB(sq);
                score += PASSEDPATHBONUS * popcount(advancePathMasked<WHITE>(sqb, ~block));
            }
        }
        // For black passers.
        BitBoard blackPassers = entry.passers & bb[p];
        {
            BitBoard block = occ[BOTH] | (~defendedByPawn[BLACK] & ((~attackedBy[BLACK] & attackedBy[WHITE]) | (multiAttacks[WHITE])));
            while (blackPassers) {
                Square sq = popLsb(blackPassers);
                BitBoard sqb = squareBB(sq);
                score -= PASSEDPATHBONUS * popcount(advancePathMasked<BLACK>(sqb, ~block));
            }
        }
        passedCount += popcount(entry.passers);
        return score;
    }
    else {
        // Recompute both sides’ pawn structure evaluation.
        auto whiteEval = pawnStructureEval<WHITE>(bb, doubledPawns, pawnFiles, protectedPawns, pawnBlockage, occ, attackedBy, multiAttacks, defendedByPawn);
        auto blackEval = pawnStructureEval<BLACK>(bb, doubledPawns, pawnFiles, protectedPawns, pawnBlockage, occ, attackedBy, multiAttacks, defendedByPawn);
        
        passedCount += whiteEval.passedCount + blackEval.passedCount;
        // The overall score is the sum of the (cacheable) structure score and the (non-cached) passed–pawn path bonus.
        score = whiteEval.structureScore + blackEval.structureScore + whiteEval.extraScore + blackEval.extraScore;

        // Save a new hash entry using only the structure (cacheable) part and
        // the union of passed–pawn bitboards (so that on a hash hit we can recalc the extra bonus).
        entry.hash = hashKey;
        entry.score = whiteEval.structureScore + blackEval.structureScore;
        entry.passers = whiteEval.passers | blackEval.passers;

        return score;
    }
}

// Template function to evaluate pawn structure for a single side.
template <int us>
void extractPawnStructureFeats(
    const BitBoard (&bb)[12],
    const BitBoard (&doubledPawns)[2],
    const BitBoard (&pawnFiles)[2],
    const BitBoard (&protectedPawns)[2],
    const BitBoard (&pawnBlockage)[2],
    const BitBoard (&occ)[3],
    const BitBoard (&attackedBy)[2],
    const BitBoard (multiAttacks)[2],
    const BitBoard (defendedByPawn)[2],
    S32& passersCount,
    S8* features
)
{
    constexpr auto them = 1 - us;
    // Use the appropriate pawn index: for WHITE we use P, for BLACK we use p.
    constexpr int ourPawnsIndex = (us == WHITE ? P : p);
    constexpr int theirPawnsIndex = (us == WHITE ? p : P);
    constexpr auto pushUp = us == WHITE ? north : south;
    constexpr auto ahead = us == WHITE ? squaresAhead : squaresBehind;
    BitBoard pieces = bb[ourPawnsIndex];

    // Compute "block" used for passed pawn bonus.
    // (For the side being evaluated, the block mask combines the overall occupancy with the enemy pawns’ defensive gaps.)
    BitBoard block = occ[BOTH] | (~defendedByPawn[us] & ((~attackedBy[us] & attackedBy[them]) | (multiAttacks[them])));
    while (pieces) {
        Square sq = popLsb(pieces);
        BitBoard sqb = squareBB(sq);
        // For white, rank runs from 0 (promotion) to 7; for black the opposite.
        U8 rank = (us == WHITE ? 7 - rankOf(sq) : rankOf(sq));

        // Common pawn evaluation flags:
        bool doubled       = (doubledPawns[us] & sqb) != 0;
        bool isolated      = !(isolatedPawnMask[sq] & bb[ourPawnsIndex]);
        bool pawnOpposed   = !(pawnFiles[them] & sqb);
        bool supported     = (protectedPawns[us] & sqb) != 0;
        bool advancable    = pawnBlockage[us] & pushUp(sqb);
        S8 phal          = popcount(phalanx[sq] & bb[ourPawnsIndex]);
        // Candidate passed logic
        const BitBoard levers = makePawnAttacks<us>(sqb) & bb[theirPawnsIndex];
        const BitBoard leverPushes = pushUp(makePawnAttacks<us>(sqb)) & bb[theirPawnsIndex];
        S8 supportCount = popcount(makePawnAttacks<them>(sqb) & bb[ourPawnsIndex]);
        const BitBoard stoppers = passedPawnMask[us][sq] & bb[theirPawnsIndex];
        const bool candidate = (stoppers == (levers | leverPushes)) // No extra pawns behind the blockade
            && !(popcount(levers) - supportCount > 1)         // No levers outnumbering our support pawns
            && !(popcount(leverPushes) - phal > 0)            // No lever pushes outnumbering our phalanx support
            && !(popcount(levers) && popcount(leverPushes));  // No lever AND lever pushes (both pushing and not pushing the pawn gets it captured)
        // Backward pawn test.
        bool backward = !( (backwardPawnMask[us][sq] & bb[ourPawnsIndex]) || advancable );
        
        // For white, structure penalties are subtracted and bonuses added.
        if (isolated) {
            if (doubled && pawnOpposed)
                features[0] += us == WHITE ? 1 : -1;
            else
                features[1] += us == WHITE ? 1 : -1;
        }
        else if (backward) {
            features[2] += us == WHITE ? 1 : -1;
        }

        if (doubled && !supported)
            features[3] += us == WHITE ? 1 : -1;
        else if (supported && phal) {
            features[4] += us == WHITE ? 1 : -1;
            features[5] += (us == WHITE ? 1 : -1) * (rank - 2);
        }
        else if (advancable && phal) {
            features[6] += us == WHITE ? 1 : -1;
            features[7] += (us == WHITE ? 1 : -1) * (rank - 2);
        }

        // Only consider most advanced pawn in a file
        if (!(ahead(sq) & bb[ourPawnsIndex])){
            // Fully passed pawn
            if (!stoppers) {
                features[8+7*supported+rank] += us == WHITE ? 1 : -1;
                features[8+7+7] += (us == WHITE ? 1 : -1) *  popcount(advancePathMasked<us>(sqb, ~block));
                passersCount += 1;
            }
            // Candidate passed pawn
            else if (candidate){
                features[8+7+7+1+7*supported+rank] += us == WHITE ? 1 : -1;
            }
        }
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
    BitBoard pinned[2] = {
        pos->blockersFor[WHITE] & occ[WHITE],
        pos->blockersFor[BLACK] & occ[BLACK]
    };

    const BitBoard blockedPawns[2] = {
        bb[P] & (occ[BOTH] << 8),
        bb[p] & (occ[BOTH] >> 8)
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

    bool wkhside = fileOf(whiteKing) <= 3;
    bool bkhside = fileOf(blackKing) <= 3;

    score += pos->psqtScores[indexColorKingsKingside(WHITE, wkhside == bkhside, wkhside)] + pos->psqtScores[indexColorKingsKingside(BLACK, wkhside == bkhside, bkhside)];

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

    BitBoard doubledPawns[2] = {bb[P] & (bb[P] >> 8), bb[p] & (bb[p] << 8)};
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

    score += evalPtPtThreats<WHITE>(*pos, weakPieces, pawnAttackedSquares, ptAttacks) - evalPtPtThreats<BLACK>(*pos, weakPieces, pawnAttackedSquares, ptAttacks);

    const Score kingThreatsDiff = (!!kingThreats[WHITE]) - (!!kingThreats[BLACK]);
    score += KINGTHREAT * kingThreatsDiff;

    // Queen infiltration
    const Score queenInfiltrationDiff = popcount(~pawnSpan[BLACK] & (ranks(0) | ranks(1) | ranks(2) | ranks(3)) & bb[Q]) - popcount(~pawnSpan[WHITE] & (ranks(7) | ranks(6) | ranks(5) | ranks(4)) & bb[q]);
    score += QUEENINFILTRATION * queenInfiltrationDiff;

    // Latent invasions

    const Score invasionsSquaresDiff = popcount(
        (ranks(0) | ranks(1) | ranks(2) | ranks(3))
        & ~pawnSpan[BLACK] 
        & ~makePawnAttacks<BLACK>(pawnSpan[BLACK])
        & multiAttacks[WHITE] 
        & ~attackedBy[BLACK] 
        & ~doublePawnAttackedSquares[WHITE]
    ) - popcount(
        (ranks(7) | ranks(6) | ranks(5) | ranks(4))
        & ~pawnSpan[WHITE] 
        & ~makePawnAttacks<WHITE>(pawnSpan[WHITE])
        & multiAttacks[BLACK] 
        & ~attackedBy[WHITE] 
        & ~doublePawnAttackedSquares[BLACK]
    );
    score += INVASIONSQUARES * invasionsSquaresDiff;

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

    // Flank attacks
    dangerIndex[WHITE] += FLANKATTACKS[0]  * popcount(attackedBy[WHITE] & kingFlank[BLACK][fileOf(blackKing)]) 
                        + FLANKATTACKS[1]  * popcount(multiAttacks[WHITE] & kingFlank[BLACK][fileOf(blackKing)])
                        + FLANKDEFENSES[0] * popcount(attackedBy[BLACK] & kingFlank[BLACK][fileOf(blackKing)])
                        + FLANKDEFENSES[1] * popcount(multiAttacks[BLACK] & kingFlank[BLACK][fileOf(blackKing)]);

    // Flank attacks
    dangerIndex[BLACK] += FLANKATTACKS[0]  * popcount(attackedBy[BLACK] & kingFlank[WHITE][fileOf(whiteKing)]) 
                        + FLANKATTACKS[1]  * popcount(multiAttacks[BLACK] & kingFlank[WHITE][fileOf(whiteKing)])
                        + FLANKDEFENSES[0] * popcount(attackedBy[WHITE] & kingFlank[WHITE][fileOf(whiteKing)])
                        + FLANKDEFENSES[1] * popcount(multiAttacks[WHITE] & kingFlank[WHITE][fileOf(whiteKing)]);

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

    //const PScore dampenWhite = PScore(dangerIndex[WHITE].mg() * DAMPEN.mg() / RESOLUTION, dangerIndex[WHITE].eg() * DAMPEN.eg() / RESOLUTION);
    //const PScore dampenBlack = PScore(dangerIndex[BLACK].mg() * DAMPEN.mg() / RESOLUTION, dangerIndex[BLACK].eg() * DAMPEN.eg() / RESOLUTION);
    
    //dangerIndex[WHITE] = dangerIndex[WHITE] - dampenBlack;
    //dangerIndex[BLACK] = dangerIndex[BLACK] - dampenWhite;

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
        weights.push_back(passedRankBonus[0][rank].mg());
    }
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(passedRankBonus[1][rank].mg());
    }
    weights.push_back(PASSEDPATHBONUS.mg());
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(candidateRankBonus[0][rank].mg());
    }
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(candidateRankBonus[1][rank].mg());
    }

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
    
    for (auto t : {PAWNTHREAT, KNIGHTTHREAT, BISHOPTHREAT, ROOKTHREAT, QUEENTHREAT}){
        for (int def : {0, 1}){
            for (Piece target : {P, N, B, R, Q}){
                weights.push_back(t[def][target].mg());
            }
        }
    }

    weights.push_back(KINGTHREAT.mg());
    weights.push_back(QUEENINFILTRATION.mg());
    weights.push_back(INVASIONSQUARES.mg());
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
        weights.push_back(passedRankBonus[0][rank].eg());
    }
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(passedRankBonus[1][rank].eg());
    }
    weights.push_back(PASSEDPATHBONUS.eg());
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(candidateRankBonus[0][rank].eg());
    }
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(candidateRankBonus[1][rank].eg());
    }

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
    
    for (auto t : {PAWNTHREAT, KNIGHTTHREAT, BISHOPTHREAT, ROOKTHREAT, QUEENTHREAT}){
        for (int def : {0, 1}){
            for (Piece target : {P, N, B, R, Q}){
                weights.push_back(t[def][target].eg());
            }
        }
    }

    weights.push_back(KINGTHREAT.eg());
    weights.push_back(QUEENINFILTRATION.eg());
    weights.push_back(INVASIONSQUARES.eg());
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
        U8 sqx = sq & 7;
        sqx = (piece < p) ?
            (wkhside ? sqx : 7 - sqx) :
            (bkhside ? sqx : 7 - sqx) ;
        U8 sqy = sq / 8; 
        sq = sqx + 8 * sqy;
        bool pieceColor = piece >= p;
        tensor[64 * (piece % 6) + sq] += (pieceColor ? 0b0100 : 0b0001) << (wkhside == bkhside);
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
    BitBoard pinned[2] = {
        pos->blockersFor[WHITE] & occ[WHITE],
        pos->blockersFor[BLACK] & occ[BLACK]
    };

    BitBoard blockedPawns[2] = {
        bb[P] & (occ[BOTH] << 8),
        bb[p] & (occ[BOTH] >> 8)
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

    BitBoard doubledPawns[2] = { bb[P] & (bb[P] >> 8), bb[p] & (bb[p] << 8) };
    BitBoard pawnFiles[2] = { filesFromBB(bb[P]), filesFromBB(bb[p]) };
    
    extractPawnStructureFeats<WHITE>(bb,doubledPawns,pawnFiles,protectedPawns,pawnBlockage,occ,attackedBy,multiAttacks,pawnAttackedSquares, passedCount, tensor);
    extractPawnStructureFeats<BLACK>(bb,doubledPawns,pawnFiles,protectedPawns,pawnBlockage,occ,attackedBy,multiAttacks,pawnAttackedSquares, passedCount, tensor);

    tensor += 8 + 7 + 7 + 1 + 7 + 7;

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

    getPtPtThreatsFeat<WHITE>(*pos, weakPieces, pawnAttackedSquares, ptAttacks, tensor);
    getPtPtThreatsFeat<BLACK>(*pos, weakPieces, pawnAttackedSquares, ptAttacks, tensor);

    tensor += 50;

    const Score kingThreatsDiff = (!!kingThreats[WHITE]) - (!!kingThreats[BLACK]);
    tensor[0] = kingThreatsDiff;
    ++tensor;

    // Queen infiltration
    const Score queenInfiltrationDiff = popcount(~pawnSpan[BLACK] & (ranks(0) | ranks(1) | ranks(2) | ranks(3)) & bb[Q]) - popcount(~pawnSpan[WHITE] & (ranks(7) | ranks(6) | ranks(5) | ranks(4)) & bb[q]);
    tensor[0] = queenInfiltrationDiff;
    ++tensor;

    // Latent invasions
    const Score invasionsSquaresDiff = popcount(
        (ranks(0) | ranks(1) | ranks(2) | ranks(3))
        & ~pawnSpan[BLACK] 
        & ~makePawnAttacks<BLACK>(pawnSpan[BLACK])
        & multiAttacks[WHITE] 
        & ~attackedBy[BLACK] 
        & ~doublePawnAttackedSquares[WHITE]
    ) - popcount(
        (ranks(7) | ranks(6) | ranks(5) | ranks(4))
        & ~pawnSpan[WHITE] 
        & ~makePawnAttacks<WHITE>(pawnSpan[WHITE])
        & multiAttacks[BLACK] 
        & ~attackedBy[WHITE] 
        & ~doublePawnAttackedSquares[BLACK]
    );
    tensor[0] = invasionsSquaresDiff;
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


#define KINGSAFETYCOLOREDPARAMS 58
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
    tensor[0] = popcount(attackedBy[WHITE] & kingFlank[BLACK][fileOf(blackKing)]);
    tensor[1] = popcount(multiAttacks[WHITE] & kingFlank[BLACK][fileOf(blackKing)]);
    tensor[2] = popcount(attackedBy[BLACK] & kingFlank[BLACK][fileOf(blackKing)]);
    tensor[3] = popcount(multiAttacks[BLACK] & kingFlank[BLACK][fileOf(blackKing)]);
    tensor += 4;
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
    tensor[0] = popcount(attackedBy[BLACK] & kingFlank[WHITE][fileOf(whiteKing)]);
    tensor[1] = popcount(multiAttacks[BLACK] & kingFlank[WHITE][fileOf(whiteKing)]);
    tensor[2] = popcount(attackedBy[WHITE] & kingFlank[WHITE][fileOf(whiteKing)]);
    tensor[3] = popcount(multiAttacks[WHITE] & kingFlank[WHITE][fileOf(whiteKing)]);
    tensor += 4;
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