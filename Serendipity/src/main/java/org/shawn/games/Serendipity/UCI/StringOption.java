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

public class StringOption implements UCIOption
{
	String value;
	final String name;
	final String defaultValue;

	public StringOption(String value, String name)
	{
		this.value = this.defaultValue = value;
		this.name = name;

		UCI.addOption(name, this);
	}

	@Override
	public void set(String value)
	{
		this.value = value;
	}

	public String get()
	{
		return value;
	}

	public String toString()
	{
		return "option name " + name + " type string default " + defaultValue;
	}

	@Override
	public String getString()
	{
		return this.value;
	}
}