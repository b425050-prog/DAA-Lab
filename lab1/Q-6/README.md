# Q-6 · Element Uniqueness

[← Lab 01](../README.md) · [Main repository](../../README.md)

## Method

The program generates `n` random integers and compares every element with all elements after it. It immediately reports a duplicate when two equal values are found. If every pair is checked without a match, all elements are unique.

## Complexity and conclusion

When no duplicate is present, the number of comparisons is:

<p align="center"><strong>n(n − 1)/2</strong></p>

- Worst-case time: `Θ(n²)`
- Extra space used by the checking method: `Θ(1)`

For sufficiently large `n`, this pairwise method becomes slow because the number of comparisons grows quadratically. It may finish earlier when a duplicate is found.

## Files

| File | Purpose |
|---|---|
| `q6_element_uniqueness.c` | Random-number generation and pairwise duplicate test |
| `q6_element_uniqueness.exe` | Compiled executable |

A graph is not included because Question 6 asks for the method and its conclusion, not for a plot.

## Run

```bash
./q6_element_uniqueness.exe
```
