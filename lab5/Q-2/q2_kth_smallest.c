/*
 * DAA Laboratory - Lab 05 - Question 2
 * Find the k-th smallest value without sorting the complete list.
 *
 * Deterministic BFPRT (median of medians) guarantees Theta(n) worst-case
 * time.  The user's list is preserved; selection rearranges a working copy.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q2_bfprt.h"

enum { MAX_INPUT_SIZE = 1000000 };

static void print_array(const long long *values, size_t n) {
    for (size_t i = 0U; i < n; ++i) {
        printf("%s%lld", i == 0U ? "" : " ", values[i]);
    }
    putchar('\n');
}

static int read_values(long long *values, size_t n) {
    for (size_t i = 0U; i < n; ++i) {
        if (scanf("%lld", &values[i]) != 1) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    size_t n = 0U;
    printf("============================================================\n");
    printf(" DAA Lab 05 - Q2: K-th Smallest without Sorting\n");
    printf("============================================================\n");
    printf("Enter the number of elements n: ");

    if (scanf("%zu", &n) != 1 || n == 0U || n > MAX_INPUT_SIZE ||
        n > SIZE_MAX / sizeof(long long)) {
        fprintf(stderr, "Invalid n. Enter a value from 1 to %d.\n",
                MAX_INPUT_SIZE);
        return 1;
    }

    long long *original = (long long *)malloc(n * sizeof *original);
    long long *work = (long long *)malloc(n * sizeof *work);
    if (original == NULL || work == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(original);
        free(work);
        return 1;
    }

    printf("Enter %zu integers (duplicates and negative values are allowed):\n",
           n);
    if (!read_values(original, n)) {
        fprintf(stderr, "Invalid list input.\n");
        free(original);
        free(work);
        return 1;
    }

    size_t k = 0U;
    printf("Enter k (1 = smallest, %zu = largest): ", n);
    if (scanf("%zu", &k) != 1 || k == 0U || k > n) {
        fprintf(stderr, "Invalid k. Enter a value from 1 to %zu.\n", n);
        free(original);
        free(work);
        return 1;
    }

    memcpy(work, original, n * sizeof *work);
    Q2Metrics metrics = {0U, 0U, 0U, 0U, 0U, 0U};
    long long answer = q2_kth_smallest(work, n, k, &metrics);

    printf("\nOriginal list (preserved) : ");
    print_array(original, n);
    printf("Partitioned working copy  : ");
    print_array(work, n);
    printf("Requested rank (1-based)  : %zu\n", k);
    printf("K-th smallest element     : %lld\n", answer);
    printf("Value comparisons         : %llu\n", metrics.comparisons);
    printf("Three-way partitions      : %llu\n", metrics.partitions);
    printf("Groups of at most 5 sorted: %llu\n", metrics.group_sorts);
    printf("Maximum recursion depth   : %zu\n", metrics.max_recursion_depth);
    printf("Method                    : deterministic median-of-medians (BFPRT)\n");
    printf("Complexity                : Theta(n) worst-case time; O(log n) stack\n");
    printf("Input preservation        : O(n) working copy\n");

    free(original);
    free(work);
    return 0;
}
