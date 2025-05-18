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

package org.shawn.games.Serendipity.Search.Listener;

import org.shawn.games.Serendipity.Chess.Board;
import org.shawn.games.Serendipity.Chess.move.Move;

public class SearchReport
{
	public final int depth;
	public final int selDepth;
	public final long nodes;
	public final int hashfull;
	public final int score;
	public final long ms;
	public final Board board;
	public final Move[] pv;

	public SearchReport(int depth, int selDepth, long nodes, int hashfull, int score, long ms, Board board, Move[] pv)
	{
		this.depth = depth;
		this.selDepth = selDepth;
		this.nodes = nodes;
		this.hashfull = hashfull;
		this.score = score;
		this.ms = ms;
		this.board = board;
		this.pv = pv;
	}
}
