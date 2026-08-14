#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int index;
    unsigned long long probes;
} SearchResult;

static double elapsed_ms(clock_t start, clock_t end) {
    return 1000.0 * (double)(end - start) / (double)CLOCKS_PER_SEC;
}

static SearchResult binary_search_counted(const int a[], int n, int x) {
    int low = 0, high = n - 1;
    SearchResult result = {-1, 0};

    while (low <= high) {
        int mid = low + (high - low) / 2;
        ++result.probes;

        if (a[mid] == x) {
            result.index = mid;
            return result;
        }
        if (a[mid] < x)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return result;
}

static SearchResult ternary_search_counted(const int a[], int n, int x) {
    int low = 0, high = n - 1;
    SearchResult result = {-1, 0};

    while (low <= high) {
        int third = (high - low) / 3;
        int mid1 = low + third;
        int mid2 = high - third;

        ++result.probes;
        if (a[mid1] == x) {
            result.index = mid1;
            return result;
        }

        if (mid2 != mid1) {
            ++result.probes;
            if (a[mid2] == x) {
                result.index = mid2;
                return result;
            }
        }

        if (x < a[mid1]) {
            high = mid1 - 1;
        } else if (x > a[mid2]) {
            low = mid2 + 1;
        } else {
            low = mid1 + 1;
            high = mid2 - 1;
        }
    }
    return result;
}

static int is_sorted_non_decreasing(const int a[], int n) {
    for (int i = 1; i < n; ++i)
        if (a[i - 1] > a[i]) return 0;
    return 1;
}

int main(void) {
    int n, x;

    printf("Binary Search vs Ternary Search\n");
    printf("Enter number of elements: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Invalid size.\n");
        return 1;
    }

    int *a = (int *)malloc((size_t)n * sizeof(*a));
    if (!a) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    printf("Enter %d elements in sorted order:\n", n);
    for (int i = 0; i < n; ++i) {
        if (scanf("%d", &a[i]) != 1) {
            fprintf(stderr, "Invalid input.\n");
            free(a);
            return 1;
        }
    }

    if (!is_sorted_non_decreasing(a, n)) {
        fprintf(stderr, "The input list must be sorted in non-decreasing order.\n");
        free(a);
        return 1;
    }

    printf("Enter element to search: ");
    if (scanf("%d", &x) != 1) {
        fprintf(stderr, "Invalid target.\n");
        free(a);
        return 1;
    }

    clock_t b0 = clock();
    SearchResult binary = binary_search_counted(a, n, x);
    clock_t b1 = clock();

    clock_t t0 = clock();
    SearchResult ternary = ternary_search_counted(a, n, x);
    clock_t t1 = clock();

    printf("\n%-18s %-10s %-12s %-12s\n", "Method", "Index", "Probes", "CPU time(ms)");
    printf("%-18s %-10d %-12llu %-12.6f\n",
           "Binary search", binary.index, binary.probes, elapsed_ms(b0, b1));
    printf("%-18s %-10d %-12llu %-12.6f\n",
           "Ternary search", ternary.index, ternary.probes, elapsed_ms(t0, t1));

    int binary_ok = (binary.index == -1) ? 0 : (a[binary.index] == x);
    int ternary_ok = (ternary.index == -1) ? 0 : (a[ternary.index] == x);
    int same_outcome = ((binary.index == -1) && (ternary.index == -1)) ||
                       (binary_ok && ternary_ok);

    printf("\nValidation: both searches %s.\n",
           same_outcome ? "agree on whether the target exists" : "DISAGREE (unexpected)");
    printf("For comparison-based searching, probe count is a more stable metric than tiny CPU timings.\n");

    free(a);
    return 0;
}
