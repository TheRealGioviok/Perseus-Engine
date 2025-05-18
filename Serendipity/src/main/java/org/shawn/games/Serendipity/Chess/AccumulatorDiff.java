package org.shawn.games.Serendipity.Chess;

public class AccumulatorDiff
{
	public class DiffInfo
	{
		public final Piece piece;
		public final Square square;

		public DiffInfo(Piece piece, Square square)
		{
			this.piece = piece;
			this.square = square;
		}
	}

	private int addedCount;
	private DiffInfo[] added;

	private int removedCount;
	private DiffInfo[] removed;

	public AccumulatorDiff()
	{
		this.addedCount = 0;
		this.added = new DiffInfo[2];

		this.removedCount = 0;
		this.removed = new DiffInfo[2];
	}

	public void addPiece(Piece piece, Square square)
	{
		this.added[this.addedCount] = new DiffInfo(piece, square);
		this.addedCount++;
	}

	public void removePiece(Piece piece, Square square)
	{
		this.removed[this.removedCount] = new DiffInfo(piece, square);
		this.removedCount++;
	}

	public int getAddedCount()
	{
		return this.addedCount;
	}

	public int getRemovedCount()
	{
		return this.removedCount;
	}

	public DiffInfo getAdded(int index)
	{
		assert index < getAddedCount();
		return this.added[index];
	}

	public DiffInfo getRemoved(int index)
	{
		assert index < getRemovedCount();
		return this.removed[index];
	}
}
