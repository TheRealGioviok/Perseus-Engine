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

import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.atomic.AtomicBoolean;

import org.shawn.games.Serendipity.NNUE.NNUE;

public class SharedThreadData
{
	final NNUE network;
	final AtomicBoolean stopped;
	final TranspositionTable tt;
	final CyclicBarrier endBarrier;
	final CyclicBarrier startBarrier;

	public SharedThreadData(TranspositionTable tt, CyclicBarrier startBarrier, CyclicBarrier endBarrier, NNUE network,
			AtomicBoolean stopped)
	{
		this.tt = tt;
		this.network = network;
		this.stopped = stopped;
		this.endBarrier = endBarrier;
		this.startBarrier = startBarrier;
	}
}
