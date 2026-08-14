#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long swaps;
} Metrics;

static void selection_sort(int a[], int n, Metrics *m) {
    m->comparisons = 0;
    m->swaps = 0;

    for (int i = 0; i < n - 1; ++i) {
        int min_index = i;

        /* Loop invariant before this iteration:
           a[0..i-1] already contains the i smallest elements in sorted order. */
        for (int j = i + 1; j < n; ++j) {
            ++m->comparisons;
            if (a[j] < a[min_index])
                min_index = j;
        }

        if (min_index != i) {
            int temp = a[i];
            a[i] = a[min_index];
            a[min_index] = temp;
            ++m->swaps;
        }
    }
}

static int is_sorted(const int a[], int n) {
    for (int i = 1; i < n; ++i)
        if (a[i - 1] > a[i]) return 0;
    return 1;
}

int main(void) {
    int n;
    printf("Selection Sort and Loop-Invariant Validation\n");
    printf("Enter array size: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Invalid size.\n");
        return 1;
    }

    int *a = (int *)malloc((size_t)n * sizeof(*a));
    if (!a) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; ++i) {
        if (scanf("%d", &a[i]) != 1) {
            fprintf(stderr, "Invalid input.\n");
            free(a);
            return 1;
        }
    }

    Metrics m;
    selection_sort(a, n, &m);

    printf("\nSorted array:\n");
    for (int i = 0; i < n; ++i)
        printf("%d%c", a[i], i + 1 == n ? '\n' : ' ');

    unsigned long long expected = (unsigned long long)n * (unsigned long long)(n - 1) / 2ULL;
    printf("Comparisons = %llu\n", m.comparisons);
    printf("Expected n(n-1)/2 = %llu\n", expected);
    printf("Swaps = %llu\n", m.swaps);
    printf("Correctness check: %s\n", is_sorted(a, n) ? "PASSED" : "FAILED");
    printf("Best-case and worst-case running time are both Theta(n^2), because the comparison count is input-independent.\n");

    free(a);
    return 0;
}
