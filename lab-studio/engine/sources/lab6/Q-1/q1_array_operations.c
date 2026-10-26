#include <stdio.h>
#include <stdlib.h>

#include "q1_algorithms.h"

static void print_values(const long long *a, size_t n) {
    for (size_t i = 0U; i < n; ++i) printf("%s%lld", i ? " " : "", a[i]);
    putchar('\n');
}

int main(void) {
    size_t n = 0U, pivot = 0U;
    printf("DAA Lab 06 - Q1: 1D Array Operations\n");
    printf("Enter n: ");
    if (scanf("%zu", &n) != 1 || n == 0U || n > 1000000U) return 1;
    long long *a = malloc(n * sizeof *a), *sorted = malloc(n * sizeof *a);
    long long *unique = malloc(n * sizeof *a), *work = malloc(n * sizeof *a);
    if (!a || !sorted || !unique || !work) {
        free(a); free(sorted); free(unique); free(work); return 1;
    }
    printf("Enter %zu integers: ", n);
    for (size_t i = 0U; i < n; ++i) if (scanf("%lld", &a[i]) != 1) return 1;
    printf("Choose pivot position [1..%zu]: ", n);
    if (scanf("%zu", &pivot) != 1 || pivot == 0U || pivot > n) return 1;

    Q1Summary s;
    size_t unique_count = 0U;
    if (!q1_analyse(a, n, &s, sorted, unique, &unique_count)) return 1;
    printf("\nMaximum                  : %lld\n", s.maximum);
    printf("Largest / second distinct: %lld / ", s.largest);
    if (s.has_second) printf("%lld\n", s.second_largest); else printf("not present\n");
    printf("Mean                     : %.6f\n", s.mean);
    printf("Median                   : %.6f\n", s.median);
    printf("Population std. deviation: %.6f\n", s.standard_deviation);
    printf("Mode                     : %lld (frequency %zu)\n", s.mode, s.mode_frequency);
    printf("Sorted unique values     : "); print_values(unique, unique_count);
    q1_reverse(a, n, work);
    printf("Reversed                 : "); print_values(work, n);
    size_t boundary = q1_partition_ge_first(a, n, pivot - 1U, work);
    printf("Partitioned (>= %lld | < %lld): ", a[pivot - 1U], a[pivot - 1U]);
    print_values(work, n);
    printf("Partition boundary       : %zu\n", boundary);
    printf("Dominant counted work    : %llu\n", s.operations);
    printf("Worst-case summary       : scans Theta(n); sort-backed median/mode/dedup Theta(n log n); reverse/partition Theta(n)\n");
    free(a); free(sorted); free(unique); free(work);
    return 0;
}
