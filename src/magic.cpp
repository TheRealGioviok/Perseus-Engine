#include "magic.h"
#include "BBmacros.h"
#include "movegen.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

/**
 * @brief The getMagicNumber function returns a 64 bit candidate for the magic number. It means that the number has a low hamming weight.
 * @return A 64 bit candidate for the magic number.
 */
U64 getMagicNumber(){
    return getRandom64() & getRandom64() & getRandom64();
}

/**
 * @brief The findMagicNumber function finds the magic number for a square, bitcount and slider type
 * @param square The square.
 * @param relevantBits The relevant bits count.
 * @param sliderType The slider type.
 * @return The magic number.
 */
U64 findMagicNumber(int square, int relevantBits, bool sliderType) {
    // init occupancies
    U64* occupancies = new U64[4096];

    // init attack tables
    U64* attacks = new U64[4096];

    // init used attacks
    U64* usedAttacks = new U64[4096];

    // init attack mask for a current piece
    U64 attackMask = sliderType ? maskBishopAttacks(square) : maskRookAttacks(square);

    // init occupancy indicies
    int occupancy_indicies = 1 << relevantBits;

    // loop over occupancy indicies
    for (int index = 0; index < occupancy_indicies; index++)
    {
        // init occupancies
        occupancies[index] = setOccupancy(index, relevantBits, attackMask);

        // init attacks
        attacks[index] = sliderType ? getBishopAttacksOnTheFly(square, occupancies[index]) : getRookAttacksOnTheFly(square, occupancies[index]);
    }

    // test magic numbers loop
    for (int random_count = 0; random_count < 100000000; random_count++) {
        // generate magic number candidate
        U64 magic_number = getMagicNumber();

        // skip inappropriate magic numbers
        if (popcount((attackMask * magic_number) & 0xFF00000000000000) < 6)
            continue;

        // init used attacks
        memset(usedAttacks, 0ULL, 4096 * sizeof(U64));

        // init index & fail flag
        int index, fail;

        // test magic index loop
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++) {
            // init magic index
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevantBits));

            // if magic index works
            if (usedAttacks[magic_index] == 0ULL)
                // init used attacks
                usedAttacks[magic_index] = attacks[index];

            // otherwise
            else if (usedAttacks[magic_index] != attacks[index])
                // magic index doesn't work
                fail = 1;
        }

        // if magic number works
        if (!fail) {
            // return it
            delete[] occupancies;
            delete[] attacks;
            delete[] usedAttacks;
            return magic_number;
        }
    }

    // if magic number doesn't work
    std::cerr << "Magic number not found!" << std::endl;
    delete[] occupancies;
    delete[] attacks;
    delete[] usedAttacks;
    return 0ULL;
}

/**
 * @brief The initMagic function initializes the magic numbers for every square and slider type.
 */
void initMagic() {
    // init magic numbers
    for (int square = 0; square < 64; square++) {
        // init magic numbers
        // bishopMagicNumbers[square] = findMagicNumber(square, bishopRelevantBits[square], true);
        // rookMagicNumbers[square] = findMagicNumber(square, rookRelevantBits[square], false);
    }
}