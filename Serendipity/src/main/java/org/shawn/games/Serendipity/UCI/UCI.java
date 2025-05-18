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

import java.io.IOException;
import java.util.*;

import org.shawn.games.Serendipity.NNUE.AccumulatorStack;
import org.shawn.games.Serendipity.NNUE.NNUE;
import org.shawn.games.Serendipity.Search.Limits;
import org.shawn.games.Serendipity.Search.ThreadManager;
import org.shawn.games.Serendipity.Search.TranspositionTable;

import org.shawn.games.Serendipity.Chess.*;
import org.shawn.games.Serendipity.Chess.move.*;

public class UCI
{
	private static Board internalBoard;
	private static Map<String, UCIOption> options;
	private static ThreadManager engine;
	private static TranspositionTable transpositionTable;
	private static NNUE network;

	private static ThreadsOption threads;
	private static HashOption hash;

	public static class NNUEOption extends StringOption
	{
		public NNUEOption(String value, String name)
		{
			super(value, name);
		}

		@Override
		public void set(String value)
		{
			try
			{
				network = new NNUE("/" + value);
			}

			catch (IOException e)
			{
				System.out.print("Error Loading NNUE");
				return;
			}

			engine.reinit(network);
			super.set(value);
		}
	}

	public static class HashOption extends IntegerOption
	{
		private final TranspositionTable tt;

		public HashOption(int value, int lowerBound, int upperBound, TranspositionTable tt, String name)
		{
			super(value, lowerBound, upperBound, name);
			this.tt = tt;
		}

		@Override
		public void set(String value)
		{
			super.set(value);
			tt.resize(super.value);
		}
	}

	public static class ThreadsOption extends IntegerOption
	{
		public ThreadsOption(int value, int lowerBound, int upperBound, String name)
		{
			super(value, lowerBound, upperBound, name);
		}

		@Override
		public void set(String value)
		{
			super.set(value);
			engine.reinit(super.value);
		}
	}

	public static void main(String[] args)
	{
		transpositionTable = new TranspositionTable(4);
		options = new HashMap<>();
		StringOption networkName = new NNUEOption("embedded.nnue", "nnuefile");
		threads = new ThreadsOption(1, 1, 1048576, "Threads");
		hash = new HashOption(4, 1, 32768, transpositionTable, "Hash");

		try
		{
			network = new NNUE("/" + networkName.get());
		}

		catch (IOException e)
		{
			System.out.print("Error Loading NNUE");
			return;
		}

		internalBoard = new Board();

		engine = new ThreadManager();
		engine.init(threads.get(), transpositionTable, network);

		if (args.length == 1 && args[0].equals("bench"))
		{
			Bench.bench(engine, 10, true);
			engine.shutdownAll();
			return;
		}

		UCIMainLoop();
	}

	public static void addOption(String name, UCIOption option)
	{
		options.put(name, option);
	}

	public static IntegerOption getOption(String name)
	{
		return (IntegerOption) options.get(name);
	}

	public static void setOption(String name, String value)
	{
		UCIOption myOption = options.get(name);
		myOption.set(value);
	}

	private static long perft(Board board, int depth, int ply, boolean root)
	{
		if (depth == 0)
		{
			return 1;
		}

		long nodes = 0;
		List<Move> moves = board.legalMoves();

		for (Move move : moves)
		{
			board.doMove(move);

			if (root)
			{
				long thisMoveNodes;
				thisMoveNodes = perft(board, depth - 1, ply + 1, false);
				System.out.println(move + ":\t" + thisMoveNodes);
				nodes += thisMoveNodes;
			}

			else
			{
				nodes += perft(board, depth - 1, ply + 1, false);
			}

			board.undoMove();
		}

		return nodes;
	}

	public static void UCIMainLoop()
	{
		Scanner input = new Scanner(System.in);
		int depth;
		while (input.hasNextLine())
		{
			String command = input.nextLine();
			String[] fullCommand = command.split(" ");

			switch (command.split(" ")[0])
			{
				case "isready":
					System.out.println("readyok");
					break;
				case "uci":
					System.out.println("id name Serendipity");
					System.out.println("id author Shawn Xu");
					for (UCIOption option : options.values())
					{
						System.out.println(option);
					}
					System.out.println("uciok");
					break;
				case "ucinewgame":
					System.gc();
					internalBoard = new Board();
					transpositionTable.clear();
					engine.init(threads.get(), transpositionTable, network);
					break;
				case "quit":
					input.close();
					engine.shutdownAll();
					return;
				case "perft":
					if (fullCommand.length >= 2)
					{
						depth = Integer.parseInt(fullCommand[1]);
					}
					else
					{
						depth = 4;
					}
					long timeBegin = System.nanoTime();
					long totalNodes = perft(internalBoard, depth, 0, true);
					long timeEnd = System.nanoTime();
					System.out.println("\nNodes searched:\t\t" + totalNodes);
					System.out.println("Time spent (ms):\t" + (timeEnd - timeBegin) / 1000000);
					System.out.println(
							"Nodes per second:\t" + totalNodes * 1000 / Math.max((timeEnd - timeBegin) / 1000000, 1));
					break;
				case "eval":
					AccumulatorStack acc = new AccumulatorStack(network);
					acc.init(internalBoard);

					System.out.println(internalBoard);
					System.out.println(NNUE.evaluate(internalBoard, network, acc));
					break;
				case "go":
					Limits limits = new Limits();
					limits.setTime(157680000000L);

					for (int i = 1; i < fullCommand.length; i++)
					{
						if (fullCommand[i].equals("infinite"))
						{
							limits.setTime(157680000000L);
							break;
						}

						if ((fullCommand[i].equals("wtime") && Side.WHITE.equals(internalBoard.getSideToMove()))
								|| (fullCommand[i].equals("btime") && Side.BLACK.equals(internalBoard.getSideToMove())))
						{
							limits.setTime(Long.parseLong(fullCommand[i + 1]));
						}

						if ((fullCommand[i].equals("winc") && Side.WHITE.equals(internalBoard.getSideToMove()))
								|| (fullCommand[i].equals("binc") && Side.BLACK.equals(internalBoard.getSideToMove())))
						{
							limits.setIncrement(Long.parseLong(fullCommand[i + 1]));
						}

						if (fullCommand[i].equals("depth"))
						{
							limits.setDepth(Integer.parseInt(fullCommand[i + 1]));
							limits.setTime(157680000000L);
							break;
						}

						if (fullCommand[i].equals("nodes"))
						{
							limits.setNodes(Long.parseLong(fullCommand[i + 1]));
							limits.setTime(157680000000L);
							break;
						}

						if (fullCommand[i].equals("movetime"))
						{
							limits.setTime(Long.parseLong(fullCommand[i + 1]));
							limits.setIncrement(0);
							limits.setMovesToGo(-1);
						}

						if (fullCommand[i].equals("movestogo"))
						{
							limits.setMovesToGo(Integer.parseInt(fullCommand[i + 1]));
						}
					}

					engine.nextMove(internalBoard.clone(), limits);
					break;
				case "position":
					for (int i = 1; i < fullCommand.length; i++)
					{
						if (fullCommand[i].equals("startpos"))
						{
							internalBoard = new Board();
						}
						if (fullCommand[i].equals("fen"))
						{
							internalBoard = new Board();
							internalBoard.loadFromFen(
									fullCommand[i + 1] + " " + fullCommand[i + 2] + " " + fullCommand[i + 3] + " "
											+ fullCommand[i + 4] + " " + fullCommand[i + 5] + " " + fullCommand[i + 6]);
						}
						if (fullCommand[i].equals("moves"))
						{
							for (int j = i + 1; j < fullCommand.length; j++)
							{
								internalBoard.doMove(fullCommand[j]);
							}
							break;
						}
					}
					break;
				case "option":
					for (UCIOption option : options.values())
					{
						System.out.println(option);
					}
					break;
				case "stop":
					engine.stop();
					break;
				case "setoption":
					String key = "";
					for (int i = 1; i < fullCommand.length; i++)
					{
						if (fullCommand[i].equals("name"))
						{
							key = fullCommand[i + 1];
						}
						if (fullCommand[i].equals("value") && options.containsKey(key))
						{
							options.get(key).set(fullCommand[i + 1]);
						}
					}
					break;
				case "showvalues":
					for (UCIOption option : options.values())
					{
						System.out.println(option + " value " + option.getString());
					}
					break;
				case "bench":
					depth = 10;
					if (fullCommand.length > 1)
					{
						depth = Integer.parseInt(fullCommand[1]);
					}
					Bench.bench(engine, depth, false);
					transpositionTable.clear();
					break;
			}
		}

		input.close();
	}
}
