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

import java.util.ArrayList;
import java.util.Collections;

import org.shawn.games.Serendipity.Search.History.History;

import org.shawn.games.Serendipity.Chess.Board;
import org.shawn.games.Serendipity.Chess.Piece;
import org.shawn.games.Serendipity.Chess.move.Move;

public class MovePicker
{
	private Board board;
	private Move ttMove;

	private Move killer;
	private History history;
	private History captureHistory;
	private History[] continuationHistories;

	private ArrayList<Move> moves;

	private int stage;
	private int moveIndex;

	private static final int STAGE_NORMAL_TT = 0;
	private static final int STAGE_NORMAL = 1;
	private static final int STAGE_QSEARCH_TT = 2;
	private static final int STAGE_QSEARCH_CAPTURE = 3;

	private static final int[] promoValue = { -2000000001, 2000000000, -2000000001, -2000000001, 2000000001 };

	public MovePicker(Board board, Move ttMove)
	{
		this(board, ttMove, null, null, null, null);
	}

	public MovePicker(Board board, Move ttMove, Move killer, History history, History captureHistory,
			History[] continuationHistories)
	{
		this.board = board;
		this.ttMove = ttMove;

		if (ttMove == null || !board.isMovePseudoLegal(ttMove))
		{
			this.ttMove = null;
			this.stage = STAGE_NORMAL;
		}
		else
		{
			this.stage = STAGE_NORMAL_TT;
		}

		this.killer = killer;
		this.history = history;
		this.captureHistory = captureHistory;
		this.continuationHistories = continuationHistories;
	}

	public MovePicker(Board board, Move ttMove, History captureHistory)
	{
		this.board = board;
		this.ttMove = ttMove;

		if (ttMove == null || !board.isMovePseudoLegal(ttMove) || !board.isCapture(ttMove))
		{
			this.ttMove = null;
			this.stage = STAGE_QSEARCH_CAPTURE;
		}

		else
		{
			this.stage = STAGE_QSEARCH_TT;
		}

		this.killer = null;
		this.history = null;
		this.captureHistory = captureHistory;
		this.continuationHistories = null;
	}

	private static int pieceValue(Piece p)
	{
		if (p.getPieceType() == null)
		{
			return 0;
		}

		return p.getPieceType().ordinal() + 1;
	}

	private int captureValue(Move move)
	{
		return pieceValue(board.getPiece(move.getTo())) * 100 + captureHistory.get(board, move) / 256;
	}

	private int scoreMove(Move move)
	{
		if (!move.getPromotion().equals(Piece.NONE))
		{
			return promoValue[move.getPromotion().getPieceType().ordinal()];
		}

		if (!board.isQuiet(move))
		{
			int score = board.staticExchangeEvaluation(move, -20) ? 900000000 : -1000000;
			score += captureValue(move);
			return score;
		}

		if (move.equals(killer))
		{
			return 800000000;
		}

		int moveValue = history.get(board, move);

		moveValue += continuationHistories[0].get(board, move);
		moveValue += continuationHistories[1].get(board, move);
		moveValue += continuationHistories[3].get(board, move);
		moveValue += continuationHistories[5].get(board, move) / 2;

		return moveValue;
	}

	public void initMoves()
	{
		this.moves = new ArrayList<Move>();
		board.generatePseudoLegalMoves(moves);

		for (int i = 0; i < this.moves.size(); i++)
		{
			Move currentMove = this.moves.get(i);
			currentMove.setScore(scoreMove(currentMove));
		}
	}

	public void initCaptures()
	{
		this.moves = new ArrayList<Move>();
		board.generatePseudoLegalCaptures(moves);

		for (int i = 0; i < this.moves.size(); i++)
		{
			Move currentMove = this.moves.get(i);
			currentMove.setScore(captureValue(currentMove));
		}
	}

	public Move selectMove()
	{
		if (this.moveIndex >= this.moves.size())
		{
			return null;
		}

		int swapIndex = this.moveIndex;
		int maxScore = this.moves.get(this.moveIndex).getScore();

		for (int i = this.moveIndex + 1; i < this.moves.size(); i++)
		{
			int currScore = this.moves.get(i).getScore();

			if (maxScore < currScore)
			{
				swapIndex = i;
				maxScore = currScore;
			}
		}

		Collections.swap(moves, swapIndex, this.moveIndex);

		this.moveIndex++;

		return moves.get(this.moveIndex - 1);
	}

	public Move next()
	{
		switch (stage)
		{
			case STAGE_NORMAL_TT:
				stage++;
				return this.ttMove;

			case STAGE_NORMAL:
				if (this.moves == null)
				{
					initMoves();
				}

			{
				Move ret = selectMove();

				if (ret != null && ret.equals(ttMove))
				{
					ret = selectMove();
				}

				return ret;
			}

			case STAGE_QSEARCH_TT:
				stage++;
				return this.ttMove;

			case STAGE_QSEARCH_CAPTURE:
				if (this.moves == null)
				{
					initCaptures();
				}

			{
				Move ret = selectMove();

				if (ret != null && ret.equals(ttMove))
				{
					ret = selectMove();
				}

				return ret;
			}
		}

		return null;
	}
}
