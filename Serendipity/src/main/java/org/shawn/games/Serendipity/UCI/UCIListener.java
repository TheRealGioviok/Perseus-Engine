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

package org.shawn.games.Serendipity.UCI;

import org.shawn.games.Serendipity.Search.AlphaBeta;
import org.shawn.games.Serendipity.Search.Listener.FinalReport;
import org.shawn.games.Serendipity.Search.Listener.ISearchListener;
import org.shawn.games.Serendipity.Search.Listener.SearchReport;

import org.shawn.games.Serendipity.Chess.move.Move;

public class UCIListener implements ISearchListener
{
	@Override
	public void notify(SearchReport report)
	{
		StringBuffer pvString = new StringBuffer();

		for (Move move : report.pv)
		{
			if (move == null)
			{
				break;
			}

			pvString.append(" ").append(move);
		}

		if (Math.abs(report.score) < AlphaBeta.MATE_EVAL - AlphaBeta.MAX_PLY)
		{
			int cp = WDLModel.normalizeEval(report.score, report.board);
			int[] wdl = WDLModel.calculateWDL(report.score, report.board);

			System.out.printf(
					"info depth %d seldepth %d nodes %d nps %d hashfull %d score cp %d wdl %d %d %d time %d pv%s\n",
					report.depth, report.selDepth, report.nodes, report.nodes * 1000L / Math.max(1, report.ms),
					report.hashfull, cp, wdl[0], wdl[1], wdl[2], report.ms, pvString);
		}

		else
		{
			int mateInPly = AlphaBeta.MATE_EVAL - Math.abs(report.score);
			int mateValue = mateInPly % 2 != 0 ? (mateInPly + 1) / 2 : -mateInPly / 2;
			int[] wdl;

			if (mateValue < 0)
			{
				wdl = new int[] { 0, 0, 1000 };
			}
			else
			{
				wdl = new int[] { 1000, 0, 0 };
			}

			System.out.printf(
					"info depth %d seldepth %d nodes %d nps %d hashfull %d score mate %d wdl %d %d %d time %d pv %s\n",
					report.depth, report.selDepth, report.nodes, report.nodes * 1000L / Math.max(1, report.ms),
					report.hashfull, mateValue, wdl[0], wdl[1], wdl[2], report.ms, pvString);
		}
	}

	@Override
	public void notify(FinalReport report)
	{
		System.out.println("bestmove " + (report.bestMove == null ? "(none)" : report.bestMove));
	}
}
