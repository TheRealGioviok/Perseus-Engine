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
        #ifndef USE_PEXT
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
        #else
        int occupancyVariations = 1 << popcount(attackMask);
        for (int index = 0; index < occupancyVariations; index++){
            U64 occupancy = setOccupancy(index, popcount(attackMask), attackMask);
            if (sliderType){
                int magicIndex = _pext_u64(occupancy, bishopMasks[square]);
                bishopAttacks[square][magicIndex] = getBishopAttacksOnTheFly(square, occupancy);
            }
            else{
                int magicIndex = _pext_u64(occupancy, rookMasks[square]);
                rookAttacks[square][magicIndex] = getRookAttacksOnTheFly(square, occupancy);
            }
        }
        #endif
    }
}