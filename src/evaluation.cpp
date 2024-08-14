
#include "evaluation.h"
#include "tables.h"
#include "tt.h"
#include "movegen.h"
#include "types.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>

Score mgTables[12][64] = { {0} };
Score egTables[12][64] = { {0} };

Score* mgPestoTables[6] = {
    (Score*)mgPawnTable,
    (Score*)mgKnightTable,
    (Score*)mgBishopTable,
    (Score*)mgRookTable,
    (Score*)mgQueenTable,
    (Score*)mgKingTable };

Score* egPestoTables[6] = {
    (Score*)egPawnTable,
    (Score*)mgKnightTable,
    (Score*)mgBishopTable,
    (Score*)mgRookTable,
    (Score*)mgQueenTable,
    (Score*)egKingTable };

void initTables() {

    for (Piece piece = P; piece <= K; piece++) {
        for(Square square = a8; square < noSquare; square++) {
            // White
            mgTables[piece][square] = mgPestoTables[piece][square] + mgValues[piece];
            egTables[piece][square] = egPestoTables[piece][square] + egValues[piece];
            // Black
            mgTables[piece + 6][square] = mgPestoTables[piece][flipSquare(square)] + mgValues[piece];
            egTables[piece + 6][square] = egPestoTables[piece][flipSquare(square)] + egValues[piece];
        }
    }
}



constexpr Score DOUBLEDEARLYMG = 11;
constexpr Score DOUBLEDEARLYEG = 4;

// (MG)
constexpr Score DOUBLEISOLATEDPENMG = 20;
constexpr Score ISOLATEDPENMG = 21;
constexpr Score BACKWARDPENMG = 14;
constexpr Score DOUBLEDPENMG = 13;
constexpr Score SUPPORTEDPHALANXMG = 9;
constexpr Score ADVANCABLEPHALANXMG = 4;
constexpr Score R_SUPPORTEDPHALANXMG = 2;
constexpr Score R_ADVANCABLEPHALANXMG = 1;
constexpr Score passedRankBonusMg [7] = {0, 2, -20, -20, 20, 62, 79, };
constexpr Score PASSEDPATHBONUSMG = -4;
constexpr Score SUPPORTEDPASSERMG = 30;
constexpr Score INNERSHELTERMG = 25;
constexpr Score OUTERSHELTERMG = 13;
constexpr Score BISHOPPAIRMG = 45;
constexpr Score ROOKONOPENFILEMG = 30;
constexpr Score ROOKONSEMIOPENFILEMG = 21;
constexpr Score TEMPOMG = 35;

// (EG)

constexpr Score DOUBLEISOLATEDPENEG = 48;
constexpr Score ISOLATEDPENEG = 17;
constexpr Score BACKWARDPENEG = 7;
constexpr Score DOUBLEDPENEG = 29;
constexpr Score SUPPORTEDPHALANXEG = 7;
constexpr Score ADVANCABLEPHALANXEG = 21;
constexpr Score R_SUPPORTEDPHALANXEG = 2;
constexpr Score R_ADVANCABLEPHALANXEG = 1;
constexpr Score passedRankBonusEg [7] = {0, -34, -21, 18, 66, 171, 263, };
constexpr Score PASSEDPATHBONUSEG = 8;
constexpr Score SUPPORTEDPASSEREG = -2;
constexpr Score INNERSHELTEREG = 9;
constexpr Score OUTERSHELTEREG = 4;
constexpr Score BISHOPPAIREG = 97;
constexpr Score ROOKONOPENFILEEG = -8;
constexpr Score ROOKONSEMIOPENFILEEG = 20;
constexpr Score TEMPOEG = 30;




constexpr Score OWNPIECEBLOCKEDPAWNMG = 0;
constexpr Score OWNPIECEBLOCKEDPAWNEG = 8;



constexpr Score WEAKUNOPPOSEDMG = 9;
constexpr Score WEAKUNOPPOSEDPENEG = 11;

constexpr Score WEAKLEVEREG = 36;

constexpr Score MG_PAWN_PASSER_BONUS = 3;
constexpr Score EG_PAWN_PASSER_BONUS = 12;

constexpr Score SUPPORTEDCONNECTEDMG = 21;

constexpr Score BISHOPPAWNSMG = 2;
constexpr Score BISHOPPAWNSEG = 4;

constexpr Score ROOKONQUEENFILEMG = 4;
constexpr Score ROOKONQUEENFILEEG = 8;

constexpr Score MG_ROOKCLOSEDFILE = 6;
constexpr Score EG_ROOKCLOSEDFILE = 3;
constexpr Score MG_ROOKSEMIOPENFILE = 13;
constexpr Score MG_ROOKOPENFILE = 20;
constexpr Score MG_ROOKCLEARFILE = 1;
constexpr Score EG_ROOKSEMIOPENFILE = 5;
constexpr Score EG_ROOKOPENFILE = 15;
constexpr Score EG_ROOKCLEARFILE = 1;
constexpr Score MG_ROOKSEVENTH = 2;
constexpr Score EG_ROOKSEVENTH = 1;
constexpr Score TRAPPEDROOKMG = 35;
constexpr Score TRAPPEDROOKEG = 8;

constexpr Score MG_KINGINREARSPAN = 4;
constexpr Score EG_KINGINREARSPAN = 1;

constexpr Score MG_PAWNMINORSUPPORTED = 11;
constexpr Score EG_PAWNMINORSUPPORTED = 2;
constexpr Score MG_MINORBEHINDPAWN = 11;
constexpr Score EG_MINORBEHINDPAWN = 2;

constexpr Score MG_KNIGHTKINGPROTECTOR = 6;
constexpr Score MG_BISHOPKINGPROTECTOR = 4;
constexpr Score EG_KNIGHTKINGPROTECTOR = 6;
constexpr Score EG_BISHOPKINGPROTECTOR = 6;

constexpr Score PAWNTHREATMG = 99;
constexpr Score PAWNTHREATEG = 50;

constexpr Score PAWNSHELTERVALUE = 5;
constexpr Score KNIGHTSHELTERVALUE = 2;
constexpr Score BISHOPSHELTERVALUE = 1;

constexpr Score OPENFILEONKINGMG = -18;
constexpr Score OPENFILEONKINGEG = -12;

constexpr Score PAWNSTORM = 1;

constexpr Score KNIGHTATTACKOUTERRING = 10;
constexpr Score KNIGHTATTACKINNERRING = 23;
constexpr Score KNIGHTATTACKZONE = 31;
constexpr Score BISHOPATTACKOUTERRING = 10;
constexpr Score BISHOPATTACKINNERRING = 27;
constexpr Score ROOKATTACKOUTERRING = 11;
constexpr Score ROOKATTACKINNERRING = 15;
constexpr Score QUEENATTACKOUTERRING = 17;
constexpr Score QUEENATTACKINNERRING = 32;

#define MOBILITYEVAL true
#define PAWNEVAL true

Score noutpostEg[5] = { 2, 6, 12, 14, 18 };
Score noutpostMg[5] = { 2, 4, 8, 9, 11 };
Score boutpostEg[5] = { 2, 5, 10, 8, 3 };
Score boutpostMg[5] = { 2, 3, 6, 4, 2 };

// Sf formula, tuned parameters
static inline constexpr Score connectedBonus(U8 rank, bool op, bool ph, bool su) {
    return connectedSeed[rank] * (2 + ph - op) + SUPPORTEDCONNECTEDMG * su;
}

static inline constexpr Score knightOutpostMg(BitBoard pAttacks, U8 pSupport){
    return pAttacks ? 0 : noutpostMg[pSupport];
}

static inline constexpr Score knightOutpostEg(BitBoard pAttacks, U8 pSupport) {
    return pAttacks ? 0 : noutpostEg[pSupport];
}

static inline constexpr Score bishopOutpostMg(BitBoard pAttacks, U8 pSupport) {
    return pAttacks ? 0 : boutpostMg[pSupport];
}

static inline constexpr Score bishopOutpostEg(BitBoard pAttacks, U8 pSupport) {
    return pAttacks ? 0 : boutpostEg[pSupport];
}

static inline constexpr BitBoard centralFiles() {
    return files(2) | files(3) | files(4) | files(5);
}

template <Piece pt>
//(bb, occ, pinned, mobilityArea, mobilityScore);
static inline void getMobilityFeat(const BitBoard (&bb)[12], const BitBoard (&occ)[3], const BitBoard (&pinned)[2], const BitBoard (&mobilityArea)[2], S8 *features) {
    constexpr Side us = pt < p ? WHITE : BLACK;
    // constexpr Side them = us ? BLACK : WHITE;
    BitBoard mob;
    BitBoard pieces = bb[pt] & ~pinned[us];
    BitBoard occCheck = occ[BOTH];

    // Consider diagonal xrays through our queens
    if constexpr (pt == B || pt == Q)                   occCheck ^= bb[Q];
    else if constexpr (pt == b || pt == q)              occCheck ^= bb[q];

    // Consider cross xrays through our rooks
    if constexpr (pt == R || pt == Q)                   occCheck ^= bb[R];
    else if constexpr (pt == r || pt == q)              occCheck ^= bb[r];
    
    // Iterate over all pieces of the given type
    
    while (pieces) {
        Square sq = popLsb(pieces);
        mob = mobilityArea[us];
        if constexpr (pt == N || pt == n) {
            mob &= knightAttacks[sq];
            U8 moveCount = popcount(mob);
            features[moveCount] += us == WHITE ? 1 : -1;
        }
        else if constexpr (pt == B || pt == b) {
            mob &= getBishopAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            features[moveCount] += us == WHITE ? 1 : -1;
        }
        else if constexpr (pt == R || pt == r) {
            mob &= getRookAttack(sq, occCheck); // X-ray through our queens and rooks
            U8 moveCount = popcount(mob);
            features[moveCount] += us == WHITE ? 1 : -1;
        }
        else if constexpr (pt == Q || pt == q) {
            mob &= getQueenAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            features[moveCount] += us == WHITE ? 1 : -1;
        }
    }
} 

template <Piece pt>
//(bb, occ, pinned, mobilityArea, mobilityScore);
static inline void mobility(const BitBoard (&bb)[12], const BitBoard (&occ)[3], const BitBoard (&pinned)[2], const BitBoard (&mobilityArea)[2], Score (&mobilityScore)[2]) {
    constexpr Side us = pt < p ? WHITE : BLACK;
    // constexpr Side them = us ? BLACK : WHITE;
    BitBoard mob;
    BitBoard pieces = bb[pt] & ~pinned[us];
    BitBoard occCheck = occ[BOTH];

    // Consider diagonal xrays through our queens
    if constexpr (pt == B || pt == Q)                   occCheck ^= bb[Q];
    else if constexpr (pt == b || pt == q)              occCheck ^= bb[q];

    // Consider cross xrays through our rooks
    if constexpr (pt == R || pt == Q)                   occCheck ^= bb[R];
    else if constexpr (pt == r || pt == q)              occCheck ^= bb[r];
    
    // Iterate over all pieces of the given type
    
    while (pieces) {
        Square sq = popLsb(pieces);
        mob = mobilityArea[us];
        if constexpr (pt == N || pt == n) {
            mob &= knightAttacks[sq];
            U8 moveCount = popcount(mob);
            // Add the mobility score
            if constexpr (pt == N) {
                mobilityScore[0] += knightMobMg[moveCount];
                mobilityScore[1] += knightMobEg[moveCount];
            }
            else {
                mobilityScore[0] -= knightMobMg[moveCount];
                mobilityScore[1] -= knightMobEg[moveCount];
            }
        }
        else if constexpr (pt == B || pt == b) {
            mob &= getBishopAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            // Add the mobility score
            if constexpr (pt == B) {
                mobilityScore[0] += bishopMobMg[moveCount];
                mobilityScore[1] += bishopMobEg[moveCount];
            }
            else {
                mobilityScore[0] -= bishopMobMg[moveCount];
                mobilityScore[1] -= bishopMobEg[moveCount];
            }
        }
        else if constexpr (pt == R || pt == r) {
            mob &= getRookAttack(sq, occCheck); // X-ray through our queens and rooks
            U8 moveCount = popcount(mob);
            // Add the mobility score
            if constexpr (pt == R) {
                mobilityScore[0] += rookMobMg[moveCount];
                mobilityScore[1] += rookMobEg[moveCount];
            }
            else {
                mobilityScore[0] -= rookMobMg[moveCount];
                mobilityScore[1] -= rookMobEg[moveCount];
            }
        }
        else if constexpr (pt == Q || pt == q) {
            mob &= getQueenAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            // Add the mobility score
            if constexpr (pt == Q) {
                mobilityScore[0] += queenMobMg[moveCount];
                mobilityScore[1] += queenMobEg[moveCount];
            }
            else {
                mobilityScore[0] -= queenMobMg[moveCount];
                mobilityScore[1] -= queenMobEg[moveCount];
            }
        }
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

Score pestoEval(Position *pos){
    auto const& bb = pos->bitboards;
    auto const& occ = pos->occupancies;
    // Setup the game phase
    S32 gamePhase = gamephaseInc[P] * popcount(bb[P] | bb[p]) +
                    gamephaseInc[N] * popcount(bb[N] | bb[n]) +
                    gamephaseInc[B] * popcount(bb[B] | bb[b]) +
                    gamephaseInc[R] * popcount(bb[R] | bb[r]) +
                    gamephaseInc[Q] * popcount(bb[Q] | bb[q]) +
                    gamephaseInc[K] * popcount(bb[K] | bb[k]);
    gamePhase = std::min(gamePhase, (S32)24); // If we have a lot of pieces, we don't want to go over 24

    Score mgScore = 0, egScore = 0;
    Square whiteKing = lsb(bb[K]);
    Square blackKing = lsb(bb[k]);

    // Mobility calculations
    BitBoard pawnAttackedSquares[2] = {
        ((bb[P] & notFile(0)) >> 9) | ((bb[P] & notFile(7)) >> 7),
        ((bb[p] & notFile(0)) << 7) | ((bb[p] & notFile(7)) << 9)
    };

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
        ((bb[P] >> 8) & occ[BOTH]) << 8,
        ((bb[p] << 8) & occ[BOTH]) >> 8
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

    // Calculate the mobility scores (index by phase and color)
    Score mobilityScore[2] = {
        0,0 
    };

    // White mobility
    mobility<N>(bb, occ, pinned, mobilityArea, mobilityScore);
    mobility<B>(bb, occ, pinned, mobilityArea, mobilityScore);
    mobility<R>(bb, occ, pinned, mobilityArea, mobilityScore);
    mobility<Q>(bb, occ, pinned, mobilityArea, mobilityScore);

    // Black mobility
    mobility<n>(bb, occ, pinned, mobilityArea, mobilityScore);
    mobility<b>(bb, occ, pinned, mobilityArea, mobilityScore);
    mobility<r>(bb, occ, pinned, mobilityArea, mobilityScore);
    mobility<q>(bb, occ, pinned, mobilityArea, mobilityScore);

    // Malus for directly doubled, undefended pawns
    BitBoard protectedPawns[2] = {
        bb[P] & pawnAttackedSquares[WHITE],
        bb[p] & pawnAttackedSquares[BLACK]
    };

    BitBoard pawnBlockage[2] = {
        bb[P] | pawnAttackedSquares[WHITE],
        bb[p] | pawnAttackedSquares[BLACK]
    };

    BitBoard doubledPawns[2] = { bb[P] & (bb[P] << 8), bb[p] & (bb[p] >> 8) };
    BitBoard pawnFiles[2] = { filesFromBB(bb[P]), filesFromBB(bb[p]) };
    // White pawns
    BitBoard pieces = bb[P];
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
        if (doubled && isolated && pawnOpposed){
            mgScore -= DOUBLEISOLATEDPENMG;
            egScore -= DOUBLEISOLATEDPENEG;
        }
        else if (isolated){
            mgScore -= ISOLATEDPENMG;
            egScore -= ISOLATEDPENEG;
        }
        else if (backward){
            mgScore -= BACKWARDPENMG;
            egScore -= BACKWARDPENEG;
        }
        if (doubled && !supported){
            mgScore -= DOUBLEDPENMG;
            egScore -= DOUBLEDPENEG;
        }
        else if (supported && phal){
            mgScore += SUPPORTEDPHALANXMG;
            mgScore += R_SUPPORTEDPHALANXMG * (rank - 2);
            egScore += SUPPORTEDPHALANXEG;
            egScore += R_SUPPORTEDPHALANXEG * (rank - 2);
        }
        else if (advancable && phal){
            mgScore += ADVANCABLEPHALANXMG;
            mgScore += R_ADVANCABLEPHALANXMG * (rank - 2);
            egScore += ADVANCABLEPHALANXEG;
            egScore += R_ADVANCABLEPHALANXEG * (rank - 2);
        }
        if (candidatePassed){
            BitBoard passedPath = advancePathMasked<WHITE>(sqb, ~bb[BOTH]);
            // Give bonus for how close the pawn is to the promotion square
            mgScore += passedRankBonusMg[rank];
            egScore += passedRankBonusEg[rank];
            // Give bonus for how many squares the pawn can advance
            mgScore += popcount(passedPath) * PASSEDPATHBONUSMG;
            egScore += popcount(passedPath) * PASSEDPATHBONUSEG;
            // Bonus for connected or supported passed pawns
            if (supported){
                mgScore += SUPPORTEDPASSERMG;
                egScore += SUPPORTEDPASSEREG;
            }
        }
    }

    // Black pawns
    pieces = bb[p];
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
        if (doubled && isolated && pawnOpposed){
            mgScore += DOUBLEISOLATEDPENMG;
            egScore += DOUBLEISOLATEDPENEG;
        }
        else if (isolated){
            mgScore += ISOLATEDPENMG;
            egScore += ISOLATEDPENEG;
        }
        else if (backward){
            mgScore += BACKWARDPENMG;
            egScore += BACKWARDPENEG;
        }
        if (doubled && !supported){
            mgScore += DOUBLEDPENMG;
            egScore += DOUBLEDPENEG;
        }
        else if (supported && phal){
            mgScore -= SUPPORTEDPHALANXMG;
            mgScore -= R_SUPPORTEDPHALANXMG * (rank - 2);
            egScore -= SUPPORTEDPHALANXEG;
            egScore -= R_SUPPORTEDPHALANXEG * (rank - 2);
        }
        else if (advancable && phal){
            mgScore -= ADVANCABLEPHALANXMG;
            mgScore -= R_ADVANCABLEPHALANXMG * (rank - 2);
            egScore -= ADVANCABLEPHALANXEG;
            egScore -= R_ADVANCABLEPHALANXEG * (rank - 2);
        }
        if (candidatePassed){
            BitBoard passedPath = advancePathMasked<BLACK>(sqb, ~bb[BOTH]);
            // Give bonus for how close the pawn is to the promotion square
            mgScore -= passedRankBonusMg[rank];
            egScore -= passedRankBonusEg[rank];
            // Give bonus for how many squares the pawn can advance
            mgScore -= popcount(passedPath) * PASSEDPATHBONUSMG;
            egScore -= popcount(passedPath) * PASSEDPATHBONUSEG;
            // Bonus for connected or supported passed pawns
            if (supported){
                mgScore -= SUPPORTEDPASSERMG;
                egScore -= SUPPORTEDPASSEREG;
            }
        }
    }

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
    mgScore += INNERSHELTERMG * (popcount(innerShelters[WHITE]) - popcount(innerShelters[BLACK]));
    egScore += INNERSHELTEREG * (popcount(innerShelters[WHITE]) - popcount(innerShelters[BLACK]));
    mgScore += OUTERSHELTERMG * (popcount(outerShelters[WHITE]) - popcount(outerShelters[BLACK]));
    egScore += OUTERSHELTEREG * (popcount(outerShelters[WHITE]) - popcount(outerShelters[BLACK]));

    // Add bonus for bishop pairs
    mgScore += ((popcount(bb[B])>=2) - (popcount(bb[b])>=2)) * BISHOPPAIRMG;
    egScore += ((popcount(bb[B])>=2) - (popcount(bb[b])>=2)) * BISHOPPAIREG;

    // Add bonus for rooks on open files
    BitBoard openFiles = ~(pawnFiles[WHITE] | pawnFiles[BLACK]);
    BitBoard semiOpenFor[2] = {
        ~pawnFiles[WHITE],
        ~pawnFiles[BLACK]
    };
    Score openDiff = popcount(bb[R] & openFiles) - popcount(bb[r] & openFiles);
    Score semiOpenDiff = popcount(bb[R] & semiOpenFor[WHITE]) - popcount(bb[r] & semiOpenFor[BLACK]);
    mgScore += openDiff * ROOKONOPENFILEMG;
    egScore += openDiff * ROOKONOPENFILEEG;
    mgScore += semiOpenDiff * ROOKONSEMIOPENFILEMG;
    egScore += semiOpenDiff * ROOKONSEMIOPENFILEEG;


    // Calculate the total score
    mgScore += mobilityScore[0];
    egScore += mobilityScore[1];

    // Calculate mg and eg scores
    Score sign = 1 - 2 * pos->side;
    mgScore += sign * TEMPOMG;
    egScore += sign * TEMPOEG;
    
    return sign * 
        (
            (pos->psqtScore[0] + mgScore) * gamePhase +
            (pos->psqtScore[1] + egScore) * (24 - gamePhase)
        ) / 24;
}

std::vector<Score> getCurrentEvalWeights(){
    std::vector<Score> weights;
    // Add the material weights
    for (Piece piece = P; piece <= Q; piece++){
        weights.push_back(mgValues[piece]);
    }
    // Add the psqt weights
    for (Piece piece = P; piece <= K; piece++){
        for (Square square = a8; square < noSquare; square++){
            weights.push_back(mgTables[piece][square]);
        }
    }
    // Add the mobility weights
    for (U8 mobcount = 0; mobcount < 9; mobcount++){
        weights.push_back(knightMobMg[mobcount]);
    }
    for (U8 mobcount = 0; mobcount < 14; mobcount++){
        weights.push_back(bishopMobMg[mobcount]);
    }
    for (U8 mobcount = 0; mobcount < 15; mobcount++){
        weights.push_back(rookMobMg[mobcount]);
    }
    for (U8 mobcount = 0; mobcount < 28; mobcount++){
        weights.push_back(queenMobMg[mobcount]);
    }
    // Add the pawn evaluation weights
    weights.push_back(DOUBLEISOLATEDPENMG);
    weights.push_back(ISOLATEDPENMG);
    weights.push_back(BACKWARDPENMG);
    weights.push_back(DOUBLEDPENMG);
    weights.push_back(SUPPORTEDPHALANXMG);
    weights.push_back(R_SUPPORTEDPHALANXMG);
    weights.push_back(ADVANCABLEPHALANXMG);
    weights.push_back(R_ADVANCABLEPHALANXMG);
    // Add the passed pawn bonus list
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(passedRankBonusMg[rank]);
    }
    weights.push_back(PASSEDPATHBONUSMG);
    weights.push_back(SUPPORTEDPASSERMG);

    // Add the shelter weights
    weights.push_back(INNERSHELTERMG);
    weights.push_back(OUTERSHELTERMG);
    
    // Add the bishop pair bonus
    weights.push_back(BISHOPPAIRMG);

    // Add the rook and queen on open file bonus
    weights.push_back(ROOKONOPENFILEMG);
    weights.push_back(ROOKONSEMIOPENFILEMG);

    // Add the tempo bonus
    weights.push_back(TEMPOMG);

    // Now, EG
    // Add the material weights
    for (Piece piece = P; piece <= K; piece++){
        weights.push_back(egValues[piece]);
    }
    // Add the psqt weights
    for (Piece piece = P; piece <= K; piece++){
        for (Square square = a8; square < noSquare; square++){
            weights.push_back(egTables[piece][square]);
        }
    }
    // Add the mobility weights
    for (U8 mobcount = 0; mobcount < 9; mobcount++){
        weights.push_back(knightMobEg[mobcount]);
    }
    for (U8 mobcount = 0; mobcount < 14; mobcount++){
        weights.push_back(bishopMobEg[mobcount]);
    }
    for (U8 mobcount = 0; mobcount < 15; mobcount++){
        weights.push_back(rookMobEg[mobcount]);
    }
    for (U8 mobcount = 0; mobcount < 28; mobcount++){
        weights.push_back(queenMobEg[mobcount]);
    }
    // Add the pawn evaluation weights
    weights.push_back(DOUBLEISOLATEDPENEG);
    weights.push_back(ISOLATEDPENEG);
    weights.push_back(BACKWARDPENEG);
    weights.push_back(DOUBLEDPENEG);
    weights.push_back(SUPPORTEDPHALANXEG);
    weights.push_back(R_SUPPORTEDPHALANXEG);
    weights.push_back(ADVANCABLEPHALANXEG);
    weights.push_back(R_ADVANCABLEPHALANXEG);
    // Add the passed pawn bonus list
    for (U8 rank = 0; rank < 7; rank++){
        weights.push_back(passedRankBonusEg[rank]);
    }
    weights.push_back(PASSEDPATHBONUSEG);
    weights.push_back(SUPPORTEDPASSEREG);

    // Add the shelter weights
    weights.push_back(INNERSHELTEREG);
    weights.push_back(OUTERSHELTEREG);

    // Add the bishop pair bonus
    weights.push_back(BISHOPPAIREG);

    // Add the rook and queen on open file bonus
    weights.push_back(ROOKONOPENFILEEG);
    weights.push_back(ROOKONSEMIOPENFILEEG);

    // Add the tempo bonus
    weights.push_back(TEMPOEG);

    return weights;
}
    
// Returns the evaluation features tensor
// This will be used to tune the evaluation function
void getEvalFeaturesTensor(Position *pos, S8* tensor, S32 tensorSize){

    auto const& bb = pos->bitboards;
    auto const& occ = pos->occupancies;
    auto us = pos->side;
    Square whiteKing = lsb(bb[K]);
    Square blackKing = lsb(bb[k]);

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

    // Add the material weights
    for (Piece piece = P; piece <= Q; piece++){
        tensor[piece] = popcount(pos->bitboards[piece]) - popcount(pos->bitboards[piece + 6]);
    }
    tensor += 5;



    // Add the psqt weights
    for (Square square = a8; square < noSquare; square++){
        Piece piece = pos->pieceOn(square);
        if (piece == NOPIECE) continue;
        Square sq = piece < p ? square : flipSquare(square);
        tensor[64 * (piece % 6) + sq] += piece < p ? 1 : -1;
    }
    tensor += 64 * 6;



    // Add the mobility weights

    // Mobility calculations
    BitBoard pawnAttackedSquares[2] = {
        ((bb[P] & notFile(0)) >> 9) | ((bb[P] & notFile(7)) >> 7),
        ((bb[p] & notFile(0)) << 7) | ((bb[p] & notFile(7)) << 9)};

    // Pinned mask
    BitBoard RQmask[2] = {
        bb[R] | bb[Q],
        bb[r] | bb[q]};
    BitBoard BQmask[2] = {
        bb[B] | bb[Q],
        bb[b] | bb[q]};
    BitBoard pinned[2] = {
        getPinnedPieces(occ[BOTH], occ[WHITE], whiteKing, RQmask[BLACK], BQmask[BLACK]),
        getPinnedPieces(occ[BOTH], occ[BLACK], blackKing, RQmask[WHITE], BQmask[WHITE])};
    BitBoard blockedPawns[2] = {
        ((bb[P] >> 8) & occ[BOTH]) << 8,
        ((bb[p] << 8) & occ[BOTH]) >> 8};
    BitBoard underDevelopedPawns[2] = {
        bb[P] & (ranks(6) | ranks(5)),
        bb[p] & (ranks(1) | ranks(2))};
    BitBoard mobilityArea[2] = {
        // Mobility area.
        // Squares in the mobility area are:
        // 1. Free of our pieces
        // 2. Not defended by enemy pawns

        // Calculate a negative mask for the mobility area, and then invert it
        (blockedPawns[WHITE] | underDevelopedPawns[WHITE] | pawnAttackedSquares[BLACK] | pinned[WHITE] | bb[K] | bb[Q]) ^ 0xFFFFFFFFFFFFFFFF,
        (blockedPawns[BLACK] | underDevelopedPawns[BLACK] | pawnAttackedSquares[WHITE] | pinned[BLACK] | bb[k] | bb[q]) ^ 0xFFFFFFFFFFFFFFFF
    };

    // Calculate the mobility scores (index by phase and color)
    getMobilityFeat<N>(bb, occ, pinned, mobilityArea, tensor);
    getMobilityFeat<n>(bb, occ, pinned, mobilityArea, tensor);
    tensor += 9;
    getMobilityFeat<B>(bb, occ, pinned, mobilityArea, tensor);
    getMobilityFeat<b>(bb, occ, pinned, mobilityArea, tensor);
    tensor += 14;
    getMobilityFeat<R>(bb, occ, pinned, mobilityArea, tensor);
    getMobilityFeat<r>(bb, occ, pinned, mobilityArea, tensor);
    tensor += 15;
    getMobilityFeat<Q>(bb, occ, pinned, mobilityArea, tensor);
    getMobilityFeat<q>(bb, occ, pinned, mobilityArea, tensor);
    tensor += 28;



    // Add the pawn evaluation weights
    // Malus for directly doubled, undefended pawns
    BitBoard protectedPawns[2] = {
        bb[P] & pawnAttackedSquares[WHITE],
        bb[p] & pawnAttackedSquares[BLACK]
    };

    BitBoard pawnBlockage[2] = {
        bb[P] | pawnAttackedSquares[WHITE],
        bb[p] | pawnAttackedSquares[BLACK]
    };

    BitBoard doubledPawns[2] = { bb[P] & (bb[P] << 8), bb[p] & (bb[p] >> 8) };
    BitBoard pawnFiles[2] = { filesFromBB(bb[P]), filesFromBB(bb[p]) };
    // White pawns
    BitBoard pieces = bb[P];
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
            BitBoard passedPath = advancePathMasked<WHITE>(sqb, ~bb[BOTH]);
            // Give bonus for how close the pawn is to the promotion square
            tensor[8 + rank] += 1;
            tensor[8 + 7] += popcount(passedPath);
            // Bonus for connected or supported passed pawns
            if (supported) tensor[8 + 7 + 1]++;
        }
    }
    

    // Black pawns
    pieces = bb[p];
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
            BitBoard passedPath = advancePathMasked<BLACK>(sqb, ~bb[BOTH]);
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

    // Add the tempo bonus
    tensor[0] += us == WHITE ? 1 : -1;

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
    std::cout << "Allocating for " << weightCount << " weights: " << 1 + weightCount / 2 + 1 << " bytes per entry" << std::endl;
    U32 entrySize = 1 + weightCount / 2 + 1; // The entry size is the 1 (gamephase) + number of weights divided by 2 + 1, since we add the result at the end, but we don't need to write the features twice for each side
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
            getEvalFeaturesTensor(&pos, features, entrySize);

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