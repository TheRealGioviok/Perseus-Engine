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

public class FromToHistory implements History
{
	private final int[] history;

	static final int MAX_BONUS = 16384;

	final int DIMENSION = Square.values().length;

	public FromToHistory()
	{
		history = new int[DIMENSION * DIMENSION];
	}

	public int get(Square from, Square to)
	{
		return history[from.ordinal() * DIMENSION + to.ordinal()];
	}

	public int get(Move move)
	{
		return get(move.getFrom(), move.getTo());
	}

	private static int clamp(int v)
	{
		return v >= FromToHistory.MAX_BONUS ? FromToHistory.MAX_BONUS : (Math.max(v, -16384));
	}

	public void register(Square from, Square to, int value)
	{
		int clampedValue = clamp(value);

		history[from.ordinal() * DIMENSION + to.ordinal()] += clampedValue
				- history[from.ordinal() * DIMENSION + to.ordinal()] * Math.abs(clampedValue) / MAX_BONUS;
	}

	@Override
	public int get(Board board, Move move)
	{
		return get(move.getFrom(), move.getTo());
	}

	@Override
	public void register(Board board, Move move, int value)
	{
		register(move.getFrom(), move.getTo(), value);
	}

	@Override
	public void fill(int x)
	{
		Arrays.fill(history, x);
	}
}
