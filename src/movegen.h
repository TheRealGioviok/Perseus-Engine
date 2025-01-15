#pragma once
#include "types.h"
#include "move.h"
#include "tables.h"
#include "magic.h"
#include "BBmacros.h"

constexpr BitBoard pawnAttacks[2][64] = {
    {   
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x2, 0x5, 0xa, 0x14, 0x28, 0x50, 0xa0, 0x40,
        0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
        0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
        0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
        0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
        0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
        0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000
    },
    {
        0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
        0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
        0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
        0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
        0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
        0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
        0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000, 0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    }
};
constexpr BitBoard knightAttacks[64] = {0x20400, 0x50800, 0xa1100, 0x142200, 0x284400, 0x508800, 0xa01000, 0x402000,
                              0x2040004, 0x5080008, 0xa110011, 0x14220022, 0x28440044, 0x50880088, 0xa0100010, 0x40200020,
                              0x204000402, 0x508000805, 0xa1100110a, 0x1422002214, 0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040,
                              0x20400040200, 0x50800080500, 0xa1100110a00, 0x142200221400, 0x284400442800, 0x508800885000, 0xa0100010a000, 0x402000204000,
                              0x2040004020000, 0x5080008050000, 0xa1100110a0000, 0x14220022140000, 0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000,
                              0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
                              0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
                              0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000, 0x44280000000000, 0x88500000000000, 0x10a00000000000, 0x20400000000000};

constexpr BitBoard kingAttacks[64] = {0x302, 0x705, 0xe0a, 0x1c14, 0x3828, 0x7050, 0xe0a0, 0xc040,
                            0x30203, 0x70507, 0xe0a0e, 0x1c141c, 0x382838, 0x705070, 0xe0a0e0, 0xc040c0,
                            0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00, 0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000,
                            0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000, 0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000,
                            0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000, 0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000,
                            0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000, 0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 0xc040c000000000,
                            0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
                            0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000};
// bishop attack masks
extern BitBoard bishopMasks[64];
// rook attack masks
extern BitBoard rookMasks[64];
// bishop attacks table
extern BitBoard bishopAttacks[64][512];
// rook attacks table
extern BitBoard rookAttacks[64][4096];

constexpr BitBoard wKCastleMask = squareBB(f1) | squareBB(g1);
constexpr BitBoard wQCastleMask = squareBB(d1) | squareBB(c1) | squareBB(b1);
constexpr BitBoard bKCastleMask = squareBB(f8) | squareBB(g8);
constexpr BitBoard bQCastleMask = squareBB(d8) | squareBB(c8) | squareBB(b8);

// castling rights update constants
constexpr S32 castlingRights[64] = {
    7, 15, 15, 15, 3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};

/**
 * @brief The setOccupancy function sets the occupancy gor a given attack mask
 * @param index The index.
 * @param bitsInMask The relevant bits in the mask.
 * @param attackMask The attack mask.
 * @return The occupancy.
 */
BitBoard setOccupancy(int index, int bitsInMask, BitBoard attackMask);

/**
 * @brief The maskPawnAttacks function returns the pawn attacks mask for a given square.
 * @param square The square.
 * @param side The side.
 * @return The pawn attacks mask for a given square.
 */
BitBoard maskPawnAttacks(int square, int side);

/**
 * @brief The maskBishopAttacks function returns the bishop attacks mask for a given square.
 * @param square The square.
 */
BitBoard maskBishopAttacks(int square);

/**
 * @brief The maskRookAttacks function returns the rook attacks mask for a given square.
 * @param square The square.
 */
BitBoard maskRookAttacks(int square);

/**
 * @brief The getBishopAttacksOnTheFly function returns the bishop attacks mask for a given square and occupancy.
 * @param square The square.
 * @param occupancy The occupancy.
 * @return The bishop attacks mask for a given square and occupancy.
 * @note This function is to be used for magic number search and not for move generation.
 */
BitBoard getBishopAttacksOnTheFly(int square, BitBoard occupancy);

/**
 * @brief The getRookAttacksOnTheFly function returns the rook attacks mask for a given square and occupancy.
 * @param square The square.
 * @param occupancy The occupancy.
 * @return The rook attacks mask for a given square and occupancy.
 * @note This function is to be used for magic number search and not for move generation.
 */
BitBoard getRookAttacksOnTheFly(int square, BitBoard occupancy);

/**
 * @brief The initSliderAttacks function initializes the slider attacks for every square and occupancy.
 * @param sliderType The slider type.
 */
void initSliderAttacks(bool sliderType);


/**
 * @brief The getBishopAttack function returns the bishop attacks for a given square and occupancy.
 * @param square The square.
 * @param occupancy The occupancy.
 * @return The bishop attack table.
 */
inline BitBoard getBishopAttack(int square, U64 occupancy) {
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
inline BitBoard getRookAttack(int square, U64 occupancy) {
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
inline BitBoard getQueenAttack(int square, U64 occupancy) {
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
inline BitBoard getPinnedPieces(BitBoard occupancy, BitBoard ownPieces, Square pinSquare, BitBoard opRQ, BitBoard opBQ){
    BitBoard pinned = 0;
    BitBoard pinner = (xRayRookAttacks(occupancy, ownPieces, pinSquare) & opRQ) | (xRayBishopAttacks(occupancy, ownPieces, pinSquare) & opBQ); // We generate the pinners for the rooks
    while (pinner){
        Square sq = popLsb(pinner);
        pinned |= squaresBetween[sq][pinSquare]; // We add the squares between the pin square and the pinner to the pinned pieces
    }
    // Only return own pieces
    return pinned & ownPieces;
}