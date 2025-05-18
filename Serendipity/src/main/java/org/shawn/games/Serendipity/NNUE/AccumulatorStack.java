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

import org.shawn.games.Serendipity.Search.AlphaBeta;

import org.shawn.games.Serendipity.Chess.AccumulatorDiff;
import org.shawn.games.Serendipity.Chess.Bitboard;
import org.shawn.games.Serendipity.Chess.Board;
import org.shawn.games.Serendipity.Chess.Piece;
import org.shawn.games.Serendipity.Chess.PieceType;
import org.shawn.games.Serendipity.Chess.Side;
import org.shawn.games.Serendipity.Chess.Square;
import org.shawn.games.Serendipity.Chess.move.Move;

public class AccumulatorStack
{
	public class Accumulator
	{
		short[] values;
		Side color;
		AccumulatorDiff diff;
		int kingBucket;
		boolean needsRefresh;

		public Accumulator()
		{
			this.values = new short[NNUE.HIDDEN_SIZE];
		}

		public Accumulator(NNUE network, Side color)
		{
			this.values = network.L1Biases.clone();
			this.color = color;
			this.needsRefresh = false;
		}

		private void addSub(Accumulator prev, int featureIndexToAdd, int featureIndexToSubtract)
		{
			featureIndexToAdd = featureIndexToAdd + kingBucket * NNUE.FEATURE_SIZE;
			featureIndexToSubtract = featureIndexToSubtract + kingBucket * NNUE.FEATURE_SIZE;

			INFERENCE.addSub(values, prev.values, network.L1Weights[featureIndexToAdd],
					network.L1Weights[featureIndexToSubtract]);
		}

		private void addSubSub(Accumulator prev, int featureIndexToAdd, int featureIndexToSubtract1,
				int featureIndexToSubtract2)
		{
			featureIndexToAdd = featureIndexToAdd + kingBucket * NNUE.FEATURE_SIZE;
			featureIndexToSubtract1 = featureIndexToSubtract1 + kingBucket * NNUE.FEATURE_SIZE;
			featureIndexToSubtract2 = featureIndexToSubtract2 + kingBucket * NNUE.FEATURE_SIZE;

			INFERENCE.addSubSub(values, prev.values, network.L1Weights[featureIndexToAdd],
					network.L1Weights[featureIndexToSubtract1], network.L1Weights[featureIndexToSubtract2]);
		}

		private void addAddSubSub(Accumulator prev, int featureIndexToAdd1, int featureIndexToAdd2,
				int featureIndexToSubtract1, int featureIndexToSubtract2)
		{
			featureIndexToAdd1 = featureIndexToAdd1 + kingBucket * NNUE.FEATURE_SIZE;
			featureIndexToAdd2 = featureIndexToAdd2 + kingBucket * NNUE.FEATURE_SIZE;
			featureIndexToSubtract1 = featureIndexToSubtract1 + kingBucket * NNUE.FEATURE_SIZE;
			featureIndexToSubtract2 = featureIndexToSubtract2 + kingBucket * NNUE.FEATURE_SIZE;

			INFERENCE.addAddSubSub(values, prev.values, network.L1Weights[featureIndexToAdd1],
					network.L1Weights[featureIndexToAdd2], network.L1Weights[featureIndexToSubtract1],
					network.L1Weights[featureIndexToSubtract2]);
		}

		private void efficientlyUpdate(Accumulator prev)
		{
			final int addedCount = this.diff.getAddedCount();
			final int removedCount = this.diff.getRemovedCount();

			if (addedCount == 1 && removedCount == 1)
			{
				final int addedIndex = NNUE.getIndex(this.diff.getAdded(0), this.color);
				final int removedIndex = NNUE.getIndex(this.diff.getRemoved(0), this.color);

				this.addSub(prev, addedIndex, removedIndex);
			}

			else if (addedCount == 1 && removedCount == 2)
			{
				final int addedIndex = NNUE.getIndex(this.diff.getAdded(0), this.color);
				final int removedIndex0 = NNUE.getIndex(this.diff.getRemoved(0), this.color);
				final int removedIndex1 = NNUE.getIndex(this.diff.getRemoved(1), this.color);

				this.addSubSub(prev, addedIndex, removedIndex0, removedIndex1);
			}

			else
			{
				assert addedCount == 2 && removedCount == 2;

				final int addedIndex0 = NNUE.getIndex(this.diff.getAdded(0), this.color);
				final int addedIndex1 = NNUE.getIndex(this.diff.getAdded(1), this.color);
				final int removedIndex0 = NNUE.getIndex(this.diff.getRemoved(0), this.color);
				final int removedIndex1 = NNUE.getIndex(this.diff.getRemoved(1), this.color);

				this.addAddSubSub(prev, addedIndex0, addedIndex1, removedIndex0, removedIndex1);
			}

			this.needsRefresh = false;
		}

		private void updateFromCache(Board board)
		{
			AccumulatorCache.Entry entry = cache.get(this.color, NNUE.chooseInputBucket(board, this.color));

			for (final Side side : Side.values())
			{
				for (final PieceType pieceType : PieceType.validValues())
				{
					final Piece piece = Piece.make(side, pieceType);
					final long oldBB = entry.getBitboard(side, pieceType);
					final long newBB = board.getBitboard(side, pieceType);

					long removed = oldBB & ~newBB;
					long added = newBB & ~oldBB;

					while (removed != 0L)
					{
						final Square sq = Square.squareAt(Bitboard.bitScanForward(removed));
						final int featureIndex = NNUE.getIndex(sq, piece, this.color) + kingBucket * NNUE.FEATURE_SIZE;

						INFERENCE.sub(entry.storedAccumulator, entry.storedAccumulator,
								network.L1Weights[featureIndex]);

						removed = Bitboard.extractLsb(removed);
					}

					while (added != 0L)
					{
						final Square sq = Square.squareAt(Bitboard.bitScanForward(added));
						final int featureIndex = NNUE.getIndex(sq, piece, this.color) + kingBucket * NNUE.FEATURE_SIZE;

						INFERENCE.add(entry.storedAccumulator, entry.storedAccumulator,
								network.L1Weights[featureIndex]);

						added = Bitboard.extractLsb(added);
					}
				}
			}

			System.arraycopy(entry.storedAccumulator, 0, values, 0, NNUE.HIDDEN_SIZE);

			entry.update(board);

			this.needsRefresh = false;
		}

		private void makeMove(Accumulator prev, Board board, final AccumulatorDiff diff)
		{
			this.diff = diff;
			this.color = prev.color;
			this.needsRefresh = true;
			this.kingBucket = NNUE.chooseInputBucket(board, this.color);
		}

		private void loadFromBoard(Board board)
		{
			this.kingBucket = NNUE.chooseInputBucket(board, this.color);
			this.needsRefresh = true;
			updateFromCache(board);
		}
	}

	public class AccumulatorPair
	{
		Accumulator[] accumulators;

		public AccumulatorPair()
		{
			accumulators = new Accumulator[] { new Accumulator(), new Accumulator() };
		}

		public void init()
		{
			this.accumulators = new Accumulator[] { new Accumulator(network, Side.WHITE),
					new Accumulator(network, Side.BLACK) };
		}

		public void loadFromBoard(Board board)
		{
			this.accumulators[0].loadFromBoard(board);
			this.accumulators[1].loadFromBoard(board);
		}

		public void makeMove(AccumulatorPair prev, Board board, Move move, final AccumulatorDiff diff)
		{
			this.accumulators[0].makeMove(prev.accumulators[0], board, diff);
			this.accumulators[1].makeMove(prev.accumulators[1], board, diff);
		}

		public Accumulator get(Side side)
		{
			return this.accumulators[side.ordinal()];
		}
	}

	private static final Inference INFERENCE = InferenceChooser.chooseInference();
	private final NNUE network;
	private final AccumulatorPair[] stack;
	private final AccumulatorCache cache;
	private int top;

	public AccumulatorStack(NNUE network)
	{
		this.network = network;
		this.stack = new AccumulatorPair[AlphaBeta.MAX_PLY + 1];
		this.cache = new AccumulatorCache(network);

		for (int i = 0; i < this.stack.length; i++)
		{
			this.stack[i] = new AccumulatorPair();
		}
	}

	public void pop()
	{
		top--;
	}

	public void push(Board board, Move move, final AccumulatorDiff diff)
	{
		top++;
		this.stack[top].makeMove(this.stack[top - 1], board, move, diff);
	}

	public void init(Board board)
	{
		this.top = 0;
		this.stack[0].init();
		this.stack[0].loadFromBoard(board);
	}

	public void printAccumulator(Side side)
	{
		for (int i = 0; i < NNUE.HIDDEN_SIZE; i++)
		{
			System.out.print(this.stack[top].accumulators[side.ordinal()].values[i] + ", ");
		}
	}

	private void efficientlyUpdate(int fromIdx, int toIdx, Side side)
	{
		for (int i = fromIdx; i < toIdx; i++)
		{
			Accumulator from = this.stack[i].get(side);
			Accumulator to = this.stack[i + 1].get(side);

			assert !from.needsRefresh && to.needsRefresh;

			to.efficientlyUpdate(from);
		}
	}

	private void refreshAccumulator(Board board, Side side)
	{
		Accumulator startingAccumulator = this.stack[top].get(side);

		if (!startingAccumulator.needsRefresh)
			return;

		int startingKingBucket = startingAccumulator.kingBucket;

		for (int currIdx = top - 1; currIdx >= 0; currIdx--)
		{
			Accumulator currAccumulator = this.stack[currIdx].get(side);

			if (currAccumulator.kingBucket != startingKingBucket)
				break;

			if (!currAccumulator.needsRefresh)
			{
				efficientlyUpdate(currIdx, top, side);
				return;
			}
		}

		startingAccumulator.updateFromCache(board);
	}

	public AccumulatorPair refreshAndGet(Board board)
	{
		refreshAccumulator(board, Side.WHITE);
		refreshAccumulator(board, Side.BLACK);

		return this.stack[top];
	}
}
