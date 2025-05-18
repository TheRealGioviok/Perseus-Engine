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

package org.shawn.games.Serendipity;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;

import org.junit.Test;
import org.shawn.games.Serendipity.Search.TranspositionTable;

import org.shawn.games.Serendipity.Chess.*;
import org.shawn.games.Serendipity.Chess.move.Move;

public class TranspositionTableTest
{
	@Test
	public void testTT()
	{
		Board board = new Board();
		TranspositionTable tt = new TranspositionTable(4);

		assertFalse(tt.probe(0).hit());

		tt.write(null, board.getIncrementalHashKey(), TranspositionTable.NODETYPE_EXACT, 12, 2, null, 0, true);
		assertEquals(12, tt.probe(board.getIncrementalHashKey()).getDepth());
		assertEquals(2, tt.probe(board.getIncrementalHashKey()).getEvaluation());
		assertTrue(tt.probe(board.getIncrementalHashKey()).verifySignature(board.getIncrementalHashKey()));
		assertTrue(tt.probe(board.getIncrementalHashKey()).wasPV());

		tt.write(null, board.getIncrementalHashKey(), TranspositionTable.NODETYPE_NONE, TranspositionTable.DEPTH_QS,
				-6900, new Move(Square.E2, Square.E4), -200, false);
		assertEquals(TranspositionTable.NODETYPE_NONE, tt.probe(board.getIncrementalHashKey()).getNodeType());
		assertEquals(TranspositionTable.DEPTH_QS, tt.probe(board.getIncrementalHashKey()).getDepth());
		assertEquals(-6900, tt.probe(board.getIncrementalHashKey()).getEvaluation());
		assertEquals(-200, tt.probe(board.getIncrementalHashKey()).getStaticEval());
		assertTrue(tt.probe(board.getIncrementalHashKey()).getMove().equals(new Move(Square.E2, Square.E4)));
		assertTrue(tt.probe(board.getIncrementalHashKey()).verifySignature(board.getIncrementalHashKey()));
		assertFalse(tt.probe(board.getIncrementalHashKey()).wasPV());
	}
}
