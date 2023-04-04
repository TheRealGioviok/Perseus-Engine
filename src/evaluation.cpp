
#include "evaluation.h"
#include "tables.h"
#include "tt.h"
#include "movegen.h"
#include "types.h"
#include <iostream>

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

constexpr Score weakDoubledMg = -5;
constexpr Score weakDoubledEg = -15;

constexpr Score doubledMg = -2;
constexpr Score doubledEg = -8;

constexpr Score DOUBLEDEARLYMG = 11;
constexpr Score DOUBLEDEARLYEG = 4;

constexpr Score DOUBLEISOLATEDPENMG = 7;
constexpr Score DOUBLEISOLATEDPENEG = 33;

constexpr Score OWNPIECEBLOCKEDPAWNMG = 0;
constexpr Score OWNPIECEBLOCKEDPAWNEG = 8;

constexpr Score ISOLATEDPENMG = 1;
constexpr Score ISOLATEDPENEG = 13;

constexpr Score BACKWARDPENMG = 7;
constexpr Score BACKWARDPENEG = 17;

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

constexpr Score MG_TEMPO = 18;
constexpr Score EG_TEMPO = 0;

#define MOBILITYEVAL true
#define PAWNEVAL true

Score passedRankBonusMg[7] = { 0, 8, 14, 12, 52, 140, 229 };
Score passedRankBonusEg[7] = { 0, 20, 24, 30, 52, 129, 189 };
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

static inline constexpr BitBoard ne(BitBoard bb) {
    return (bb & notFile(7)) << 9;
}

static inline constexpr BitBoard nw(BitBoard bb) {
    return (bb & notFile(0)) << 7;
}

static inline constexpr BitBoard se(BitBoard bb) {
    return (bb & notFile(7)) >> 7;
}

static inline constexpr BitBoard sw(BitBoard bb) {
    return (bb & notFile(0)) >> 9;
}

template <Piece pt>
//(bb, occ, pinned, mobilityArea, mobilityScore);
static inline void mobility(const BitBoard (&bb)[12], const BitBoard (&occ)[3], const BitBoard (&pinned)[2], const BitBoard (&mobilityArea)[2], Score (&mobilityScore)[2][2]) {
    constexpr Side us = pt < p ? WHITE : BLACK;
    constexpr Side them = us ? BLACK : WHITE;
    BitBoard mob;
    BitBoard pieces = bb[pt] & ~pinned[us];
    BitBoard occCheck = occ[BOTH];

    // Queen squares are already excluded from the mobility area calculation
    if constexpr (pt == R || pt == Q)                   occCheck ^= bb[R];
    else if constexpr (pt == r || pt == q)              occCheck ^= bb[r];
    
    // Iterate over all pieces of the given type
    while (pieces) {
        Square sq = popLsb(pieces);
        // Get the moves, according to the piece type
        mob = mobilityArea[us];
        if constexpr (pt == N || pt == n) {
            mob &= knightAttacks[sq];
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][us] += knightMobMg[moveCount];
            mobilityScore[1][us] += knightMobEg[moveCount];
        }
        else if constexpr (pt == B) {
            mob &= getBishopAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][us] += bishopMobMg[moveCount];
            mobilityScore[1][us] += bishopMobEg[moveCount];
        }
        else if constexpr (pt == R) {
            mob &= getRookAttack(sq, occCheck); // X-ray through our queens and rooks
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][us] += rookMobMg[moveCount];
            mobilityScore[1][us] += rookMobEg[moveCount];
        }
        else if constexpr (pt == Q) {
            mob &= getQueenAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][us] += queenMobMg[moveCount];
            mobilityScore[1][us] += queenMobEg[moveCount];
        }
        else if constexpr (pt == b) {
            mob &= getBishopAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][them] += bishopMobMg[moveCount];
            mobilityScore[1][them] += bishopMobEg[moveCount];
        }
        else if constexpr (pt == r) {
            mob &= getRookAttack(sq, occCheck); // X-ray through our queens and rooks
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][them] += rookMobMg[moveCount];
            mobilityScore[1][them] += rookMobEg[moveCount];
        }
        else if constexpr (pt == q) {
            mob &= getQueenAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][them] += queenMobMg[moveCount];
            mobilityScore[1][them] += queenMobEg[moveCount];
        }
    }
}

#define TEMPOMG 14
#define TEMPOEG 3
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

    Score mgScore[2] = { 0,0 }, egScore[2] = { 0,0 };
    Square whiteKing = lsb(bb[K]);
    Square blackKing = lsb(bb[k]);

    // Mobility calculation
    BitBoard pawnAttackedSquares[2] = {
        (sw(bb[P]) | se(bb[P])),
        (nw(bb[p]) | ne(bb[p]))
    };

    // Doubled pawns calculation
    BitBoard doubledPawns[2] = {
        (bb[P] & (bb[P] >> 8)),
        (bb[p] & (bb[p] << 8))
    }; // Of the couple, we take the advanced pawn

    BitBoard weakDoubledPawns[2] {
        (doubledPawns[WHITE] & ~pawnAttackedSquares[WHITE]),
        (doubledPawns[BLACK] & ~pawnAttackedSquares[BLACK])
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

    BitBoard mobilityArea[2] = {
        // Mobility area. 
        // Squares in the mobility area are:
        // 1. Free of our pieces
        // 2. Not defended by enemy pawns
        
        // Calculate a negative mask for the mobility area, and then invert it
        (occ[WHITE] | pawnAttackedSquares[BLACK]) ^ 0xFFFFFFFFFFFFFFFF,
        (occ[BLACK] | pawnAttackedSquares[WHITE]) ^ 0xFFFFFFFFFFFFFFFF
    };

    // Calculate the mobility scores (index by phase and color)
    Score mobilityScore[2][2] = {
        { 0,0 },
        { 0,0 }
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

    // Calculate the total score
    mgScore[WHITE] += mobilityScore[0][WHITE];
    mgScore[BLACK] += mobilityScore[0][BLACK];
    egScore[WHITE] += mobilityScore[1][WHITE];
    egScore[BLACK] += mobilityScore[1][BLACK];

    // Calculate the weak doubled pawn score
    U8 weakDoubledCount[2] = {
        popcount(weakDoubledPawns[WHITE]),
        popcount(weakDoubledPawns[BLACK])
    };
    U8 doubledCount[2] = {
        popcount(doubledPawns[WHITE]),
        popcount(doubledPawns[BLACK])
    };
    
    // Add weak doubled pawn score
    mgScore[WHITE] += weakDoubledCount[WHITE] * weakDoubledMg;
    mgScore[BLACK] += weakDoubledCount[BLACK] * weakDoubledMg;
    egScore[WHITE] += weakDoubledCount[WHITE] * weakDoubledEg;
    egScore[BLACK] += weakDoubledCount[BLACK] * weakDoubledEg;

    // Add doubled pawn score
    mgScore[WHITE] += doubledCount[WHITE] * doubledMg;
    mgScore[BLACK] += doubledCount[BLACK] * doubledMg;
    egScore[WHITE] += doubledCount[WHITE] * doubledEg;
    egScore[BLACK] += doubledCount[BLACK] * doubledEg;

    // Calculate mg and eg scores
    Score mg = mgScore[WHITE] - mgScore[BLACK];
    Score eg = egScore[WHITE] - egScore[BLACK];

    Score sign = 1 - 2*pos->side;
    return sign * 
        (
            (pos->psqtScore[0] + mg) * gamePhase +
            (pos->psqtScore[1] + eg) * (24 - gamePhase)
        ) / 24;
}

