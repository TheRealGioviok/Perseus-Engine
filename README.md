# Perseus Chess Engine

Perseus is a UCI-compliant chess engine written in C++. It's designed to play chess at a competitive level and provides a rich set of features for both developers and users.

## Features

- UCI Protocol: Perseus fully supports the Universal Chess Interface (UCI) protocol, allowing it to seamlessly integrate with various chess GUIs (Graphical User Interfaces).
- Search Algorithms: The engine implements advanced search algorithms such as Alpha-Beta pruning with various enhancements like Transposition Tables, Iterative Deepening, and Quiescence Search. Refer to `search.cpp` for the code.
- Evaluation Function: Perseus incorporates an evaluation function that assesses the strength of a given chess position based on factors like piece values, mobility, and more.
- Bitboard Representation: Utilizing efficient bitboard representation, Perseus achieves fast move generation and evaluation.

## Usage

- Compile: Compile the Perseus source code just by using `make`.
- Run: Execute the compiled binary file.
- Integrate: Connect Perseus to your preferred chess GUI using the UCI protocol.
- Configure: Adjust engine parameters and options as desired to customize its behavior.
- Play: Enjoy playing chess against Perseus or watch it compete against other engines.

## Configuration Options

Perseus provides several configuration options that can be adjusted via the 'setoption' . One can adjust the hash table size, as well as some of the many heuristic's parameters to his liking. The Weather Factory framework is used to tune these parameters.

For a comprehensive list of available options and their descriptions, refer to the source code in `uci.cpp`.

## Contributions

Contributions to Perseus are welcome! Whether it's bug fixes, performance improvements, or new features, feel free to fork the repository, make your changes, and submit a pull request.

In the pull request, make sure to include the bench of the submitted version, and the SPRT result of the main vs submitted engine. SPRT results are not needed if the change doesn't impact the engine playing itself (e.g. correcting a string, formatting, adding comments etc...).

## License

Perseus is distributed under the GNU General Public License. See the LICENSE file for details.

## Credits

Perseus is developed and maintained by me. Special thanks to BluefeverSoftware and CodeMonkeyKing for their evergreen videos that are the starting point for nearly anyone that approaches Computer Chess Programming, the whole Stockfish Discord whose users are always polite and ready to help, and PGG106 for both his help and a lot of the ideas which are (heavily) inspired from his engine. 
Also Cie is a is a strange individual. Just saying. But gives ELO. So we will tolerate. 
