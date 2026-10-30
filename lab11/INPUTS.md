# Lab 11 input guide

All programs read standard input and print plain text. Labels use 1-based vertices. Extra tokens, malformed integers and out-of-range values are rejected.

## Q1 · Shortest Path on Parity-Constrained Graphs via 0-1 BFS

```text
V E S C
u1 v1 w1
... E directed edges ...
```
Vertices are 1-based. `1 <= V <= 200000`, `0 <= E <= 400000`, `1 <= S <= V`, `0 <= C <= 10^12`, and every edge weight is 0 or 1. Parallel edges and self-loops are allowed. Output gives the shortest raw distance for each parity and the minimum effective cost. `INF` marks a missing parity; `UNREACHABLE` means both are missing. Routes may revisit an original vertex in a different parity state.

## Q2 · Multi-Source BFS for Radius-Constrained Voronoi Cell Partitioning

```text
V E K R
h1 h2 ... hK
u1 v1
... E undirected edges ...
```
Vertices are 1-based. `1 <= V <= 200000`, `0 <= E <= 400000`, `0 <= K <= V`, `0 <= R <= 200000`. Hubs must be distinct and their **input order defines their indices**. A smaller vertex number has no priority. The empty hub set is supported; its hub line may be omitted. Output rows contain vertex, owner index and distance. An unassigned vertex has `-1 -1`; no distance to an arbitrary hub is reported. Self-loops and parallel edges are allowed.

## Q3 · Bidirectional BFS for Shortest Transformation Sequence in a Word Graph

```text
N L
A B
word1
... N unique dictionary words ...
```
`1 <= N <= 100000`, `1 <= L <= 10`; all words use exactly L lowercase English letters. A may be absent from D. B must be in D: even A=B returns -1 when B is missing, following the paper's explicit target-membership condition. Output is the minimum number of mutations (edges, not words), one shortest sequence, and a measured work counter. No possible sequence prints `Sequence: NONE`. Duplicate dictionary entries are rejected.