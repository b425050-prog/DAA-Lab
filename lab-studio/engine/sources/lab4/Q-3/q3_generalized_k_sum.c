/*
 * DAA Laboratory - Lab 04 - Question 3
 * Test whether k distinct elements of a set sum to T in
 * O(n^(k-1) log n) time for fixed k >= 2.
 *
 * Sort once, enumerate increasing-index choices for the first k-1 values,
 * and binary-search the required final value strictly to their right.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long sort_comparisons;
    unsigned long long recursive_states;
    unsigned long long candidate_prefixes;
    unsigned long long binary_comparisons;
} Metrics;

static void merge(long long *a, long long *temp, size_t lo, size_t mid,
                  size_t hi, Metrics *m) {
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        ++m->sort_comparisons;
        temp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    }
    while (i < mid) temp[k++] = a[i++];
    while (j < hi) temp[k++] = a[j++];
    for (i = lo; i < hi; ++i) a[i] = temp[i];
}

static void merge_sort_recursive(long long *a, long long *temp, size_t lo,
                                 size_t hi, Metrics *m) {
    if (hi - lo <= 1) return;
    size_t mid = lo + (hi - lo) / 2;
    merge_sort_recursive(a, temp, lo, mid, m);
    merge_sort_recursive(a, temp, mid, hi, m);
    merge(a, temp, lo, mid, hi, m);
}

static int safe_subtract(long long target, long long partial,
                         long long *result) {
    if (partial > 0 && target < LLONG_MIN + partial) return 0;
    if (partial < 0 && target > LLONG_MAX + partial) return 0;
    *result = target - partial;
    return 1;
}

static int binary_search_suffix(const long long *a, size_t lo, size_t hi,
                                long long key, size_t *position, Metrics *m) {
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        ++m->binary_comparisons;
        if (a[mid] == key) {
            *position = mid;
            return 1;
        }
        if (a[mid] < key) lo = mid + 1;
        else hi = mid;
    }
    return 0;
}

static int choose_prefix(const long long *a, size_t n, int k, int depth,
                         size_t start, long long partial_sum, long long target,
                         size_t *chosen, Metrics *m) {
    ++m->recursive_states;
    if (depth == k - 1) {
        long long needed;
        size_t position = 0;
        ++m->candidate_prefixes;
        if (!safe_subtract(target, partial_sum, &needed)) return 0;
        if (binary_search_suffix(a, start, n, needed, &position, m)) {
            chosen[depth] = position;
            return 1;
        }
        return 0;
    }

    size_t values_still_needed = (size_t)(k - depth);
    if (n - start < values_still_needed) return 0;
    size_t last_allowed = n - values_still_needed;
    for (size_t i = start; i <= last_allowed; ++i) {
        /* The program validates beforehand that every k-term sum fits. */
        chosen[depth] = i;
        if (choose_prefix(a, n, k, depth + 1, i + 1,
                          partial_sum + a[i], target, chosen, m)) {
            return 1;
        }
    }
    return 0;
}

static unsigned long long magnitude(long long value) {
    if (value >= 0) return (unsigned long long)value;
    return (unsigned long long)(-(value + 1)) + 1ULL;
}

static int all_k_sums_fit(const long long *a, size_t n, int k) {
    unsigned long long largest = 0;
    for (size_t i = 0; i < n; ++i) {
        unsigned long long current = magnitude(a[i]);
        if (current > largest) largest = current;
    }
    return largest <= (unsigned long long)LLONG_MAX / (unsigned long long)k;
}

int main(void) {
    size_t n;
    int k;
    long long target;
    printf("============================================================\n");
    printf(" DAA Lab 04 - Q3: Generalized k-Sum\n");
    printf("============================================================\n");
    printf("Enter set size n and k (2 <= k <= n): ");
    if (scanf("%zu %d", &n, &k) != 2 || n == 0 || n > 100000U ||
        k < 2 || (size_t)k > n) {
        fprintf(stderr, "Invalid n or k.\n");
        return 1;
    }

    long long *set = (long long *)malloc(n * sizeof *set);
    long long *temp = (long long *)malloc(n * sizeof *temp);
    size_t *chosen = (size_t *)malloc((size_t)k * sizeof *chosen);
    if (!set || !temp || !chosen) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(set); free(temp); free(chosen);
        return 1;
    }

    printf("Enter %zu distinct integers of S:\n", n);
    for (size_t i = 0; i < n; ++i) {
        if (scanf("%lld", &set[i]) != 1) return 1;
    }
    printf("Enter target T: ");
    if (scanf("%lld", &target) != 1) return 1;

    if (!all_k_sums_fit(set, n, k)) {
        fprintf(stderr,
                "Input rejected: magnitudes are too large for safe %d-term signed 64-bit sums.\n",
                k);
        free(set); free(temp); free(chosen);
        return 1;
    }

    Metrics metrics = {0, 0, 0, 0};
    merge_sort_recursive(set, temp, 0, n, &metrics);
    for (size_t i = 1; i < n; ++i) {
        if (set[i - 1] == set[i]) {
            fprintf(stderr, "Precondition failed: S must be a set (no duplicates).\n");
            free(set); free(temp); free(chosen);
            return 1;
        }
    }

    int found = choose_prefix(set, n, k, 0, 0, 0, target, chosen, &metrics);
    printf("\nSorted S: ");
    for (size_t i = 0; i < n; ++i) printf("%s%lld", i ? " " : "", set[i]);
    putchar('\n');
    if (found) {
        long long sum = 0;
        printf("Solution: ");
        for (int i = 0; i < k; ++i) {
            printf("%s%lld", i ? " + " : "", set[chosen[i]]);
            sum += set[chosen[i]];
        }
        printf(" = %lld\n", sum);
    } else {
        printf("Solution: NO %d distinct elements sum to %lld\n", k, target);
    }
    printf("Candidate (k-1)-prefixes : %llu\n", metrics.candidate_prefixes);
    printf("Binary comparisons       : %llu\n", metrics.binary_comparisons);
    printf("Sort comparisons         : %llu\n", metrics.sort_comparisons);
    printf("Complexity               : O(n^(k-1) log n) for fixed k\n");
    printf("Distinctness             : increasing indices + suffix-only search\n");

    free(set); free(temp); free(chosen);
    return 0;
}
