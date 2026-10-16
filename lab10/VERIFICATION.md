# Lab 10 verification

[Dashboard](README.md)

All five programs compile with GCC as C17 using `-O2 -Wall -Wextra -Wpedantic -Werror`. The core checker completed **526 executions** with seed 42505010. The scaling checker completed **40 executions** with seed 20261013, using sizes 16 through 2048. The 566 runs include malformed inputs, self-loops, duplicate operations, signed weights, empty eligible sets and explicit regression cases. This is evidence over finite cases, not exhaustive coverage of the input limits.

| Q | Independent oracle |
|---|---|
| 1 | Attempt each edge in a BFS two-colouring; retain only consistent edges; replay the reported final colouring |
| 2 | Recompute all islands by four-neighbour flood fill after each insertion, including duplicates |
| 3 | Rebuild components with BFS after each edge, sum vertex weights and filter by size; include the isolated initial state |
| 4 | Delete vertices in the stated forward order, rebuild induced-graph components with BFS and compare every maximum |
| 5 | Enumerate reachable permutations by BFS of legal swaps for small inputs; also compare component-wise sorted multisets |

The validator is deliberately based on graph traversal and explicit reachable states, not a second union-find implementation. The negative-weight regression cases catch stale maxima; Q3 also checks `NONE`, and Q4 distinguishes an empty graph from a nonempty negative maximum.

## Counter scope

DSU finds count each visited node. Q2 also counts neighbour probes. Q3 and Q4 count heap comparisons and snapshot-validity checks; Q4 additionally counts adjacency probes. Q5 counts both merge-sort comparisons and DSU visits. Counters omit dense array initialisation, parsing, printing and arithmetic not explicitly instrumented. A graph describes these events on a specified family; the README derives the total asymptotic bound separately.

## Reproduce

```sh
python3 lab10/tools/build.py
python3 lab10/tools/validate.py
python3 lab10/tools/generate_evidence.py
```

[Core record](verification.json) · [Scaling inputs and outputs](scaling_evidence.json) · [Checker](tools/validate.py)

The report screenshots are real executions of the compiled samples in full-screen VS Code. Each stdout was compared with the saved sample before capture. Native document crops exclude the UI, wrapper input and cursor. Complete printed standalone code is compiled and checked again during report creation.

The GitHub Actions workflow repeats strict builds and these checks using GCC and Clang after upload. Its hosted status is established only by the actual run.
