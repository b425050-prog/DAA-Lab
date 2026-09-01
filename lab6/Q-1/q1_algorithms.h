#ifndef LAB6_Q1_ALGORITHMS_H
#define LAB6_Q1_ALGORITHMS_H

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long long maximum;
    long long largest;
    long long second_largest;
    int has_second;
    double mean;
    double median;
    double standard_deviation;
    long long mode;
    size_t mode_frequency;
    unsigned long long operations;
} Q1Summary;

static void q1_merge(long long *a, long long *tmp, size_t left, size_t mid,
                     size_t right, unsigned long long *ops) {
    size_t i = left, j = mid, k = left;
    while (i < mid && j < right) {
        ++*ops;
        tmp[k++] = a[i] <= a[j] ? a[i++] : a[j++];
    }
    while (i < mid) tmp[k++] = a[i++];
    while (j < right) tmp[k++] = a[j++];
    for (i = left; i < right; ++i) a[i] = tmp[i];
    *ops += (unsigned long long)(right - left);
}

static void q1_merge_sort_rec(long long *a, long long *tmp, size_t left,
                              size_t right, unsigned long long *ops) {
    if (right - left < 2U) return;
    size_t mid = left + (right - left) / 2U;
    q1_merge_sort_rec(a, tmp, left, mid, ops);
    q1_merge_sort_rec(a, tmp, mid, right, ops);
    q1_merge(a, tmp, left, mid, right, ops);
}

static int q1_sorted_copy(const long long *a, size_t n, long long *sorted,
                          unsigned long long *ops) {
    long long *tmp = (long long *)malloc(n * sizeof *tmp);
    if (tmp == NULL) return 0;
    memcpy(sorted, a, n * sizeof *sorted);
    q1_merge_sort_rec(sorted, tmp, 0U, n, ops);
    free(tmp);
    return 1;
}

static int q1_analyse(const long long *a, size_t n, Q1Summary *result,
                      long long *sorted, long long *unique,
                      size_t *unique_count) {
    if (a == NULL || n == 0U || result == NULL || sorted == NULL ||
        unique == NULL || unique_count == NULL) return 0;
    result->operations = 0U;
    result->maximum = a[0];
    result->largest = a[0];
    result->has_second = 0;
    result->second_largest = 0;
    long double sum = 0.0L;
    for (size_t i = 0U; i < n; ++i) {
        ++result->operations;
        if (a[i] > result->maximum) result->maximum = a[i];
        sum += (long double)a[i];
        if (a[i] > result->largest) {
            result->second_largest = result->largest;
            result->largest = a[i];
            result->has_second = 1;
        } else if (a[i] < result->largest &&
                   (!result->has_second || a[i] > result->second_largest)) {
            result->second_largest = a[i];
            result->has_second = 1;
        }
    }
    result->mean = (double)(sum / (long double)n);
    long double variance_sum = 0.0L;
    for (size_t i = 0U; i < n; ++i) {
        long double d = (long double)a[i] - (long double)result->mean;
        variance_sum += d * d;
        ++result->operations;
    }
    result->standard_deviation = sqrt((double)(variance_sum / (long double)n));

    if (!q1_sorted_copy(a, n, sorted, &result->operations)) return 0;
    result->median = n % 2U != 0U
        ? (double)sorted[n / 2U]
        : (double)(((long double)sorted[n / 2U - 1U] +
                    (long double)sorted[n / 2U]) / 2.0L);

    result->mode = sorted[0];
    result->mode_frequency = 1U;
    size_t run = 1U;
    *unique_count = 0U;
    unique[(*unique_count)++] = sorted[0];
    for (size_t i = 1U; i < n; ++i) {
        ++result->operations;
        if (sorted[i] == sorted[i - 1U]) {
            ++run;
        } else {
            run = 1U;
            unique[(*unique_count)++] = sorted[i];
        }
        if (run > result->mode_frequency) {
            result->mode_frequency = run;
            result->mode = sorted[i];
        }
    }
    return 1;
}

static void q1_reverse(const long long *a, size_t n, long long *out) {
    for (size_t i = 0U; i < n; ++i) out[i] = a[n - 1U - i];
}

static size_t q1_partition_ge_first(const long long *a, size_t n,
                                    size_t pivot_index, long long *out) {
    long long pivot = a[pivot_index];
    size_t write = 0U;
    for (size_t i = 0U; i < n; ++i) if (a[i] >= pivot) out[write++] = a[i];
    size_t boundary = write;
    for (size_t i = 0U; i < n; ++i) if (a[i] < pivot) out[write++] = a[i];
    return boundary;
}

#endif
