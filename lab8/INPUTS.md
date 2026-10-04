# Lab 08 input guide

[← Lab dashboard](README.md)

All programs read standard input and print human-readable answers. Add `--json` for state traces. Invalid input or unrepresentable exact arithmetic exits with code 2 and a diagnostic. Extra non-whitespace input is rejected.

## Q1 · Minimum coin change

`c V`, followed by `c` distinct positive coin values.

`0 <= c <= 256`, `0 <= V <= 200000`; denominations fit a positive C `int`.

The empty target needs zero coins. An unreachable positive target returns **-1**. Duplicate denominations are rejected.

## Q2 · Count coin combinations

`c V`, followed by `c` distinct positive coin values.

Same bounds as Q1. Counts use `uint64_t`.

There is one way to make zero: choose no coins. Checked addition rejects overflow rather than printing a wrapped count.

## Q3 · Longest common subsequence

Two complete lines: sequence X, then sequence Y.

Printable ASCII, up to 2000 characters per line; a blank line represents an empty string.

Whitespace within each line is part of the sequence. Ties prefer the upper DP cell, so repeated runs reconstruct the same LCS.

## Q4 · Longest increasing subsequence

`n`, followed by `n` signed integer array elements.

`0 <= n <= 10000`; values fit `int64_t`.

Strictly increasing means `<`, never `<=`. Empty input has length zero. The implementation deliberately uses quadratic DP for this lab.

## Q5 · Maximum sum increasing subsequence

`n`, followed by `n` positive array elements.

`0 <= n <= 10000`; values and sums fit `uint64_t`.

Zero and negative values are rejected as the handout specifies positive integers. Any overflowing candidate sum produces an explicit error.

## Q6 · Edit distance & traceback

Two complete lines: original A, then desired B.

Printable ASCII, up to 2000 characters per line; empty strings use blank lines.

Each operation costs one. Output is in forward order. M=match, S=substitute, D=delete, I=insert. Operation codes distinguish a real hyphen from a displayed gap.

## Q7 · Rod cutting & reconstruction

`n`, followed by prices `p1 ... pn`.

`0 <= n <= 5000`; each signed price satisfies `abs(price) <= INT64_MAX/n` for n>0.

Prices may be negative: the entire rod must still be sold. Every reconstruction consumes exactly n inches; the bound prevents signed addition overflow.

## Q8 · Optimal binary search trees

`n`, then `n` sorted keys, `n` successful-search probabilities p, and `n+1` failed-search probabilities q.

`0 <= n <= 200`; distinct increasing `int64_t` keys; finite nonnegative probabilities summing to 1 within 1e-8.

Dummy leaves are included in the expected cost: `sum p_i(depth(k_i)+1) + sum q_i(depth(d_i)+1)`. For a convention that counts only key comparisons on failed searches, subtract `sum q_i` from this reported cost. All-zero or unnormalized probabilities are rejected.

## Q9 · Collatz trajectories

Four unsigned decimal values: `start a b step_cap`.

`start >= 1`, `1 <= a <= b`; at most 10000 interval starts; `1 <= step_cap <= 1000000`; values fit `uint64_t`.

Each trajectory ends with `reached_1`, `overflow`, or `step_limit`. A capped/overflowing path is not reported as a counterexample. Intervals are inclusive. The champion considers completed paths only; ties prefer the first start.

