# Q3 · Reve’s Puzzle

[← Lab 07 dashboard](../README.md) · [Problem sheet](../Problem-Sheet-Lab-07.pdf) · [Input conventions](../INPUTS.md)

## Files

| File | Purpose |
|---|---|
| [q3_reves_puzzle.c](q3_reves_puzzle.c) | C17 solution: frame–stewart dynamic programming |
| [q3_sample_input.txt](q3_sample_input.txt) | Ready-to-run input |
| [q3_sample_output.txt](q3_sample_output.txt) | Captured program output |
| [../tools/test_all.py](../tools/test_all.py) | Independent validators for all seven questions |

## Build and run

From this question folder:

```bash
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q3_reves_puzzle.c -o q3
./q3 < q3_sample_input.txt
```

In Windows PowerShell:

```powershell
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q3_reves_puzzle.c -o q3.exe
Get-Content q3_sample_input.txt | .\q3.exe
```

Input: **`n` disks**. Keep the lab's `common/` folder beside the `Q-*` folders.

Append `--count` to suppress the move trace: `./q3 --count < q3_sample_input.txt`.

## Sample input

```text
8
```

<details>
<summary>Sample output</summary>

```text
Minimum moves: 33
Top-level split: 4 small disks
Move 1: disk 1 A -> D
Move 2: disk 2 A -> B
Move 3: disk 3 A -> C
Move 4: disk 2 B -> C
Move 5: disk 4 A -> B
Move 6: disk 2 C -> A
Move 7: disk 3 C -> B
Move 8: disk 2 A -> B
Move 9: disk 1 D -> B
Move 10: disk 5 A -> C
Move 11: disk 6 A -> D
Move 12: disk 5 C -> D
Move 13: disk 7 A -> C
Move 14: disk 5 D -> A
Move 15: disk 6 D -> C
Move 16: disk 5 A -> C
Move 17: disk 8 A -> D
Move 18: disk 5 C -> D
Move 19: disk 6 C -> A
Move 20: disk 5 D -> A
Move 21: disk 7 C -> D
Move 22: disk 5 A -> C
Move 23: disk 6 A -> D
Move 24: disk 5 C -> D
Move 25: disk 1 B -> A
Move 26: disk 2 B -> D
Move 27: disk 3 B -> C
Move 28: disk 2 D -> C
Move 29: disk 4 B -> D
Move 30: disk 2 C -> B
Move 31: disk 3 C -> D
Move 32: disk 2 B -> D
Move 33: disk 1 A -> D
Verified: all disks legally transferred from A to D.
```

</details>

## Algorithm and analysis
**Answer.** Eight disks can be moved legally in **33 moves**.

Let `H(n)` be the optimal four-peg transfer count, with `H(0)=0`. For each `0<=k<n`:

1. Move the top `k` disks from A to a spare peg with all four pegs.
2. Move the remaining `n-k` larger disks from A to D with the other three pegs. The peg storing smaller disks cannot receive larger ones.
3. Move the stored `k` disks onto D with all four pegs.

This gives the Frameâ€“Stewart recurrence:

```math
H(n)=\min_{0\leq k<n}\left(2H(k)+2^{n-k}-1\right).
```

Compute bottom-up, storing the best split. Reconstruct with mutually supporting four-peg and standard three-peg recursive routines. Disk offsets preserve the original disk numbers in recursive subproblems.

For eight disks the first minimizing split is `k=4`:

```text
4 small disks A -> B:  H(4) = 9 moves
4 large disks A -> D:  2^4-1 = 15 moves
4 small disks B -> D:  H(4) = 9 moves
Total: 9 + 15 + 9 = 33 moves
```

The complete move list is in [`q3_sample_output.txt`](../Q-3/q3_sample_output.txt).

### Correctness and optimality

Inductively, every recursive transfer preserves disk order. During the middle phase, the stored small disks are untouched, so the standard three-peg algorithm moves the larger disks legally. The final phase places smaller disks on larger disks at D. The move count is exactly the selected recurrence value.

The recurrence directly proves optimality **within this split strategy**. Optimality among all legal four-peg strategies is a deeper theorem proved by [Thierry Bousch, *La quatriÃ¨me tour de HanoÃ¯* (2014)](https://www.imo.universite-paris-saclay.fr/~thierry.bousch/preprints/). It should not be assumed to follow merely from enumerating splits, and the same general optimality claim is not made here for arbitrary numbers of pegs.

**Complexity.** Count DP: `O(n^2)` time and `O(n)` space. Producing and validating the moves: `O(H(n))` additional time, so total `O(n^2+H(n))`, with `O(n)` stack and peg storage. Writing exponentially growing move sequences cannot be hidden inside the polynomial DP cost.
