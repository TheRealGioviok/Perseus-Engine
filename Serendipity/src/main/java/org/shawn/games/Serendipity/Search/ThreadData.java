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

import java.util.List;
import java.util.concurrent.atomic.AtomicLong;

import org.shawn.games.Serendipity.Search.History.CaptureHistory;
import org.shawn.games.Serendipity.Search.History.ContinuationHistories;
import org.shawn.games.Serendipity.Search.History.FromToHistory;
import org.shawn.games.Serendipity.Search.History.History;
import org.shawn.games.Serendipity.Search.Listener.ISearchListener;

import org.shawn.games.Serendipity.Chess.move.Move;

public class ThreadData
{
	final History history;
	final History captureHistory;
	final ContinuationHistories continuationHistories;
	final MainThreadData mainThreadData;
	Move[][] pv;

	int rootDepth;
	int selDepth;
	final int id;
	AtomicLong nodes;

	public static class MainThreadData
	{
		final List<ISearchListener> listeners;
		final List<AlphaBeta> threads;
		Limits limits;

		public MainThreadData(Limits limits, List<ISearchListener> listeners, List<AlphaBeta> threads)
		{
			this.limits = limits;
			this.listeners = listeners;
			this.threads = threads;
		}
	}

	public ThreadData(int id, MainThreadData mainThreadData)
	{
		this.id = id;

		this.history = new FromToHistory();
		this.captureHistory = new CaptureHistory();
		this.continuationHistories = new ContinuationHistories();

		this.rootDepth = 0;
		this.selDepth = 0;
		this.pv = new Move[AlphaBeta.MAX_PLY + 1][AlphaBeta.MAX_PLY + 1];
		this.mainThreadData = mainThreadData;
		this.nodes = new AtomicLong(0);
	}

	public ThreadData(int id)
	{
		this(id, null);
	}
}
