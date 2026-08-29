/* DAA Lab 05 Q3 - deterministic correctness and growth validation. */

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q3_quicksort.h"

enum { TRIALS = 5 };

static int compare_long_long(const void *left, const void *right) {
    long long a = *(const long long *)left;
    long long b = *(const long long *)right;
    return (a > b) - (a < b);
}

static int validate_case(long long *values, long long *oracle, size_t n,
                         uint64_t seed, Q3Metrics *metrics) {
    Q3Fingerprint before = q3_fingerprint(values, n);
    if (n > 0) {
        memcpy(oracle, values, n * sizeof *oracle);
        qsort(oracle, n, sizeof *oracle, compare_long_long);
    }
    q3_quicksort(values, n, seed, metrics);
    Q3Fingerprint after = q3_fingerprint(values, n);
    int exact_oracle_match = n == 0 ||
                             memcmp(values, oracle, n * sizeof *values) == 0;
    return q3_is_sorted(values, n) && exact_oracle_match &&
           q3_same_fingerprint(before, after) &&
           metrics->max_stack_frames <= q3_stack_frame_bound(n);
}

static void fill_adversarial(long long *values, size_t n, int pattern) {
    for (size_t i = 0; i < n; ++i) {
        if (pattern == 0) {
            values[i] = (long long)i;
        } else if (pattern == 1) {
            values[i] = (long long)(n - i);
        } else if (pattern == 2) {
            values[i] = 7;
        } else if (pattern == 3) {
            values[i] = (long long)(i % 17U) - 8;
        } else {
            values[i] = i % 2U == 0 ? LLONG_MIN : LLONG_MAX;
        }
    }
}

int main(void) {
    FILE *data = fopen("q3_experimental_data.dat", "w");
    if (!data) {
        perror("q3_experimental_data.dat");
        return 1;
    }
    fprintf(data, "# n avg_comparisons avg_swaps avg_partitions n_log2_n "
                  "comparison_ratio max_stack valid_sorted_fingerprint_stack_qsort_oracle\n");
    printf("%-9s %-16s %-14s %-13s %-10s %-8s\n", "n", "avg comparisons",
           "avg swaps", "avg parts", "stack", "check");

    for (size_t n = 128; n <= 262144; n *= 2) {
        long long *values = (long long *)malloc(n * sizeof *values);
        long long *oracle = (long long *)malloc(n * sizeof *oracle);
        if (!values || !oracle) {
            free(values);
            free(oracle);
            fclose(data);
            return 1;
        }
        uint64_t comparisons = 0;
        uint64_t swaps = 0;
        uint64_t partitions = 0;
        size_t max_stack = 0;
        int all_valid = 1;
        for (unsigned trial = 0; trial < TRIALS; ++trial) {
            uint64_t seed = UINT64_C(20260829) + UINT64_C(104729) * trial;
            q3_generate_values(values, n, seed);
            Q3Metrics metrics;
            int ok = validate_case(values, oracle, n, seed, &metrics);
            comparisons += metrics.comparisons;
            swaps += metrics.swaps;
            partitions += metrics.partitions;
            if (metrics.max_stack_frames > max_stack) {
                max_stack = metrics.max_stack_frames;
            }
            if (!ok) all_valid = 0;
        }

        double average_comparisons = comparisons / (double)TRIALS;
        double average_swaps = swaps / (double)TRIALS;
        double average_partitions = partitions / (double)TRIALS;
        double reference = (double)n * log2((double)n);
        fprintf(data, "%zu %.3f %.3f %.3f %.3f %.9f %zu %d\n", n,
                average_comparisons, average_swaps, average_partitions,
                reference, average_comparisons / reference, max_stack,
                all_valid);
        printf("%-9zu %-16.1f %-14.1f %-13.1f %-10zu %-8s\n", n,
               average_comparisons, average_swaps, average_partitions,
               max_stack, all_valid ? "PASS" : "FAIL");
        free(values);
        free(oracle);
        if (!all_valid) {
            fclose(data);
            return 2;
        }
    }
    if (fclose(data) != 0) return 1;

    printf("\nAdversarial and duplicate-rich validation (n = 65536)\n");
    printf("%-22s %-16s %-14s %-10s %-8s\n", "case", "comparisons", "swaps",
           "stack", "check");
    const char *names[] = {"already sorted", "reverse sorted", "all equal",
                           "17-value duplicate pool", "alternating limits"};
    const size_t adversarial_n = 65536;
    long long *values = (long long *)malloc(adversarial_n * sizeof *values);
    long long *oracle = (long long *)malloc(adversarial_n * sizeof *oracle);
    if (!values || !oracle) {
        free(values);
        free(oracle);
        return 1;
    }
    for (int pattern = 0; pattern < 5; ++pattern) {
        fill_adversarial(values, adversarial_n, pattern);
        Q3Metrics metrics;
        int ok = validate_case(values, oracle, adversarial_n,
                               UINT64_C(0x5eed0000) + (uint64_t)pattern,
                               &metrics);
        printf("%-22s %-16" PRIu64 " %-14" PRIu64 " %-10zu %-8s\n",
               names[pattern], metrics.comparisons, metrics.swaps,
               metrics.max_stack_frames, ok ? "PASS" : "FAIL");
        if (!ok) {
            free(values);
            free(oracle);
            return 2;
        }
    }
    free(values);
    free(oracle);

    long long singleton = LLONG_MIN;
    long long singleton_oracle = 0;
    Q3Metrics empty_metrics;
    Q3Metrics singleton_metrics;
    int small_ok = validate_case(NULL, NULL, 0, 1, &empty_metrics) &&
                   validate_case(&singleton, &singleton_oracle, 1, 2,
                                 &singleton_metrics);
    printf("%-22s %-16s %-14s %-10s %-8s\n", "empty + singleton", "0", "0",
           "0 / 1", small_ok ? "PASS" : "FAIL");
    if (!small_ok) return 2;

    printf("\nAll rows exactly match an independent qsort oracle and pass\n");
    printf("sortedness, multiset-fingerprint, and stack-bound checks.\n");
    printf("Five deterministic trials per n were written to q3_experimental_data.dat.\n");
    return 0;
}
