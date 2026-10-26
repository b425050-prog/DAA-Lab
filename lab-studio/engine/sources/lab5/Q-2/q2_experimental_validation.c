/*
 * DAA Lab 05 Q2 - deterministic correctness and linear-growth experiment.
 * qsort is used only on a separate test-oracle copy, never by the solution.
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q2_bfprt.h"

enum {
    TRIALS_PER_SIZE = 5,
    FUZZ_CASES = 2000,
    FUZZ_MAX_N = 97
};

static int compare_long_long(const void *left, const void *right) {
    long long a = *(const long long *)left;
    long long b = *(const long long *)right;
    return (a > b) - (a < b);
}

static int verify_every_rank(const long long *source, size_t n) {
    long long *oracle = (long long *)malloc(n * sizeof *oracle);
    long long *work = (long long *)malloc(n * sizeof *work);
    if (oracle == NULL || work == NULL) {
        free(oracle);
        free(work);
        return 0;
    }

    memcpy(oracle, source, n * sizeof *oracle);
    qsort(oracle, n, sizeof *oracle, compare_long_long);
    for (size_t k = 1U; k <= n; ++k) {
        memcpy(work, source, n * sizeof *work);
        Q2Metrics metrics = {0U, 0U, 0U, 0U, 0U, 0U};
        if (q2_kth_smallest(work, n, k, &metrics) != oracle[k - 1U]) {
            free(oracle);
            free(work);
            return 0;
        }
    }

    free(oracle);
    free(work);
    return 1;
}

static int run_edge_case_suite(void) {
    const long long singleton[] = {42LL};
    const long long signed_duplicates[] = {
        5LL, -3LL, LLONG_MAX, 5LL, 0LL, LLONG_MIN,
        -3LL, 12LL, 5LL, LLONG_MIN, LLONG_MAX
    };
    long long ascending[31];
    long long descending[32];
    long long all_equal[37];

    for (size_t i = 0U; i < 31U; ++i) {
        ascending[i] = (long long)i - 15LL;
    }
    for (size_t i = 0U; i < 32U; ++i) {
        descending[i] = 100LL - (long long)i * 7LL;
    }
    for (size_t i = 0U; i < 37U; ++i) {
        all_equal[i] = -9LL;
    }

    return verify_every_rank(singleton,
                             sizeof singleton / sizeof singleton[0]) &&
           verify_every_rank(signed_duplicates,
                             sizeof signed_duplicates /
                                 sizeof signed_duplicates[0]) &&
           verify_every_rank(ascending,
                             sizeof ascending / sizeof ascending[0]) &&
           verify_every_rank(descending,
                             sizeof descending / sizeof descending[0]) &&
           verify_every_rank(all_equal,
                             sizeof all_equal / sizeof all_equal[0]);
}

static uint64_t next_random(uint64_t *state) {
    uint64_t value = *state;
    value ^= value >> 12U;
    value ^= value << 25U;
    value ^= value >> 27U;
    *state = value;
    return value * UINT64_C(2685821657736338717);
}

static void make_deterministic_input(long long *values, size_t n,
                                     uint64_t seed) {
    uint64_t state = seed;
    for (size_t i = 0U; i < n; ++i) {
        uint64_t random_value = next_random(&state);
        values[i] = (long long)(random_value % UINT64_C(2000001)) - 1000000LL;
        if (i > 0U && i % 11U == 0U) {
            values[i] = values[i - 1U];
        }
    }
    values[0] = LLONG_MIN;
    values[n - 1U] = LLONG_MAX;
}

static int run_deterministic_fuzz_suite(void) {
    long long source[FUZZ_MAX_N];
    long long oracle[FUZZ_MAX_N];
    long long work[FUZZ_MAX_N];
    uint64_t state = UINT64_C(0x5E1EC7105EED1234);

    for (int test = 0; test < FUZZ_CASES; ++test) {
        size_t n = (size_t)(next_random(&state) % FUZZ_MAX_N) + 1U;
        for (size_t i = 0U; i < n; ++i) {
            /* The small range intentionally creates many duplicates. */
            source[i] =
                (long long)(next_random(&state) % UINT64_C(41)) - 20LL;
        }
        if (test % 31 == 0) {
            source[0] = LLONG_MIN;
        }
        if (test % 37 == 0) {
            source[n - 1U] = LLONG_MAX;
        }

        size_t k = (size_t)(next_random(&state) % (uint64_t)n) + 1U;
        memcpy(oracle, source, n * sizeof *oracle);
        memcpy(work, source, n * sizeof *work);
        qsort(oracle, n, sizeof *oracle, compare_long_long);

        Q2Metrics metrics = {0U, 0U, 0U, 0U, 0U, 0U};
        if (q2_kth_smallest(work, n, k, &metrics) != oracle[k - 1U]) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    if (!run_edge_case_suite()) {
        fprintf(stderr, "Edge-case validation failed.\n");
        return 2;
    }
    if (!run_deterministic_fuzz_suite()) {
        fprintf(stderr, "Deterministic fuzz validation failed.\n");
        return 2;
    }

    FILE *data = fopen("q2_experimental_data.dat", "w");
    if (data == NULL) {
        fprintf(stderr, "Could not create q2_experimental_data.dat.\n");
        return 1;
    }

    fprintf(data,
            "# n trials average_comparisons comparisons_per_n "
            "average_partitions max_depth linear_reference valid\n");
    printf("Edge-case suite: PASS (single, duplicates, signed extremes, "
           "ordered, reverse, all equal)\n\n");
    printf("Deterministic fuzz suite: PASS (%d duplicate-heavy arrays)\n\n",
           FUZZ_CASES);
    printf("%-8s %-7s %-16s %-12s %-12s %-10s\n", "n", "trials",
           "avg comparisons", "cmp / n", "avg parts", "validation");

    for (size_t n = 128U; n <= 131072U; n *= 2U) {
        long long *source = (long long *)malloc(n * sizeof *source);
        long long *oracle = (long long *)malloc(n * sizeof *oracle);
        long long *work = (long long *)malloc(n * sizeof *work);
        if (source == NULL || oracle == NULL || work == NULL) {
            fprintf(stderr, "Memory allocation failed for n=%zu.\n", n);
            free(source);
            free(oracle);
            free(work);
            fclose(data);
            return 1;
        }

        make_deterministic_input(source, n,
                                 UINT64_C(0xDAA50000) + (uint64_t)n);
        memcpy(oracle, source, n * sizeof *oracle);
        qsort(oracle, n, sizeof *oracle, compare_long_long);

        const size_t ranks[TRIALS_PER_SIZE] = {
            1U, n / 4U + 1U, n / 2U + 1U, (3U * n) / 4U + 1U, n
        };
        unsigned long long total_comparisons = 0U;
        unsigned long long total_partitions = 0U;
        size_t max_depth = 0U;
        int valid = 1;

        for (size_t trial = 0U; trial < TRIALS_PER_SIZE; ++trial) {
            size_t k = ranks[trial];
            memcpy(work, source, n * sizeof *work);
            Q2Metrics metrics = {0U, 0U, 0U, 0U, 0U, 0U};
            long long selected = q2_kth_smallest(work, n, k, &metrics);
            if (selected != oracle[k - 1U]) {
                valid = 0;
            }
            total_comparisons += metrics.comparisons;
            total_partitions += metrics.partitions;
            if (metrics.max_recursion_depth > max_depth) {
                max_depth = metrics.max_recursion_depth;
            }
        }

        double average_comparisons =
            (double)total_comparisons / (double)TRIALS_PER_SIZE;
        double comparisons_per_n = average_comparisons / (double)n;
        double average_partitions =
            (double)total_partitions / (double)TRIALS_PER_SIZE;
        double linear_reference = 10.0 * (double)n;

        fprintf(data, "%zu %d %.3f %.9f %.3f %zu %.3f %d\n", n,
                TRIALS_PER_SIZE, average_comparisons, comparisons_per_n,
                average_partitions, max_depth, linear_reference, valid);
        printf("%-8zu %-7d %-16.1f %-12.4f %-12.1f %-10s\n", n,
               TRIALS_PER_SIZE, average_comparisons, comparisons_per_n,
               average_partitions, valid ? "PASS" : "FAIL");

        free(source);
        free(oracle);
        free(work);
        if (!valid) {
            fclose(data);
            return 2;
        }
    }

    fclose(data);
    printf("\nAll deterministic checks passed.\n");
    printf("The comparisons/n ratio remains bounded, supporting Theta(n) growth.\n");
    printf("Data written to q2_experimental_data.dat\n");
    return 0;
}
