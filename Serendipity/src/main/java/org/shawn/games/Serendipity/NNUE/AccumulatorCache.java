package org.shawn.games.Serendipity.NNUE;

import java.util.Arrays;

import org.shawn.games.Serendipity.Chess.Board;
import org.shawn.games.Serendipity.Chess.PieceType;
import org.shawn.games.Serendipity.Chess.Side;

public class AccumulatorCache
{
	public class Entry
	{
		public final short[] storedAccumulator;
		private final long[] bySide;
		private final long[] byPieceType;

		public Entry()
		{
			storedAccumulator = new short[NNUE.HIDDEN_SIZE];
			bySide = new long[Side.values().length];
			byPieceType = new long[PieceType.validValues().length];
		}

		public long getBitboard(Side side, PieceType pieceType)
		{
			return bySide[side.ordinal()] & byPieceType[pieceType.ordinal()];
		}

		public void update(Board board)
		{
			for (Side side : Side.values())
			{
				bySide[side.ordinal()] = board.getBitboard(side);
			}

			for (PieceType pieceType : PieceType.validValues())
			{
				byPieceType[pieceType.ordinal()] = board.getBitboard(pieceType);
			}
		}
	}

	final private Entry[][] entries;

	public AccumulatorCache(NNUE network)
	{
		entries = new Entry[Side.values().length][NNUE.INPUT_BUCKET_SIZE];

		for (int i = 0; i < entries.length; i++)
		{
			for (int j = 0; j < entries[0].length; j++)
			{
				entries[i][j] = new Entry();
			}
		}

		clear(network);
	}

	public void clear(NNUE network)
	{
		for (Entry[] row : entries)
		{
			for (Entry entry : row)
			{
				assert entry.storedAccumulator.length == NNUE.HIDDEN_SIZE;

				System.arraycopy(network.L1Biases, 0, entry.storedAccumulator, 0, NNUE.HIDDEN_SIZE);
				Arrays.fill(entry.bySide, 0L);
				Arrays.fill(entry.byPieceType, 0L);
			}
		}
	}

	public Entry get(Side side, int kingBucket)
	{
		return entries[side.ordinal()][kingBucket];
	}
}
