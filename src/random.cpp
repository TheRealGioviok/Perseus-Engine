#include "random.h"
U64 state = 123456789;

/**
 * @brief The getRandom32 function returns a random 32 bit integer.
 * @return A random 32 bit integer.
 * @note This function updates the internal state of the random number generator.
 */
U32 getRandom32() {
    U32 number = state;
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    state = number;
    return number;
}

/**
 * @brief The getRandom64 function returns a random 64 bit integer.
 * @return A random 64 bit integer.
 * @note This function updates the internal state of the random number generator.
 */
U64 getRandom64(){
    U64 number = state;
    number ^= number << 13;
    number ^= number >> 7;
    number ^= number << 17;
    state = number;
    return number;
}
