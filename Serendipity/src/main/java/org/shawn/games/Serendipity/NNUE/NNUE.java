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

package org.shawn.games.Serendipity.NNUE;

import java.io.*;
import java.util.Objects;

import org.shawn.games.Serendipity.Chess.*;

public class NNUE
{
	private static final int COLOR_STRIDE = 64 * 6;
	private static final int PIECE_STRIDE = 64;

	static final int HIDDEN_SIZE = 1536;
	static final int FEATURE_SIZE = 768;
	static final int OUTPUT_BUCKETS = 8;
	private static final int DIVISOR = (32 + OUTPUT_BUCKETS - 1) / OUTPUT_BUCKETS;
	static final int INPUT_BUCKET_SIZE = 7;
	// @formatter:off
	private static final int[] INPUT_BUCKETS = new int[]
	{
			0, 0, 1, 1, 2, 2, 3, 3,
			4, 4, 4, 4, 5, 5, 5, 5,
			6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6,
	};
	// @formatter:on

	public static final int SCALE = 400;
	public static final int QA = 255;
	public static final int QB = 64;

	final short[][] L1Weights;
	final short[] L1Biases;
	private final short[][] L2Weights;
	private final short[] outputBiases;

	private static final Inference INFERENCE = InferenceChooser.chooseInference();

	private short toLittleEndian(short input)
	{
		return (short) (((input & 0xFF) << 8) | ((input & 0xFF00) >> 8));
	}

	public NNUE(String filePath) throws IOException
	{
		DataInputStream networkData = new DataInputStream(
				Objects.requireNonNull(getClass().getResourceAsStream(filePath)));

		L1Weights = new short[FEATURE_SIZE * INPUT_BUCKET_SIZE][HIDDEN_SIZE];

		for (int i = 0; i < FEATURE_SIZE * INPUT_BUCKET_SIZE; i++)
		{
			for (int j = 0; j < HIDDEN_SIZE; j++)
			{
				L1Weights[i][j] = toLittleEndian(networkData.readShort());
			}
		}

		L1Biases = new short[HIDDEN_SIZE];

		for (int i = 0; i < HIDDEN_SIZE; i++)
		{
			L1Biases[i] = toLittleEndian(networkData.readShort());
		}

		L2Weights = new short[OUTPUT_BUCKETS][HIDDEN_SIZE * 2];

		for (int i = 0; i < HIDDEN_SIZE * 2; i++)
		{
			for (int j = 0; j < OUTPUT_BUCKETS; j++)
			{
				L2Weights[j][i] = toLittleEndian(networkData.readShort());
			}
		}

		outputBiases = new short[OUTPUT_BUCKETS];

		for (int i = 0; i < OUTPUT_BUCKETS; i++)
		{
			outputBiases[i] = toLittleEndian(networkData.readShort());
		}

		networkData.close();
	}

	public static int chooseOutputBucket(Board board)
	{
		return (Long.bitCount(board.getBitboard()) - 2) / DIVISOR;
	}

	public static int evaluate(Board board, NNUE network, AccumulatorStack accumulators)
	{
		final int chosenBucket = chooseOutputBucket(board);

		return INFERENCE.forward(accumulators.refreshAndGet(board), board.getSideToMove(),
				network.L2Weights[chosenBucket], network.outputBiases[chosenBucket]);
	}

	public static int chooseInputBucket(Board board, Side side)
	{
		return side.equals(Side.WHITE) ? INPUT_BUCKETS[board.getKingSquare(side).ordinal()]
				: INPUT_BUCKETS[board.getKingSquare(side).ordinal() ^ 0b111000];
	}

	public static int chooseInputBucket(Square square, Side side)
	{
		return side.equals(Side.WHITE) ? INPUT_BUCKETS[square.ordinal()] : INPUT_BUCKETS[square.ordinal() ^ 0b111000];
	}

	public static int getIndex(Square square, Piece piece, Side perspective)
	{
		return Side.WHITE.equals(perspective)
				? piece.getPieceSide().ordinal() * COLOR_STRIDE + piece.getPieceType().ordinal() * PIECE_STRIDE
						+ square.ordinal()
				: (piece.getPieceSide().ordinal() ^ 1) * COLOR_STRIDE + piece.getPieceType().ordinal() * PIECE_STRIDE
						+ (square.ordinal() ^ 0b111000);
	}

	public static int getIndex(final AccumulatorDiff.DiffInfo diff, Side perspective)
	{
		return getIndex(diff.square, diff.piece, perspective);
	}

	public static boolean requiresRefresh(final AccumulatorDiff diff, Side perspective)
	{
		assert diff.getAddedCount() <= 1 || !diff.getAdded(1).piece.getPieceType().equals(PieceType.KING);

		if (!diff.getAdded(0).piece.getPieceType().equals(PieceType.KING))
		{
			return false;
		}

		if (!diff.getAdded(0).piece.getPieceSide().equals(perspective))
		{
			return false;
		}

		assert diff.getRemoved(0).piece.getPieceType().equals(PieceType.KING);

		final Square prevKing = diff.getRemoved(0).square;
		final Square currKing = diff.getAdded(0).square;

		return chooseInputBucket(prevKing, perspective) != chooseInputBucket(currKing, perspective);
	}
}
