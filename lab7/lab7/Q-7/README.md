# Q7 · Matrix-Chain Multiplication

[← Lab 07 dashboard](../README.md) · [Problem sheet](../Problem-Sheet-Lab-07.pdf) · [Input conventions](../INPUTS.md)

## Files

| File | Purpose |
|---|---|
| [q7_matrix_chain_dp.c](q7_matrix_chain_dp.c) | C17 solution: interval dp and split reconstruction |
| [q7_sample_input.txt](q7_sample_input.txt) | Ready-to-run input |
| [q7_sample_output.txt](q7_sample_output.txt) | Captured program output |
| [../tools/test_all.py](../tools/test_all.py) | Independent validators for all seven questions |

## Build and run

From this question folder:

```bash
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q7_matrix_chain_dp.c -o q7
./q7 < q7_sample_input.txt
```

In Windows PowerShell:

```powershell
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q7_matrix_chain_dp.c -o q7.exe
Get-Content q7_sample_input.txt | .\q7.exe
```

Input: **`n`, followed by `n+1` matrix dimensions**. Keep the lab's `common/` folder beside the `Q-*` folders.

## Sample input

```text
6
30 35 15 5 10 20 25
```

<details>
<summary>Sample output</summary>

```text
Minimum scalar multiplications: 15125
Optimal ordering: ((A1 x (A2 x A3)) x ((A4 x A5) x A6))
```

</details>

## Algorithm and analysis
**Answer.** Use interval dynamic programming and reconstruct its split choices.

For matrix dimensions `p0,p1,...,pn`, let `C(i,j)` be the minimum scalar multiplications to multiply `Ai ... Aj`, using one-based matrix indices. A final split after `Ak` costs the two subchains plus their final multiplication:

```math
C(i,i)=0,
\qquad C(i,j)=\min_{i\leq k<j}\big(C(i,k)+C(k+1,j)+p_{i-1}p_kp_j\big).
```

Fill intervals in increasing chain length, recording the minimizing `k`. Recursively print each split, or `Ai` for a one-matrix interval. The C implementation uses zero-based arrays and converts labels to one-based output.

For `30,35,15,5,10,20,25`:

```text
Minimum scalar multiplications: 15125
Optimal ordering: ((A1 x (A2 x A3)) x ((A4 x A5) x A6))
```

### Correctness

Every parenthesization has some final split. If either subchain of an optimal solution were not optimal, replacing it by a cheaper one would improve the total, a contradiction. The recurrence therefore considers every possible final split using optimal subchain values. Induction on interval length proves all entries optimal; the stored splits reconstruct one attaining the minimum.

All dimensions are positive. If a subchain's minimum exceeds 64-bit range, any parenthesization containing that subchain also exceeds the range; skipping it cannot remove a representable optimum. Every multiplication and addition is checked before evaluation. Separate validity flags distinguish an unreachable/overflowing value from the valid integer `UINT64_MAX`.

**Complexity.** `O(n^3)` time and `O(n^2)` table space. Reconstructing the expression uses `O(n)` recursive calls and `O(n)` worst-case stack depth. The code computes multiplication counts; it does not multiply matrix entries.
