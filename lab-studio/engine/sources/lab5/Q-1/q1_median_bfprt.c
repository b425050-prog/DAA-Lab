/*
 * DAA Laboratory - Lab 05 - Question 1
 * Find the median without sorting the complete list.
 *
 * This implementation uses deterministic BFPRT (median of medians) selection
 * and a duplicate-safe three-way partition.  Its worst-case running time is
 * Theta(n), not merely expected Theta(n).
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long array_writes;
    unsigned long long partitions;
} Metrics;

typedef struct {
    size_t equal_begin;
    size_t equal_end; /* Half-open: [equal_begin, equal_end). */
} EqualRange;

static void swap_values(long long *left, long long *right, Metrics *metrics) {
    if (left == right || *left == *right) return;
    long long temporary = *left;
    *left = *right;
    *right = temporary;
    metrics->array_writes += 2U;
}

/* Only a group of at most five elements is sorted at a time. */
static void insertion_sort_group(long long values[], size_t left, size_t right,
                                 Metrics *metrics) {
    for (size_t i = left + 1U; i <= right; ++i) {
        long long key = values[i];
        size_t j = i;
        while (j > left) {
            ++metrics->comparisons;
            if (values[j - 1U] <= key) break;
            values[j] = values[j - 1U];
            ++metrics->array_writes;
            --j;
        }
        if (j != i) {
            values[j] = key;
            ++metrics->array_writes;
        }
    }
}

static long long bfprt_select(long long values[], size_t left, size_t right,
                              size_t target, Metrics *metrics);

static long long deterministic_pivot(long long values[], size_t left,
                                     size_t right, Metrics *metrics) {
    size_t median_count = 0U;

    for (size_t group_left = left; group_left <= right; group_left += 5U) {
        size_t remaining = right - group_left + 1U;
        size_t group_right = group_left + (remaining < 5U ? remaining : 5U) - 1U;
        insertion_sort_group(values, group_left, group_right, metrics);

        size_t group_median = group_left + (group_right - group_left) / 2U;
        swap_values(&values[left + median_count], &values[group_median], metrics);
        ++median_count;
    }

    size_t medians_target = left + median_count / 2U;
    return bfprt_select(values, left, left + median_count - 1U,
                        medians_target, metrics);
}

static EqualRange partition_three_way(long long values[], size_t left,
                                      size_t right, long long pivot,
                                      Metrics *metrics) {
    size_t less_end = left;
    size_t scan = left;
    size_t greater_begin = right + 1U;

    ++metrics->partitions;
    while (scan < greater_begin) {
        ++metrics->comparisons;
        if (values[scan] < pivot) {
            swap_values(&values[less_end], &values[scan], metrics);
            ++less_end;
            ++scan;
        } else {
            ++metrics->comparisons;
            if (values[scan] > pivot) {
                --greater_begin;
                swap_values(&values[scan], &values[greater_begin], metrics);
            } else {
                ++scan;
            }
        }
    }

    EqualRange result = {less_end, greater_begin};
    return result;
}

/* target is an absolute, zero-based index in values[left..right]. */
static long long bfprt_select(long long values[], size_t left, size_t right,
                              size_t target, Metrics *metrics) {
    for (;;) {
        size_t length = right - left + 1U;
        if (length <= 5U) {
            insertion_sort_group(values, left, right, metrics);
            return values[target];
        }

        long long pivot = deterministic_pivot(values, left, right, metrics);
        EqualRange equal = partition_three_way(values, left, right, pivot, metrics);

        if (target < equal.equal_begin) {
            right = equal.equal_begin - 1U;
        } else if (target >= equal.equal_end) {
            left = equal.equal_end;
        } else {
            return pivot;
        }
    }
}

static int rank_is_valid(const long long values[], size_t n, long long selected,
                         size_t target) {
    size_t smaller = 0U;
    size_t equal = 0U;
    for (size_t i = 0U; i < n; ++i) {
        if (values[i] < selected) {
            ++smaller;
        } else if (values[i] == selected) {
            ++equal;
        }
    }
    return smaller <= target && target < smaller + equal;
}

/*
 * Print (lower + upper) / 2 exactly, without ever forming the possibly
 * overflowing sum and without losing precision by converting to double.
 */
static void print_exact_average(long long lower, long long upper) {
    long long floor_half_sum = lower / 2LL + upper / 2LL;
    int remainder = (int)(lower % 2LL + upper % 2LL);

    if (remainder < 0) {
        --floor_half_sum;
        remainder += 2;
    } else if (remainder >= 2) {
        ++floor_half_sum;
        remainder -= 2;
    }

    if (remainder == 0) {
        printf("%lld", floor_half_sum);
    } else if (floor_half_sum >= 0) {
        printf("%lld.5", floor_half_sum);
    } else if (floor_half_sum == -1LL) {
        printf("-0.5");
    } else {
        printf("%lld.5", floor_half_sum + 1LL);
    }
}

int main(void) {
    size_t n;

    printf("============================================================\n");
    printf(" DAA Lab 05 - Q1: Median Without Sorting (BFPRT)\n");
    printf("============================================================\n");
    printf("Enter the number of elements: ");
    if (scanf("%zu", &n) != 1 || n == 0U || n > 1000000U ||
        n > SIZE_MAX / sizeof(long long)) {
        fprintf(stderr, "Invalid n. Enter a value from 1 to 1000000.\n");
        return 1;
    }

    long long *values = (long long *)malloc(n * sizeof *values);
    if (!values) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    printf("Enter %zu integer%s: ", n, n == 1U ? "" : "s");
    for (size_t i = 0U; i < n; ++i) {
        if (scanf("%lld", &values[i]) != 1) {
            fprintf(stderr, "Invalid integer at position %zu.\n", i + 1U);
            free(values);
            return 1;
        }
    }

    if (n <= 24U) {
        printf("\nOriginal list : [");
        for (size_t i = 0U; i < n; ++i) {
            printf("%s%lld", i == 0U ? "" : ", ", values[i]);
        }
        printf("]\n");
    }

    size_t lower_rank = (n - 1U) / 2U;
    size_t upper_rank = n / 2U;
    Metrics metrics = {0U, 0U, 0U};
    long long lower = bfprt_select(values, 0U, n - 1U, lower_rank, &metrics);
    long long upper = lower;
    if (upper_rank != lower_rank) {
        upper = bfprt_select(values, 0U, n - 1U, upper_rank, &metrics);
    }

    int valid = rank_is_valid(values, n, lower, lower_rank) &&
                rank_is_valid(values, n, upper, upper_rank) && lower <= upper;

    if (lower_rank == upper_rank) {
        printf("Middle order statistic (rank %zu): %lld\n", lower_rank + 1U,
               lower);
    } else {
        printf("Lower middle (rank %zu): %lld\n", lower_rank + 1U, lower);
        printf("Upper middle (rank %zu): %lld\n", upper_rank + 1U, upper);
    }
    printf("Median          : ");
    print_exact_average(lower, upper);
    printf("\n\nMethod          : deterministic median-of-medians selection\n");
    printf("Full-array sort : not performed (only groups of at most five are sorted)\n");
    printf("Comparisons     : %llu\n", metrics.comparisons);
    printf("Array writes    : %llu\n", metrics.array_writes);
    printf("Partitions      : %llu\n", metrics.partitions);
    printf("Rank validation : %s\n", valid ? "PASS" : "FAIL");
    printf("Complexity      : Theta(n) worst-case time, O(log n) stack space\n");

    free(values);
    return valid ? 0 : 2;
}
