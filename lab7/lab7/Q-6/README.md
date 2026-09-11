# Q6 · The Best Time to Be Alive

[← Lab 07 dashboard](../README.md) · [Problem sheet](../Problem-Sheet-Lab-07.pdf) · [Input conventions](../INPUTS.md)

## Files

| File | Purpose |
|---|---|
| [q6_best_time_alive.c](q6_best_time_alive.c) | C17 solution: death-before-birth event sweep |
| [q6_sample_input.txt](q6_sample_input.txt) | Ready-to-run input |
| [q6_sample_output.txt](q6_sample_output.txt) | Captured program output |
| [../tools/test_all.py](../tools/test_all.py) | Independent validators for all seven questions |

## Build and run

From this question folder:

```bash
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q6_best_time_alive.c -o q6
./q6 < q6_sample_input.txt
```

In Windows PowerShell:

```powershell
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q6_best_time_alive.c -o q6.exe
Get-Content q6_sample_input.txt | .\q6.exe
```

Input: **`N`, then `N` rows of `name birth death`**. Keep the lab's `common/` folder beside the `Q-*` folders.

## Sample input

```text
5
Ada 1815 1852
Albert 1879 1955
Galileo 1564 1642
Isaac 1642 1727
Marie 1867 1934
```

<details>
<summary>Sample output</summary>

```text
Maximum scientists alive: 2
Best interval: [1879, 1934)
```

</details>

## Algorithm and analysis
**Answer.** Sort lifetime events by year and sweep to find maximum overlap. Alphabetical order in the original index is irrelevant to chronology.

Represent each nonempty lifetime by `[birth, death)`. Add event `(birth,+1)` and `(death,-1)`. At equal years sort deaths before births, as required by the question. Merge sort gives a guaranteed `O(N log N)` bound.

Process all events at each distinct year `y`. After processing that group, the running count is the number alive throughout `[y,next_event_year)`. Record those counts, find the maximum, then print all adjacent peak segments as merged intervals. This reports all tied best periods, including the earliest one.

Example:

```text
5
Ada 1815 1852
Albert 1879 1955
Galileo 1564 1642
Isaac 1642 1727
Marie 1867 1934
```

The maximum is **2**, during **`[1879,1934)`**. Galileo's death and Isaac's birth in 1642 do not create an overlap. Names are illustrative input labels; the program uses only the provided years.

### Correctness

Before the first event no interval is active. Each birth adds exactly one newly active interval and each death removes exactly one ending interval. By induction, after each event-year group the running count equals the number of intervals containing that year. No membership changes between event years, so checking every such segment considers every possible count. Taking the maximum and emitting all segments attaining it solves the problem.

Deaths before births ensure an ending scientist never overlaps a starting scientist in the same year. Grouping is safe for the maximum because deaths can only decrease the count and births can only increase it within that year; a spurious intermediate peak cannot occur. Equal-endpoint lifetimes are empty under the chosen half-open convention and are skipped.

**Complexity.** `O(N log N)` time and `O(N)` auxiliary space for events, merge workspace, and year counts. The output contains at most `O(N)` intervals. Years need not fall in a small range.
