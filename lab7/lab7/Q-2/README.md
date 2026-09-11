# Q2 · Super Egg Testing

[← Lab 07 dashboard](../README.md) · [Problem sheet](../Problem-Sheet-Lab-07.pdf) · [Input conventions](../INPUTS.md)

## Files

| File | Purpose |
|---|---|
| [q2_egg_dropping_dp.c](q2_egg_dropping_dp.c) | C17 solution: coverage dynamic programming |
| [q2_sample_input.txt](q2_sample_input.txt) | Ready-to-run input |
| [q2_sample_output.txt](q2_sample_output.txt) | Captured program output |
| [../tools/test_all.py](../tools/test_all.py) | Independent validators for all seven questions |

## Build and run

From this question folder:

```bash
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q2_egg_dropping_dp.c -o q2
./q2 < q2_sample_input.txt
```

In Windows PowerShell:

```powershell
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q2_egg_dropping_dp.c -o q2.exe
Get-Content q2_sample_input.txt | .\q2.exe
```

Input: **`E F`: eggs and floors**. Keep the lab's `common/` folder beside the `Q-*` folders.

## Sample input

```text
2 100
```

<details>
<summary>Sample output</summary>

```text
Minimum worst-case drops: 14
One optimal first drop: floor 14
```

</details>

## Algorithm and analysis
**Answer.** **14 drops** guarantee finding the highest safe floor using two eggs in a 100-storey building.

### Generalized dynamic programming

Let `R(m,e)` be the maximum number of floors whose threshold can be resolved with at most `m` drops and `e` eggs. One drop separates the unresolved floors into a lower part, the tested floor, and an upper part:

```math
R(m,e)=R(m-1,e-1)+1+R(m-1,e),\qquad R(0,e)=R(m,0)=0.
```

If the egg breaks, the lower part must be solvable with one fewer egg and one fewer drop. If it survives, the upper part has one fewer drop and the same number of eggs. Both parts must be covered for a worst-case guarantee. This proves the upper bound; choosing the tested floor to split the interval into exactly those capacities achieves it.

Increase `m` until `R(m,E)>=F`. A one-dimensional array holds the previous row; update egg counts downward to avoid overwriting a needed value. Cap entries at `F`, since larger coverage is irrelevant. The chosen input bound keeps every addition below `2F+1`, well within 64 bits. For one egg, the exact answer is `F`, which the implementation returns directly.

For two eggs, the recurrence simplifies to `R(m,2)=m(m+1)/2`. Since `R(13,2)=91<100` and `R(14,2)=105>=100`, 14 is both necessary and sufficient.

### Actual dropping procedure

Maintain `lo`, the highest known safe floor, and `hi`, the largest possible safe floor; initially `(lo,hi)=(0,F)`. With `m` remaining drops and `e` eggs:

```text
x = min(hi, lo + R(m-1,e-1) + 1)
Drop at x.
If it breaks:   hi = x-1; e = e-1
If it survives: lo = x
m = m-1
Stop when lo == hi; the threshold is lo.
```

The coverage invariant ensures both branches remain resolvable. A table of the relevant `R` values supports this procedure; the supplied executable computes the optimal budget and its first drop using the compressed table.

For two eggs and 100 floors, one first-egg sequence along the all-surviving branch is:

```text
14, 27, 39, 50, 60, 69, 77, 84, 90, 95, 99, 100
```

After a break, use the remaining egg sequentially from the previous safe floor plus one up to the floor below the break. For example, a break at 27 leaves floors 15 through 26: two drops have been used and at most 12 remain, totaling 14. If no egg ever breaks, the threshold is 100.

**Complexity.** For the implemented answer calculation, `O(E*M)` time and `O(E)` space, where `M` is the optimal drop budget. One egg has an `O(1)` shortcut. A full coverage table for adaptive reconstruction would take `O(E*M)` space. The classical floor-based recurrence, used independently in the tests, takes `O(E*F^2)` time and `O(E*F)` space.
