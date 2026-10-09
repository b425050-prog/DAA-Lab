# Lab 09 verification

[Dashboard](README.md)

All ten C17 sources compiled with GCC using `-O2 -Wall -Wextra -Wpedantic -Werror`. The portable checker passed **805 executions** with seed 42505009, including malformed-input checks, random small instances and saved samples. A separate deterministic scaling run passed **80 executions** with seed 900425050. This is **885 executed cases**, not a claim about all possible inputs.

| Question | Independent check |
|---|---|
| 1 | Integrated schedule replay, capacity/fraction feasibility, concavity-based global optimality gap via a separate linear fractional-knapsack oracle |
| 2 | Independent heap cost, prefix-free codes, canonical integer replay and weighted lengths |
| 3 | Dynamic programme for greatest distance reachable with k stops; physical travel-order replay |
| 4 | Exhaustive pairwise merge search on tiny cases; independent heap optimum on scaling cases |
| 5 | Repeated neighbour-constraint relaxation and exact 2(n-1) counter |
| 6 | Maximum-frequency cooldown feasibility bound; multiset and every repeated-character distance |
| 7 | Cartesian product of all reachable values on tiny cases; independent smallest-range heap over value lists on scaling cases |
| 8 | End-before-start event sweep plus conflict-free room assignment |
| 9 | Independent interval DP; alphabetic order, prefix freedom, weighted leaf depths and merge-cost sum; 250 tied-weight random cases |
| 10 | Exhaustive word permutations on tiny cases, independent memoised subset recurrence on scaling cases, greedy replay and containment of every original word |

Q1 uses double precision and validates numerical tolerances, not symbolic arithmetic. Its documented consumption model is an assumption missing from the paper. Q3 solves the body with fixed F. Q9 models ordered leaves, and does not substitute adjacent-only merging. Q10's exact check is restricted to n<=12 and does not validate the cited preprint.

The full-screen VS Code screenshots in the reports were captured after executing each compiled C sample. The output text was compared with saved stdout before capture. Native Word cropping hides editor UI, input wrappers and cursors. The reports contain the same programme with only the shared helper definitions inlined.

The checked-in datasets contain measured events, not fabricated running times. Q1 face counts exclude the detailed linear-solve arithmetic. Q10 reports overlap candidate probes (including exact-overlap precomputation) and DP transitions separately. Sorting uses the platform C library `qsort`; comparison counts can differ across libraries. The stated sorting bounds assume an O(n log n) comparison-sort implementation, and are not an ISO C guarantee about `qsort`.

## Reproduce

```sh
python3 lab9/tools/build.py
python3 lab9/tools/validate.py
python3 lab9/tools/generate_evidence.py
```

See [core results](verification.json), [scaling traces](scaling_evidence.json) and [checker source](tools/validate.py). GitHub Actions repeats the strict build and checks with GCC and Clang; its actual pass/fail status is established only after upload.
