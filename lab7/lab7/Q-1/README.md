# Q1 · Invert the Coin Triangle

[← Lab 07 dashboard](../README.md) · [Problem sheet](../Problem-Sheet-Lab-07.pdf) · [Input conventions](../INPUTS.md)

## Files

| File | Purpose |
|---|---|
| [q1_coin_triangle.c](q1_coin_triangle.c) | C17 solution: balanced corner relocation |
| [q1_sample_input.txt](q1_sample_input.txt) | Ready-to-run input |
| [q1_sample_output.txt](q1_sample_output.txt) | Captured program output |
| [../tools/test_all.py](../tools/test_all.py) | Independent validators for all seven questions |

## Build and run

From this question folder:

```bash
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q1_coin_triangle.c -o q1
./q1 < q1_sample_input.txt
```

In Windows PowerShell:

```powershell
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q1_coin_triangle.c -o q1.exe
Get-Content q1_sample_input.txt | .\q1.exe
```

Input: **`n` rows**. Keep the lab's `common/` folder beside the `Q-*` folders.

Append `--count` to suppress the move trace: `./q1 --count < q1_sample_input.txt`.

## Sample input

```text
4
```

<details>
<summary>Sample output</summary>

```text
Coins: 10
Minimum moves: 3
Target offsets: 2 2
Move 1: (0,0) -> (2,2)
Move 2: (0,3) -> (2,-1)
Move 3: (3,0) -> (-1,2)
Verified: every destination is outside the original triangle.
```

</details>

## Algorithm and analysis
**Answer.** For `n` rows containing `T(n)=n(n+1)/2` coins, the minimum number of relocations is

```math
M(n)=\left\lfloor\frac{n(n+1)}{6}\right\rfloor.
```

The pictured four-row triangle has 10 coins and needs **3 moves**.

### Construction

Represent the original by `S={(i,j): i>=0, j>=0, i+j<=n-1}`. An inverted triangle translated by `(a,b)` is

```text
D = {(a-i,b-j) : (i,j) in S}
  = {(u,v) : u<=a, v<=b, u+v>=a+b-(n-1)}.
```

Keep all coins in `S âˆ© D`. Pair every position in `S \ D` with one in `D \ S`, moving each once. The two sets have equal size because the triangles contain the same number of coins.

Let `n-1=3q+r`, where `0<=r<3`. Choose the three corner side lengths as evenly as possible:

```text
x = q + (r >= 1)
y = q + (r >= 2)
z = q
a = n-1-x
b = n-1-y
```

The discarded corners have `T(x)`, `T(y)`, and `T(z)` coins. For four rows, `x=y=z=1`, so three individual corner coins move. The program lists their actual source and destination lattice coordinates.

### Why this is minimum

For a fixed final placement, every original coin outside the intersection must move. Moving exactly those coins attains that lower bound. Thus the task is to maximize intersection size over translations.

For a placement with `0<=a,b<=n-1` and `a+b>=n-1`, the three discarded corner triangles are disjoint. Their side lengths are `x=n-1-a`, `y=n-1-b`, and `z=a+b-(n-1)`, so `x+y+z=n-1`.

It suffices to consider these placements: if `a>n-1`, decreasing it to `n-1` only relaxes the diagonal bound on original coins; the same holds for `b`. If `a+b<n-1`, the diagonal bound excludes no original coins, and increasing the offsets until their sum is `n-1` can only increase the overlap. A negative offset gives no overlap and cannot improve on the construction. Non-lattice translations share no coin centers and require moving every coin.

Consequently, minimize `T(x)+T(y)+T(z)` subject to nonnegative integer side lengths summing to `n-1`. If `x>=y+2`, replacing `(x,y)` by `(x-1,y+1)` decreases the sum by `x-y-1>0`. A minimum therefore has side lengths differing by at most one. Substituting the balanced lengths above gives `floor(n(n+1)/6)`. The program attains this bound.

**Complexity.** Count only: `O(1)` time and space with fixed-width arithmetic. Listing moves: `Theta(n^2)` time and `Theta(n^2)` auxiliary space to store source/destination coordinates, with `Theta(n^2)` output for growing `n`.

**Validation.** Replay every move and compare the complete final set with `D`. For small `n`, exhaust all translations that could share an original coin center and verify no larger overlap exists. Related geometric treatment: [McCaffrey and Atwill, 2018](https://arxiv.org/abs/1810.02202).
