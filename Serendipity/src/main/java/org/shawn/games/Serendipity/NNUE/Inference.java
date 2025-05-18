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

package org.shawn.games.Serendipity.NNUE;

import org.shawn.games.Serendipity.Chess.Side;

public interface Inference
{
	int forward(AccumulatorStack.AccumulatorPair accumulators, Side side, final short[] weights, final short bias);

	void add(short[] to, final short[] from, final short[] added);

	void sub(short[] to, final short[] from, final short[] removed);

	void addSub(short[] to, final short[] from, final short[] added, final short[] subtracted);

	void addSubSub(short[] to, final short[] from, final short[] added, final short[] subtracted1,
			final short[] subtracted2);

	void addAddSubSub(short[] to, final short[] from, final short[] added1, final short[] added2,
			final short[] subtracted1, final short[] subtracted2);
}
