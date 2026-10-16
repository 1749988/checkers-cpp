# Checkers C++ · Damas

An 8×8 checkers game with a portable C++17 engine, a two-player terminal application, and saved-game replay. Originally developed for **Metodologia de la Programació** at the Universitat Autònoma de Barcelona.

**Authors:** Ismail Boukhzar and Alex Caro.

## What you can try

- Play a local two-player game from the terminal, with enforced turns.
- Ask for legal moves before selecting a piece.
- Play mandatory captures, complete multi-capture sequences, flying kings, and promotion.
- Save a game and replay its full capture paths.
- Run engine and persistence tests without graphics dependencies.

The original SDL-based university interface is retained in `Project-Dames/Project-Dames`. The **terminal build is the supported, tested entry point**. The graphical project still requires its SDL dependencies and has not been validated on Windows after these repairs.

## Quick start

Requirements: a C++17 compiler and CMake 3.16 or newer.

```bash
git clone https://github.com/1749988/checkers-cpp.git
cd checkers-cpp
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

On Linux/macOS:

```bash
cd build
./checkers
```

On Windows with a Visual Studio CMake generator:

```powershell
cd build
.\Release\checkers.exe
```

With GNU Make, you can also run `make all test` from the repository root, followed by `cd build` and `./checkers`.

## Playing

White moves first. `O` and `X` are white and black men; `D` and `R` are kings. Use board coordinates from `a1` to `h8`.

| Input | Result |
| --- | --- |
| `a3 b4` | Move from a3 to b4 |
| `moves a3` | List legal paths for the piece on a3 |
| `a3 c5 e7` | Select an exact multi-capture route, when legal |
| `save` | Save the current move history |
| `quit` | Save a non-empty history and exit |

Games are saved as `recorded_moves.txt` in the working directory. Pass `--record FILE` to choose a different output file. Replaying never overwrites the input file.

```bash
./checkers --replay recorded_moves.txt
./checkers --board tauler_inicial.txt --record my-game.txt
./checkers --help
```

For a move entered using only origin and destination, the engine selects the first legal full path to that destination. Use the full path when different captures end on the same square.

## Rules implemented

This is an academic 8×8 variant, not a claim of full compliance with every national or tournament ruleset:

- Men move one diagonal square forward and may capture in either direction.
- Kings slide diagonally and may land on any free square beyond a captured opponent.
- Captures are compulsory; the route capturing the most pieces across the current player's pieces must be chosen.
- A capture sequence must finish before the turn changes; captured pieces are removed during the sequence.
- Promotion occurs at the end of the move.
- A player with no legal move loses. Automatic repetition and move-count draws are not implemented.

## Design

| Component | Responsibility |
| --- | --- |
| `Tauler` | Board state, recursive capture search, legal moves, promotion |
| `Fitxa` / `Posicio` / `Moviment` | Piece, coordinate, and full-route models |
| `CuaMoviments` | Linked queue for move recording and replay |
| `cli/main.cpp` | Two-player turns, commands, persistence, end-of-game detection |
| `Joc` / `GraphicManager` | Original graphical interface and supplied rendering framework |

Board cells use value ownership (`std::array`) so copies and repeated loads do not leak or share piece pointers. Replay loading validates complete records before replacing the queue. Invalid moves leave the board unchanged.

## Tests and maintenance

The regression suite covers invalid coordinates, forward moves, backward captures, compulsory and maximum captures, flying kings, promotion, board reloads, and complete replay serialization. Tests throw on failure, including in Release builds.

Changes also repair the broken graphical entry point, local asset/include paths, repeated mouse-click handling, and player selection. The original Catalan class names and co-author attribution are preserved.

## Remaining work

- Validate and package the legacy SDL interface on Windows.
- Add draw detection and an optional computer opponent.
- Define a distributable license with all contributors; third-party framework notices remain in their source files.
