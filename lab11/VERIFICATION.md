# Lab 11 verification

Strict C17 builds passed with `-O2 -Wall -Wextra -Wpedantic -Werror`. All **807 executions passed**: 783 core and 24 scaling runs. The core random seed is 42505011. Run `python tools/validate.py` and `python tools/generate_evidence.py` from this lab directory to reproduce them.

| Scope | Independent evidence |
|---|---|
| Q1 | Bellman-Ford on a separately materialised 2V-state graph; all parity distances and effective costs compared |
| Q2 | K independent single-source BFS runs; explicit minimum (distance, hub-index) pairs |
| Q3 | Explicit pairwise Hamming-adjacency graph with ordinary BFS on small dictionaries; shortest length and every returned mutation checked |
| Random cases | 220 directed graphs, 220 hub/graph cases and 300 word dictionaries |
| Regressions | 19 cases covering zero/huge penalties, self-loops, parallel edges, empty hub sets, zero radius, ties, missing targets and disconnected words |
| Larger word cases | Complete binary cubes for L=1,2,5,10 and one 100000-word length-ten dictionary with a known Hamming-distance optimum |
| Invalid input | 16 executions must reject duplicate hubs/words, malformed values, wrong lengths, wrong alphabet, out-of-range labels and trailing input |
| Samples | Three ordinary-output sample executions |
| Scaling | 24 C executions checked against independent exact oracles; input/output records saved |

The 100000-word test uses the first 100000 base-26 encodings. The all-a source and largest code differ in d positions. Assigning the target's nonzero digits one at a time creates only codes no larger than the target, so every step exists in the dictionary; the Hamming lower bound d is achievable.

## Counter definitions and limits

- Q1: vertex/state initialisations + arc scans + deque actions + answer rows.
- Q2: vertex initialisations + hub seeds + arc scans + queue actions + answer rows.
- Q3: encoded letters + distance initialisations + mutation candidates + hash probes + queue actions.

Input parsing, allocation internals and character printing are excluded. These are selected operation counts, not complete instruction counts or elapsed seconds. Every plot uses linear axes and describes a specified input family. Full per-question scaling inputs and results are in [scaling_evidence.json](scaling_evidence.json).

Q1 and Q2 have O(V+E) worst-case bounds independent of the plotted family. Q3's expected O(NL) bound uses expected constant-time hash lookup; the deterministic open-addressed table can suffer adversarial collisions, with an O(N^2 L) conservative worst-case bound. The word graph is generated implicitly, so there is no O(N^2) pairwise adjacency construction in the C program.
