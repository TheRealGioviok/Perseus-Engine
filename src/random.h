#pragma once
#include "types.h"

extern U64 state;

/**
 * @brief The getRandom32 function returns a random 32 bit integer.
 * @return A random 32 bit integer.
 * @note This function updates the internal state of the random number generator.
 */
U32 getRandom32();

/**
 * @brief The getRandom64 function returns a random 64 bit integer.
 * @return A random 64 bit integer.
 * @note This function updates the internal state of the random number generator.
 */
U64 getRandom64();