#ifndef LAB5_Q2_BFPRT_H
#define LAB5_Q2_BFPRT_H

#include <stddef.h>

/*
 * Deterministic linear-time selection (BFPRT / median of medians).
 *
 * The array is rearranged in place but is never fully sorted.  Groups of at
 * most five are insertion-sorted only to obtain their medians.  A three-way
 * partition makes duplicate values safe and efficient.
 */

typedef struct {
    unsigned long long comparisons;
    unsigned long long writes;
    unsigned long long swaps;
    unsigned long long group_sorts;
    unsigned long long partitions;
    size_t max_recursion_depth;
} Q2Metrics;

static void q2_swap(long long *a, size_t i, size_t j, Q2Metrics *metrics) {
    if (i == j) {
        return;
    }
    long long temporary = a[i];
    a[i] = a[j];
    a[j] = temporary;
    ++metrics->swaps;
    metrics->writes += 3U;
}

static void q2_insertion_sort(long long *a, size_t left, size_t right,
                              Q2Metrics *metrics) {
    ++metrics->group_sorts;
    for (size_t i = left + 1U; i < right; ++i) {
        long long key = a[i];
        size_t j = i;
        while (j > left) {
            ++metrics->comparisons;
            if (a[j - 1U] <= key) {
                break;
            }
            a[j] = a[j - 1U];
            ++metrics->writes;
            --j;
        }
        if (j != i) {
            a[j] = key;
            ++metrics->writes;
        }
    }
}

static long long q2_select_range(long long *a, size_t left, size_t right,
                                 size_t target, Q2Metrics *metrics,
                                 size_t depth);

static long long q2_choose_pivot(long long *a, size_t left, size_t right,
                                 Q2Metrics *metrics, size_t depth) {
    size_t median_count = 0U;

    for (size_t group_left = left; group_left < right; group_left += 5U) {
        size_t remaining = right - group_left;
        size_t group_right = group_left + (remaining < 5U ? remaining : 5U);
        q2_insertion_sort(a, group_left, group_right, metrics);

        size_t median = group_left + (group_right - group_left) / 2U;
        q2_swap(a, left + median_count, median, metrics);
        ++median_count;
    }

    /* Select the median of the compacted group medians recursively. */
    size_t median_target = left + median_count / 2U;
    return q2_select_range(a, left, left + median_count, median_target,
                           metrics, depth + 1U);
}

static long long q2_select_range(long long *a, size_t left, size_t right,
                                 size_t target, Q2Metrics *metrics,
                                 size_t depth) {
    if (depth > metrics->max_recursion_depth) {
        metrics->max_recursion_depth = depth;
    }

    for (;;) {
        size_t count = right - left;
        if (count <= 5U) {
            q2_insertion_sort(a, left, right, metrics);
            return a[target];
        }

        long long pivot = q2_choose_pivot(a, left, right, metrics, depth);
        ++metrics->partitions;

        /* Dutch-national-flag partition: [left,lt) < pivot,
           [lt,gt) == pivot, and [gt,right) > pivot. */
        size_t less = left;
        size_t scan = left;
        size_t greater = right;
        while (scan < greater) {
            ++metrics->comparisons;
            if (a[scan] < pivot) {
                q2_swap(a, less, scan, metrics);
                ++less;
                ++scan;
            } else {
                ++metrics->comparisons;
                if (a[scan] > pivot) {
                    --greater;
                    q2_swap(a, scan, greater, metrics);
                } else {
                    ++scan;
                }
            }
        }

        if (target < less) {
            right = less;
        } else if (target >= greater) {
            left = greater;
        } else {
            return pivot;
        }
    }
}

/* k is one-based and must satisfy 1 <= k <= n. */
static long long q2_kth_smallest(long long *work, size_t n, size_t k,
                                 Q2Metrics *metrics) {
    return q2_select_range(work, 0U, n, k - 1U, metrics, 1U);
}

#endif
