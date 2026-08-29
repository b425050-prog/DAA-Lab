/* DAA Lab 05 Q4 - deterministic heap-sort growth and correctness checks. */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long long build_comparisons;
    unsigned long long sort_comparisons;
    unsigned long long swaps;
} Metrics;

static uint64_t mix64(uint64_t value) {
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static uint64_t next_random(uint64_t *state) {
    *state += UINT64_C(0x9e3779b97f4a7c15);
    return mix64(*state);
}

static void swap_values(long long *a, long long *b, Metrics *metrics) {
    long long temporary = *a;
    *a = *b;
    *b = temporary;
    ++metrics->swaps;
}

static void sift_down(long long *values, size_t heap_size, size_t root,
                      Metrics *metrics, int building) {
    while (root < heap_size / 2U) {
        size_t child = 2U * root + 1U;
        if (child + 1U < heap_size) {
            if (building) ++metrics->build_comparisons;
            else ++metrics->sort_comparisons;
            if (values[child] < values[child + 1U]) ++child;
        }
        if (building) ++metrics->build_comparisons;
        else ++metrics->sort_comparisons;
        if (values[root] >= values[child]) break;
        swap_values(&values[root], &values[child], metrics);
        root = child;
    }
}

static int is_max_heap(const long long *values, size_t n) {
    for (size_t child = 1; child < n; ++child) {
        if (values[(child - 1U) / 2U] < values[child]) return 0;
    }
    return 1;
}

static int heap_sort(long long *values, size_t n, Metrics *metrics) {
    for (size_t start = n / 2U; start > 0U; --start) {
        sift_down(values, n, start - 1U, metrics, 1);
    }
    int heap_valid = is_max_heap(values, n);
    for (size_t end = n; end > 1U; --end) {
        swap_values(&values[0], &values[end - 1U], metrics);
        sift_down(values, end - 1U, 0U, metrics, 0);
    }
    return heap_valid;
}

static int compare_long_long(const void *left, const void *right) {
    long long a = *(const long long *)left;
    long long b = *(const long long *)right;
    return (a > b) - (a < b);
}

static int verify_case(const long long *input, size_t n) {
    long long *actual = n ? (long long *)malloc(n * sizeof *actual) : NULL;
    long long *expected = n ? (long long *)malloc(n * sizeof *expected) : NULL;
    if (n && (!actual || !expected)) {
        free(actual);
        free(expected);
        return 0;
    }
    if (n) {
        memcpy(actual, input, n * sizeof *actual);
        memcpy(expected, input, n * sizeof *expected);
    }
    Metrics metrics = {0, 0, 0};
    int heap_valid = heap_sort(actual, n, &metrics);
    qsort(expected, n, sizeof *expected, compare_long_long);
    int ok = heap_valid && (!n || memcmp(actual, expected, n * sizeof *actual) == 0);
    free(actual);
    free(expected);
    return ok;
}

int main(void) {
    static const long long one[] = {42};
    static const long long duplicates[] = {5, 5, 5, -2, 5, -2, 0, 0};
    static const long long ascending[] = {-8, -3, 0, 4, 9, 12};
    static const long long descending[] = {12, 9, 4, 0, -3, -8};
    if (!verify_case(NULL, 0) || !verify_case(one, 1) ||
        !verify_case(duplicates, sizeof duplicates / sizeof duplicates[0]) ||
        !verify_case(ascending, sizeof ascending / sizeof ascending[0]) ||
        !verify_case(descending, sizeof descending / sizeof descending[0])) {
        fputs("A fixed edge-case check failed.\n", stderr);
        return 2;
    }

    FILE *data = fopen("q4_experimental_data.dat", "w");
    if (!data) return 1;
    fprintf(data, "# n build_cmp extraction_cmp total_cmp swaps nlog2n ratio exact_valid\n");
    printf("%-9s %-12s %-15s %-13s %-12s %-9s\n",
           "n", "build cmp", "extraction cmp", "total cmp", "ratio", "check");

    for (size_t n = 128U; n <= 131072U; n *= 2U) {
        long long *actual = (long long *)malloc(n * sizeof *actual);
        long long *expected = (long long *)malloc(n * sizeof *expected);
        if (!actual || !expected) {
            free(actual);
            free(expected);
            fclose(data);
            return 1;
        }

        uint64_t state = UINT64_C(0x48454150534f5254) + (uint64_t)n;
        for (size_t i = 0; i < n; ++i) {
            actual[i] = (long long)(next_random(&state) % UINT64_C(200001)) - 100000LL;
        }
        memcpy(expected, actual, n * sizeof *expected);
        qsort(expected, n, sizeof *expected, compare_long_long);

        Metrics metrics = {0, 0, 0};
        int heap_valid = heap_sort(actual, n, &metrics);
        int exact = heap_valid && memcmp(actual, expected, n * sizeof *actual) == 0;
        unsigned long long total = metrics.build_comparisons + metrics.sort_comparisons;
        double reference = (double)n * log2((double)n);
        double ratio = (double)total / reference;

        fprintf(data, "%zu %llu %llu %llu %llu %.3f %.9f %d\n",
                n, metrics.build_comparisons, metrics.sort_comparisons,
                total, metrics.swaps, reference, ratio, exact);
        printf("%-9zu %-12llu %-15llu %-13llu %-12.6f %-9s\n",
               n, metrics.build_comparisons, metrics.sort_comparisons,
               total, ratio, exact ? "PASS" : "FAIL");

        free(actual);
        free(expected);
        if (!exact) {
            fclose(data);
            return 2;
        }
    }

    fclose(data);
    printf("\nEdge cases: PASS (empty, singleton, duplicates, ascending, descending)\n");
    printf("Every measured output matched an independent qsort result exactly.\n");
    printf("Data written to q4_experimental_data.dat\n");
    return 0;
}
