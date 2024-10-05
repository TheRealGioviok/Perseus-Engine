#pragma once
#include <cassert>
#include <limits>
#include <cstring>
#include <iostream>

typedef unsigned long long U64;
typedef signed long long S64;
typedef unsigned int U32;
typedef signed int S32;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned char U8;
typedef signed char S8;
typedef signed char Depth;
typedef signed short Ply;
typedef __uint128_t U128;

// The BitBoard type is a typedef for U64.
typedef unsigned long long BitBoard;
// The ScoredMove type is a typedef for U64.
typedef unsigned long long ScoredMove;
// The HashKey type is a typedef for U64
typedef unsigned long long HashKey;
// The Move type is a typedef for U32.
typedef unsigned int Move;
// The packedMove type is a typedef for U16.
typedef unsigned short PackedMove;
// The Score type is a typedef for S16.
typedef signed short Score;
// The Square type is a typedef for U8.
typedef unsigned char Square;
// The Piece type is a typedef for U8.
typedef unsigned char Piece;


// ( I LOVED LIFTING SF CODE, (apparently it was UB), NOW POLARIS IS MY NEW BEST FRIEND )
class PScore {
    private:
		S32 m_score;
		explicit constexpr PScore(S32 score) : m_score{score} {}

	public:
		constexpr PScore() : m_score{} {}

		constexpr PScore(Score midgame, Score endgame)
			: m_score{static_cast<S32>(static_cast<U32>(endgame) << 16) + midgame}
		{
			assert(std::numeric_limits<S16>::min() <= midgame && std::numeric_limits<S16>::max() >= midgame);
			assert(std::numeric_limits<S16>::min() <= endgame && std::numeric_limits<S16>::max() >= endgame);
		}

		[[nodiscard]] inline auto mg() const
		{
			const auto mg = static_cast<U16>(m_score);

			S16 v{};
			std::memcpy(&v, &mg, sizeof(mg));

			return static_cast<Score>(v);
		}

		[[nodiscard]] inline auto eg() const
		{
			const auto eg = static_cast<U16>(static_cast<U32>(m_score + 0x8000) >> 16);

			S16 v{};
			std::memcpy(&v, &eg, sizeof(eg));

			return static_cast<Score>(v);
		}

		[[nodiscard]] constexpr auto operator+(const PScore &other) const
		{
			return PScore{m_score + other.m_score};
		}

		constexpr auto operator+=(const PScore &other) -> auto &
		{
			m_score += other.m_score;
			return *this;
		}

		[[nodiscard]] constexpr auto operator-(const PScore &other) const
		{
			return PScore{m_score - other.m_score};
		}

		constexpr auto operator-=(const PScore &other) -> auto &
		{
			m_score -= other.m_score;
			return *this;
		}

		[[nodiscard]] constexpr auto operator*(S32 v) const
		{
			return PScore{m_score * v};
		}

		constexpr auto operator*=(S32 v) -> auto &
		{
			m_score *= v;
			return *this;
		}

		[[nodiscard]] constexpr auto operator-() const
		{
			return PScore{-m_score};
		}

		[[nodiscard]] constexpr bool operator==(const PScore &other) const = default;

		friend std::ostream& operator<< (std::ostream& stream, const PScore& score) {
            stream << "(" << score.mg() << "\t" << score.eg() << ")";
			return stream;
        }

};

#define S(a,b) PScore((a),(b))
#define SCORE_ZERO S(0,0)