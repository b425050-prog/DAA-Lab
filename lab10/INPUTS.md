# Lab 10 input formats

[Dashboard](README.md)

All programs read whitespace-separated integers and use standard output. Vertex IDs are one based; grid coordinates are zero based. Signed component sums use 64-bit integers.

## Q01 Online Graph-Bipartiteness Testing

`V E`, followed by E rows `u v`. Vertices are numbered 1..V. V = 1..200000 and E = 0..200000. Self-loops are rejected; repeated consistent edges are accepted arrivals. Rejected edges never enter the maintained graph.

[Sample](Q-1/q1_sample_input.txt)

## Q02 Dynamic Connected Components in a Grid

`R C k`, followed by k rows `r c`. Grid coordinates are zero based: 0 <= r < R and 0 <= c < C. R and C are positive, R*C <= 1000000, and k = 0..200000. Repeated additions are harmless duplicates.

[Sample](Q-2/q2_sample_input.txt)

## Q03 Maximum Weight Component with Size Constraints

`V E K`, then V signed vertex weights, then E rows `u v edge_weight`. V = 1..200000, E = 0..200000 and K = 0..V. Vertex IDs are 1..V; vertex and edge weights are -10^9..10^9. Step 0 reports the initial isolated-vertex state. `NONE` means no nonempty component meets the size limit.

[Sample](Q-3/q3_sample_input.txt)

## Q04 Reverse Time-Step DSU Application

`V E`, then V signed vertex weights, E rows `u v`, and finally a permutation of all V vertex IDs giving deletion order. V = 1..200000, E = 0..200000, IDs = 1..V and vertex weights = -10^9..10^9. The empty graph is reported as 0; a nonempty graph can have a negative maximum.

[Sample](Q-4/q4_sample_input.txt)

## Q05 Lexicographically Smallest Connected Component Permutation

`n m`, then a permutation of 1..n, followed by m rows `u v` of allowed position swaps. n = 1..200000, m = 0..200000 and position IDs are 1..n. Duplicate edges and self-loops are harmless.

[Sample](Q-5/q5_sample_input.txt)