/*
 * Copyright 2017 Ben-Hur Carlos Vieira Langoni Junior
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.shawn.games.Serendipity.Chess;

import java.util.LinkedList;
import java.util.List;

/**
 * A collection of bitboards and related constant values useful to perform
 * efficient board manipulations, fast squares comparisons, and to mask some
 * operations to limited portions of the board.
 * <p>
 * A bitboard is a specialized bit array data structure used in chess
 * programming, where each bit corresponds to some binary information stored in
 * a board (e.g. the presence of a piece, the property of a square, etc.). Given
 * chessboards have 64 squares, bitboards can be represented by 64-bits numbers
 * (long unsigned integer values). Data manipulation and comparison of different
 * bitboards can be performed using bitwise operations.
 */
public class Bitboard
{
	/**
	 * The bitboard representing the light squares on a chessboard.
	 */
	public static final long lightSquares = 0x55AA55AA55AA55AAL;
	/**
	 * The bitboard representing the dark squares on a chessboard.
	 */
	public static final long darkSquares = 0xAA55AA55AA55AA55L;

	/**
	 * The bitboards representing the ranks on a chessboard. Bitboard at index 0
	 * identifies the 1st rank on a board, bitboard at index 1 the 2nd rank, etc.
	 */
	final static long[] rankBB = { 0x00000000000000FFL, 0x000000000000FF00L, 0x0000000000FF0000L, 0x00000000FF000000L,
			0x000000FF00000000L, 0x0000FF0000000000L, 0x00FF000000000000L, 0xFF00000000000000L };
	/**
	 * The bitboards representing the files on a chessboard. Bitboard at index 0
	 * identifies the 1st file on a board, bitboard at index 1 the 2nd file, etc.
	 */
	final static long[] fileBB = { 0x0101010101010101L, 0x0202020202020202L, 0x0404040404040404L, 0x0808080808080808L,
			0x1010101010101010L, 0x2020202020202020L, 0x4040404040404040L, 0x8080808080808080L };

	/**
	 * Table of bitboards that represent portions of board included between two
	 * squares, specified by the indexes used to access the table. A portion of
	 * board is the two-dimensional space defined by the ranks and files of the
	 * squares.
	 * <p>
	 * For instance, the bitboard looked up by coordinates {@code (0, 18)} defines
	 * the portion of the board included between squares {@code A1} and {@code C3}
	 * (indexes 0 and 18 respectively), i.e. the set of squares
	 * {@code [A1, B1, C1, A2, B2, C2, A3, B3, C3]}.
	 */
	final static long[][] bbTable = new long[64][64];

	static
	{
		for (int x = 0; x < 64; x++)
		{
			for (int y = 0; y < 64; y++)
			{
				bbTable[x][y] = ((1L << y) | ((1L << y) - (1L << x)));
			}
		}
	}

	/**
	 * Returns the bitboard representing the single square provided in input.
	 *
	 * @param sq the square for which the bitboard must be returned
	 * @return the bitboard representation of the square
	 */
	static long sq2Bb(Square sq)
	{
		return sq.getBitboard();
	}

	/**
	 * Returns the index of the first (<i>rightmost</i>) bit set to 1 in the
	 * bitboard provided in input. The bit is the Least Significant 1-bit (LS1B).
	 *
	 * @param bb the bitboard for which the LS1B is to be returned
	 * @return the index of the first bit set to 1
	 */
	public static int bitScanForward(long bb)
	{
		return Long.numberOfTrailingZeros(bb);
	}

	/**
	 * Returns the index of the last (<i>leftmost</i>) bit set to 1 in the bitboard
	 * provided in input. The bit is the Most Significant 1-bit (MS1B).
	 *
	 * @param bb the bitboard for which the MS1B is to be returned
	 * @return the index of the last bit set to 1
	 */
	public static int bitScanReverse(long bb)
	{
		return 63 - Long.numberOfLeadingZeros(bb);
	}

	/**
	 * Returns the <i>sub-bitboard</i> included between the two squares provided in
	 * input, that is, the portion of the bitboard included between two squares. The
	 * <i>sub-bitboard</i> is a two-dimensional space defined by the ranks and files
	 * of the two squares. For example, if squares {@code A1} and {@code C3} are
	 * provided in input, the method returns only the following squares of the
	 * original bitboard: {@code [A1, B1, C1, A2, B2, C2, A3, B3, C3]}. All other
	 * bits are set to 0.
	 *
	 * @param bb  the bitboard the portion between the two squares must be returned
	 * @param sq1 the first square that defines the two-dimensional space
	 * @param sq2 the second square that defines the two-dimensional space
	 * @return the portion of the original bitboard included between the two squares
	 */
	public static long bitsBetween(long bb, int sq1, int sq2)
	{
		return bbTable[sq1][sq2] & bb;
	}

	/**
	 * Unsets the first bit set to 1. In other words, it sets to 0 the Least
	 * Significant 1-bit (LS1B).
	 *
	 * @param bb the bitboard to compute
	 * @return the resulting bitboard, from which the first bit set to 1 has been
	 *         unset
	 */
	public static long extractLsb(Long bb)
	{
		return bb & (bb - 1);
	}

	/**
	 * Check whether the given bitboard has only one bit set to 1.
	 *
	 * @param bb the bitboard to check
	 * @return {@code true} if the bitboard has only one bit set to 1
	 */
	public static boolean hasOnly1Bit(Long bb)
	{
		return bb != 0L && extractLsb(bb) == 0L;
	}

	/**
	 * Returns the bitboard representing the square provided in input.
	 *
	 * @param sq the square for which the bitboard must be returned
	 * @return the bitboard representing the single square
	 */
	public static long getBbtable(Square sq)
	{
		return 1L << sq.ordinal();
	}

	/**
	 * Returns the list of squares that are set in the bitboard provided in input,
	 * that is, the squares corresponding to the indexes set to 1 in the bitboard.
	 *
	 * @param bb the bitboard from which the list of squares must be returned
	 * @return the list of squares corresponding to the bits set to 1 in the
	 *         bitboard
	 */
	public static List<Square> bbToSquareList(long bb)
	{
		List<Square> squares = new LinkedList<Square>();
		while (bb != 0L)
		{
			int sq = Bitboard.bitScanForward(bb);
			bb = Bitboard.extractLsb(bb);
			squares.add(Square.squareAt(sq));
		}
		return squares;
	}

	/**
	 * Returns the array of squares that are set in the bitboard provided in input,
	 * that is, the squares corresponding to the indexes set to 1 in the bitboard.
	 *
	 * @param bb the bitboard from which the array of squares must be returned
	 * @return the array of squares corresponding to the bits set to 1 in the
	 *         bitboard
	 */
	public static Square[] bbToSquareArray(long bb)
	{
		Square[] squares = new Square[Long.bitCount(bb)];
		int index = 0;
		while (bb != 0L)
		{
			int sq = bitScanForward(bb);
			bb = extractLsb(bb);
			squares[index++] = Square.squareAt(sq);
		}
		return squares;
	}

	/**
	 * Returns the bitboards representing the ranks on a chessboard.
	 *
	 * @return the bitboards representing the ranks
	 */
	public static long[] getRankbb()
	{
		return rankBB;
	}

	/**
	 * Returns the bitboards representing the files on a chessboard.
	 *
	 * @return the bitboards representing the files
	 */
	public static long[] getFilebb()
	{
		return fileBB;
	}

	/**
	 * Returns the bitboard representing the entire rank of the square given in
	 * input.
	 *
	 * @param sq the square for which the bitboard rank must be returned
	 * @return the bitboards representing the rank of the square
	 */
	public static long getRankbb(Square sq)
	{
		return rankBB[sq.getRank().ordinal()];
	}

	/**
	 * Returns the bitboard representing the entire file of the square given in
	 * input.
	 *
	 * @param sq the square for which the bitboard file must be returned
	 * @return the bitboards representing the file of the square
	 */
	public static long getFilebb(Square sq)
	{
		return fileBB[sq.getFile().ordinal()];
	}

	/**
	 * Returns the bitboard representing the rank given in input.
	 *
	 * @param rank the rank for which the corresponding bitboard must be returned
	 * @return the bitboards representing the rank
	 */
	public static long getRankbb(Rank rank)
	{
		return rankBB[rank.ordinal()];
	}

	/**
	 * Returns the bitboard representing the file given in input.
	 *
	 * @param file the file for which the corresponding bitboard must be returned
	 * @return the bitboards representing the file
	 */
	public static long getFilebb(File file)
	{
		return fileBB[file.ordinal()];
	}

	/**
	 * Returns the string representation of a bitboard in a readable format.
	 *
	 * @param bb the bitboard to print
	 * @return the string representation of the bitboard
	 */
	public static String bitboardToString(long bb)
	{
		StringBuilder b = new StringBuilder();
		for (int x = 0; x < 64; x++)
		{
			if (((1L << x) & bb) != 0L)
			{
				b.append("1");
			}
			else
			{
				b.append("0");
			}
			if ((x + 1) % 8 == 0)
			{
				b.append("\n");
			}
		}
		return b.toString();
	}
}
