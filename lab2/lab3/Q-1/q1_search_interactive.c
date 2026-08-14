/*
 * DAA Laboratory - Lab 03 - Question 1
 * Binary Search vs Ternary Search
 *
 * The program runs both searches on the same sorted array and counts:
 *   1. array probes      - how many candidate positions are inspected,
 *   2. key comparisons  - equality/order comparisons involving the target,
 *   3. recursive calls.
 *
 * Both algorithms are O(log n), but ternary search can inspect two
 * positions per level. In the comparison model this larger per-level
 * constant makes binary search preferable for an in-memory sorted array.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long probes;
    unsigned long long comparisons;
    unsigned long long calls;
} Metrics;

static int binary_search_rec(const int a[], int left, int right, int x, Metrics *m) {
    m->calls++;
    if (left > right) return -1;

    int mid = left + (right - left) / 2;
    m->probes++;

    m->comparisons++;
    if (a[mid] == x) return mid;

    m->comparisons++;
    if (x < a[mid])
        return binary_search_rec(a, left, mid - 1, x, m);
    return binary_search_rec(a, mid + 1, right, x, m);
}

static int ternary_search_rec(const int a[], int left, int right, int x, Metrics *m) {
    m->calls++;
    if (left > right) return -1;

    int third = (right - left) / 3;
    int mid1 = left + third;
    int mid2 = right - third;

    m->probes++;
    m->comparisons++;
    if (a[mid1] == x) return mid1;

    if (mid2 != mid1) {
        m->probes++;
        m->comparisons++;
        if (a[mid2] == x) return mid2;
    }

    m->comparisons++;
    if (x < a[mid1])
        return ternary_search_rec(a, left, mid1 - 1, x, m);

    m->comparisons++;
    if (x > a[mid2])
        return ternary_search_rec(a, mid2 + 1, right, x, m);

    return ternary_search_rec(a, mid1 + 1, mid2 - 1, x, m);
}

static int is_sorted(const int a[], int n) {
    for (int i = 1; i < n; ++i)
        if (a[i - 1] > a[i]) return 0;
    return 1;
}

static void print_metrics(const char *name, int index, const Metrics *m) {
    printf("%-16s | %-10s | %-8llu | %-15llu | %-8llu\n",
           name,
           index >= 0 ? "FOUND" : "NOT FOUND",
           m->probes,
           m->comparisons,
           m->calls);
}

int main(void) {
    int n, x;

    printf("============================================================\n");
    printf(" DAA Lab 03 - Q1: Binary Search vs Ternary Search\n");
    printf("============================================================\n");
    printf("Enter number of elements: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid size.\n");
        return 1;
    }

    int *a = (int *)malloc((size_t)n * sizeof(int));
    if (!a) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("Enter %d elements in sorted order:\n", n);
    for (int i = 0; i < n; ++i) {
        if (scanf("%d", &a[i]) != 1) {
            free(a);
            return 1;
        }
    }

    if (!is_sorted(a, n)) {
        printf("Input rejected: binary/ternary search requires a sorted array.\n");
        free(a);
        return 1;
    }

    printf("Enter element to search: ");
    if (scanf("%d", &x) != 1) {
        free(a);
        return 1;
    }

    Metrics binary = {0, 0, 0};
    Metrics ternary = {0, 0, 0};
    int bi = binary_search_rec(a, 0, n - 1, x, &binary);
    int ti = ternary_search_rec(a, 0, n - 1, x, &ternary);

    printf("\nResult index (0-based): binary = %d, ternary = %d\n", bi, ti);
    printf("\nAlgorithm        | Status     | Probes   | Key comparisons | Calls\n");
    printf("-----------------+------------+----------+-----------------+---------\n");
    print_metrics("Binary search", bi, &binary);
    print_metrics("Ternary search", ti, &ternary);

    printf("\nAsymptotic analysis:\n");
    printf("  Binary : T(n) = T(n/2) + O(1) = O(log_2 n)\n");
    printf("  Ternary: T(n) = T(n/3) + O(1) = O(log_3 n)\n");
    printf("\nWhy binary is normally better here:\n");
    printf("  A binary-search level probes one middle element.\n");
    printf("  A ternary-search level may probe two middle elements.\n");
    printf("  Worst-case probe constants are approximately\n");
    printf("      (1/ln 2) ln n = 1.4427 ln n     [binary]\n");
    printf("      (2/ln 3) ln n = 1.8205 ln n     [ternary]\n");
    printf("  Therefore both are logarithmic, but binary uses fewer array probes\n");
    printf("  in the standard comparison model for sorted arrays.\n");

    free(a);
    return 0;
}
