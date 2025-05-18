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

public class IntegerOption implements UCIOption
{
	int value;
	final String name;
	final int defaultValue;
	final int lowerBound;
	final int upperBound;

	public IntegerOption(int value, int lowerBound, int upperBound, String name)
	{
		if (lowerBound > upperBound)
		{
			throw new IllegalArgumentException("lowerBound must be less than or equal to upperBound");
		}

		this.value = this.defaultValue = value;
		this.lowerBound = lowerBound;
		this.upperBound = upperBound;
		this.name = name;

		UCI.addOption(name, this);
	}

	@Override
	public void set(String value)
	{
		int intValue = Integer.parseInt(value);
		if (intValue < this.lowerBound)
		{
			this.value = this.lowerBound;
		}

		else if (intValue > this.upperBound)
		{
			this.value = this.upperBound;
		}

		else
		{
			this.value = intValue;
		}
	}

	public int get()
	{
		return value;
	}

	public String toString()
	{
		return "option name " + name + " type spin default " + defaultValue + " min " + lowerBound + " max "
				+ upperBound;
	}

	@Override
	public String getString()
	{
		return Integer.toString(this.value);
	}
}