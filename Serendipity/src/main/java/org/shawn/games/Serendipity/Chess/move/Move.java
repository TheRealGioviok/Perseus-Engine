/*
 * Copyright 2017 Ben-Hur Carlos Vieira Langoni Junior
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.shawn.games.Serendipity.Chess.move;

import org.shawn.games.Serendipity.Chess.Piece;
import org.shawn.games.Serendipity.Chess.Side;
import org.shawn.games.Serendipity.Chess.Square;
import org.apache.commons.lang3.StringUtils;

/**
 * The definition of a chess move, that is, a piece movement from its starting
 * square (the origin square) to a destination square. Optionally, the move
 * could specify a promotion piece used to replace a pawn in case of promotion.
 */
public class Move
{
	private final Square from;
	private final Square to;
	private final Piece promotion;
	private int score;

	/**
	 * Creates a new move, using its origin and destination squares.
	 * <p>
	 * Same as {@code new Move(from, to, Piece.NONE)}.
	 *
	 * @param from the origin square
	 * @param to   the destination square
	 */
	public Move(Square from, Square to)
	{
		this(from, to, Piece.NONE);
	}

	/**
	 * Creates a new move, defined by its origin square, its destination, and a
	 * promotion piece.
	 *
	 * @param from      the origin square
	 * @param to        the destination square
	 * @param promotion the promotion piece
	 */
	public Move(Square from, Square to, Piece promotion)
	{
		this.promotion = promotion;
		this.from = from;
		this.to = to;
		this.score = 0;
	}

	/**
	 * Creates a new move using a string representing the coordinates of the origin
	 * and destination squares, and possibly a promotion piece. The side is used to
	 * disambiguate the color of the promotion piece.
	 * <p>
	 * Valid examples of strings that can be used to instantiate the move are
	 * {@code "e2e4"}, {@code "f1b5"} or {@code "a7a8Q"}.
	 *
	 * @param move the string representing the coordinates of the move
	 * @param side the side used to disambiguate the promotion piece
	 */
	public Move(String move, Side side)
	{
		this(Square.valueOf(move.substring(0, 2).toUpperCase()), Square.valueOf(move.substring(2, 4).toUpperCase()),
				move.length() < 5 ? Piece.NONE
						: Side.WHITE.equals(side) ? Piece.fromFenSymbol(move.substring(4, 5).toUpperCase())
								: Piece.fromFenSymbol(move.substring(4, 5).toLowerCase()));
	}

	/**
	 * Returns the origin square.
	 *
	 * @return the origin square
	 */
	public Square getFrom()
	{
		return from;
	}

	/**
	 * Returns the destination square.
	 *
	 * @return the destination square
	 */
	public Square getTo()
	{
		return to;
	}

	/**
	 * Returns the promotion piece, if present.
	 *
	 * @return the promotion piece, or {@link Piece#NONE} if move is not a promotion
	 */
	public Piece getPromotion()
	{
		return promotion;
	}

	/**
	 * Returns whether the move is a promotion.
	 *
	 * @return {@code true} if the move is a promotion
	 */
	public boolean isPromotion()
	{
		return !Piece.NONE.equals(getPromotion());
	}

	/**
	 * Checks if this move is equivalent to another, according to its definition.
	 *
	 * @param obj the other object reference to compare to this move
	 * @return {@code true} if this move and the object reference are equivalent
	 */
	@Override
	public boolean equals(Object obj)
	{
		if (obj == null || !(obj instanceof Move))
		{
			return false;
		}
		Move move = (Move) obj;
		return move.getFrom().equals(getFrom()) && move.getTo().equals(getTo())
				&& move.getPromotion().equals(getPromotion());

	}

	/**
	 * Returns a hash code value for this move.
	 *
	 * @return a hash value for this move
	 */
	@Override
	public int hashCode()
	{
		return toString().hashCode();
	}

	/**
	 * Returns a string representation of this move.
	 *
	 * @return a string representation of this move
	 */
	@Override
	public String toString()
	{
		String promo = StringUtils.EMPTY;
		if (!Piece.NONE.equals(promotion))
		{
			promo = promotion.getFenSymbol();
		}
		return from.toString().toLowerCase() + to.toString().toLowerCase() + promo.toLowerCase();
	}

	/**
	 * Returns the score of the move
	 * 
	 * @return the score of the move
	 */
	public int getScore()
	{
		return score;
	}

	/**
	 * Sets the score of the move
	 * 
	 * @param the new score of the move
	 */
	public void setScore(int score)
	{
		this.score = score;
	}
}
