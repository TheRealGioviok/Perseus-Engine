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

import org.shawn.games.Serendipity.UCI.IntegerOption;

public class TimeManager
{

	private long startTime;
	private long hardLimitTimeStamp;
	private long softLimitTimeStamp;

	private static final IntegerOption moveOverHead = new IntegerOption(100, 0, 30000, "Move_Overhead");

	public void set(Limits limits)
	{
		set(limits.getTime(), limits.getIncrement(), limits.getMovesToGo());
	}

	public void set(long timeLeft, long increment, int movesToGo)
	{
		if (timeLeft < 0)
		{
			timeLeft = 1000;
		}

		timeLeft -= Math.min(moveOverHead.get(), timeLeft) / 2;
		long hardLimit;
		long softLimit;

		if (movesToGo != 0 && movesToGo != -1)
		{
			hardLimit = timeLeft / movesToGo + increment * 3 / 4;
			softLimit = hardLimit / 2;

			this.startTime = System.nanoTime();
			this.hardLimitTimeStamp = startTime + 1000000L * hardLimit;
			this.softLimitTimeStamp = startTime + 1000000L * softLimit;

			return;
		}

		else if (movesToGo == -1)
		{
			hardLimit = softLimit = timeLeft;

			this.startTime = System.nanoTime();
			this.hardLimitTimeStamp = this.softLimitTimeStamp = startTime + 1000000L * timeLeft;

			return;
		}

		int baseTime = (int) (timeLeft * 0.054 + increment * 0.85);
		int maxTime = (int) (timeLeft * 0.76);

		hardLimit = Math.min(maxTime, (int) (baseTime * 3.04));
		softLimit = Math.min(maxTime, (int) (baseTime * 0.76));

		this.startTime = System.nanoTime();
		this.hardLimitTimeStamp = startTime + 1000000L * hardLimit;
		this.softLimitTimeStamp = startTime + 1000000L * softLimit;
	}

	public boolean shouldStop()
	{
		return System.nanoTime() > this.hardLimitTimeStamp;
	}

	public boolean shouldStopIterativeDeepening()
	{
		return System.nanoTime() > this.softLimitTimeStamp;
	}

	public long timePassed()
	{
		return (System.nanoTime() - this.startTime) / 1000000;
	}
}
