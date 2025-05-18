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

public class CaptureHistory implements History
{
	private final int[][][] history;

	static final int MAX_BONUS = 16384;

	public CaptureHistory()
	{
		history = new int[Piece.values().length][Square.values().length][PieceType.values().length];
	}

	public int get(Piece piece, Square to, PieceType captured)
	{
		return history[piece.ordinal()][to.ordinal()][captured.ordinal()];
	}

	private static int clamp(int v)
	{
		return v >= CaptureHistory.MAX_BONUS ? CaptureHistory.MAX_BONUS : (Math.max(v, -16384));
	}

	public void register(Piece piece, Square to, PieceType captured, int value)
	{
		int clampedValue = clamp(value);

		if (captured == null)
		{
			captured = PieceType.NONE;
		}

		history[piece.ordinal()][to.ordinal()][captured.ordinal()] += clampedValue
				- history[piece.ordinal()][to.ordinal()][captured.ordinal()] * Math.abs(clampedValue) / MAX_BONUS;
	}

	@Override
	public int get(Board board, Move move)
	{
		Piece movedPiece = board.getPiece(move.getFrom());

		if (move.getTo().equals(board.getEnPassant()) && movedPiece.getPieceType().equals(PieceType.PAWN))
		{
			return get(movedPiece, board.getEnPassant(), PieceType.PAWN);
		}
		
		if (Piece.NONE.equals(board.getPiece(move.getTo())))
		{
			return get(movedPiece, move.getTo(), PieceType.NONE);
		}

		return get(movedPiece, move.getTo(), board.getPiece(move.getTo()).getPieceType());
	}

	@Override
	public void register(Board board, Move move, int value)
	{
		Piece movedPiece = board.getPiece(move.getFrom());

		if (move.getTo().equals(board.getEnPassant()) && movedPiece.getPieceType().equals(PieceType.PAWN))
		{
			register(movedPiece, board.getEnPassant(), PieceType.PAWN, value);
			return;
		}

		register(movedPiece, move.getTo(), board.getPiece(move.getTo()).getPieceType(), value);
	}

	@Override
	public void fill(int x)
	{
		for (int[][] x1 : history)
		{
			for (int[] x2 : x1)
			{
				Arrays.fill(x2, x);
			}
		}
	}
}