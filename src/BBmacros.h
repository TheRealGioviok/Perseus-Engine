// The BBmacros.h and BBmacros.cpp files contain utility definitions to write some more readable code
#pragma once
#include <algorithm>
#include <iostream>


#include <cassert>

#if defined(_MSC_VER) && (_MSC_VER >= 1500) && (defined(_M_IX86) || defined(_M_X64))
#ifdef __clang__
/* Many of the intrinsics SDL uses are not implemented by clang with Visual Studio */
#undef __MMX__
#undef __SSE__
#undef __SSE2__
#else
#include <intrin.h>
#ifndef _WIN64
#ifndef __MMX__
#define __MMX__
#endif
#ifndef __3dNOW__
#define __3dNOW__
#endif
#endif
#ifndef __SSE__
#define __SSE__
#endif
#ifndef __SSE2__
#define __SSE2__
#endif
#endif /* __clang__ */
#elif defined(__MINGW64_VERSION_MAJOR)
#include <intrin.h>
#else
/* altivec.h redefining bool causes a number of problems, see bugs 3993 and 4392, so you need to explicitly define SDL_ENABLE_ALTIVEC_H to have it included. */
#if defined(HAVE_ALTIVEC_H) && defined(__ALTIVEC__) && !defined(__APPLE_ALTIVEC__) && defined(SDL_ENABLE_ALTIVEC_H)
#include <altivec.h>
#endif
#if !defined(SDL_DISABLE_ARM_NEON_H)
#  if defined(__ARM_NEON)
#    include <arm_neon.h>
#  elif defined(__WINDOWS__) || defined(__WINRT__)
/* Visual Studio doesn't define __ARM_ARCH, but _M_ARM (if set, always 7), and _M_ARM64 (if set, always 1). */
#    if defined(_M_ARM)
#      include <armintr.h>
#      include <arm_neon.h>
#      define __ARM_NEON 1 /* Set __ARM_NEON so that it can be used elsewhere, at compile time */
#    endif
#    if defined (_M_ARM64)
#      include <arm64intr.h>
#      include <arm64_neon.h>
#      define __ARM_NEON 1 /* Set __ARM_NEON so that it can be used elsewhere, at compile time */
#    endif
#  endif
#endif
#if defined(__3dNOW__) && !defined(SDL_DISABLE_MM3DNOW_H)
#include <mm3dnow.h>
#endif
#if defined(HAVE_IMMINTRIN_H) && !defined(SDL_DISABLE_IMMINTRIN_H)
  #if (defined(_M_IX86) || defined(_M_X64))
    #include <immintrin.h>
  #endif
#else
#if defined(__MMX__) && !defined(SDL_DISABLE_MMINTRIN_H)
#include <mmintrin.h>
#endif
#if defined(__SSE__) && !defined(SDL_DISABLE_XMMINTRIN_H)
#include <xmmintrin.h>
#endif
#if defined(__SSE2__) && !defined(SDL_DISABLE_EMMINTRIN_H)
#include <emmintrin.h>
#endif
#if defined(__SSE3__) && !defined(SDL_DISABLE_PMMINTRIN_H)
#include <pmmintrin.h>
#endif
#endif /* HAVE_IMMINTRIN_H */
#endif /* compiler version */

#include "types.h"
#include "constants.h"

inline constexpr BitBoard 	squareBB(Square square) 					{ return (1ULL << square); } 
inline U64 		getBit(BitBoard bitboard, Square square) 	{ return (bitboard & squareBB(square)); }
inline U8 		testBit(BitBoard bitboard, Square square) 	{ return (getBit(bitboard, square) > 0); }
inline void 		setBit(BitBoard& bitboard, Square square) 	{ bitboard |= squareBB(square); }
inline void 		clearBit(BitBoard& bitboard, Square square) { bitboard &= (~squareBB(square)); }
inline void 		clearBit(U8 &u8, U8 pos) { u8 &= ~(1 << pos); }

inline U8 rankOf(Square square) 				{ return (square >> 3); }
inline U8 fileOf(Square square) 				{ return (square & 7); }
inline Square flipSquare(Square square) 		{ return (square ^ 56); }
inline Square makeSquare(U8 rank, U8 file) 	{ return (rank * 8 + file); }

inline constexpr BitBoard files(U8 fileNo) 				{ return (0x0101010101010101ULL << fileNo); }
inline constexpr BitBoard ranks(U8 rankNo) 				{ return  (0xFFULL << (rankNo<<3)); }
inline constexpr BitBoard notFile(U8 fileNo) 			{ return ~files(fileNo); }
inline constexpr BitBoard notRank(U8 rankNo) 			{ return ~ranks(rankNo); }
inline constexpr BitBoard squaresAhead(Square square) 	{ return (files(7) ^ 0x8000000000000000) >> (63 - square); }
inline constexpr BitBoard squaresBehind(Square square) 	{ return (files(0) ^ 0x1ULL) << square; }
constexpr BitBoard squaresOfColor[2] = 					{ 0xAA55AA55AA55AA55ULL,	0x55AA55AA55AA55AAULL };

static inline constexpr BitBoard ne(BitBoard bb) 		{ return (bb & notFile(7)) >> 7; }
static inline constexpr BitBoard north(BitBoard bb) 	{ return bb >> 8; }
static inline constexpr BitBoard nw(BitBoard bb) 		{ return (bb & notFile(0)) >> 9; }
static inline constexpr BitBoard se(BitBoard bb) 		{ return (bb & notFile(7)) << 9; }
static inline constexpr BitBoard south(BitBoard bb) 	{ return bb << 8; }
static inline constexpr BitBoard sw(BitBoard bb) 		{ return (bb & notFile(0)) << 7; }
static inline constexpr BitBoard east(BitBoard bb) 		{ return (bb & notFile(7)) >> 1; }
static inline constexpr BitBoard west(BitBoard bb) 		{ return (bb & notFile(0)) << 1; }

static inline constexpr S32 indexColorSide(U8 color, U8 side) {return (color << 1) + side; }

#define bitScanForward _BitScanForward64 //ls1b
#define bitScanReverse _BitScanReverse64 //ms1b
#define makeSquareColor(color,square) (((color) * 64) + (square))

#define emptyBoard "8/8/8/8/8/8/8/8 w - - "
#define startPosition "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define trickyPosition "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq e8 0 1 "
#define killerPosition "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmkPosition "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "


// The squares name
const std::string coords[65] = {
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "noSquare"
};

/**
 * @brief The printBitBoard function prints a bitboard.
 * @notes For debugging purposes.
 */
void printBitBoard(BitBoard bitboard);

#if defined(__GNUC__)  // GCC, Clang, ICC

inline Square lsb(BitBoard b) {
	assert(b);
	return Square(__builtin_ctzll(b));
}

inline Square msb(BitBoard b) {
	assert(b);
	return Square(63 ^ __builtin_clzll(b));
}

inline int popcount(BitBoard b) {
	return __builtin_popcountll(b);
}

inline BitBoard reflectBitBoard(BitBoard bb) { return __builtin_bswap64(bb); }

#elif defined(_MSC_VER)  // MSVC

#ifdef _WIN64  // MSVC, WIN64

inline Square lsb(BitBoard b) {
	assert(b);
	unsigned long idx;
	_BitScanForward64(&idx, b);
	return (Square)idx;
}

inline Square msb(BitBoard b) {
	assert(b);
	unsigned long idx;
	_BitScanReverse64(&idx, b);
	return (Square)idx;
}

inline int popcount(BitBoard b) {
	return (int)__popcnt64(b);
}

inline BitBoard reflectBitBoard(BitBoard bb) { return _byteswap_uint64(bb); }

#else  // MSVC, WIN32

inline Square lsb(BitBoard b) {
	assert(b);
	unsigned long idx;

	if (b & 0xffffffff) {
		_BitScanForward(&idx, int32_t(b));
		return Square(idx);
	}
	else {
		_BitScanForward(&idx, int32_t(b >> 32));
		return Square(idx + 32);
	}
}

inline Square msb(BitBoard b)
{
	assert(b);
	unsigned long idx;

	if (b >> 32) {
		_BitScanReverse(&idx, int32_t(b >> 32));
		return Square(idx + 32);
	}
	else {
		_BitScanReverse(&idx, int32_t(b));
		return Square(idx);
	}
}

#endif


#else  // Compiler is neither GCC nor MSVC compatible

#error "Compiler not supported."

#endif

/// pop_lsb() finds and clears the least significant bit in a non-zero bitboard

inline Square popLsb(BitBoard& b) {
	assert(b);
	const Square s = lsb(b);
	b &= b - 1;
	return s;
}

inline void prefetch(void* addr) {

#  if defined(__INTEL_COMPILER)
	// This hack prevents prefetches from being optimized away by
	// Intel compiler. Both MSVC and gcc seem not be affected by this.
	__asm__("");
#  endif

#  if defined(__INTEL_COMPILER) || defined(_MSC_VER)
	_mm_prefetch((char*)addr, _MM_HINT_T0);
#  else
	__builtin_prefetch(addr);
#  endif
}