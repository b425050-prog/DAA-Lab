# Input and output contracts

Run from `lab7/` after building, or compile directly in a question folder. Programs read from standard input and write results to standard output. No prompts are printed, so input files and pipes work directly.

All numeric fields are decimal integers. Whitespace separates tokens. Missing fields, extra fields, malformed numbers, and out-of-range values produce `Error: ...` on standard error and a nonzero exit code. Names are single tokens of at most 127 characters; use underscores for spaces.

| Program | Input | Accepted range | Output |
|---|---|---|---|
| Q1 | `n` rows | `1..1000`; `1..10^9` with `--count` | Coin count, minimum moves, target offsets, explicit moves |
| Q2 | `E F` | `1..64` eggs; `0..10^12` floors | Minimum worst-case drops and one optimal first floor |
| Q3 | `n` disks | `0..64` | Optimal count and every legal move from A to D |
| Q4 | `n` switches | `0..63` | Optimal count and every legal toggle |
| Q5 | `n` hiding spots | `2..2000` | Guaranteed shooting schedule and surviving-position counts |
| Q6 | `N`, then `name birth death` for each scientist | `0..100000` records; signed 64-bit years; `birth <= death` | Maximum alive count and all maximal best intervals |
| Q7 | `n`, then `n+1` dimensions | `1..200` matrices; dimensions `1..10^9` | Minimum scalar cost and a fully parenthesized ordering |

Q1, Q3, Q4 accept one optional command-line flag, `--count`. Other command-line flags for these programs are rejected. Q4 refuses a full trace longer than 1,000,000 moves; use `--count` for large `n`. These are implementation limits chosen for readable traces, finite memory, and exact 64-bit arithmetic; the mathematical algorithms generalize beyond them.

## Q1: coordinates

The original triangle is the set of lattice pairs `(i,j)` with `i >= 0`, `j >= 0`, and `i+j <= n-1`. Physical coordinates can be taken as:

```text
X = i - j
Y = sqrt(3) * (i + j)
```

With screen Y increasing downward, this is the upright triangle from the handout. A pair `(a-i,b-j)` belongs to the reflected target. Printed coordinates are lattice indices, not Cartesian distances. All coins in the overlap remain fixed; each other coin moves directly to an unoccupied final destination. As in the puzzle, a move is a relocation; there is no obstacle/path-length cost model.

## Q2: floor convention

Floors are `1..F`. Floor 0 is safe. The unknown highest safe floor can be any integer `0..F`, including every floor being unsafe or every floor being safe. An egg survives at or below that threshold and breaks above it. The program prints the optimal worst-case count and one first drop; the full adaptive continuation is described in [the solution](Q-2/README.md).

## Q3: disk and peg convention

Disk 1 is smallest; disk `n` is largest. Pegs are A, B, C, D. All disks start on A and finish on D. An empty puzzle (`n=0`) takes zero moves. Tied optimal splits choose the smallest `k`.

## Q4: switch convention

Switch **1 is the rightmost**. The printed bitstring runs from switch `n` on the left to switch 1 on the right. `1` means on. For `n=0`, `-` represents the empty row.

## Q5: target timing

Spots are `1..n` from left to right. A shot happens first. If the target survives, it moves exactly one spot left or right before the next shot, staying within the line. The target cannot stay still. The printed surviving-position count is measured immediately **after the shot and before the next movement**. It counts possible current locations, not distinct histories.

For `n=2`, the schedule is `1,1`. For `n>=3`, it is `2,3,...,n-1,n-1,...,3,2`.

## Q6: deaths before births

Lifetimes are modeled as half-open intervals `[birth, death)`. A scientist who dies in year `y` does not overlap someone born in year `y`. A record with equal birth and death years contributes an empty interval and is skipped. A reversed interval is rejected. Negative years are supported as integer coordinates; the program does not convert historical BC/AD notation.

The alphabetical order supplied by the index does not affect the result. The program sorts year events internally. It prints every maximal interval attaining the peak; adjacent peak segments are merged. `[1900, 1910)` means from 1900 up to, but excluding, 1910. Empty input or only empty lifetimes yields count 0 and no best interval.

## Q7: matrix dimensions and overflow

For dimensions `p0 p1 ... pn`, matrix `Ai` has size `p(i-1) x pi`. Matrix entries are not required because only multiplication cost and parenthesization are being computed.

All arithmetic is exact unsigned 64-bit integer arithmetic. An overflowing candidate split is skipped; another split may still fit. If the minimum itself exceeds `18446744073709551615`, the program reports an error instead of wrapping. A single matrix costs zero multiplications. Tied optimal orderings choose the smallest split index.

## Supplying input on Windows

Use the provided files or pipe a string. For example, from `lab7/` after `build_windows.bat`:

```powershell
"2 100" | .\Q-2\output\q2_egg_dropping_dp.exe
Get-Content Q-7/q7_sample_input.txt | .\Q-7\output\q7_matrix_chain_dp.exe
```

If you launch a program interactively and type its input, end it with Enter, then Ctrl+Z, then Enter on Windows (Ctrl+D on Linux/macOS). Programs validate that there is no trailing input, so they wait for this end-of-input signal. Piping or redirecting an input file supplies it automatically.
