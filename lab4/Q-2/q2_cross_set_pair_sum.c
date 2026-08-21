/*
 * DAA Laboratory - Lab 04 - Question 2
 * Find a in S1 and b in S2 such that a + b = x in O(n log n).
 *
 * S2 is merge-sorted (worst-case O(n log n)); for every value in S1,
 * the required complement is searched in S2 by binary search.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long long sort_comparisons;
    unsigned long long sort_writes;
    unsigned long long binary_comparisons;
    unsigned long long s1_values_examined;
} Metrics;

static void merge(long long *a, long long *temp, size_t left, size_t middle,
                  size_t right, Metrics *m) {
    size_t i = left, j = middle, k = left;
    while (i < middle && j < right) {
        ++m->sort_comparisons;
        temp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
        ++m->sort_writes;
    }
    while (i < middle) {
        temp[k++] = a[i++];
        ++m->sort_writes;
    }
    while (j < right) {
        temp[k++] = a[j++];
        ++m->sort_writes;
    }
    for (i = left; i < right; ++i) {
        a[i] = temp[i];
        ++m->sort_writes;
    }
}

static void merge_sort_recursive(long long *a, long long *temp, size_t left,
                                 size_t right, Metrics *m) {
    if (right - left <= 1) return;
    size_t middle = left + (right - left) / 2;
    merge_sort_recursive(a, temp, left, middle, m);
    merge_sort_recursive(a, temp, middle, right, m);
    merge(a, temp, left, middle, right, m);
}

static int merge_sort(long long *a, size_t n, Metrics *m) {
    long long *temp = (long long *)malloc(n * sizeof *temp);
    if (!temp) return 0;
    merge_sort_recursive(a, temp, 0, n, m);
    free(temp);
    return 1;
}

static int safe_complement(long long target, long long value,
                           long long *complement) {
    if (value > 0 && target < LLONG_MIN + value) return 0;
    if (value < 0 && target > LLONG_MAX + value) return 0;
    *complement = target - value;
    return 1;
}

static int binary_search_value(const long long *a, size_t n, long long key,
                               size_t *position, Metrics *m) {
    size_t left = 0, right = n;
    while (left < right) {
        size_t middle = left + (right - left) / 2;
        ++m->binary_comparisons;
        if (a[middle] == key) {
            *position = middle;
            return 1;
        }
        if (a[middle] < key) left = middle + 1;
        else right = middle;
    }
    return 0;
}

static int find_cross_set_pair(const long long *s1, const long long *sorted_s2,
                               size_t n, long long target, long long *first,
                               long long *second, Metrics *m) {
    for (size_t i = 0; i < n; ++i) {
        long long complement;
        ++m->s1_values_examined;
        if (!safe_complement(target, s1[i], &complement)) continue;
        size_t position = 0;
        if (binary_search_value(sorted_s2, n, complement, &position, m)) {
            *first = s1[i];
            *second = sorted_s2[position];
            return 1;
        }
    }
    return 0;
}

static void print_array(const long long *a, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        printf("%s%lld", i == 0 ? "" : " ", a[i]);
    }
    putchar('\n');
}

int main(void) {
    size_t n;
    printf("============================================================\n");
    printf(" DAA Lab 04 - Q2: Pair Sum Across Two Sets\n");
    printf("============================================================\n");
    printf("Enter the common set size n: ");
    if (scanf("%zu", &n) != 1 || n == 0 || n > 1000000U) {
        fprintf(stderr, "Invalid n.\n");
        return 1;
    }

    long long *s1 = (long long *)malloc(n * sizeof *s1);
    long long *s2 = (long long *)malloc(n * sizeof *s2);
    long long *sorted_s2 = (long long *)malloc(n * sizeof *sorted_s2);
    if (!s1 || !s2 || !sorted_s2) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(s1); free(s2); free(sorted_s2);
        return 1;
    }

    printf("Enter %zu elements of S1:\n", n);
    for (size_t i = 0; i < n; ++i) {
        if (scanf("%lld", &s1[i]) != 1) return 1;
    }
    printf("Enter %zu elements of S2:\n", n);
    for (size_t i = 0; i < n; ++i) {
        if (scanf("%lld", &s2[i]) != 1) return 1;
    }
    long long target;
    printf("Enter target x: ");
    if (scanf("%lld", &target) != 1) return 1;

    memcpy(sorted_s2, s2, n * sizeof *sorted_s2);
    Metrics metrics = {0, 0, 0, 0};
    if (!merge_sort(sorted_s2, n, &metrics)) {
        fprintf(stderr, "Memory allocation failed during merge sort.\n");
        free(s1); free(s2); free(sorted_s2);
        return 1;
    }

    long long first = 0, second = 0;
    int found = find_cross_set_pair(s1, sorted_s2, n, target,
                                    &first, &second, &metrics);

    printf("\nSorted copy of S2: ");
    print_array(sorted_s2, n);
    if (found) {
        printf("Pair found          : %lld (from S1) + %lld (from S2) = %lld\n",
               first, second, target);
    } else {
        printf("Pair found          : NO\n");
    }
    printf("S1 values examined  : %llu\n", metrics.s1_values_examined);
    printf("Sort comparisons    : %llu\n", metrics.sort_comparisons);
    printf("Binary comparisons  : %llu\n", metrics.binary_comparisons);
    printf("Complexity          : O(n log n) worst-case time, O(n) extra space\n");
    printf("Method              : merge-sort S2, then binary-search x-a for each a in S1\n");

    free(s1); free(s2); free(sorted_s2);
    return 0;
}
