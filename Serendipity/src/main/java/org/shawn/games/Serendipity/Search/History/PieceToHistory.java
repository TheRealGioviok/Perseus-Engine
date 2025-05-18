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

package org.shawn.games.Serendipity.Search.History;

import java.util.Arrays;

import org.shawn.games.Serendipity.Chess.*;
import org.shawn.games.Serendipity.Chess.move.Move;

public class PieceToHistory implements History
{
	private final int[][] history;

	static final int MAX_BONUS = 16384;

	public PieceToHistory()
	{
		history = new int[Piece.values().length][Square.values().length];
	}

	public int get(Piece piece, Square to)
	{
		return history[piece.ordinal()][to.ordinal()];
	}

	public int get(Piece piece, Move move)
	{
		return get(piece, move.getTo());
	}

	private static int clamp(int v)
	{
		return v >= PieceToHistory.MAX_BONUS ? PieceToHistory.MAX_BONUS : (Math.max(v, -16384));
	}

	public void register(Piece piece, Square to, int value)
	{
		int clampedValue = clamp(value);

		history[piece.ordinal()][to.ordinal()] += clampedValue
				- history[piece.ordinal()][to.ordinal()] * Math.abs(clampedValue) / MAX_BONUS;
	}

	@Override
	public int get(Board board, Move move)
	{
		return get(board.getPiece(move.getFrom()), move.getTo());
	}

	@Override
	public void register(Board board, Move move, int value)
	{
		register(board.getPiece(move.getFrom()), move.getTo(), value);
	}

	@Override
	public void fill(int x)
	{
		for (int[] row : history)
		{
			Arrays.fill(row, x);
		}
	}
}
