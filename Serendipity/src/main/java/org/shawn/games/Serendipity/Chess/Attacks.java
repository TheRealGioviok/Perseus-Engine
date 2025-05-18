package org.shawn.games.Serendipity.Chess;

public class Attacks
{
	private static final long[] kingAttacks;
	private static final long[] knightAttacks;
	private static final long[] bishopMask;
	private static final int[] bishopShift;
	private static final long[] bishopMagic;
	private static final long[][] bishopAttacks;
	private static final long[] rookMask;
	private static final int[] rookShift;
	private static final long[] rookMagic;
	private static final long[][] rookAttacks;

	static
	{
		knightAttacks = new long[64];
		kingAttacks = new long[64];
		bishopMask = new long[64];
		bishopShift = new int[64];
		bishopMagic = Precomputed.bishopMagic;
		bishopAttacks = new long[64][512];
		rookMask = new long[64];
		rookShift = new int[64];
		rookAttacks = new long[64][4096];
		rookMagic = Precomputed.rookMagic;

		for (int sqIdx = 0; sqIdx < 64; sqIdx++)
		{
			final Square sq = Square.values()[sqIdx];

			knightAttacks[sqIdx] = generateKnightAttacks(sq);
			kingAttacks[sqIdx] = generateKingAttacks(sq);

			bishopMask[sqIdx] = generateBishopMask(sq);
			bishopShift[sqIdx] = generateMagicShift(bishopMask[sqIdx]);
			bishopAttacks[sqIdx] = generateBishopAttacks(sq, bishopMask[sqIdx], bishopMagic[sqIdx], bishopShift[sqIdx]);

			rookMask[sqIdx] = generateRookMask(sq);
			rookShift[sqIdx] = generateMagicShift(rookMask[sqIdx]);
			rookAttacks[sqIdx] = generateRookAttacks(sq, rookMask[sqIdx], rookMagic[sqIdx], rookShift[sqIdx]);
		}
	}

	public enum Direction
	{
		NORTH, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST
	}

	public static Square shift(Square square, Direction direction)
	{
		return Square.fromBitboard(shift(square.getBitboard(), direction));
	}

	public static long shift(long bitboard, Direction direction)
	{
		switch (direction)
		{
			case NORTH:
				return bitboard << 8;
			case SOUTH:
				return bitboard >>> 8;
			case EAST:
				return (bitboard & ~Bitboard.fileBB[7]) << 1;
			case WEST:
				return (bitboard & ~Bitboard.fileBB[0]) >>> 1;
			case NORTHEAST:
				return (bitboard & ~Bitboard.fileBB[7]) << 9;
			case NORTHWEST:
				return (bitboard & ~Bitboard.fileBB[0]) << 7;
			case SOUTHEAST:
				return (bitboard & ~Bitboard.fileBB[7]) >>> 7;
			case SOUTHWEST:
				return (bitboard & ~Bitboard.fileBB[0]) >>> 9;
		}

		return bitboard;
	}

	private static long generateKnightAttacks(Square square)
	{
		long attacks = 0L;
		long squareBB = square.getBitboard();

		attacks |= shift(shift(squareBB, Direction.NORTH), Direction.NORTHEAST);
		attacks |= shift(shift(squareBB, Direction.NORTH), Direction.NORTHWEST);
		attacks |= shift(shift(squareBB, Direction.SOUTH), Direction.SOUTHEAST);
		attacks |= shift(shift(squareBB, Direction.SOUTH), Direction.SOUTHWEST);
		attacks |= shift(shift(squareBB, Direction.EAST), Direction.NORTHEAST);
		attacks |= shift(shift(squareBB, Direction.EAST), Direction.SOUTHEAST);
		attacks |= shift(shift(squareBB, Direction.WEST), Direction.NORTHWEST);
		attacks |= shift(shift(squareBB, Direction.WEST), Direction.SOUTHWEST);

		return attacks;
	}

	private static long generateKingAttacks(Square square)
	{
		long attacks = 0L;
		long squareBB = square.getBitboard();

		attacks |= shift(squareBB, Direction.NORTH);
		attacks |= shift(squareBB, Direction.SOUTH);
		attacks |= shift(squareBB, Direction.EAST);
		attacks |= shift(squareBB, Direction.WEST);
		attacks |= shift(squareBB, Direction.NORTHEAST);
		attacks |= shift(squareBB, Direction.NORTHWEST);
		attacks |= shift(squareBB, Direction.SOUTHEAST);
		attacks |= shift(squareBB, Direction.SOUTHWEST);

		return attacks;
	}

	private static long generateRayMask(Square square, Direction direction)
	{
		long mask = 0L;
		long sqBB = square.getBitboard();
		long nextBB = shift(sqBB, direction);

		while (true)
		{
			sqBB = nextBB;
			nextBB = shift(sqBB, direction);

			if (0L == nextBB)
			{
				break;
			}

			mask |= sqBB;
		}

		return mask;
	}

	private static long generateBishopMask(Square square)
	{
		long mask = 0L;

		mask |= generateRayMask(square, Direction.NORTHEAST);
		mask |= generateRayMask(square, Direction.NORTHWEST);
		mask |= generateRayMask(square, Direction.SOUTHEAST);
		mask |= generateRayMask(square, Direction.SOUTHWEST);

		return mask;
	}

	private static long generateRookMask(Square square)
	{
		long mask = 0L;

		mask |= generateRayMask(square, Direction.NORTH);
		mask |= generateRayMask(square, Direction.SOUTH);
		mask |= generateRayMask(square, Direction.EAST);
		mask |= generateRayMask(square, Direction.WEST);

		return mask;
	}

	private static int generateMagicShift(long mask)
	{
		return 64 - Long.bitCount(mask);
	}

	private static long generateRayAttack(Square square, Direction direction, long occ)
	{
		long attack = 0L;
		long sqBB = square.getBitboard();

		while (true)
		{
			sqBB = shift(sqBB, direction);
			attack |= sqBB;

			if (sqBB == 0 || 0L != (attack & occ))
			{
				break;
			}
		}

		return attack;
	}

	private static long generateBishopAttack(Square square, long occ)
	{
		long attack = 0L;

		attack |= generateRayAttack(square, Direction.NORTHEAST, occ);
		attack |= generateRayAttack(square, Direction.NORTHWEST, occ);
		attack |= generateRayAttack(square, Direction.SOUTHEAST, occ);
		attack |= generateRayAttack(square, Direction.SOUTHWEST, occ);

		return attack;
	}

	private static long generateRookAttack(Square square, long occ)
	{
		long attack = 0L;

		attack |= generateRayAttack(square, Direction.NORTH, occ);
		attack |= generateRayAttack(square, Direction.SOUTH, occ);
		attack |= generateRayAttack(square, Direction.EAST, occ);
		attack |= generateRayAttack(square, Direction.WEST, occ);

		return attack;
	}

	private static long[] generateBishopAttacks(Square square, long mask, long magic, int shift)
	{
		long[] attacks = new long[512];
		long cur_mask = mask;

		while (true)
		{
			attacks[(int) (cur_mask * magic >>> shift)] = generateBishopAttack(square, cur_mask);

			if (0L == cur_mask)
			{
				break;
			}

			cur_mask = (cur_mask - 1) & mask;
		}

		return attacks;
	}

	private static long[] generateRookAttacks(Square square, long mask, long magic, int shift)
	{
		long[] attacks = new long[4096];
		long cur_mask = mask;

		while (true)
		{
			attacks[(int) (cur_mask * magic >>> shift)] = generateRookAttack(square, cur_mask);

			if (0L == cur_mask)
			{
				break;
			}

			cur_mask = (cur_mask - 1) & mask;
		}

		return attacks;
	}

	/**
	 * Returns the bitboard representing the squares attacked by pawns placed on a
	 * set of locations for a given side.
	 *
	 * @param side   the side to move
	 * @param pawnBB the bitboard of pawns
	 * @return the bitboard of the squares attacked by the pawn
	 */
	public static long getPawnAttacks(Side side, long pawnBB)
	{
		if (side.equals(Side.WHITE))
		{
			return shift(pawnBB, Direction.NORTHEAST) | shift(pawnBB, Direction.NORTHWEST);
		}

		else
		{
			return shift(pawnBB, Direction.SOUTHEAST) | shift(pawnBB, Direction.SOUTHWEST);
		}
	}

	/**
	 * Returns the bitboard representing the possible captures by a pawn placed on
	 * the input square for a given side. The method expects a bitboard of possible
	 * targets, the occupied squares, and the square for which en passant is
	 * playable ({@link Square#NONE} if en passant can not be played).
	 *
	 * @param side      the side to move
	 * @param square    the square the pawn is placed
	 * @param occupied  a bitboard of possible targets
	 * @param enPassant the square in which en passant capture is possible,
	 *                  {@link Square#NONE} otherwise
	 * @return the bitboard of the squares where the pawn can move to capturing a
	 *         piece
	 */
	public static long getPawnCaptures(Side side, long pawnBB, long occupied, Square enPassant)
	{
		long pawnAttacks = getPawnAttacks(side, pawnBB);

		if (!enPassant.equals(Square.NONE))
		{
			long ep = enPassant.getBitboard();
			occupied |= side.equals(Side.WHITE) ? ep << 8L : ep >> 8L;
		}

		return pawnAttacks & occupied;
	}

	/**
	 * Returns the bitboard representing the possible moves, excluding captures, by
	 * a pawn placed on the input square for a given side. The method expects a
	 * bitboard of occupied squares where the pawn can not move to.
	 *
	 * @param side     the side to move
	 * @param square   the square the pawn is placed
	 * @param occupied a bitboard of occupied squares
	 * @return the bitboard of the squares where the pawn can move to
	 */
	public static long getPawnMoves(Side side, long pawnBB, long occupied)
	{
		Direction pushDirection = side.equals(Side.WHITE) ? Direction.NORTH : Direction.SOUTH;
		final long doublePushMask = side.equals(Side.WHITE) ? Bitboard.rankBB[3] : Bitboard.rankBB[4];

		final long pawnPushes = shift(pawnBB, pushDirection) & ~occupied;
		final long pawnDoublePushes = shift(pawnPushes, pushDirection) & doublePushMask & ~occupied;

		return pawnPushes | pawnDoublePushes;
	}

	/**
	 * Returns the bitboard representing the knight movement attacks, computed
	 * applying the provided mask. It could either refer to the squares attacked by
	 * a knight placed on the input square, or conversely the knights that can
	 * attack the square.
	 *
	 * @param square the square for which to calculate the knight attacks
	 * @param mask   the mask to apply to the knight attacks
	 * @return the bitboard of knight movement attacks
	 */
	public static long getKnightAttacks(Square square, long mask)
	{
		return knightAttacks[square.ordinal()] & mask;
	}

	/**
	 * Returns the bitboard representing the bishop movement attacks, computed
	 * applying the provided mask. It could either refer to the squares attacked by
	 * a bishop placed on the input square, or conversely the bishops that can
	 * attack the square.
	 *
	 * @param square the square for which to calculate the bishop attacks
	 * @param mask   the mask to apply to the bishop attacks
	 * @return the bitboard of bishop movement attacks
	 */
	public static long getBishopAttacks(long mask, Square square)
	{
		long blockerMask = bishopMask[square.ordinal()];
		long magic = bishopMagic[square.ordinal()];
		int shift = bishopShift[square.ordinal()];

		return bishopAttacks[square.ordinal()][(int) ((mask & blockerMask) * magic >>> shift)];
	}

	/**
	 * Returns the bitboard representing the rook movement attacks, computed
	 * applying the provided mask. It could either refer to the squares attacked by
	 * a rook placed on the input square, or conversely the rooks that can attack
	 * the square.
	 *
	 * @param square the square for which to calculate the rook attacks
	 * @param mask   the mask to apply to the rook attacks
	 * @return the bitboard of rook movement attacks
	 */
	public static long getRookAttacks(long mask, Square square)
	{
		long blockerMask = rookMask[square.ordinal()];
		long magic = rookMagic[square.ordinal()];
		int shift = rookShift[square.ordinal()];

		return rookAttacks[square.ordinal()][(int) ((mask & blockerMask) * magic >>> shift)];
	}

	/**
	 * Returns the bitboard representing the queen movement attacks, computed
	 * applying the provided mask. It could either refer to the squares attacked by
	 * a queen placed on the input square, or conversely the queens that can attack
	 * the square.
	 *
	 * @param square the square for which to calculate the queen attacks
	 * @param mask   the mask to apply to the queen attacks
	 * @return the bitboard of queen movement attacks
	 */
	public static long getQueenAttacks(long mask, Square square)
	{
		return getRookAttacks(mask, square) | getBishopAttacks(mask, square);
	}

	/**
	 * Returns the bitboard representing the king movement attacks, computed
	 * applying the provided mask. It could either refer to the squares attacked by
	 * a king placed on the input square, or conversely the kings that can attack
	 * the square.
	 *
	 * @param square the square for which to calculate the king attacks
	 * @param mask   the mask to apply to the king attacks
	 * @return the bitboard of king movement attacks
	 */
	public static long getKingAttacks(Square square, long mask)
	{
		return kingAttacks[square.ordinal()] & mask;
	}

	private class Precomputed
	{
		private static final long[] bishopMagic = { //
				0x0808431002060060L, 0x0010420214086902L, 0x0004211401000114L, 0x02060a1200005290L, //
				0x8012021000400000L, 0x0801015840800080L, 0x0004120804044500L, 0x0501040184240604L, //
				0x0a00682310020601L, 0x4001041002004901L, 0x6804500102002464L, 0x4800022082014400L, //
				0x00000a0210800008L, 0x0280c088044041b0L, 0x84000044100c1080L, 0x24101888c8021002L, //
				0x2008080451540810L, 0x0004080801080208L, 0xc0021a4400620200L, 0x0098000082004400L, //
				0x800e10c401040400L, 0x0420400809101021L, 0x0052000101901401L, 0x2001820032011000L, //
				0x0002109662604a03L, 0x0738580002100104L, 0x04030400018c0400L, 0x0001802108020020L, //
				0x0089001021014000L, 0x4000820001010080L, 0x880401406c064200L, 0x100202000c84c14dL, //
				0x0021211080200400L, 0x0108041010020200L, 0x000c004800011200L, 0x0280401048060200L, //
				0x0102008400220020L, 0x4022080200004050L, 0x0122008401050402L, 0x0014040150812100L, //
				0x0582020240212000L, 0x0218a80402031000L, 0x0101420041001020L, 0x0020902024200800L, //
				0x0800401093021202L, 0x00a2100202020020L, 0x1120640421408080L, 0x00888084008054c8L, //
				0x0004120804044500L, 0x1240908090102005L, 0x8000226884100400L, 0x0880008042020000L, //
				0x2000040410442040L, 0x041010200169002aL, 0x0808431002060060L, 0x0010420214086902L, //
				0x0501040184240604L, 0x24101888c8021002L, 0x6003250104431002L, 0x8012a0040c208801L, //
				0x0108041010020200L, 0x0400000420440100L, 0x0a00682310020601L, 0x0808431002060060L, };

		private static final long[] rookMagic = { //
				0x0080004008811020L, 0x4840004020021009L, 0x5900130060004028L, 0xa480100080050801L, //
				0x1900100800050002L, 0xa400a00802040010L, 0x4100010020920024L, 0x1080002100005c80L, //
				0x0002800840028020L, 0x0000402000401000L, 0x0000808010002000L, 0x202200401a011020L, //
				0x0021001048010204L, 0x0001000844004300L, 0x0201000402000100L, 0x0c01800100004080L, //
				0x001c808004400220L, 0x0060810021024001L, 0x0020030040102101L, 0x0040808010030800L, //
				0x0078008048240080L, 0x400301001c004618L, 0x6400040006300108L, 0x00000a0000440081L, //
				0x0084400080028022L, 0x11005000c0002001L, 0x2410001080200080L, 0x00010029001000a4L, //
				0x8008080080040080L, 0x0402008080140026L, 0x00080c0101000200L, 0x6800006600010884L, //
				0x400840042080008aL, 0x2081a00080804008L, 0x0400504105002000L, 0x4022214202001048L, //
				0x0281800402800801L, 0x0002804401800600L, 0x0200b0018c000248L, 0x0800010062000094L, //
				0x0084400080028022L, 0x0001600050014000L, 0x400200c0208a0010L, 0x400200c0208a0010L, //
				0x0002006810060020L, 0x10860008101a0005L, 0x1482980302040050L, 0x000ca41680420001L, //
				0x8002409600630200L, 0x8002409600630200L, 0x2280460410208200L, 0x0010608810010100L, //
				0x8008080080040080L, 0x0001000844004300L, 0x0201000402000100L, 0x4080008244051600L, //
				0x0020210041108005L, 0x8009400021001281L, 0x0100a230802a00c2L, 0x11c2004008200432L, //
				0x402100020410c801L, 0x0112001001082452L, 0x6802101148820804L, 0x9028840023811042L, };

	}
}