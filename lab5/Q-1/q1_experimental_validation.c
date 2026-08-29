/*
 * DAA Lab 05 Q1 - deterministic correctness and growth validation for BFPRT.
 * qsort is used only by the independent test oracle, never by the solution.
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long writes;
    unsigned long long partitions;
} Metrics;

typedef struct {
    size_t begin;
    size_t end;
} EqualRange;

static void swap_values(long long *a, long long *b, Metrics *m) {
    if (a == b || *a == *b) return;
    long long temporary = *a;
    *a = *b;
    *b = temporary;
    m->writes += 2U;
}

static void sort_group(long long values[], size_t left, size_t right,
                       Metrics *m) {
    for (size_t i = left + 1U; i <= right; ++i) {
        long long key = values[i];
        size_t j = i;
        while (j > left) {
            ++m->comparisons;
            if (values[j - 1U] <= key) break;
            values[j] = values[j - 1U];
            ++m->writes;
            --j;
        }
        if (j != i) {
            values[j] = key;
            ++m->writes;
        }
    }
}

static long long select_bfprt(long long values[], size_t left, size_t right,
                              size_t target, Metrics *m);

static long long choose_pivot(long long values[], size_t left, size_t right,
                              Metrics *m) {
    size_t medians = 0U;
    for (size_t start = left; start <= right; start += 5U) {
        size_t remaining = right - start + 1U;
        size_t finish = start + (remaining < 5U ? remaining : 5U) - 1U;
        sort_group(values, start, finish, m);
        size_t median = start + (finish - start) / 2U;
        swap_values(&values[left + medians], &values[median], m);
        ++medians;
    }
    return select_bfprt(values, left, left + medians - 1U,
                        left + medians / 2U, m);
}

static EqualRange partition3(long long values[], size_t left, size_t right,
                             long long pivot, Metrics *m) {
    size_t less = left;
    size_t scan = left;
    size_t greater = right + 1U;
    ++m->partitions;

    while (scan < greater) {
        ++m->comparisons;
        if (values[scan] < pivot) {
            swap_values(&values[less], &values[scan], m);
            ++less;
            ++scan;
        } else {
            ++m->comparisons;
            if (values[scan] > pivot) {
                --greater;
                swap_values(&values[scan], &values[greater], m);
            } else {
                ++scan;
            }
        }
    }
    EqualRange result = {less, greater};
    return result;
}

static long long select_bfprt(long long values[], size_t left, size_t right,
                              size_t target, Metrics *m) {
    for (;;) {
        if (right - left + 1U <= 5U) {
            sort_group(values, left, right, m);
            return values[target];
        }
        long long pivot = choose_pivot(values, left, right, m);
        EqualRange equal = partition3(values, left, right, pivot, m);
        if (target < equal.begin) {
            right = equal.begin - 1U;
        } else if (target >= equal.end) {
            left = equal.end;
        } else {
            return pivot;
        }
    }
}

static int compare_long_long(const void *left, const void *right) {
    long long a = *(const long long *)left;
    long long b = *(const long long *)right;
    return (a > b) - (a < b);
}

static int validate_case(const long long input[], size_t n, Metrics *result) {
    long long *working = (long long *)malloc(n * sizeof *working);
    long long *oracle = (long long *)malloc(n * sizeof *oracle);
    if (!working || !oracle) {
        free(working);
        free(oracle);
        return -1;
    }
    memcpy(working, input, n * sizeof *working);
    memcpy(oracle, input, n * sizeof *oracle);
    qsort(oracle, n, sizeof *oracle, compare_long_long);

    size_t lower_rank = (n - 1U) / 2U;
    size_t upper_rank = n / 2U;
    Metrics m = {0U, 0U, 0U};
    long long lower = select_bfprt(working, 0U, n - 1U, lower_rank, &m);
    long long upper = lower;
    if (lower_rank != upper_rank) {
        upper = select_bfprt(working, 0U, n - 1U, upper_rank, &m);
    }
    int valid = lower == oracle[lower_rank] && upper == oracle[upper_rank];
    *result = m;
    free(working);
    free(oracle);
    return valid;
}

static uint64_t next_random(uint64_t *state) {
    *state = *state * UINT64_C(6364136223846793005) +
             UINT64_C(1442695040888963407);
    return *state;
}

static void fill_deterministic(long long values[], size_t n) {
    uint64_t state = UINT64_C(0x5E1EC7A11AB50001) ^ (uint64_t)n;
    for (size_t i = 0U; i < n; ++i) {
        /* A deliberately small range creates many duplicate pivot values. */
        values[i] = (long long)(next_random(&state) % UINT64_C(2001)) - 1000LL;
    }
    if (n >= 2U) {
        values[n / 3U] = LLONG_MIN;
        values[(2U * n) / 3U] = LLONG_MAX;
    }
}

static int run_targeted_tests(size_t *passed) {
    static const long long single[] = {LLONG_MIN};
    static const long long two_extremes[] = {LLONG_MAX, LLONG_MIN};
    static const long long duplicates[] = {7, 7, 7, 7, 7, 7, 7, 7};
    static const long long odd_mixed[] = {9, -4, 2, 2, LLONG_MAX, 0, LLONG_MIN};
    static const long long even_negative_half[] = {4, -1, 0, -9};
    static const long long ordered[] = {-8, -3, 0, 1, 4, 9, 12, 18, 23};
    static const long long reversed[] = {23, 18, 12, 9, 4, 1, 0, -3, -8};
    const long long *cases[] = {
        single, two_extremes, duplicates, odd_mixed,
        even_negative_half, ordered, reversed
    };
    const size_t lengths[] = {
        sizeof single / sizeof single[0],
        sizeof two_extremes / sizeof two_extremes[0],
        sizeof duplicates / sizeof duplicates[0],
        sizeof odd_mixed / sizeof odd_mixed[0],
        sizeof even_negative_half / sizeof even_negative_half[0],
        sizeof ordered / sizeof ordered[0],
        sizeof reversed / sizeof reversed[0]
    };

    *passed = 0U;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) {
        Metrics ignored;
        int status = validate_case(cases[i], lengths[i], &ignored);
        if (status != 1) return status;
        ++*passed;
    }
    return 1;
}

static int run_exhaustive_ternary_tests(size_t *passed) {
    long long values[9];
    *passed = 0U;
    size_t combinations = 1U;

    for (size_t n = 1U; n <= 9U; ++n) {
        combinations *= 3U;
        for (size_t code = 0U; code < combinations; ++code) {
            size_t digits = code;
            for (size_t i = 0U; i < n; ++i) {
                values[i] = (long long)(digits % 3U) - 1LL;
                digits /= 3U;
            }
            Metrics ignored;
            int status = validate_case(values, n, &ignored);
            if (status != 1) return status;
            ++*passed;
        }
    }
    return 1;
}

int main(void) {
    size_t targeted_passed;
    size_t exhaustive_passed;
    int targeted_status = run_targeted_tests(&targeted_passed);
    int exhaustive_status = run_exhaustive_ternary_tests(&exhaustive_passed);
    if (targeted_status != 1 || exhaustive_status != 1) {
        fprintf(stderr, "Correctness validation failed or memory was exhausted.\n");
        return targeted_status < 0 || exhaustive_status < 0 ? 1 : 2;
    }

    FILE *data = fopen("q1_experimental_data.dat", "w");
    if (!data) {
        perror("q1_experimental_data.dat");
        return 1;
    }

    static const size_t sizes[] = {
        128U, 256U, 512U, 1024U, 2048U, 4096U,
        8192U, 16384U, 32768U, 65536U, 131072U
    };

    fprintf(data, "# n selections comparisons array_writes partitions "
                  "dominant_operations reference_40n operations_per_n valid\n");
    printf("DAA Lab 05 Q1 - deterministic BFPRT validation\n");
    printf("============================================================\n");
    printf("Targeted edge cases             : %zu/%zu PASS\n",
           targeted_passed, targeted_passed);
    printf("Exhaustive {-1,0,1}, lengths 1-9: %zu/%zu PASS\n\n",
           exhaustive_passed, exhaustive_passed);
    printf("%-9s %-12s %-12s %-12s %-12s %-10s\n",
           "n", "comparisons", "writes", "operations", "ops/n", "oracle");

    for (size_t row = 0U; row < sizeof sizes / sizeof sizes[0]; ++row) {
        size_t n = sizes[row];
        long long *values = (long long *)malloc(n * sizeof *values);
        if (!values) {
            fclose(data);
            return 1;
        }
        fill_deterministic(values, n);
        Metrics metrics;
        int valid = validate_case(values, n, &metrics);
        free(values);
        if (valid != 1) {
            fclose(data);
            fprintf(stderr, "Scaling validation failed at n = %zu.\n", n);
            return valid < 0 ? 1 : 2;
        }

        unsigned long long operations = metrics.comparisons + metrics.writes;
        unsigned long long reference = 40ULL * (unsigned long long)n;
        double operations_per_n = (double)operations / (double)n;
        fprintf(data, "%zu 2 %llu %llu %llu %llu %llu %.6f 1\n",
                n, metrics.comparisons, metrics.writes, metrics.partitions,
                operations, reference, operations_per_n);
        printf("%-9zu %-12llu %-12llu %-12llu %-12.3f %-10s\n",
               n, metrics.comparisons, metrics.writes, operations,
               operations_per_n, "PASS");
    }

    fclose(data);
    printf("\nAll %zu correctness cases passed.\n",
           targeted_passed + exhaustive_passed + sizeof sizes / sizeof sizes[0]);
    printf("Each scaling row uses two selections because n is even.\n");
    printf("Data written to q1_experimental_data.dat\n");
    return 0;
}
