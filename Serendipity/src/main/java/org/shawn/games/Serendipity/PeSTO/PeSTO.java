/*
  This file is part of Serendipity, an UCI chess engine written in Java.

  Copyright (C) 2024-2025  Shawn Xu <shawn@shawnxu.org>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

package org.shawn.games.Serendipity.PeSTO;

import org.shawn.games.Serendipity.Chess.*;

public class PeSTO
{
	// @formatter:off
	final static int[] MG_PAWN_TABLE = new int[]
	{
		      0,   0,   0,   0,   0,   0,  0,   0,
		     98, 134,  61,  95,  68, 126, 34, -11,
		     -6,   7,  26,  31,  65,  56, 25, -20,
		    -14,  13,   6,  21,  23,  12, 17, -23,
		    -27,  -2,  -5,  12,  17,   6, 10, -25,
		    -26,  -4,  -4, -10,   3,   3, 33, -12,
		    -35,  -1, -20, -23, -15,  24, 38, -22,
		      0,   0,   0,   0,   0,   0,  0,   0,
	};

	final static int[] EG_PAWN_TABLE = new int[]
	{
		      0,   0,   0,   0,   0,   0,   0,   0,
		    178, 173, 158, 134, 147, 132, 165, 187,
		     94, 100,  85,  67,  56,  53,  82,  84,
		     32,  24,  13,   5,  -2,   4,  17,  17,
		     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
		      4,   7,  -6,   1,   0,  -5,  -1,  -8,
		     13,   8,   8,  10,  13,   0,   2,  -7,
		      0,   0,   0,   0,   0,   0,   0,   0,
	};

	final static int[] MG_KNIGHT_TABLE = new int[]
	{
		    -167, -89, -34, -49,  61, -97, -15, -107,
		     -73, -41,  72,  36,  23,  62,   7,  -17,
		     -47,  60,  37,  65,  84, 129,  73,   44,
		      -9,  17,  19,  53,  37,  69,  18,   22,
		     -13,   4,  16,  13,  28,  19,  21,   -8,
		     -23,  -9,  12,  10,  19,  17,  25,  -16,
		     -29, -53, -12,  -3,  -1,  18, -14,  -19,
		    -105, -21, -58, -33, -17, -28, -19,  -23,
	};

	final static int[] EG_KNIGHT_TABLE = new int[]
	{
		    -58, -38, -13, -28, -31, -27, -63, -99,
		    -25,  -8, -25,  -2,  -9, -25, -24, -52,
		    -24, -20,  10,   9,  -1,  -9, -19, -41,
		    -17,   3,  22,  22,  22,  11,   8, -18,
		    -18,  -6,  16,  25,  16,  17,   4, -18,
		    -23,  -3,  -1,  15,  10,  -3, -20, -22,
		    -42, -20, -10,  -5,  -2, -20, -23, -44,
		    -29, -51, -23, -15, -22, -18, -50, -64,
	};

	final static int[] MG_BISHOP_TABLE = new int[]
	{
		    -29,   4, -82, -37, -25, -42,   7,  -8,
		    -26,  16, -18, -13,  30,  59,  18, -47,
		    -16,  37,  43,  40,  35,  50,  37,  -2,
		     -4,   5,  19,  50,  37,  37,   7,  -2,
		     -6,  13,  13,  26,  34,  12,  10,   4,
		      0,  15,  15,  15,  14,  27,  18,  10,
		      4,  15,  16,   0,   7,  21,  33,   1,
		    -33,  -3, -14, -21, -13, -12, -39, -21,
	};

	final static int[] EG_BISHOP_TABLE = new int[]
	{
		    -14, -21, -11,  -8, -7,  -9, -17, -24,
		     -8,  -4,   7, -12, -3, -13,  -4, -14,
		      2,  -8,   0,  -1, -2,   6,   0,   4,
		     -3,   9,  12,   9, 14,  10,   3,   2,
		     -6,   3,  13,  19,  7,  10,  -3,  -9,
		    -12,  -3,   8,  10, 13,   3,  -7, -15,
		    -14, -18,  -7,  -1,  4,  -9, -15, -27,
		    -23,  -9, -23,  -5, -9, -16,  -5, -17,
	};

	final static int[] MG_ROOK_TABLE = new int[]
	{
		     32,  42,  32,  51, 63,  9,  31,  43,
		     27,  32,  58,  62, 80, 67,  26,  44,
		     -5,  19,  26,  36, 17, 45,  61,  16,
		    -24, -11,   7,  26, 24, 35,  -8, -20,
		    -36, -26, -12,  -1,  9, -7,   6, -23,
		    -45, -25, -16, -17,  3,  0,  -5, -33,
		    -44, -16, -20,  -9, -1, 11,  -6, -71,
		    -19, -13,   1,  17, 16,  7, -37, -26,
	};

	final static int[] EG_ROOK_TABLE = new int[]
	{
		    13, 10, 18, 15, 12,  12,   8,   5,
		    11, 13, 13, 11, -3,   3,   8,   3,
		     7,  7,  7,  5,  4,  -3,  -5,  -3,
		     4,  3, 13,  1,  2,   1,  -1,   2,
		     3,  5,  8,  4, -5,  -6,  -8, -11,
		    -4,  0, -5, -1, -7, -12,  -8, -16,
		    -6, -6,  0,  2, -9,  -9, -11,  -3,
		    -9,  2,  3, -1, -5, -13,   4, -20,
	};

	final static int[] MG_QUEEN_TABLE = new int[]
	{
		    -28,   0,  29,  12,  59,  44,  43,  45,
		    -24, -39,  -5,   1, -16,  57,  28,  54,
		    -13, -17,   7,   8,  29,  56,  47,  57,
		    -27, -27, -16, -16,  -1,  17,  -2,   1,
		     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
		    -14,   2, -11,  -2,  -5,   2,  14,   5,
		    -35,  -8,  11,   2,   8,  15,  -3,   1,
		     -1, -18,  -9,  10, -15, -25, -31, -50,
	};

	final static int[] EG_QUEEN_TABLE = new int[]
	{
		     -9,  22,  22,  27,  27,  19,  10,  20,
		    -17,  20,  32,  41,  58,  25,  30,   0,
		    -20,   6,   9,  49,  47,  35,  19,   9,
		      3,  22,  24,  45,  57,  40,  57,  36,
		    -18,  28,  19,  47,  31,  34,  39,  23,
		    -16, -27,  15,   6,   9,  17,  10,   5,
		    -22, -23, -30, -16, -16, -23, -36, -32,
		    -33, -28, -22, -43,  -5, -32, -20, -41,
		};

	final static int[] MG_KING_TABLE = new int[]
	{
		    -65,  23,  16, -15, -56, -34,   2,  13,
		     29,  -1, -20,  -7,  -8,  -4, -38, -29,
		     -9,  24,   2, -16, -20,   6,  22, -22,
		    -17, -20, -12, -27, -30, -25, -14, -36,
		    -49,  -1, -27, -39, -46, -44, -33, -51,
		    -14, -14, -22, -46, -44, -30, -15, -27,
		      1,   7,  -8, -64, -43, -16,   9,   8,
		    -15,  36,  12, -54,   8, -28,  24,  14,
	};

	final static int[] EG_KING_TABLE = new int[]
	{
		    -74, -35, -18, -18, -11,  15,   4, -17,
		    -12,  17,  14,  17,  17,  38,  23,  11,
		     10,  17,  23,  15,  20,  45,  44,  13,
		     -8,  22,  24,  27,  26,  33,  26,   3,
		    -18,  -4,  21,  24,  27,  23,   9, -11,
		    -19,  -3,  11,  21,  23,  16,   7,  -9,
		    -27, -11,   4,  13,  14,   4,  -5, -17,
		    -53, -34, -21, -11, -28, -14, -24, -43
	};
	
	final static int[] MIRRORED_SQUARE_VALUE = new int[]
	{
			56, 57, 58, 59, 60, 61, 62, 63,
			48, 49, 50, 51, 52, 53, 54, 55,
			40, 41, 42, 43, 44, 45, 46, 47,
			32, 33, 34, 35, 36, 37, 38, 39,
			24, 25, 26, 27, 28, 29, 30, 31,
			16, 17, 18, 19, 20, 21, 22, 23,
			8,   9, 10, 11, 12, 13, 14, 15,
			0,   1,  2,  3,  4,  5,  6,  7
	};
	// @formatter:on

	final static int MG_PAWN_VALUE = 82;
	final static int MG_KNIGHT_VALUE = 337;
	final static int MG_BISHOP_VALUE = 365;
	final static int MG_ROOK_VALUE = 477;
	final static int MG_QUEEN_VALUE = 1025;

	final static int EG_PAWN_VALUE = 94;
	final static int EG_KNIGHT_VALUE = 281;
	final static int EG_BISHOP_VALUE = 297;
	final static int EG_ROOK_VALUE = 512;
	final static int EG_QUEEN_VALUE = 936;

	final static int PAWN_PHASE = 0;
	final static int KNIGHT_PHASE = 1;
	final static int BISHOP_PHASE = 1;
	final static int ROOK_PHASE = 2;
	final static int QUEEN_PHASE = 4;

	final static int TEMPO = 8;

	final static int MAX_PHASE = KNIGHT_PHASE * 4 + BISHOP_PHASE * 4 + ROOK_PHASE * 4 + QUEEN_PHASE * 2;

	private static int getIndex(Side side, Square square)
	{
		return side == Side.BLACK ? square.ordinal() : MIRRORED_SQUARE_VALUE[square.ordinal()];
	}

	private static int pieceMiddleGameValue(Piece piece, Square square)
	{
		switch (piece.getPieceType())
		{
			case PAWN:
				return MG_PAWN_TABLE[getIndex(piece.getPieceSide(), square)] + MG_PAWN_VALUE;
			case KNIGHT:
				return MG_KNIGHT_TABLE[getIndex(piece.getPieceSide(), square)] + MG_KNIGHT_VALUE;
			case BISHOP:
				return MG_BISHOP_TABLE[getIndex(piece.getPieceSide(), square)] + MG_BISHOP_VALUE;
			case ROOK:
				return MG_ROOK_TABLE[getIndex(piece.getPieceSide(), square)] + MG_ROOK_VALUE;
			case QUEEN:
				return MG_QUEEN_TABLE[getIndex(piece.getPieceSide(), square)] + MG_QUEEN_VALUE;
			case KING:
				return MG_KING_TABLE[getIndex(piece.getPieceSide(), square)];
			default:
				return 0;

		}
	}

	private static int pieceEndGameValue(Piece piece, Square square)
	{
		switch (piece.getPieceType())
		{
			case PAWN:
				return EG_PAWN_TABLE[getIndex(piece.getPieceSide(), square)] + EG_PAWN_VALUE;
			case KNIGHT:
				return EG_KNIGHT_TABLE[getIndex(piece.getPieceSide(), square)] + EG_KNIGHT_VALUE;
			case BISHOP:
				return EG_BISHOP_TABLE[getIndex(piece.getPieceSide(), square)] + EG_BISHOP_VALUE;
			case ROOK:
				return EG_ROOK_TABLE[getIndex(piece.getPieceSide(), square)] + EG_ROOK_VALUE;
			case QUEEN:
				return EG_QUEEN_TABLE[getIndex(piece.getPieceSide(), square)] + EG_QUEEN_VALUE;
			case KING:
				return EG_KING_TABLE[getIndex(piece.getPieceSide(), square)];
			default:
				return 0;

		}
	}

	private static int middleGameEval(Board board)
	{
		long pieces = board.getBitboard(board.getSideToMove());

		int score = 0;

		while (pieces != 0)
		{
			Square square = Square.squareAt(Bitboard.bitScanForward(pieces));
			pieces = Bitboard.extractLsb(pieces);
			score += pieceMiddleGameValue(board.getPiece(square), square);
		}

		pieces = board.getBitboard(board.getSideToMove().flip());

		while (pieces != 0)
		{
			Square square = Square.squareAt(Bitboard.bitScanForward(pieces));
			pieces = Bitboard.extractLsb(pieces);
			score -= pieceMiddleGameValue(board.getPiece(square), square);
		}

		return score;
	}

	private static int endGameEval(Board board)
	{
		long pieces = board.getBitboard(board.getSideToMove());

		int score = 0;

		while (pieces != 0)
		{
			Square square = Square.squareAt(Bitboard.bitScanForward(pieces));
			pieces = Bitboard.extractLsb(pieces);
			score += pieceEndGameValue(board.getPiece(square), square);
		}

		pieces = board.getBitboard(board.getSideToMove().flip());

		while (pieces != 0)
		{
			Square square = Square.squareAt(Bitboard.bitScanForward(pieces));
			pieces = Bitboard.extractLsb(pieces);
			score -= pieceEndGameValue(board.getPiece(square), square);
		}

		return score;
	}

	private static int gamePhase(Board board)
	{
		Side ourSide = board.getSideToMove();
		Side opposite = board.getSideToMove().flip();
		// @formatter:off
		return Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.PAWN))) * PAWN_PHASE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.KNIGHT))) * KNIGHT_PHASE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.BISHOP))) * BISHOP_PHASE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.ROOK))) * ROOK_PHASE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.QUEEN))) * QUEEN_PHASE
				+ Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.PAWN))) * PAWN_PHASE
				+ Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.KNIGHT))) * KNIGHT_PHASE
				+ Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.BISHOP))) * BISHOP_PHASE
				+ Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.ROOK))) * ROOK_PHASE
				+ Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.QUEEN))) * QUEEN_PHASE;
		// @formatter:on
	}

	private static int middleGameMaterialEval(Board board)
	{

		Side ourSide = board.getSideToMove();
		Side opposite = board.getSideToMove().flip();
		// @formatter:off
		return Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.PAWN))) * MG_PAWN_VALUE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.KNIGHT))) * MG_KNIGHT_VALUE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.BISHOP))) * MG_BISHOP_VALUE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.ROOK))) * MG_ROOK_VALUE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.QUEEN))) * MG_QUEEN_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.PAWN))) * MG_PAWN_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.KNIGHT))) * MG_KNIGHT_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.BISHOP))) * MG_BISHOP_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.ROOK))) * MG_ROOK_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.QUEEN))) * MG_QUEEN_VALUE;
		// @formatter:on
	}

	private static int endGameMaterialEval(Board board)
	{

		Side ourSide = board.getSideToMove();
		Side opposite = board.getSideToMove().flip();
		// @formatter:off
		return Long.bitCount(board.getBitboard(Piece.make(board.getSideToMove(), PieceType.PAWN))) * EG_PAWN_VALUE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.KNIGHT))) * EG_KNIGHT_VALUE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.BISHOP))) * EG_BISHOP_VALUE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.ROOK))) * EG_ROOK_VALUE
				+ Long.bitCount(board.getBitboard(Piece.make(ourSide, PieceType.QUEEN))) * EG_QUEEN_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.PAWN))) * EG_PAWN_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.KNIGHT))) * EG_KNIGHT_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.BISHOP))) * EG_BISHOP_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.ROOK))) * EG_ROOK_VALUE
				- Long.bitCount(board.getBitboard(Piece.make(opposite, PieceType.QUEEN))) * EG_QUEEN_VALUE;
		// @formatter:on
	}

	private static int bishopPairAdjustment(Board board)
	{
		return (Long.bitCount(board.getBitboard(Piece.make(board.getSideToMove(), PieceType.BISHOP))) == 2 ? 720 : 0)
				- (Long.bitCount(board.getBitboard(Piece.make(board.getSideToMove().flip(), PieceType.BISHOP))) == 2
						? 720
						: 0);
	}

	public static int materialEval(Board board)
	{
		int gamePhase = Math.min(MAX_PHASE, gamePhase(board));
		return middleGameMaterialEval(board) * gamePhase + endGameMaterialEval(board) * (MAX_PHASE - gamePhase);
	}

	public static int evaluate(Board board)
	{
		int gamePhase = Math.min(MAX_PHASE, gamePhase(board));
		return middleGameEval(board) * gamePhase + endGameEval(board) * (MAX_PHASE - gamePhase)
				+ bishopPairAdjustment(board) + TEMPO * MAX_PHASE;
	}
}
