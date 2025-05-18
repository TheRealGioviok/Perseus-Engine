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

package org.shawn.games.Serendipity.Search;

import org.shawn.games.Serendipity.Search.History.History;
import org.shawn.games.Serendipity.Search.History.PieceToHistory;

import org.shawn.games.Serendipity.Chess.move.Move;

public class SearchStack
{
	public static class SearchState
	{
		public boolean inCheck;
		public boolean ttHit;
		public int moveCount;
		public int staticEval;
		public Move killer;
		public Move move;
		public Move excludedMove;
		public History continuationHistory;

		public SearchState()
		{
			this.staticEval = AlphaBeta.VALUE_NONE;
		}
	}

	private final SearchState[] stack;

	public SearchStack(int n)
	{
		stack = new SearchState[n + 9];

		for (int i = 0; i < stack.length; i++)
		{
			stack[i] = new SearchState();
		}

		for (int i = 0; i < 6; i++)
		{
			stack[i].continuationHistory = new PieceToHistory();
		}
	}

	public SearchState get(int index)
	{
		return stack[index + 6];
	}
}
