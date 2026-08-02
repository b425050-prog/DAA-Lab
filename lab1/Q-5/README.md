# Q-5 · Find the Partition Point

[← Lab 01](../README.md) · [Main repository](../../README.md)

## Method

The array contains a run of 0s followed by a run of 1s, so it is monotonic. Binary search is used to find the first index containing `1`.

1. Start with the half-open search interval `[low, high) = [0, n)`.
2. Check the middle element.
3. When the middle element is `0`, continue in the right half.
4. When the middle element is `1`, keep that position as a possible answer and continue in the left half.
5. When `low = high`, `low` is the first index containing `1`.

The program uses zero-based indexing and also checks that the input really contains only 0s followed by 1s.

## Complexity

- Time: `Θ(log₂ n)`
- Extra space: `Θ(1)`

## Example

```text
Enter the number of elements: 8
Enter 8 elements containing 0s followed by 1s:
0 0 0 0 1 1 1 1
The transition occurs between indices 3 and 4.
First index containing 1: 4
```

## Files

| File | Purpose |
|---|---|
| `q5_partition_point.c` | Binary-search solution |
| `q5_partition_point.exe` | Compiled executable |

A graph is not included because Question 5 asks only for a method to find the transition point.

## Run

```bash
./q5_partition_point.exe
```
