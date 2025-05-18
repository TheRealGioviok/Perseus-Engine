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

import org.shawn.games.Serendipity.Chess.Board;
import org.shawn.games.Serendipity.Chess.Piece;
import org.shawn.games.Serendipity.Chess.Square;
import org.shawn.games.Serendipity.Chess.move.Move;

public class ContinuationHistories
{
	private final PieceToHistory[][] continuationHistories;

	public ContinuationHistories()
	{
		continuationHistories = new PieceToHistory[Piece.values().length][Square.values().length];

		for (int i = 0; i < Piece.values().length; i++)
		{
			for (int j = 0; j < Square.values().length; j++)
			{
				continuationHistories[i][j] = new PieceToHistory();
			}
		}
	}

	public History get(Piece piece, Square to)
	{
		return continuationHistories[piece.ordinal()][to.ordinal()];
	}

	public History get(Board board, Move move)
	{
		return get(board.getPiece(move.getFrom()), move.getTo());
	}

	public void fill(int x)
	{
		for (PieceToHistory[] row : continuationHistories)
		{
			Arrays.stream(row).forEach(history -> history.fill(x));
		}
	}
}
