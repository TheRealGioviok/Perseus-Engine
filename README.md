<div align="center">
  <h1>
    Serendipity
  </h1>
</div>

Serendipity is a UCI chess engine written in the Java programming language. With a CCRL rating of more than 3500 elo, it is the strongest Java chess engine in the world.

### History

In early January 2024, I was inspired by a [tom7 video][elo_world] to create a [chess arena][chess_arena] in Java.
It didn't get very far, however, as I soon focused all my development effort in a minimax implementation.

In March, I began to work on NNUE support.
After completing NNUE, the strength of Serendipity skyrocketed, I had several commits with gains of over 100 Elo.
On March 27, I published my [first release][first_release]. The results were much better than I had expected, as it turns out that I have
vastly underestimated Serendipity's strength. Two days later, I made my first Stockfish commit.

In May, four other engine developers and I co-founded an [OpenBench][openbench_url] instance, where Serendipity testing happens to this day.

In August, I implemented multithreading for Serendipity's first debut Top Chess Engines Championship.

#### Timeline

- January 9, 2024: First commit to arena project
- January 12, 2024: First main repo commit
- March 10, 2024: NNUE support
- March 27, 2024: First release of Serendipity
- May 24, 2024: OpenBench instance founded
- August 23, 2024: Multithreading support
- August 26, 2024: TCEC Debut

### Strength

As of November 2024, Serendipity is the strongest Java engine in the world and around top 50 overall.

<div align="center">

| Version | [CCRL Blitz][ccrl-blitz] | [CCRL 40/15][ccrl-4040] |
|:-------:|:------------------------:|:-----------------------:|
|  v0.1   |             -            |          3227           |
|  v0.2   |           3411           |            -            |
|  v0.3   |             -            |          3390           |
|  v0.4   |           3516           |          3446           |
|  v1.0   |           3559           |          3487           |

</div>

### Build

To build Serendipity from source, install [`maven`][maven_url], [`make`][make_url], [`git`][git_url] and [`wget`][wget_url]. Then run the following:

```bash
git clone git@github.com:xu-shawn/Serendipity.git
cd Serendipity
make
```

This will produce two versions of Serendipity executable: `Serendipity-Dev` and `Serendipity.jar`.

`Serendipity-Dev` is a standalone executable that can be run on Linux or MacOS directly.

`Serendipity.jar` is cross platform, but it requires extra commands to run. See section below for more details.

### Run

Serendipity only supports Java version 17 or higher due to its dependency on Java's (incubating) [Vector API][vector_api]. To run `Serendipity.jar`:

```
java -jar --add-modules jdk.incubator.vector Serendipity.jar
```

Alternatively, if you are on Linux/MacOS, you can also opt to run the Serendipity executable file:

```
./Serendipity
```

### Search

- Efficiency
  - Iterative Deepening
  - Aspiration Windows
  - Principle Variation Search (PVS)
  - Transposition Table
    - Shared Transposition Table
    - 10-byte entries
    - Parallel array implementation
- Multithreading
  - Lazy SMP
- Move Ordering
  - Butterfly History
  - Capture History
  - Continuation Histories
    - 1-ply Continuation History
    - 2-ply Continuation History
    - 4-ply Continuation History
    - 6-ply Continuation History
  -  Ordering of Captures by Static Exchange Evaluation (SEE)
- Selectivity
  - Quiescent Search (QS)
    - QS SEE Pruning
    - QS Futility Pruning
  - Extensions
    - Singular Extensions
      - 1-ply Extension of PV Nodes
      - 2-ply Extension of Non-PV Nodes
  - Pruning
    - Before Moves Loop
      - Null Move Pruning
      - Reverse Futility Pruning
      - Razoring
    - Moves Loop Prunings
      - Futility Pruning
      - PVS SEE Pruning
      - Multi-cut Pruning
  - Reductions
    - Late Move Reductions
    - Internal Iterative Reductions

### Evaluation

Serendipity relies on a side-relative and efficiently updatable neural network for evaluation.
This avoids full network refreshes between moves, which helps the engine obtain good evaluation with reasonable speed.

The description of the network architecture, as well as the training procedure, is beyond the scope of this document.

### Acknowledgements

Big shoutout to everyone who has [contributed][contributors_url] ideas or code to this project.

Serendipity depends on the following libraries:
- [`chesslib`][chesslib_url] for move generation and board representation
- [`JUnit`][JUnit_url] for testing critical components of the engine

Serendipity's neural network is trained using [bullet][bullet_url] and on open data generously provided by the [Leela Chess Zero][lc0_url] project.

Testing Serendipity requires thosands of hours of compute, so thanks to everyone on our [OpenBench instance][furybench_url] for their contributions, however big or small.
The names of these people are listed below, by alphabetical order:
- [ArjunBasandrai][ArjunBasandrai]
- [aronpetko][aronpetko]
- [gab8192][gab8192]
- [Haxk20][Haxk20]
- [Jochengehtab][Jochengehtab]
- [noobpwnftw][noobpwnftw]
- Styxdoto
- [Vast342][Vast342]
- [Witek902][Witek902]
- [yl25946][yl25946]
- [Yoshie2000][Yoshie2000]

Thanks to engine testers such as CCRL and Silvian Rucsandescu for running and testing the engine.

Finally, thanks to everyone on the [Stockfish Discord][sf_discord], [Engine Programming Discord][ep_discord], and the unofficial [CPW Discord][cpw_discord] for being kind, knowledgeable, and collaborative.

[elo_world]: https://www.youtube.com/watch?v=DpXy041BIlA
[chess_arena]: https://github.com/xu-shawn/SimplerChessEngine
[first_release]: https://github.com/xu-shawn/Serendipity/releases/tag/v0.1
[openbench_url]: https://github.com/AndyGrant/OpenBench

[maven_url]: https://maven.apache.org/
[make_url]: https://www.gnu.org/software/make/
[git_url]: https://git-scm.com/book/en/v2/Getting-Started-Installing-Git
[wget_url]: https://www.gnu.org/software/wget/

[vector_api]: https://docs.oracle.com/en/java/javase/17/docs/api/jdk.incubator.vector/jdk/incubator/vector/Vector.html

[ccrl-blitz]: https://www.computerchess.org.uk/ccrl/404/cgi/compare_engines.cgi?family=Serendipity&print=Rating+list
[ccrl-4040]: https://www.computerchess.org.uk/ccrl/4040/cgi/compare_engines.cgi?family=Serendipity&print=Rating+list

[contributors_url]: https://github.com/xu-shawn/Serendipity/graphs/contributors
[chesslib_url]: https://github.com/bhlangonijr/chesslib
[JUnit_url]: https://github.com/junit-team/junit5
[bullet_url]: https://github.com/jw1912/bullet
[lc0_url]: https://lczero.org/
[furybench_url]: https://chess.aronpetkovski.com/

[ArjunBasandrai]: https://github.com/ArjunBasandrai
[aronpetko]: https://github.com/aronpetko
[gab8192]: https://github.com/gab8192
[Haxk20]: https://github.com/Haxk20
[Jochengehtab]: https://github.com/Jochengehtab
[noobpwnftw]: https://github.com/noobpwnftw
[Vast342]: https://github.com/Vast342
[Witek902]: https://github.com/Witek902
[yl25946]: https://github.com/yl25946
[Yoshie2000]: https://github.com/Yoshie2000

[sf_discord]: https://discord.com/invite/GWDRS3kU6R
[ep_discord]: https://discord.com/invite/F6W6mMsTGN
[cpw_discord]: https://discord.gg/kWDrFSB2GG
