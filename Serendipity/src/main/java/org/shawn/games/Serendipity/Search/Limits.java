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

public class Limits
{
	private long time;
	private long increment;
	private int movesToGo;

	private long nodes;
	private int depth;

	public Limits()
	{
		this(100, 0, 0, -1, 256);
	}

	public Limits(long time, long increment, int movesToGo, long nodes, int depth)
	{
		this.time = time;
		this.increment = increment;
		this.movesToGo = movesToGo;
		this.nodes = nodes;
		this.depth = depth;
	}

	public long getTime()
	{
		return time;
	}

	public void setTime(long time)
	{
		this.time = time;
	}

	public long getIncrement()
	{
		return increment;
	}

	public void setIncrement(long increment)
	{
		this.increment = increment;
	}

	public int getMovesToGo()
	{
		return movesToGo;
	}

	public void setMovesToGo(int movesToGo)
	{
		this.movesToGo = movesToGo;
	}

	public long getNodes()
	{
		return nodes;
	}

	public void setNodes(long nodes)
	{
		this.nodes = nodes;
	}

	public int getDepth()
	{
		return depth;
	}

	public void setDepth(int depth)
	{
		this.depth = depth;
	}

	public Limits clone()
	{
		return new Limits(time, increment, movesToGo, nodes, depth);
	}
}
