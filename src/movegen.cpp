#include "movegen.h"
#include "BBmacros.h"
#include "magic.h"
#include "tables.h"

/**
 * @brief The maskPawnAttacks function returns the pawn attacks mask for a given square.
 * @param square The square.
 * @param side The side.
 * @return The pawn attacks mask for a given square.
 */
BitBoard maskPawnAttacks(int square, int side){
    BitBoard attacks = 0;
    BitBoard bb = squareBB(square);
    if(side == WHITE){
        if (bb & (notFile(7))) attacks |= (bb >> 7);
        if (bb & (notFile(0))) attacks |= (bb >> 9);
    }
    else{
        if (bb & (notFile(0))) attacks |= (bb << 7);
        if (bb & (notFile(7))) attacks |= (bb << 9);
    }
    return attacks;
}

/**
 * @brief The maskBishopAttacks function returns the bishop attacks mask for a given square.
 * @param square The square.
 */
BitBoard maskBishopAttacks(int square){
    BitBoard attacks = 0ULL;
    int r=0,f=0;
    int tr = square/8, tf = square%8;
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= squareBB(r * 8 + f);
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= squareBB(r * 8 + f);
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= squareBB(r * 8 + f);
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= squareBB(r * 8 + f);
    return attacks;
}

/**
 * @brief The maskRookAttacks function returns the rook attacks mask for a given square.
 * @param square The square.
 */
BitBoard maskRookAttacks(int square){
    BitBoard attacks = 0;
    int r=0,f=0;
    int tr = square/8, tf = square%8;
    for (r = tr + 1; r <= 6; r++) attacks |= squareBB(r * 8 + tf);
    for (r = tr - 1; r >= 1; r--) attacks |= squareBB(r * 8 + tf);
    for (f = tf + 1; f <= 6; f++) attacks |= squareBB(tr * 8 + f);
    for (f = tf - 1; f >= 1; f--) attacks |= squareBB(tr * 8 + f);
    return attacks;
}

/**
 * @brief The getBishopAttacksOnTheFly function returns the bishop attacks mask for a given square and occupancy.
 * @param square The square.
 * @param occupancy The occupancy.
 * @return The bishop attacks mask for a given square and occupancy.
 * @note This function is to be used for magic number search and not for move generation.
 */
BitBoard getBishopAttacksOnTheFly(int square, BitBoard occupancy){
    //results
    BitBoard attacks = 0ULL;

    //init ranks & files
    int r, f;

    //initialize target ranks & files
    int tr = square / 8;
    int tf = square & 0x7;

    //generate bishop attacks

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
        BitBoard possB = squareBB(r * 8 + f);
        attacks |= possB;
        if (occupancy & possB) break;
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
        BitBoard possB = squareBB(r * 8 + f);
        attacks |= possB;
        if (occupancy & possB) break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
        BitBoard possB = squareBB(r * 8 + f);
        attacks |= possB;
        if (occupancy & possB) break;
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        BitBoard possB = squareBB(r * 8 + f);
        attacks |= possB;
        if (occupancy & possB) break;
    }

    //return attack map
    return attacks;
}

/**
 * @brief The getRookAttacksOnTheFly function returns the rook attacks mask for a given square and occupancy.
 * @param square The square.
 * @param occupancy The occupancy.
 * @return The rook attacks mask for a given square and occupancy.
 * @note This function is to be used for magic number search and not for move generation.
 */
BitBoard getRookAttacksOnTheFly(int square, BitBoard occupancy){
    //results
    BitBoard attacks = 0ULL;

    //init ranks & files
    int r, f;

    //initialize target ranks & files
    int tr = square / 8;
    int tf = square & 0x7;

    //mask relevante bishop occupancy bits

    for (r = tr + 1; r <= 7; r++) {
        BitBoard possB = squareBB(r * 8 + tf);
        attacks |= possB;
        if (occupancy & possB) break;
    }
    for (r = tr - 1; r >= 0; r--) {
        BitBoard possB = squareBB(r * 8 + tf);
        attacks |= possB;
        if (occupancy & possB) break;
    }
    for (f = tf + 1; f <= 7; f++) {
        BitBoard possB = squareBB(tr * 8 + f);
        attacks |= possB;
        if (occupancy & possB) break;
    }
    for (f = tf - 1; f >= 0; f--) {
        BitBoard possB = squareBB(tr * 8 + f);
        attacks |= possB;
        if (occupancy & possB) break;
    }

    //return attack map
    return attacks;
}

/**
 * @brief The setOccupancy function sets the occupancy gor a given attack mask
 * @param index The index.
 * @param bitsInMask The relevant bits in the mask.
 * @param attackMask The attack mask.
 * @return The occupancy.
 */
BitBoard setOccupancy(int index, int bitsInMask, BitBoard attackMask){
    // occupancy map
    BitBoard occupancy = 0ULL;
    //loop over range of bits within attack mask
    for (int i = 0; i < bitsInMask; i++) {
        // get LS1b index of attack mask
        Square square = popLsb(attackMask);
        //check wheter occupancy is on board, if true populate occupancy
        if (index & squareBB(i)) occupancy |= squareBB(square);
    }
    return occupancy;
}



/**
 * @brief The initSliderAttacks function initializes the slider attacks for every square and occupancy.
 * @param sliderType The slider type.
 */
void initSliderAttacks(bool sliderType) {
    //init bishop & rook masks
    for (int square = 0; square < 64; square++) { //loop over squares
        //init bishop & rook masks
        bishopMasks[square] = maskBishopAttacks(square);
        rookMasks[square] = maskRookAttacks(square);

        //init current mask
        U64 attackMask = sliderType ? bishopMasks[square] : rookMasks[square];

        //init relevant occupancy bit cnt
        int relevantBitsCount = popcount(attackMask);

        //init occupancy indexs
        int occupancyIndexs = squareBB(relevantBitsCount);

        //loop over occupancyIndexs
        for (int index = 0; index < occupancyIndexs; index++) {
            if (sliderType) { //first case
                //init current occupancy variation
                U64 occupancy = setOccupancy(index, relevantBitsCount, attackMask);

                //init magic index
                int magicIndex = (occupancy * bishopMagicNumbers[square]) >> (64 - bishopRelevantBits[square]);

                //init bishopAttacks (finally)
                bishopAttacks[square][magicIndex] = getBishopAttacksOnTheFly(square, occupancy);
            } //second case
            else {
                //init current occupancy variation
                U64 occupancy = setOccupancy(index, relevantBitsCount, attackMask);

                //init magic index
                int magicIndex = (occupancy * rookMagicNumbers[square]) >> (64 - rookRelevantBits[square]);

                //init bishopAttacks (finally)
                rookAttacks[square][magicIndex] = getRookAttacksOnTheFly(square, occupancy);
            }
        }
    }
}

/**
 * @brief The getBishopAttack function returns the bishop attacks for a given square and occupancy.
 * @param square The square.
 * @param occupancy The occupancy.
 * @return The bishop attack table.
 */
BitBoard getBishopAttack(int square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bishopMasks[square];
    occupancy *= bishopMagicNumbers[square];
    occupancy >>= 64ULL - (U64)bishopRelevantBits[square];

    // return bishop attacks
    return bishopAttacks[square][occupancy];
}

/**
 * @brief The getRookAttack function returns the rook attacks for a given square and occupancy.
 * @param square The square.
 * @param occupancy The occupancy.
 * @return The rook attack table.
 */
BitBoard getRookAttack(int square, U64 occupancy) {
    // get rook attacks assuming current board occupancy
    occupancy &= rookMasks[square];
    occupancy *= rookMagicNumbers[square];
    occupancy >>= 64ULL - (U64)rookRelevantBits[square];

    // return rook attacks
    return rookAttacks[square][occupancy];
}

/**
 * @brief The getQueenAttack function returns the queen attacks for a given square and occupancy.
 * @param square The square.
 * @param occupancy The occupancy.
 * @return The queen attack table.
 */
BitBoard getQueenAttack(int square, U64 occupancy) {
    return getBishopAttack(square, occupancy) | getRookAttack(square, occupancy);
}

/**
 * @brief The xrayRookAttacks function determines the xray rook attacks for a given square, blockers and occupancy
 * @param occupancy The occupancy.
 * @param blockers The blockers.
 * @param square The square.
 * @return The xray rook attacks.
 */
inline BitBoard xRayRookAttacks(BitBoard occupancy, BitBoard blockers, Square square){
    BitBoard attacks = getRookAttack(square, occupancy); // First, we generate normal rook attacks
    blockers &= attacks; // Then, we remove the blockers (the candidate pinned pieces) from the attacks
    return attacks ^ getRookAttack(square, occupancy ^ blockers); // Then we calculate attacks like if the blockers were not there. Finally we only take those attacks only in this new set of blockers.
}

/**
 * @brief The xrayBishopAttacks function determines the xray bishop attacks for a given square, blockers and occupancy
 * @param occupancy The occupancy.
 * @param blockers The blockers.
 * @param square The square.
 * @return The xray bishop attacks.
 */
inline BitBoard xRayBishopAttacks(BitBoard occupancy, BitBoard blockers, Square square){
    BitBoard attacks = getBishopAttack(square, occupancy); // First, we generate normal bishop attacks
    blockers &= attacks; // Then, we remove the blockers (the candidate pinned pieces) from the attacks
    return attacks ^ getBishopAttack(square, occupancy ^ blockers); // Then we calculate attacks like if the blockers were not there. Finally we only take those attacks only in this new set of blockers.
}

/**
 * @brief The getPinnedPieces function returns the squares of a certain bitboard that are pinned to a certain square.
 * @param occupancy The general occupancy.
 * @param ownPieces The own pieces. (the pieces that are candidates to be pinned)
 * @param pinSquare The square that is pinned.
 * @param opRQ The opponent's Rook and Queen bitboard.
 * @param opBQ The opponent's Bishop and Queen bitboard.
 * @return The pinned pieces, if any.
 */
BitBoard getPinnedPieces(BitBoard occupancy, BitBoard ownPieces, Square pinSquare, BitBoard opRQ, BitBoard opBQ){
    BitBoard pinned = 0;
    BitBoard pinner = xRayRookAttacks(occupancy, ownPieces, pinSquare) & opRQ; // We generate the pinners for the rooks
    while (pinner){
        Square sq = popLsb(pinner);
        pinned |= squaresBetween[sq][pinSquare] & ownPieces; // We add the squares between the pin square and the pinner to the pinned pieces
    }
    pinner = xRayBishopAttacks(occupancy, ownPieces, pinSquare) & opBQ; // We generate the pinners for the bishops
    while (pinner){
        Square sq = popLsb(pinner);
        pinned |= squaresBetween[sq][pinSquare] & ownPieces; // We add the squares between the pin square and the pinner to the pinned pieces
    }
    return pinned;
}