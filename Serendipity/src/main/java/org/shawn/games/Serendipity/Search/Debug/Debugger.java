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

import java.util.*;

public class Debugger
{
	private static Map<String, AveragedEntry> averages = new HashMap<>();
	private static Map<String, FrequencyEntry> counters = new HashMap<>();

	public static void dbg_mean_of(String name, long value)
	{
		if (!averages.containsKey(name))
		{
			averages.put(name, new AveragedEntry());
		}

		averages.get(name).add(value);
	}

	public static void dbg_hit_on(String name, boolean hit)
	{
		if (!counters.containsKey(name))
		{
			counters.put(name, new FrequencyEntry());
		}

		counters.get(name).add(hit);
	}

	public static void print()
	{
		for (Map.Entry<String, AveragedEntry> entry : averages.entrySet())
		{
			System.out.printf("Average of <%s>\t | %s\n", entry.getKey(), entry.getValue());
		}

		for (Map.Entry<String, FrequencyEntry> entry : counters.entrySet())
		{
			System.out.printf("Hits of <%s>\t | %s\n", entry.getKey(), entry.getValue());
		}
	}
}
