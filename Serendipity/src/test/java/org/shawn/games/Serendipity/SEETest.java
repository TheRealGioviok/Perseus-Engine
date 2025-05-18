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

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

import org.shawn.games.Serendipity.Chess.Attacks;
import org.shawn.games.Serendipity.Chess.Bitboard;
import org.shawn.games.Serendipity.Chess.Board;
import org.shawn.games.Serendipity.Chess.Piece;
import org.shawn.games.Serendipity.Chess.PieceType;
import org.shawn.games.Serendipity.Chess.Side;
import org.shawn.games.Serendipity.Chess.Square;
import org.shawn.games.Serendipity.Chess.move.Move;

public class SEETest
{
	private String rawTestString = "6k1/1pp4p/p1pb4/6q1/3P1pRr/2P4P/PP1Br1P1/5RKN w - - | f1f4 | -100 | P - R + B\n"
			+ "5rk1/1pp2q1p/p1pb4/8/3P1NP1/2P5/1P1BQ1P1/5RK1 b - - | d6f4 | 0 | -N + B\n"
			+ "4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - - | h5g4 | 0\n"
			+ "4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - - | h5g4 | 0\n"
			+ "4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - - | g4f3 | 0\n"
			+ "2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - - | d6e5 | 100 | P\n"
			+ "7r/5qpk/p1Qp1b1p/3r3n/BB3p2/5p2/P1P2P2/4RK1R w - - | e1e8 | 0\n"
			+ "6rr/6pk/p1Qp1b1p/2n5/1B3p2/5p2/P1P2P2/4RK1R w - - | e1e8 | -500 | -R\n"
			+ "7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - - | e1e8 | -500 | -R\n"
			+ "6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - - | f7f8q | 200 | B - P\n"
			+ "6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - - | f7f8n | 200 | N - P\n"
			+ "7R/5P2/8/8/6r1/3K4/5p2/4k3 w - - | f7f8q | 800 | Q - P\n"
			+ "7R/5P2/8/8/6r1/3K4/5p2/4k3 w - - | f7f8b | 200 | B - P\n"
			+ "7R/4bP2/8/8/1q6/3K4/5p2/4k3 w - - | f7f8r | -100 | -P\n"
			+ "8/4kp2/2npp3/1Nn5/1p2PQP1/7q/1PP1B3/4KR1r b - - | h1f1 | 0\n"
			+ "8/4kp2/2npp3/1Nn5/1p2P1P1/7q/1PP1B3/4KR1r b - - | h1f1 | 0\n"
			+ "2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - - | c5c1 | 100 | R - Q + R\n"
			+ "r2qk1nr/pp2ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq - | f3e5 | 100 | P\n"
			+ "6r1/4kq2/b2p1p2/p1pPb3/p1P2B1Q/2P4P/2B1R1P1/6K1 w - - | f4e5 | 0\n"
			+ "3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6 | g5h6 | 0\n"
			+ "3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R1B2B/PQ3P1P/3R2K1 w - h6 | g5h6 | 100 | P\n"
			+ "2r4r/1P4pk/p2p1b1p/7n/BB3p2/2R2p2/P1P2P2/4RK2 w - - | c3c8 | 500 | R\n"
//			+ "2r5/1P4pk/p2p1b1p/5b1n/BB3p2/2R2p2/P1P2P2/4RK2 w - - | c3c8 | 500 | R\n" // CANT PASS
			+ "2r4k/2r4p/p7/2b2p1b/4pP2/1BR5/P1R3PP/2Q4K w - - | c3c5 | 300 | B\n"
			+ "8/pp6/2pkp3/4bp2/2R3b1/2P5/PP4B1/1K6 w - - | g2c6 | -200 | P - B\n"
			+ "4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - | e6e4 | -400 | P - R\n"
			+ "4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - | h7e4 | 100 | P\n"
			+ "3r3k/3r4/2n1n3/8/3p4/2PR4/1B1Q4/3R3K w - - | d3d4 | -100 | P - R + N - P + N - B + R - Q + R\n"
			+ "1k1r4/1ppn3p/p4b2/4n3/8/P2N2P1/1PP1R1BP/2K1Q3 w - - | d3e5 | 100 | N - N + B - R + N\n"
			+ "1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - - | d3e5 | -200 | P - N\n"
			+ "rnb2b1r/ppp2kpp/5n2/4P3/q2P3B/5R2/PPP2PPP/RN1QKB2 w Q - | h4f6 | 100 | N - B + P\n"
			+ "r2q1rk1/2p1bppp/p2p1n2/1p2P3/4P1b1/1nP1BN2/PP3PPP/RN1QR1K1 b - - | g4f3 | 0 | N - B\n"
			+ "r1bqkb1r/2pp1ppp/p1n5/1p2p3/3Pn3/1B3N2/PPP2PPP/RNBQ1RK1 b kq - | c6d4 | 0 | P - N + N - P\n"
			+ "r1bq1r2/pp1ppkbp/4N1p1/n3P1B1/8/2N5/PPP2PPP/R2QK2R w KQ - | e6g7 | 0 | B - N\n"
			+ "r1bq1r2/pp1ppkbp/4N1pB/n3P3/8/2N5/PPP2PPP/R2QK2R w KQ - | e6g7 | 300 | B\n"
			+ "rnq1k2r/1b3ppp/p2bpn2/1p1p4/3N4/1BN1P3/PPP2PPP/R1BQR1K1 b kq - | d6h2 | -200 | P - B\n"
			+ "rn2k2r/1bq2ppp/p2bpn2/1p1p4/3N4/1BN1P3/PPP2PPP/R1BQR1K1 b kq - | d6h2 | 100 | P\n"
			+ "r2qkbn1/ppp1pp1p/3p1rp1/3Pn3/4P1b1/2N2N2/PPP2PPP/R1BQKB1R b KQq - | g4f3 | 100 | N - B + P\n"
			+ "rnbq1rk1/pppp1ppp/4pn2/8/1bPP4/P1N5/1PQ1PPPP/R1B1KBNR b KQ - | b4c3 | 0 | N - B\n"
			+ "r4rk1/3nppbp/bq1p1np1/2pP4/8/2N2NPP/PP2PPB1/R1BQR1K1 b - - | b6b2 | -800 | P - Q\n"
			+ "r4rk1/1q1nppbp/b2p1np1/2pP4/8/2N2NPP/PP2PPB1/R1BQR1K1 b - - | f6d5 | -200 | P - N\n"
			+ "1r3r2/5p2/4p2p/2k1n1P1/2PN1nP1/1P3P2/8/2KR1B1R b - - | b8b3 | -400 | P - R\n"
			+ "1r3r2/5p2/4p2p/4n1P1/kPPN1nP1/5P2/8/2KR1B1R b - - | b8b4 | 100 | P\n"
			+ "2r2rk1/5pp1/pp5p/q2p4/P3n3/1Q3NP1/1P2PP1P/2RR2K1 b - - | c8c1 | 0 | R - R\n"
			+ "5rk1/5pp1/2r4p/5b2/2R5/6Q1/R1P1qPP1/5NK1 b - - | f5c2 | -100 | P - B + R - Q + R\n"
			+ "1r3r1k/p4pp1/2p1p2p/qpQP3P/2P5/3R4/PP3PP1/1K1R4 b - - | a5a2 | -800 | P - Q\n"
			+ "1r5k/p4pp1/2p1p2p/qpQP3P/2P2P2/1P1R4/P4rP1/1K1R4 b - - | a5a2 | 100 | P\n"
			+ "r2q1rk1/1b2bppp/p2p1n2/1ppNp3/3nP3/P2P1N1P/BPP2PP1/R1BQR1K1 w - - | d5e7 | 0 | B - N\n"
			+ "rnbqrbn1/pp3ppp/3p4/2p2k2/4p3/3B1K2/PPP2PPP/RNB1Q1NR w - - | d3e4 | 100 | P\n"
			+ "rnb1k2r/p3p1pp/1p3p1b/7n/1N2N3/3P1PB1/PPP1P1PP/R2QKB1R w KQkq - | e4d6 | -200 | -N + P\n"
			+ "r1b1k2r/p4npp/1pp2p1b/7n/1N2N3/3P1PB1/PPP1P1PP/R2QKB1R w KQkq - | e4d6 | 0 | -N + N\n"
			+ "2r1k2r/pb4pp/5p1b/2KB3n/4N3/2NP1PB1/PPP1P1PP/R2Q3R w k - | d5c6 | -300 | -B\n"
			+ "2r1k2r/pb4pp/5p1b/2KB3n/1N2N3/3P1PB1/PPP1P1PP/R2Q3R w k - | d5c6 | 0 | -B + B\n"
			+ "2r1k3/pbr3pp/5p1b/2KB3n/1N2N3/3P1PB1/PPP1P1PP/R2Q3R w - - | d5c6 | -300 | -B + B - N\n"
			+ "5k2/p2P2pp/8/1pb5/1Nn1P1n1/6Q1/PPP4P/R3K1NR w KQ - | d7d8q | 800 | (Q - P)\n"
			+ "r4k2/p2P2pp/8/1pb5/1Nn1P1n1/6Q1/PPP4P/R3K1NR w KQ - | d7d8q | -100 | (Q - P) - Q\n"
			+ "5k2/p2P2pp/1b6/1p6/1Nn1P1n1/8/PPP4P/R2QK1NR w KQ - | d7d8q | 200 | (Q - P) - Q + B\n"
			+ "4kbnr/p1P1pppp/b7/4q3/7n/8/PP1PPPPP/RNBQKBNR w KQk - | c7c8q | -100 | (Q - P) - Q\n"
			+ "4kbnr/p1P1pppp/b7/4q3/7n/8/PPQPPPPP/RNB1KBNR w KQk - | c7c8q | 200 | (Q - P) - Q + B\n"
			+ "4kbnr/p1P1pppp/b7/4q3/7n/8/PPQPPPPP/RNB1KBNR w KQk - | c7c8q | 200 | (Q - P)\n"
			+ "4kbnr/p1P4p/b1q5/5pP1/4n3/5Q2/PP1PPP1P/RNB1KBNR w KQk f6 | g5f6 | 0 | P - P\n"
			+ "4kbnr/p1P4p/b1q5/5pP1/4n3/5Q2/PP1PPP1P/RNB1KBNR w KQk f6 | g5f6 | 0 | P - P\n"
			+ "4kbnr/p1P4p/b1q5/5pP1/4n2Q/8/PP1PPP1P/RNB1KBNR w KQk f6 | g5f6 | 0 | P - P\n"
			+ "1n2kb1r/p1P4p/2qb4/5pP1/4n2Q/8/PP1PPP1P/RNB1KBNR w KQk - | c7b8q | 200 | N + (Q - P) - Q\n"
			+ "rnbqk2r/pp3ppp/2p1pn2/3p4/3P4/N1P1BN2/PPB1PPPb/R2Q1RK1 w kq - | g1h2 | 300 | B\n"
			+ "3N4/2K5/2n5/1k6/8/8/8/8 b - - | c6d8 | 0 | N - N\n"
			+ "3n3r/2P5/8/1k6/8/8/3Q4/4K3 w - - | c7d8q | 700 | (N + Q - P) - Q + R\n"
			+ "r2n3r/2P1P3/4N3/1k6/8/8/8/4K3 w - - | e6d8 | 300 | N\n"
			+ "8/8/8/1k6/6b1/4N3/2p3K1/3n4 w - - | e3d1 | 0 | N - N\n"
			+ "8/8/1k6/8/8/2N1N3/4p1K1/3n4 w - - | c3d1 | 100 | N - (N + Q - P) + Q\n"
			+ "r1bqk1nr/pppp1ppp/2n5/1B2p3/1b2P3/5N2/PPPP1PPP/RNBQK2R w KQkq - | e1g1 | 0";

	private class SEE
	{
		private static int[] SEEPieceValues = new int[] { 100, 300, 300, 500, 900, 0 };

		public static int moveEstimatedValue(Board board, Move move)
		{
			// Start with the value of the piece on the target square
			int value = !board.getPiece(move.getTo()).equals(Piece.NONE)
					? SEEPieceValues[board.getPiece(move.getTo()).getPieceType().ordinal()]
					: 0;

			// Factor in the new piece's value and remove our promoted pawn
			if (!Piece.NONE.equals(move.getPromotion()))
				value += SEEPieceValues[move.getPromotion().getPieceType().ordinal()]
						- SEEPieceValues[PieceType.PAWN.ordinal()];

			// Target square is encoded as empty for enpass moves
			else if (PieceType.PAWN.equals(board.getPiece(move.getFrom()).getPieceType())
					&& board.getEnPassant().equals(move.getTo()))
				value = SEEPieceValues[PieceType.PAWN.ordinal()];

			return value;
		}

		public static boolean staticExchangeEvaluation(Board board, Move move, int threshold)
		{
			Square from, to;
			PieceType nextVictim;
			Side colour;
			int balance;
			long bishops, rooks, occupied, attackers, myAttackers;
			boolean isPromotion, isEnPassant;

			// Unpack move information
			from = move.getFrom();
			to = move.getTo();

			isPromotion = !Piece.NONE.equals(move.getPromotion());
			isEnPassant = PieceType.PAWN.equals(board.getPiece(from).getPieceType()) && board.getEnPassant().equals(to);

			// Next victim is moved piece or promotion type
			nextVictim = !isPromotion ? board.getPiece(from).getPieceType() : move.getPromotion().getPieceType();

			// Balance is the value of the move minus threshold. Function
			// call takes care for Enpass, Promotion and Castling moves.
			balance = moveEstimatedValue(board, move) - threshold;

			// Best case still fails to beat the threshold
			if (balance < 0)
				return false;

			// Worst case is losing the moved piece
			balance -= SEEPieceValues[nextVictim.ordinal()];

			// If the balance is positive even if losing the moved piece,
			// the exchange is guaranteed to beat the threshold.
			if (balance >= 0)
				return true;

			// Grab sliders for updating revealed attackers
			bishops = board.getBitboard(Piece.BLACK_BISHOP) | board.getBitboard(Piece.WHITE_BISHOP)
					| board.getBitboard(Piece.BLACK_QUEEN) | board.getBitboard(Piece.WHITE_QUEEN);
			rooks = board.getBitboard(Piece.BLACK_ROOK) | board.getBitboard(Piece.WHITE_ROOK)
					| board.getBitboard(Piece.BLACK_QUEEN) | board.getBitboard(Piece.WHITE_QUEEN);

			// Let occupied suppose that the move was actually made
			occupied = board.getBitboard();
			occupied = (occupied ^ (1L << from.ordinal())) | (1L << to.ordinal());
			if (isEnPassant)
				occupied ^= (1L << board.getEnPassant().ordinal());

			// Get all pieces which attack the target square. And with occupied
			// so that we do not let the same piece attack twice
			attackers = board.squareAttackedBy(to, board.getSideToMove(), occupied)
					| board.squareAttackedBy(to, board.getSideToMove().flip(), occupied) & occupied;

			// Now our opponents turn to recapture
			colour = board.getSideToMove().flip();

			while (true)
			{
				// If we have no more attackers left we lose
				myAttackers = attackers & board.getBitboard(colour);

				if (myAttackers == 0)
					break;

				if ((myAttackers & board.getBitboard(Piece.make(colour, PieceType.PAWN))) != 0L)
				{
					nextVictim = PieceType.PAWN;
				}

				else if ((myAttackers & board.getBitboard(Piece.make(colour, PieceType.KNIGHT))) != 0L)
				{
					nextVictim = PieceType.KNIGHT;
				}

				else if ((myAttackers & board.getBitboard(Piece.make(colour, PieceType.BISHOP))) != 0L)
				{
					nextVictim = PieceType.BISHOP;
				}

				else if ((myAttackers & board.getBitboard(Piece.make(colour, PieceType.ROOK))) != 0L)
				{
					nextVictim = PieceType.ROOK;
				}

				else if ((myAttackers & board.getBitboard(Piece.make(colour, PieceType.QUEEN))) != 0L)
				{
					nextVictim = PieceType.QUEEN;
				}

				else if ((myAttackers & board.getBitboard(Piece.make(colour, PieceType.KING))) != 0L)
				{
					nextVictim = PieceType.KING;
				}

				else
				{
					assert (false);
				}

				// Remove this attacker from the occupied
				occupied ^= (1L << Bitboard
						.bitScanForward(myAttackers & board.getBitboard(Piece.make(colour, nextVictim))));

				// A diagonal move may reveal bishop or queen attackers
				if (nextVictim.equals(PieceType.PAWN) || nextVictim.equals(PieceType.BISHOP)
						|| nextVictim.equals(PieceType.QUEEN))
					attackers |= Attacks.getBishopAttacks(occupied, to) & bishops;

				// A vertical or horizontal move may reveal rook or queen attackers
				if (nextVictim.equals(PieceType.ROOK) || nextVictim.equals(PieceType.QUEEN))
					attackers |= Attacks.getRookAttacks(occupied, to) & rooks;

				// Make sure we did not add any already used attacks
				attackers &= occupied;

				// Swap the turn
				colour = colour.flip();

				// Negamax the balance and add the value of the next victim
				balance = -balance - 1 - SEEPieceValues[nextVictim.ordinal()];

				// If the balance is non-negative after giving away our piece then we win
				if (balance >= 0)
				{
					// As a slide speed up for move legality checking, if our last attacking
					// piece is a king, and our opponent still has attackers, then we've
					// lost as the move we followed would be illegal
					if (nextVictim.equals(PieceType.KING) && (attackers & board.getBitboard(colour)) != 0)
						colour = colour.flip();

					break;
				}
			}

			// Side to move after the loop loses
			return !board.getSideToMove().equals(colour);
		}
	}

	@Test
	public void testSEE()
	{
		Board board = new Board();

		for (String s : rawTestString.split("\n"))
		{
			String[] data = s.split("\\s\\|\\s");

			board.loadFromFen(data[0]);
			Move move = new Move(data[1], board.getSideToMove());

			int expectedValue = Integer.parseInt(data[2]);

			if (!SEE.staticExchangeEvaluation(board, move, expectedValue))
			{
				System.out.println(s);
			}

			assertFalse(SEE.staticExchangeEvaluation(board, move, expectedValue + 1));
			assertTrue(SEE.staticExchangeEvaluation(board, move, expectedValue));
		}
	}
}
