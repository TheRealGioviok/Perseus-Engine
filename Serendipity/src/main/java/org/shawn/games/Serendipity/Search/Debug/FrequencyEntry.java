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

package org.shawn.games.Serendipity.Search.Debug;

public class FrequencyEntry implements DebugEntry
{
	long count;
	long hit;

	public FrequencyEntry()
	{
		count = 0;
		hit = 0;
	}

	public void add(boolean hit)
	{
		count++;

		if (hit)
		{
			this.hit++;
		}
	}

	public String toString()
	{
		return "Hits: " + count + " Frequency: " + String.format("%.02f", 100d * hit / count) + "%";
	}
}
