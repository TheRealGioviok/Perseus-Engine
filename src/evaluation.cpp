
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

constexpr Score DOUBLEDPENMG = 7;
constexpr Score DOUBLEDPENEG = 33;

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
    // constexpr Side them = us ? BLACK : WHITE;
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
        else if constexpr (pt == B || pt == b) {
            mob &= getBishopAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][us] += bishopMobMg[moveCount];
            mobilityScore[1][us] += bishopMobEg[moveCount];
        }
        else if constexpr (pt == R || pt == r) {
            mob &= getRookAttack(sq, occCheck); // X-ray through our queens and rooks
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][us] += rookMobMg[moveCount];
            mobilityScore[1][us] += rookMobEg[moveCount];
        }
        else if constexpr (pt == Q || pt == q) {
            mob &= getQueenAttack(sq, occCheck); // X-ray through our queens
            U8 moveCount = popcount(mob);
            // Add the mobility score
            mobilityScore[0][us] += queenMobMg[moveCount];
            mobilityScore[1][us] += queenMobEg[moveCount];
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

    BitBoard mobilityArea[2] = {
        // Mobility area. 
        // Squares in the mobility area are:
        // 1. Free of our pieces
        // 2. Not defended by enemy pawns
        
        // Calculate a negative mask for the mobility area, and then invert it
        (occ[WHITE] | pawnAttackedSquares[BLACK] | pinned[WHITE]) ^ 0xFFFFFFFFFFFFFFFF,
        (occ[BLACK] | pawnAttackedSquares[WHITE] | pinned[BLACK]) ^ 0xFFFFFFFFFFFFFFFF
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

Score pestoEval2(Position* pos) {

#if USINGEVALCACHE
    Score cachedEval = getCachedEval(pos->hashKey);
    if (cachedEval != noScore) {
        int fCs = std::min(60, std::max(0, pos->fiftyMove - 12));
        return cachedEval * (100 - fCs) / 100;
    }
#endif

    auto const& bb = pos->bitboards;
    Score mg[2] = { 0,0 }, eg[2] = { 0,0 };
    Score whiteMaterial, blackMaterial;
    Score gamePhase;
    BitBoard whiteOccupancy, blackOccupancy, occupancy;
    BitBoard whiteNonPawns, blackNonPawns;
    BitBoard attackedByWhitePawns, attackedByBlackPawns;
    BitBoard unadvancablePawnsFor[2] = { 0,0 };
    BitBoard whiteRearSpan, blackRearSpan;

    BitBoard whiteBQ, whiteRQ, whiteKQ;
    BitBoard blackBQ, blackRQ, blackKQ;
    Square whiteKing, blackKing;

    U8 openFileFor[2] = { 0,0 };
    U8 openFile;
    S8 pawnStormTo[2] = { 0,0 };
    S32 tropismToWhiteKing, tropismToBlackKing;

    BitBoard pinnedWhite;
    BitBoard pinnedBlack;

    BitBoard kingShelter[2] = { 0,0 };

    BitBoard blockedWhitePawns, blockedBlackPawns;
    U8 blockedOnMiddleWhitePawns, blockedOnMiddleBlackPawns;
    BitBoard whiteOnWhitePawns, blackOnWhitePawns, whiteOnBlackPawns, blackOnBlackPawns;
    U8 cWW, cBW, cWB, cBB;

    Score mgScore = 0;
    Score egScore = 0;


    gamePhase = 24;

    whiteOccupancy = (bb[P] | bb[N]) | (bb[B] | bb[R]) | (bb[Q] | bb[K]);
    blackOccupancy = (bb[p] | bb[n]) | (bb[b] | bb[r]) | (bb[q] | bb[k]);
    occupancy = whiteOccupancy | blackOccupancy;

    whiteNonPawns = whiteOccupancy ^ bb[P];
    blackNonPawns = blackOccupancy ^ bb[p];

		

    attackedByWhitePawns = ((bb[P] & notFile(0)) >> 9) | ((bb[P] & notFile(7)) >> 7);
    attackedByBlackPawns = ((bb[p] & notFile(0)) << 7) | ((bb[p] & notFile(7)) << 9);

    whiteRearSpan = blackRearSpan = 0ULL;

    whiteBQ = bb[B] | bb[Q];
    whiteRQ = bb[R] | bb[Q];
    whiteKQ = bb[K] | bb[Q];
    blackBQ = bb[b] | bb[q];
    blackRQ = bb[r] | bb[q];
    blackKQ = bb[k] | bb[q];
    whiteKing = lsb(bb[K]);
    blackKing = lsb(bb[k]);

    openFileFor[WHITE] = openFileFor[BLACK] = 0xFF;
    pawnStormTo[WHITE] = pawnStormTo[BLACK] = 0;
    pinnedWhite = getPinnedPieces(occupancy, whiteOccupancy, whiteKing, blackRQ, blackBQ);
    pinnedBlack = getPinnedPieces(occupancy, blackOccupancy, blackKing, whiteRQ, whiteBQ);

    // King shelter
    kingShelter[WHITE] = rankOf(whiteKing) > 0 ? (kingAttacks[whiteKing - 8] & notRank(rankOf(whiteKing))) : 0;
    kingShelter[BLACK] = rankOf(blackKing) < 7 ? (kingAttacks[blackKing + 8] & notRank(rankOf(blackKing))) : 0;

    // TODO: https://www.talkchess.com/forum3/viewtopic.php?f=7&t=76976 (interesting king safety topics)

    // Mobility area calculations
	unadvancablePawnsFor[WHITE] = unadvancablePawnsFor[BLACK] = 0;
    blockedWhitePawns = ((bb[P] >> 8) & occupancy) << 8;
    blockedBlackPawns = ((bb[p] << 8) & occupancy) >> 8;
    blockedOnMiddleWhitePawns = popcount(blockedWhitePawns & centralFiles());
    blockedOnMiddleBlackPawns = popcount(blockedBlackPawns & centralFiles());
    whiteOnWhitePawns = bb[P] & squaresOfColor[WHITE];
    blackOnWhitePawns = bb[p] & squaresOfColor[WHITE];
    whiteOnBlackPawns = bb[P] & squaresOfColor[BLACK];
    blackOnBlackPawns = bb[p] & squaresOfColor[BLACK];
    cWW = popcount(whiteOnWhitePawns);
    cBW = popcount(blackOnWhitePawns);
    cWB = popcount(whiteOnBlackPawns);
    cBB = popcount(blackOnBlackPawns);

    BitBoard whitePawnsFirstRanks = bb[P] & (ranks(6) | ranks(5) | ranks(4));
    BitBoard blackPawnsFirstRanks = bb[p] & (ranks(1) | ranks(2) | ranks(3));

    BitBoard mobilityAreaWhite = (0xFFFFFFFFFFFFFFFF ^ ((whiteKQ | attackedByBlackPawns) | (blockedWhitePawns | whitePawnsFirstRanks)));
    BitBoard mobilityAreaBlack = (0xFFFFFFFFFFFFFFFF ^ ((blackKQ | attackedByWhitePawns) | (blockedBlackPawns | blackPawnsFirstRanks)));

    U8 whitePawnCount = popcount(bb[P]);
    U8 blackPawnCount = popcount(bb[p]);

    whiteMaterial = blackMaterial = 0;

    tropismToWhiteKing = tropismToBlackKing = 0;

    whiteMaterial += whitePawnCount * mgValues[P];
    blackMaterial += blackPawnCount * mgValues[P];

    int side = pos->side;
    int other = side ^ 1;
    int whiteKingShield = (rankOf(whiteKing) > 0 ? popcount(kingAttacks[whiteKing] & notRank(rankOf(whiteKing) - 1)) : 0);
    int blackKingShield = (rankOf(blackKing) < 7 ? popcount(kingAttacks[blackKing] & notRank(rankOf(blackKing) + 1)) : 0);

	bool KPendgame = ((bb[K] | bb[k]) | (bb[P] | bb[p])) == occupancy;
    if (KPendgame) {
        if (!bb[p]) eg[WHITE] += egValues[Q] - egValues[P];
        if (!bb[P]) eg[BLACK] += egValues[Q] - egValues[P];                 
    }


    Score whiteNonPawnMaterial = whiteMaterial - mgValues[P] * whitePawnCount;
    Score blackNonPawnMaterial = blackMaterial - mgValues[P] * blackPawnCount;

    BitBoard supportedWhitePawns = attackedByWhitePawns & bb[P];
    BitBoard supportedBlackPawns = attackedByBlackPawns & bb[p];

    BitBoard kingAsterix[2] = { getQueenAttack(whiteKing, occupancy), getQueenAttack(blackKing, occupancy) };
    BitBoard kingKnightZone[2] = { knightAttacks[whiteKing], knightAttacks[blackKing] };
    BitBoard kingDangerZone[2] = { kingAsterix[WHITE] & fiveSquare[whiteKing], kingAsterix[BLACK] & fiveSquare[blackKing] };
	BitBoard kingDangerSquare[2] = { kingAttacks[whiteKing], kingAttacks[blackKing] };
    Score kingDangerScores[2] = { 0, 0 };
    
    BitBoard pawns = bb[P]; // We need to use a temporary variable because we will modify it
    // For each pawn:
    while (pawns) {

        // Get the pawn square
        Square pawn = popLsb(pawns);

        mg[WHITE] += mgTables[P][pawn];
        eg[WHITE] += egTables[P][pawn];

        // Compute the square bitboard mask
        BitBoard pawnBB = squareBB(pawn);

        // Compute the square rank
        S8 pawnRank = 7 - rankOf(pawn);
        U8 pawnFile = fileOf(pawn);

        // Add rearSpanOf
        whiteRearSpan |= squaresBehind(pawn);

        // Open file calculations
        clearBit(openFileFor[BLACK], pawnFile);

        // Useful bitboards
        BitBoard forward = squaresAhead(pawn);
        BitBoard backward = squaresBehind(pawn);
        Square frontSquare = pawn - 8;

        bool isOpposed = forward & bb[p]; // Is the pawn opposed to an enemy pawn?
        bool isDirectlyOpposed = squareBB(frontSquare) & bb[p]; // Is the pawn directly opposed to an enemy pawn?
        bool isBehindFriendly = forward & bb[P];      // Is the pawn behind a friendly pawn?
        bool isDoubled = backward & bb[P]; // Is the pawn doubled?
        bool isSupported = pawnBB & supportedWhitePawns; // Is the pawn supported?
        bool isIsolated = !(isolatedPawnMask[pawn] & bb[P]); // Is the pawn isolated? (No friendly pawns on the flanks)
        bool isBackward = (!(backwardPawnMask[WHITE][pawn] & bb[P])) && (isDirectlyOpposed || ((pawnBB>>8) & attackedByBlackPawns)); // Is the pawn backward? (has no friendly pawns behind on flank files and is stopped by enemy pawn from advancing)
        bool isPhalanx = phalanx[pawn] & bb[P]; // Is the pawn a phalanx (at least one friendly pawn on adjacent file and same rank)
        bool isConnected = isSupported || isPhalanx;
        bool isWeakUnopposed = (!isOpposed) && (isIsolated || isBackward); // Is the pawn weak (isolated or backward), while not being opposed by enemy pawn?
        bool isWeakLever = !isSupported && (popcount(pawnAttacks[WHITE][pawn] & bb[p]) == 2); // Is the pawn weak (lever), while not being opposed by enemy pawn?
        bool isOwnPieceBlocked = ((pawnBB >> 8) & whiteOccupancy) && !isSupported && !isDoubled;
        S8 r56 = std::min(2, std::max(0, pawnRank - 3)) * (isDirectlyOpposed);
        Score blocked56mg = blockBonusMg[r56] ; // Is the pawn blocked by a pawn on the 5th or 6th rank? 
        Score blocked56eg = blockBonusEg[r56]; // Is the pawn blocked by a pawn on the 5th or 6th rank?
        Score connectedScore = (isConnected) ? connectedBonus(pawnRank, isOpposed, isPhalanx, isSupported) : 0; // How good is the connected pawn?
		bool isUnadvancable = isDirectlyOpposed || (popcount(pawnAttacks[WHITE][frontSquare] & bb[p]) == 2); // Is the pawn unadvancable? (stopped by enemy pawns or directly opposed)
        bool earlyDoubled = !(bb[P] & ((bb[p] | attackedByBlackPawns) >> 8));
        // Update bitboard masks (TODO)
		unadvancablePawnsFor[WHITE] |= pawnBB * isUnadvancable;
        // Add scores

        // mg
        mg[WHITE] -= isIsolated * ISOLATEDPENMG;
        mg[WHITE] -= isBackward * BACKWARDPENMG;
        

		mg[WHITE] -= isDoubled * (DOUBLEDPENMG + earlyDoubled * DOUBLEDEARLYMG);
        mg[WHITE] -= isWeakUnopposed * WEAKUNOPPOSEDMG;

        mg[WHITE] += connectedScore;
        mg[WHITE] += blocked56mg;
        mg[WHITE] -= isOwnPieceBlocked * OWNPIECEBLOCKEDPAWNMG * (7 - pawnRank);

        // mg
        eg[WHITE] -= isIsolated * ISOLATEDPENEG;
        eg[WHITE] -= isBackward * BACKWARDPENEG;


        eg[WHITE] -= isDoubled * (DOUBLEDPENEG + earlyDoubled * DOUBLEDEARLYEG);
		
        eg[WHITE] -= isWeakUnopposed * WEAKUNOPPOSEDPENEG;
        eg[WHITE] -= isWeakLever * WEAKLEVEREG;
        eg[WHITE] -= isOwnPieceBlocked * OWNPIECEBLOCKEDPAWNEG * (7-pawnRank);

        eg[WHITE] += connectedScore * ((pawnRank - 2) / 4);
        eg[WHITE] += blocked56eg;
        
        // Passed pawns
        // We need to identify passed pawns, as well as candidate passers.
        // A pawn is passed if one of the three following conditions is true:
        // (a) there is no stoppers except some levers
        // (b) the only stoppers are the leverPush, but we outnumber them
        // (c) there is only one front stopper which can be levered.
        // If there is a pawn of our color in the same file in front of a current pawn it's no longer counts as passed.

        bool candidate = !isBehindFriendly && (popcount((passedPawnMask[WHITE][pawn] ^ pawnAttacks[WHITE][pawn]) & bb[p]) <= popcount((pawnAttacks[BLACK][pawn]) & bb[P]) - popcount((phalanx[pawn]) & bb[p]));
		bool passed = candidate && ((passedPawnMask[WHITE][pawn] & bb[p]) == 0) && ((passedPawnMask[WHITE][pawn] & attackedByBlackPawns) == 0);
        
        Score candidateScore = candidate + 3 * passed + (isConnected && candidate) + (!isUnadvancable && candidate);
        
        // Add scores
        // mg
        mg[WHITE] += (candidateScore * passedRankBonusMg[pawnRank])>>2;
        // TODO: consider the whole path to promotion (needs  to have attack mask set)
        mg[WHITE] -= (candidateScore * 6 * std::min(pawnFile, U8(7 - pawnFile)))>>2;
        // eg
        eg[WHITE] += (candidateScore * passedRankBonusEg[pawnRank])>>2;
        // TODO: consider the whole path to promotion (needs  to have attack mask set)
        eg[WHITE] -= (candidateScore * 4 * std::min(pawnFile, U8(7 - pawnFile))) >> 2;

        Score pawnEgScore = (1 + !isOpposed + isConnected - isDoubled) * ((1 + (KPendgame && candidate)) << candidate);
        eg[WHITE] += (7 - chebyshevDistance[pawn][whiteKing]) * pawnEgScore;
        eg[WHITE] -= (7 - chebyshevDistance[pawn][blackKing]) * pawnEgScore;
        if (KPendgame && passed) {
            Square promoSquare = lsb(squaresAhead(pawn) & ranks(0));
            eg[WHITE] += 500 * (std::min(U8(5), chebyshevDistance[pawn][promoSquare]) < (chebyshevDistance[blackKing][promoSquare] - side));
        }
        pawnStormTo[BLACK] += std::max(0, (5 + candidate + isSupported + (passed && isSupported) - isWeakLever - isOpposed - (S8)chebyshevDistance[pawn][blackKing]));
    }

    // BLACK
    pawns = bb[p]; // We need to use a temporary variable because we will modify it
    // For each pawn
    while (pawns) { 

        // Get the pawn square
        Square pawn = popLsb(pawns);

        // Compute the square bitboard mask
        BitBoard pawnBB = squareBB(pawn);

        mg[BLACK] += mgTables[p][pawn];
        eg[BLACK] += egTables[p][pawn];

        // Compute the square rank
        S8 pawnRank = rankOf(pawn);
        U8 pawnFile = fileOf(pawn);

        // Add rearSpanOf
        blackRearSpan |= squaresAhead(pawn);

        // Open file calculations
        clearBit(openFileFor[WHITE], pawnFile);

        // Useful bitboards
        BitBoard forward = squaresBehind(pawn);
        BitBoard backward = squaresAhead(pawn);
        Square frontSquare = pawn + 8;

        bool isOpposed = forward & bb[P]; // Is the pawn opposed to an enemy pawn?
        bool isDirectlyOpposed = squareBB(frontSquare) & bb[P]; // Is the pawn directly opposed to an enemy pawn?
        bool isBehindFriendly = forward & bb[p];      // Is the pawn behind a friendly pawn?
        bool isDoubled = backward & bb[p]; // Is the pawn doubled?
        bool isSupported = pawnBB & supportedBlackPawns; // Is the pawn supported?
        bool isIsolated = !(isolatedPawnMask[pawn] & bb[p]); // Is the pawn isolated? (No friendly pawns on the flanks)
        bool isBackward = (!(backwardPawnMask[BLACK][pawn] & bb[p])) && (isDirectlyOpposed || ((pawnBB << 8) & attackedByWhitePawns)); // Is the pawn backward? (has no friendly pawns behind on flank files and is stopped by enemy pawn from advancing)
        bool isPhalanx = phalanx[pawn] & bb[p]; // Is the pawn a phalanx (at least one friendly pawn on adjacent file and same rank)
        bool isConnected = isSupported || isPhalanx;
        bool isWeakUnopposed = (!isOpposed) && (isIsolated || isBackward); // Is the pawn weak (isolated or backward), while not being opposed by enemy pawn?
        bool isOwnPieceBlocked = ((pawnBB << 8) & blackOccupancy) && !isSupported && !isDoubled;
        bool isWeakLever = !isSupported && (popcount(pawnAttacks[BLACK][pawn] & bb[P]) == 2); // Is the pawn weak (lever), while not being opposed by enemy pawn?
        S8 r56 = std::min(2, std::max(0, pawnRank - 3)) * (isDirectlyOpposed);
        Score blocked56mg = blockBonusMg[r56]; // Is the pawn blocked by a pawn on the 5th or 6th rank? 
        Score blocked56eg = blockBonusEg[r56]; // Is the pawn blocked by a pawn on the 5th or 6th rank?
		Score connectedScore = (isConnected) ? connectedBonus(pawnRank, isOpposed, isPhalanx, isSupported) : 0; // How good is the connected pawn?
		bool isUnadvancable = isDirectlyOpposed || (popcount(pawnAttacks[BLACK][frontSquare] & bb[P]) == 2); // Is the pawn unadvancable? (stopped by enemy pawns or directly opposed)
        bool earlyDoubled = !(bb[p] & ((bb[P] | attackedByWhitePawns) << 8));
        // Update bitboard masks (TODO)
        unadvancablePawnsFor[BLACK] |= pawnBB * isUnadvancable;

        // Add scores

        // mg
        mg[BLACK] -= isIsolated * ISOLATEDPENMG;
        mg[BLACK] -= isBackward * BACKWARDPENMG;


        mg[BLACK] -= isDoubled * (DOUBLEDPENMG + earlyDoubled * DOUBLEDEARLYMG);
		
        mg[BLACK] -= isWeakUnopposed * WEAKUNOPPOSEDMG;

        mg[BLACK] += connectedScore;
        mg[BLACK] += blocked56mg;
        mg[BLACK] -= isOwnPieceBlocked * OWNPIECEBLOCKEDPAWNMG * (7-pawnRank);

        // mg
        eg[BLACK] -= isIsolated * ISOLATEDPENEG;
        eg[BLACK] -= isBackward * BACKWARDPENEG;


        eg[BLACK] -= isDoubled * (DOUBLEDPENEG + earlyDoubled * DOUBLEDEARLYEG);
		
        eg[BLACK] -= isWeakUnopposed * WEAKUNOPPOSEDPENEG;
        eg[BLACK] -= isWeakLever * WEAKLEVEREG;
        eg[BLACK] -= isOwnPieceBlocked * OWNPIECEBLOCKEDPAWNEG * (7-pawnRank);

        eg[BLACK] += connectedScore * ((pawnRank - 2) / 4);
        eg[BLACK] += blocked56eg;

        // Passed pawns
        // We need to identify passed pawns, as well as candidate passers.
        // A pawn is passed if one of the three following conditions is true:
        // (a) there is no stoppers except some levers
        // (b) the only stoppers are the leverPush, but we outnumber them
        // (c) there is only one front stopper which can be levered.
        // If there is a pawn of our color in the same file in front of a current pawn it's no longer counts as passed.

        bool candidate = !isBehindFriendly && (popcount((passedPawnMask[BLACK][pawn] ^ pawnAttacks[BLACK][pawn]) & bb[P]) <= popcount((pawnAttacks[WHITE][pawn]) & bb[p]) - popcount((phalanx[pawn]) & bb[P]));
        bool passed = candidate && ((passedPawnMask[BLACK][pawn] & bb[P]) == 0) && ((passedPawnMask[BLACK][pawn] & attackedByWhitePawns) == 0);

        Score candidateScore = candidate + 3 * passed + (isConnected && candidate) + (!isUnadvancable && candidate);

        // Add scores
        // mg
        mg[BLACK] += (candidateScore * passedRankBonusMg[pawnRank]) >> 2;
        // TODO: consider the whole path to promotion (needs  to have attack mask set)
        mg[BLACK] -= (candidateScore * 6 * std::min(pawnFile, U8(7 - pawnFile))) >> 2;
        // eg
        eg[BLACK] += (candidateScore * passedRankBonusEg[pawnRank]) >> 2;
        // TODO: consider the whole path to promotion (needs  to have attack mask set)
        eg[BLACK] -= (candidateScore * 4 * std::min(pawnFile, U8(7 - pawnFile))) >> 2;

        Score pawnEgScore = (1 + !isOpposed + isConnected - isDoubled) * ((1 + (KPendgame && candidate)) << candidate);
        eg[BLACK] += (7 - chebyshevDistance[pawn][blackKing]) * pawnEgScore;
        eg[BLACK] -= (7 - chebyshevDistance[pawn][whiteKing]) * pawnEgScore;
        if (KPendgame && passed) {
            Square promoSquare = lsb(squaresBehind(pawn) & ranks(7));
            eg[BLACK] += 500 * (std::min(U8(5), chebyshevDistance[pawn][promoSquare]) < (chebyshevDistance[whiteKing][promoSquare] - 1 + side));
        }
        pawnStormTo[WHITE] += std::max(0, (5 + candidate + isSupported + (passed && isSupported) - isWeakLever - isOpposed - (S8)chebyshevDistance[pawn][whiteKing]));
    }

    openFile = openFileFor[WHITE] & openFileFor[BLACK];

    if (KPendgame) goto kingEval;

    //std::cout << "Pawn checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";

    // White knights
    {
        BitBoard knights = bb[N];
        while (knights) {

            whiteMaterial += mgValues[N];

            Square knight = popLsb(knights);

            tropismToBlackKing += nkdist[knight][blackKing];
            bool pawnSupported = attackedByWhitePawns & squareBB(knight);

            mg[WHITE] += mgTables[N][knight];
            eg[WHITE] += egTables[N][knight];
            
            BitBoard sqb = squareBB(knight);
            mg[WHITE] += MG_MINORBEHINDPAWN * ((bool)(sqb & whiteRearSpan));
            eg[WHITE] += MG_MINORBEHINDPAWN * ((bool)(sqb & whiteRearSpan));
            mg[WHITE] += pawnSupported * MG_PAWNMINORSUPPORTED;
            eg[WHITE] += pawnSupported * EG_PAWNMINORSUPPORTED;

            // Outpost calculations
            Score outpostBonusMg = knightOutpostMg(passedPawnMask[WHITE][knight] & bb[p], popcount(pawnAttacks[BLACK][knight] & bb[P]) + popcount(pawnAttacks[BLACK][knight] & supportedWhitePawns));
            Score outpostBonusEg = knightOutpostEg(passedPawnMask[WHITE][knight] & bb[p], popcount(pawnAttacks[BLACK][knight] & bb[P]) + popcount(pawnAttacks[BLACK][knight] & supportedWhitePawns));
            
            // Malus if on protected pawn attacked square
            bool safePawnThreat = pawnAttacks[BLACK][knight] & supportedBlackPawns;
            mg[WHITE] += outpostBonusMg - safePawnThreat * PAWNTHREATMG;
            eg[WHITE] += outpostBonusEg - safePawnThreat * PAWNTHREATEG;
			
			// King distance penalization for std::minor pieces
			mg[WHITE] -= (chebyshevDistance[knight][whiteKing]) * MG_KNIGHTKINGPROTECTOR;
			eg[WHITE] -= (chebyshevDistance[knight][whiteKing]) * EG_KNIGHTKINGPROTECTOR;

            // Mobility area calculations
            if (!(sqb & pinnedWhite)) {
                BitBoard moves = knightAttacks[knight];
                kingDangerScores[BLACK] +=
                    popcount(kingDangerSquare[BLACK] & moves) * KNIGHTATTACKINNERRING
                    + popcount(kingDangerZone[BLACK] & moves) * KNIGHTATTACKOUTERRING
                    + popcount(kingKnightZone[BLACK] & moves) * KNIGHTATTACKZONE;
                moves &= mobilityAreaWhite;
                U8 moveCount = popcount(moves);
                mg[WHITE] += knightMobMg[moveCount];
                eg[WHITE] += knightMobEg[moveCount];
				// In the middle game, knights are more valuable the more closed a position is
				mg[WHITE] += (whitePawnCount + blackPawnCount) + popcount(blockedBlackPawns);
            }
        }
    }
    // Black knights
    {
        BitBoard knights = bb[n];
        while (knights) {

            blackMaterial += mgValues[N];

            Square knight = popLsb(knights);

            tropismToWhiteKing += nkdist[knight][whiteKing];
            bool pawnSupported = attackedByBlackPawns & squareBB(knight); 

            mg[BLACK] += mgTables[n][knight];
            eg[BLACK] += egTables[n][knight];

            BitBoard sqb = squareBB(knight);

            mg[BLACK] += MG_MINORBEHINDPAWN * ((bool)(sqb & blackRearSpan));
            eg[BLACK] += MG_MINORBEHINDPAWN * ((bool)(sqb & blackRearSpan));
            mg[BLACK] += pawnSupported * MG_PAWNMINORSUPPORTED;
            eg[BLACK] += pawnSupported * EG_PAWNMINORSUPPORTED;

            // Outpost calculations
            Score outpostBonusMg = knightOutpostMg(passedPawnMask[BLACK][knight] & bb[P], popcount(pawnAttacks[WHITE][knight] & bb[p]) + popcount(pawnAttacks[WHITE][knight] & supportedBlackPawns));
            Score outpostBonusEg = knightOutpostEg(passedPawnMask[BLACK][knight] & bb[P], popcount(pawnAttacks[WHITE][knight] & bb[p]) + popcount(pawnAttacks[WHITE][knight] & supportedBlackPawns));

            // Malus if on protected pawn attacked square
            bool safePawnThreat = pawnAttacks[WHITE][knight] & supportedWhitePawns;
            mg[BLACK] += outpostBonusMg - safePawnThreat * PAWNTHREATMG;
            eg[BLACK] += outpostBonusEg - safePawnThreat * PAWNTHREATEG;

            // King distance penalization for std::minor pieces
            mg[BLACK] -= (chebyshevDistance[knight][blackKing]) * MG_KNIGHTKINGPROTECTOR;
            eg[BLACK] -= (chebyshevDistance[knight][blackKing]) * EG_KNIGHTKINGPROTECTOR;

            // Mobility area calculations
            if (!(sqb & pinnedBlack)) {
                BitBoard moves = knightAttacks[knight];
				kingDangerScores[WHITE] +=
					popcount(kingDangerSquare[WHITE] & moves) * KNIGHTATTACKINNERRING
					+ popcount(kingDangerZone[WHITE] & moves) * KNIGHTATTACKOUTERRING
					+ popcount(kingKnightZone[WHITE] & moves) * KNIGHTATTACKZONE;
                moves &= mobilityAreaBlack;
                U8 moveCount = popcount(moves);
                mg[BLACK] += knightMobMg[moveCount];
                eg[BLACK] += knightMobEg[moveCount];
                // In the middle game, knights are more valuable the more closed a position is
                mg[BLACK] += (whitePawnCount + blackPawnCount) + popcount(blockedWhitePawns);
            }
        }
    }

    //std::cout << "Knight checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";
    // White bishops
    {
        BitBoard bishops = bb[B];
        while (bishops) {

            whiteMaterial += mgValues[B];

            Square bishop = popLsb(bishops);

            tropismToBlackKing += kbdist[bishop][blackKing];

            mg[WHITE] += mgTables[B][bishop];
            eg[WHITE] += egTables[B][bishop];

            // we also will account for the mobility of the bishop
            // we will add the number of squares that the bishop can move to (and give them a score based on if they are attacked by an enemy pawn)
            BitBoard sqb = squareBB(bishop);
            bool pawnSupported = attackedByWhitePawns & sqb;

            
            mg[WHITE] += MG_MINORBEHINDPAWN * ((bool)(sqb & whiteRearSpan));
            eg[WHITE] += MG_MINORBEHINDPAWN * ((bool)(sqb & whiteRearSpan));
            mg[WHITE] += pawnSupported * MG_PAWNMINORSUPPORTED;
            eg[WHITE] += pawnSupported * EG_PAWNMINORSUPPORTED;

            bool bishopColor = sqb & squaresOfColor[BLACK];
            mg[WHITE] -= BISHOPPAWNSMG * (bishopColor ? cWB : cWW) * (1 + blockedOnMiddleWhitePawns);
            eg[WHITE] -= BISHOPPAWNSEG * (bishopColor ? cWB : cWW) * (1 + blockedOnMiddleWhitePawns);

            // Outpost calculations
            Score outpostBonusMg = bishopOutpostMg(passedPawnMask[WHITE][bishop] & bb[p], popcount(pawnAttacks[BLACK][bishop] & bb[P]));
            Score outpostBonusEg = bishopOutpostEg(passedPawnMask[WHITE][bishop] & bb[p], popcount(pawnAttacks[BLACK][bishop] & bb[P]));

            // Malus if on protected pawn attacked square
            bool safePawnThreat = pawnAttacks[BLACK][bishop] & supportedBlackPawns;
            mg[WHITE] += outpostBonusMg - safePawnThreat * PAWNTHREATMG;
            eg[WHITE] += outpostBonusEg - safePawnThreat * PAWNTHREATEG;
			
            // King distance penalization for std::minor pieces
            mg[WHITE] -= (chebyshevDistance[bishop][whiteKing]) * MG_BISHOPKINGPROTECTOR;
            eg[WHITE] -= (chebyshevDistance[bishop][whiteKing]) * EG_BISHOPKINGPROTECTOR;


            if (!(sqb & pinnedWhite)) {
                BitBoard mob = getBishopAttack((Square)bishop, occupancy ^ bb[Q]) & ~whiteOccupancy; // To count bishop-queen batteries, we exclude white queen from occupancy
                mob &= ~(bb[Q]); // To remove squares the queen is on
                kingDangerScores[BLACK] += 
                    popcount(kingDangerSquare[BLACK] & mob) * BISHOPATTACKINNERRING
                    + popcount(kingDangerZone[BLACK] & mob) * BISHOPATTACKOUTERRING;
                mob &= mobilityAreaWhite;
                U8 moveCount = popcount(mob);

                mg[WHITE] += bishopMobMg[moveCount];// +otherKingAttacked;
                eg[WHITE] += bishopMobEg[moveCount];// +otherKingAttacked;
            }
        }
    }
    // Black bishops
    {
        BitBoard bishops = bb[b];
        while (bishops) {

            blackMaterial += mgValues[B];

            Square bishop = popLsb(bishops);

            tropismToWhiteKing += kbdist[bishop][whiteKing];

            mg[BLACK] += mgTables[b][bishop];
            eg[BLACK] += egTables[b][bishop];

            BitBoard sqb = squareBB(bishop);
            bool pawnSupported = attackedByBlackPawns & sqb;

            mg[BLACK] += MG_MINORBEHINDPAWN * ((bool)(sqb & blackRearSpan));
            eg[BLACK] += MG_MINORBEHINDPAWN * ((bool)(sqb & blackRearSpan));
            mg[BLACK] += pawnSupported * MG_PAWNMINORSUPPORTED;
            eg[BLACK] += pawnSupported * EG_PAWNMINORSUPPORTED;

            bool bishopColor = sqb & squaresOfColor[BLACK];
            mg[BLACK] -= BISHOPPAWNSMG * (bishopColor ? cBB : cBW) * (1 + blockedOnMiddleBlackPawns);
            eg[BLACK] -= BISHOPPAWNSEG * (bishopColor ? cBB : cBW) * (1 + blockedOnMiddleBlackPawns);

            // Outpost calculations
            Score outpostBonusMg = bishopOutpostMg(passedPawnMask[BLACK][bishop] & bb[P], popcount(pawnAttacks[WHITE][bishop] & bb[p]));
            Score outpostBonusEg = bishopOutpostEg(passedPawnMask[BLACK][bishop] & bb[P], popcount(pawnAttacks[WHITE][bishop] & bb[p]));

            // Malus if on protected pawn attacked square
            bool safePawnThreat = pawnAttacks[WHITE][bishop] & supportedWhitePawns;
            mg[BLACK] += outpostBonusMg - safePawnThreat * PAWNTHREATMG;
            eg[BLACK] += outpostBonusEg - safePawnThreat * PAWNTHREATEG;

            // King distance penalization for std::minor pieces
            mg[BLACK] -= (chebyshevDistance[bishop][blackKing]) * MG_BISHOPKINGPROTECTOR;
            eg[BLACK] -= (chebyshevDistance[bishop][blackKing]) * EG_BISHOPKINGPROTECTOR;

            if (!(sqb & pinnedBlack)) {
                BitBoard mob = getBishopAttack((Square)bishop, occupancy ^ bb[q]) & ~blackOccupancy; // To count bishop-queen batteries, we exclude black queen from occupancy
                mob &= ~(bb[q]); // To remove squares the queen is on
				kingDangerScores[WHITE] +=
					popcount(kingDangerSquare[WHITE] & mob) * BISHOPATTACKINNERRING
					+ popcount(kingDangerZone[WHITE] & mob) * BISHOPATTACKOUTERRING;
                mob &= mobilityAreaBlack;
                U8 moveCount = popcount(mob);
                
                mg[BLACK] += bishopMobMg[moveCount];// +otherKingAttacked;
                eg[BLACK] += bishopMobEg[moveCount];// +otherKingAttacked;
            }
        }
    }
    //std::cout << "Bishop checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";
    // White rooks
    {
        BitBoard rooks = bb[R];
        while (rooks) {

            whiteMaterial += mgValues[R];

            Square rook = popLsb(rooks);
            U8 rookFile = fileOf(rook);

            tropismToBlackKing += rkdist[rook][blackKing];

            mg[WHITE] += mgTables[R][rook];
            eg[WHITE] += egTables[R][rook];

            // we also will account for the mobility of the rook
            // we will add the number of squares that the rook can move to (and give them a score based on if they are attacked by an enemy pawn)
            
            // We will give a bonus to the rook if it is on a semi-open file
            bool closed = (files(fileOf(rook)) & blockedWhitePawns);
            bool semiOpen = testBit(openFileFor[WHITE], rookFile);
            bool open = testBit(openFile, rookFile);
            bool onClearFile = (files(rookFile) & blackOccupancy) == 0;
            mg[WHITE] -= closed * MG_ROOKCLOSEDFILE;
            eg[WHITE] -= closed * EG_ROOKCLOSEDFILE;
            mg[WHITE] += open * MG_ROOKOPENFILE;
            eg[WHITE] += open * EG_ROOKOPENFILE;
            mg[WHITE] += onClearFile * MG_ROOKCLEARFILE;
            eg[WHITE] += onClearFile * EG_ROOKCLEARFILE;
            mg[WHITE] += semiOpen * MG_ROOKSEMIOPENFILE;
            eg[WHITE] += semiOpen * EG_ROOKSEMIOPENFILE;
            bool queenFile = files(rookFile) & (bb[Q] | bb[q]);
            mg[WHITE] += queenFile * ROOKONQUEENFILEMG;
            eg[WHITE] += queenFile * ROOKONQUEENFILEEG;

            if (!(squareBB(rook) & pinnedWhite)) {
                BitBoard mob = getRookAttack((Square)rook, occupancy ^ (whiteRQ)) & ~whiteOccupancy; // Include xray thorugh our rooks and queens
				kingDangerScores[BLACK] +=
					popcount(kingDangerSquare[BLACK] & (mob & ~whiteRQ)) * ROOKATTACKINNERRING
					+ popcount(kingDangerZone[BLACK] & (mob & ~whiteRQ)) * ROOKATTACKOUTERRING;
                mob &= mobilityAreaWhite;
                U8 moveCount = popcount(mob);
                //mob &= kingAttacks[blackKing];
                //U8 otherKingAttacked = popcount(mob);
                mg[WHITE] += rookMobMg[moveCount];// +otherKingAttacked;
                eg[WHITE] += rookMobEg[moveCount];// +otherKingAttacked;

                if (!semiOpen && moveCount <= 3) {
                    U8 kingFile = fileOf(whiteKing);
                    if ((kingFile < 4) == (rookFile < kingFile)) {
                        Score malus = (1 + (!(pos->castle & 0x3)));
                        mg[WHITE] -= malus* TRAPPEDROOKMG;
                        eg[WHITE] -= malus* TRAPPEDROOKEG;
                    }
                }
            }
        }
    }
    // Black rooks
    {
        BitBoard rooks = bb[r];
        while (rooks) {

            blackMaterial += mgValues[R];

            Square rook = popLsb(rooks);
            U8 rookFile = fileOf(rook);

            tropismToWhiteKing += rkdist[rook][whiteKing];

            mg[BLACK] += mgTables[r][rook];
            eg[BLACK] += egTables[r][rook];

            // we also will account for the mobility of the rook
            // we will add the number of squares that the rook can move to (and give them a score based on if they are attacked by an enemy pawn)   
            
            // We will give a bonus to the rook if it is on a semi-open file
            bool semiOpen = testBit(openFileFor[BLACK], rookFile);
            bool open = testBit(openFile, rookFile);
            bool closed = (files(fileOf(rook)) & blockedBlackPawns);
            bool onClearFile = (files(rookFile) & whiteOccupancy) == 0;
            mg[BLACK] -= closed * MG_ROOKCLOSEDFILE;
            eg[BLACK] -= closed * EG_ROOKCLOSEDFILE;
            mg[BLACK] += open * MG_ROOKOPENFILE;
            eg[BLACK] += open * EG_ROOKOPENFILE;
            mg[BLACK] += onClearFile * MG_ROOKCLEARFILE;
            eg[BLACK] += onClearFile * EG_ROOKCLEARFILE;
            mg[BLACK] += semiOpen * MG_ROOKSEMIOPENFILE;
            eg[BLACK] += semiOpen * EG_ROOKSEMIOPENFILE;

            bool queenFile = files(rookFile) & (bb[Q] | bb[q]);
            mg[BLACK] += queenFile * ROOKONQUEENFILEMG;
            eg[BLACK] += queenFile * ROOKONQUEENFILEEG;

            if (!(squareBB(rook) & pinnedBlack)) {
                BitBoard mob = getRookAttack((Square)rook, occupancy ^ (blackRQ)) & ~blackOccupancy; // Include xray thorugh our rooks and queens
				kingDangerScores[WHITE] +=
					popcount(kingDangerSquare[WHITE] & (mob & ~blackRQ)) * ROOKATTACKINNERRING
					+ popcount(kingDangerZone[WHITE] & (mob & ~blackRQ)) * ROOKATTACKOUTERRING;
                mob &= mobilityAreaBlack;
                U8 moveCount = popcount(mob);
                //mob &= kingAttacks[whiteKing];
                //U8 otherKingAttacked = popcount(mob);
                mg[BLACK] += rookMobMg[moveCount];// +otherKingAttacked;
                eg[BLACK] += rookMobEg[moveCount];// +otherKingAttacked;

                if (!semiOpen && moveCount <= 3) {
                    U8 kingFile = fileOf(blackKing);
                    if ((kingFile < 4) == (rookFile < kingFile)) {
                        Score malus = (1 + (!(pos->castle & 0xC)));
                        mg[BLACK] -= malus* TRAPPEDROOKMG;
                        eg[BLACK] -= malus* TRAPPEDROOKEG;
                    }
                }
            }
        }
    }
    //std::cout << "Rook checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";
    // White queens
    {
        BitBoard queens = bb[Q];
        while (queens) {

            whiteMaterial += mgValues[Q];

            Square queen = popLsb(queens);

            tropismToBlackKing += qkdist[queen][blackKing];

            mg[WHITE] += mgTables[Q][queen];
            eg[WHITE] += egTables[Q][queen];

            // we also will account for the mobility of the queen
            // we will add the number of squares that the queen can move to (and give them a score based on if they are attacked by an enemy pawn)
            if (!(squareBB(queen) & pinnedWhite)) {
                BitBoard moves = getQueenAttack((Square)queen, occupancy) & ~whiteOccupancy;
				kingDangerScores[BLACK] +=
					popcount(kingDangerSquare[BLACK] & (moves & ~whiteRQ)) * QUEENATTACKINNERRING
					+ popcount(kingDangerZone[BLACK] & (moves & ~whiteRQ)) * QUEENATTACKOUTERRING;
                moves &= mobilityAreaWhite;
                U8 moveCount = popcount(moves);
                mg[WHITE] += queenMobMg[moveCount];
                eg[WHITE] += queenMobEg[moveCount];
            }
        }
    }
    //Black queens
    {
        BitBoard queens = bb[q];
        while (queens) {

            blackMaterial += mgValues[Q];

            Square queen = popLsb(queens);

            tropismToWhiteKing += qkdist[queen][whiteKing];

            mg[BLACK] += mgTables[q][queen];
            eg[BLACK] += egTables[q][queen];

            // we also will account for the mobility of the queen
            // we will add the number of squares that the queen can move to (and give them a score based on if they are attacked by an enemy pawn)
            if (!(squareBB(queen) & pinnedBlack)) {
                BitBoard moves = getQueenAttack((Square)queen, occupancy) & ~blackOccupancy;
				kingDangerScores[WHITE] +=
					popcount(kingDangerSquare[WHITE] & (moves & ~blackRQ)) * QUEENATTACKINNERRING
					+ popcount(kingDangerZone[WHITE] & (moves & ~blackRQ)) * QUEENATTACKOUTERRING;
                moves &= mobilityAreaBlack;
                U8 moveCount = popcount(moves);
                mg[BLACK] += queenMobMg[moveCount];
                eg[BLACK] += queenMobEg[moveCount];
            }
        }
    }

    //std::cout << "Queen checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";
    
    if ((blackMaterial <= 100) && (whiteMaterial >= 477)) {
        eg[WHITE] += 5 * 47 * (centerDistance[blackKing]) + 5 * 16 * (14 - manhattanDistance[blackKing][whiteKing]);
    }
    else if ((whiteMaterial <= 100) && (blackMaterial >= 477)) {
        eg[BLACK] += 5 * 47 * (centerDistance[whiteKing]) + 5 * 16 * (14 - manhattanDistance[whiteKing][blackKing]);
    }
    else {
        kingEval:
        // White King (no need to iterate, since there is only one king per side)
        {
            mg[WHITE] += mgTables[K][whiteKing];
            eg[WHITE] += egTables[K][whiteKing];
            mg[WHITE] += ((whiteRearSpan & bb[K]) > 0) * MG_KINGINREARSPAN;
            eg[WHITE] += ((whiteRearSpan & bb[K]) > 0) * EG_KINGINREARSPAN;
        }
        // Black King (no need to iterate, since there is only one king per side)
        {
            mg[BLACK] += mgTables[k][blackKing];
            eg[BLACK] += egTables[k][blackKing];
            mg[BLACK] += ((blackRearSpan & bb[k]) > 0) * MG_KINGINREARSPAN;
            eg[BLACK] += ((blackRearSpan & bb[k]) > 0) * EG_KINGINREARSPAN;
        }
    //std::cout << "King checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";
        
#define INITIAL_PIECE_MATERIAL 5790
#define kingSafetyMG 2
#define kingSafetyEG 0
        Score matCntOnShelter[] = {
            Score(popcount(bb[P] & kingShelter[WHITE]) * PAWNSHELTERVALUE + popcount(bb[N] & kingShelter[WHITE]) * KNIGHTSHELTERVALUE + popcount(bb[B] & kingShelter[WHITE]) * BISHOPSHELTERVALUE),
            Score(popcount(bb[p] & kingShelter[BLACK]) * PAWNSHELTERVALUE + popcount(bb[n] & kingShelter[BLACK]) * KNIGHTSHELTERVALUE + popcount(bb[b] & kingShelter[BLACK]) * BISHOPSHELTERVALUE)
        };
        Score mobSquaresInKingShelter[] = {
            (Score)popcount(mobilityAreaBlack & kingShelter[WHITE]),
            (Score)popcount(mobilityAreaWhite & kingShelter[BLACK])
        };
        // Adjust the king safety score based on the number of squares that the king can move to
        kingDangerScores[WHITE] -= (mobSquaresInKingShelter[WHITE] >> 1);
        kingDangerScores[BLACK] -= (mobSquaresInKingShelter[BLACK] >> 1);
        kingDangerScores[WHITE] = std::max(Score(0), kingDangerScores[WHITE]);
        kingDangerScores[BLACK] = std::max(Score(0), kingDangerScores[BLACK]);

        // If the attacking side has no queen, the kingDanger score is halved
        if (!(bb[Q]|bb[q])) {
            kingDangerScores[WHITE] >>= 1;
            kingDangerScores[BLACK] >>= 1;
        }

        U8 kingFiles[] = {
            U8((kingAttacks[whiteKing] >> (U64(rankOf(whiteKing)) * 8ULL)) & 0xFFULL),
            U8((kingAttacks[blackKing] >> (U64(rankOf(blackKing)) * 8ULL)) & 0xFFULL),
        };

        // Divide score by 16
        kingDangerScores[WHITE] >>= 4;
        kingDangerScores[BLACK] >>= 4;
		// The std::max score can only be 63
		kingDangerScores[WHITE] = std::min(kingDangerScores[WHITE], Score(63));
		kingDangerScores[BLACK] = std::min(kingDangerScores[BLACK], Score(63));
        Score whiteKingSafety = ((matCntOnShelter[WHITE] - tropismToWhiteKing  + mobSquaresInKingShelter[WHITE]) * whiteMaterial) / INITIAL_PIECE_MATERIAL - kingSafety[kingDangerScores[WHITE]];
        Score blackKingSafety = ((matCntOnShelter[BLACK] - tropismToBlackKing  + mobSquaresInKingShelter[BLACK]) * blackMaterial) / INITIAL_PIECE_MATERIAL - kingSafety[kingDangerScores[BLACK]];
        mg[WHITE] += whiteKingSafety * kingSafetyMG;
        mg[BLACK] += blackKingSafety * kingSafetyMG;
        //std::cout << "kingsafety1 checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";
        
        mg[WHITE] += popcount(kingFiles[WHITE] & openFile) * OPENFILEONKINGMG;
        eg[WHITE] += popcount(kingFiles[WHITE] & openFile) * OPENFILEONKINGEG;
        mg[BLACK] += popcount(kingFiles[BLACK] & openFile) * OPENFILEONKINGMG;
        eg[BLACK] += popcount(kingFiles[BLACK] & openFile) * OPENFILEONKINGEG;

        //std::cout << "openfile checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";

        mg[WHITE] += pawnStormTo[BLACK] * PAWNSTORM;
        mg[BLACK] += pawnStormTo[WHITE] * PAWNSTORM;
        eg[WHITE] += pawnStormTo[BLACK] * PAWNSTORM;
        eg[BLACK] += pawnStormTo[WHITE] * PAWNSTORM;

        //std::cout << "Pawnstorm checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";
        //std::cout << "Pawnstorm valueas are:\t\t" << (int)pawnStormTo[WHITE] << " / " << (int)pawnStormTo[BLACK] << "\n";
        
        mg[side] += MG_TEMPO;
        eg[side] += EG_TEMPO;
        mgScore += (int)(kingSafetyMG * (whiteKingShield - blackKingShield) * (1 - (2 * side)));
        egScore += (int)(kingSafetyEG * (whiteKingShield - blackKingShield) * (1 - (2 * side)));
        if (whiteNonPawnMaterial + blackNonPawnMaterial > 6434) {
            BitBoard safeCentralWhite = (~attackedByBlackPawns) & (files(2) | files(3) | files(4) | files(5)) & (ranks(6) | ranks(5) | ranks(4));
            BitBoard safeCentralBlack = (~attackedByWhitePawns) & (files(2) | files(3) | files(4) | files(5)) & (ranks(1) | ranks(2) | ranks(3));
            
            Score cW = popcount(safeCentralWhite);
            Score cB = popcount(safeCentralBlack);

            safeCentralWhite &= whiteRearSpan;
            safeCentralBlack &= blackRearSpan;
            cW += popcount(safeCentralWhite);
            cB += popcount(safeCentralBlack);

            Score w = popcount(whiteNonPawns | blackNonPawns) - 3 + std::min(7, popcount(unadvancablePawnsFor[WHITE]|unadvancablePawnsFor[BLACK]));
            mg[WHITE] += (cW * w * (w)) >> 5;
            mg[BLACK] += (cB * w * (w)) >> 5;
            //std::cout << "space checkpoint:\t\t" << "Mg is " << mg[0] << " / " << mg[1] << "\t-\t" << "Eg is " << eg[0] << " / " << eg[1] << "\n";
        }
    }
    
    int fC = std::min(60,std::max(0, pos->fiftyMove-12));
    mgScore += (int)(mg[side] - mg[other]);// *(100 - fC) / 100;
    egScore += (int)(eg[side] - eg[other]);// *(100 - fC) / 100;

    // phase is capped to 24, in case of early promotion
#define midgameLimit (11444)
#define endgameLimit (2936)
	
    Score mgPhase = std::min(Score(24), gamePhase);
    Score egPhase = 24 - mgPhase;   

    // We add a tempo bonus to the score, meaning that we will give a bonus to the side that has to move (10 cp)
    // Score tempoBonus = (side == WHITE) ? 10 : -10;

    Score res = (Score)(((int)mgScore * (int)mgPhase + (int)egScore * (int)egPhase) / 24);
    //if (res >= egValues[Q] + egValues[N]) res += (KNOWNWIN/2) - ((res) * abs(KNOWNWIN/2))/mateScore;
	//if (res <= egValues[Q] + egValues[N]) res += (-KNOWNWIN / 2) - ((res) * abs(-KNOWNWIN / 2)) / mateScore;
    
#if USINGEVALCACHE
    // Cache res before scaling
    cacheEval(pos->hashKey, res);
#endif
    return res * (100 - fC) / 100;

}