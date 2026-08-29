[← Lab 05](../README.md) · [Main repository](../../README.md)

# Q3 · Quick Sort of Random Elements Stored in a File

This solution completes the full pipeline required by the question:

```text
N + seed → generate → write random-data file → read it back
         → 3-way Quick Sort → write sorted file → reopen and verify
```

It does not silently sort an in-memory array and treat the file as decoration. Both files are genuinely reopened, parsed, counted, and checked.

<p align="center"><img src="q3_three_way_quicksort.gif" width="96%" alt="Animated seeded file generation, three-way partitioning, and validation"></p>

## Interactive program

File: [`q3_random_file_quicksort.c`](q3_random_file_quicksort.c)

The program asks for:

1. `N`, from `1` through `1,000,000`;
2. an unsigned 64-bit seed;
3. a random-data path, or Enter for `q3_random_input.txt`;
4. a sorted-output path, or Enter for `q3_sorted_output.txt`.

The same `(N, seed)` pair always generates the same signed `long long` sequence. Values are kept in a readable `[-1,000,000,000, 1,000,000,000]` range. One eighth of the generated positions use a small duplicate pool so the three-way duplicate handling is exercised naturally.

```bash
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic \
    q3_random_file_quicksort.c -o q3_random_file_quicksort
./q3_random_file_quicksort
```

The committed reproducible example uses `N = 24` and `seed = 20260829`:

- [`q3_sample_input.txt`](q3_sample_input.txt) — answers supplied to the prompts;
- [`q3_sample_output.txt`](q3_sample_output.txt) — complete console transcript;
- [`q3_random_input.txt`](q3_random_input.txt) — generated unsorted file;
- [`q3_sorted_output.txt`](q3_sorted_output.txt) — verified ascending file.

## Reproducible random generation

[`q3_quicksort.h`](q3_quicksort.h) contains the shared, tested algorithm. It uses **SplitMix64** because its operations are completely specified with `uint64_t`; results do not depend on the C library's `rand()` implementation.

Two independently derived states are used:

```text
data state  = mix(seed, N, data-domain constant)
pivot state = mix(seed, N, pivot-domain constant)
```

Keeping the streams separate means that changing the sort implementation cannot change the generated input. Pivot indices use rejection sampling, avoiding modulo bias.

## Algorithm · seeded randomized 3-way Quick Sort

For a half-open range `[lo, hi)`, choose a seeded pseudo-random pivot and maintain four regions:

```text
[lo, lt)      values < pivot
[lt, scan)    values = pivot
[scan, gt)    not examined yet
[gt, hi)      values > pivot
```

```text
THREE-WAY-QUICKSORT(A, lo, hi)
    while hi - lo > 1
        pivot = A[random index in [lo, hi))
        lt = scan = lo
        gt = hi

        while scan < gt
            if A[scan] < pivot
                swap A[lt], A[scan]
                lt++, scan++
            else if A[scan] > pivot
                gt--
                swap A[scan], A[gt]
            else
                scan++

        recurse on the smaller of [lo, lt) and [gt, hi)
        continue with the larger range in this loop
```

The entire equal block `[lt, gt)` is already in its final relative region and is never processed again. This matters for repeated values: an all-equal input finishes after one linear partition.

## Why it is correct

### Partition correctness

Before each partition-loop iteration, the four regions above form a complete, non-overlapping decomposition of `[lo, hi)`.

- `< pivot`: moving `A[scan]` to `lt` grows the less-than region by one.
- `> pivot`: moving it to `--gt` grows the greater-than region; the incoming value at `scan` remains unexamined.
- `= pivot`: advancing `scan` grows the equal region.

Every iteration shrinks `[scan, gt)`, so partitioning terminates. At termination it is empty and the range is exactly `<`, `=`, `>`.

### Sorting correctness

Use induction on range length. Ranges of length zero or one are sorted. For a longer range, partitioning places every left value below every equal value and every right value above every equal value. The two strict partitions are shorter; the inductive hypothesis sorts them. Their concatenation with the equal block is therefore sorted. Tail-processing the larger side changes stack use, not this argument.

### File and multiset correctness

The sorting engine only exchanges array elements. The program additionally computes an order-independent fingerprint containing:

- the element count;
- the 64-bit modular value sum;
- the sum of a strong 64-bit mix of every value;
- the XOR of independently rotated mixed values.

It compares this fingerprint after generation, input-file read-back, sorting, and output-file read-back. This is a high-confidence corruption check rather than a mathematical collision-free proof; together with sortedness and a swap-only algorithm, it is strong practical validation. The separate experiment removes even that theoretical ambiguity by requiring exact element-by-element equality with an independently sorted oracle copy.

## Complexity analysis

Let `n` be the number of values.

| Situation | Sorting time | Reason |
|---|---:|---|
| Distinct keys, perfectly balanced pivots | `Θ(n log n)` | `log n` levels with linear partition work per level |
| Random pivot sequence, expected | `Θ(n log n)` | every pivot rank is selected uniformly |
| Unlucky repeatedly extreme pivots | `Θ(n²)` | subproblem sizes can be `n-1, n-2, ...` |
| All keys equal | `Θ(n)` | one 3-way partition finishes the entire range |

Thus the traditional **best case for distinct keys** and the **expected case** are `Θ(n log n)`, while the worst case remains `Θ(n²)`. With duplicates, three-way partitioning can improve on the distinct-key best case; the global best case is the all-equal `Θ(n)` case.

The generate/write/read and final write/read phases are each `Θ(n)`. Therefore the complete pipeline is expected `Θ(n log n)` and worst-case `Θ(n²)`.

### Space

- Quick Sort rearranges the supplied array in place.
- It always recurses on the smaller strict partition and iterates over the larger one.
- A recursive child therefore contains at most half its parent's values, giving at most `⌊log₂ n⌋ + 1` live stack frames — **`O(log n)` even for a worst-case partition sequence**.
- The interactive file-validation program deliberately owns two `n`-element arrays so it can compare round trips, making total program memory `Θ(n)`.

## What the metrics mean

The transcript reports:

- **pivot comparisons** — each executed `< pivot` or `> pivot` test;
- **swaps** — exchanges of two different array positions;
- **partitions** — nontrivial ranges partitioned;
- **values partitioned** — sum of all partitioned range lengths;
- **maximum stack** — peak number of live Quick Sort frames and its proven bound.

Metrics do not include file parsing or validation work, so the experiment isolates the sorting phase.

## Deterministic validation and plot

File: [`q3_experimental_validation.c`](q3_experimental_validation.c)

For powers of two from `128` through `262144`, it performs five deterministic trials, then refuses to accept a run unless all of these hold:

- output is nondecreasing;
- before/after multiset fingerprints match;
- every output element exactly matches an independent copy sorted with library `qsort`;
- measured stack frames respect the logarithmic bound.

The library oracle is used **only by the validator**, never by the submitted Quick Sort solution. It separately checks already sorted, reverse sorted, all-equal, 17-value duplicate-pool, alternating `LLONG_MIN`/`LLONG_MAX`, empty, and singleton inputs. The committed console evidence is in [`q3_experiment_output.txt`](q3_experiment_output.txt), and the measurements are in [`q3_experimental_data.dat`](q3_experimental_data.dat).

File: [`q3_plot_complexity.c`](q3_plot_complexity.c)

<p align="center"><img src="q3_quicksort_complexity.svg" width="96%" alt="Measured Quick Sort comparisons and swaps against n log n"></p>

The SVG is generated entirely by the C program from the validated `.dat` file. Both axes use a base-2 logarithmic scale; the comparison and swap curves remain parallel to the `n log₂ n` reference.

```bash
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic \
    q3_experimental_validation.c -lm -o q3_experimental_validation
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic \
    q3_plot_complexity.c -lm -o q3_plot_complexity
./q3_experimental_validation
./q3_plot_complexity
```

## Input protection and edge cases

- rejects zero, out-of-range, negative, or malformed `N`;
- accepts every unsigned 64-bit seed, including `0` and `UINT64_MAX`;
- rejects identical input and output paths;
- detects missing, extra, malformed, or out-of-range file values;
- checks allocation, open, read, write, and close failures;
- supports spaces inside a file path;
- handles negative values, duplicates, one value, and signed extremes;
- prints only a bounded preview for large inputs;
- never depends on recursion depth proportional to `n`.

## Viva conclusion

> The program proves the complete file workflow, not only the in-memory sort. Seeded randomized pivots give expected `Θ(n log n)` time, three-way partitioning removes equal keys in one pass, and smaller-side recursion guarantees `O(log n)` stack space even when time reaches the `Θ(n²)` worst case.
