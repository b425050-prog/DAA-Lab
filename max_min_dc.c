#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int min_value;
    int max_value;
} MinMax;

static unsigned long long comparisons = 0;

/* Divide-and-conquer minimum on a candidate list. */
static int dc_min(const int a[], int left, int right) {
    if (left == right) return a[left];

    int mid = left + (right - left) / 2;
    int x = dc_min(a, left, mid);
    int y = dc_min(a, mid + 1, right);
    ++comparisons;
    return x < y ? x : y;
}

/* Divide-and-conquer maximum on a candidate list. */
static int dc_max(const int a[], int left, int right) {
    if (left == right) return a[left];

    int mid = left + (right - left) / 2;
    int x = dc_max(a, left, mid);
    int y = dc_max(a, mid + 1, right);
    ++comparisons;
    return x > y ? x : y;
}

/*
   Pair elements first. One comparison per pair tells us which element can
   still be a minimum and which can still be a maximum. We then use D&C
   separately on those two candidate sets.
*/
static MinMax find_min_max(const int a[], int n) {
    if (n == 1) {
        MinMax only = {a[0], a[0]};
        return only;
    }

    int pairs = n / 2;
    int *low = (int *)malloc((size_t)pairs * sizeof(*low));
    int *high = (int *)malloc((size_t)pairs * sizeof(*high));
    if (!low || !high) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(low);
        free(high);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < pairs; ++i) {
        int x = a[2 * i];
        int y = a[2 * i + 1];
        ++comparisons;
        if (x < y) {
            low[i] = x;
            high[i] = y;
        } else {
            low[i] = y;
            high[i] = x;
        }
    }

    MinMax result;
    result.min_value = dc_min(low, 0, pairs - 1);
    result.max_value = dc_max(high, 0, pairs - 1);

    if (n % 2 == 1) {
        int leftover = a[n - 1];
        ++comparisons;
        if (leftover < result.min_value) result.min_value = leftover;
        ++comparisons;
        if (leftover > result.max_value) result.max_value = leftover;
    }

    free(low);
    free(high);
    return result;
}

int main(void) {
    int n;
    printf("Maximum and Minimum using Divide and Conquer\n");
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

    comparisons = 0;
    MinMax ans = find_min_max(a, n);

    printf("\nMinimum = %d\n", ans.min_value);
    printf("Maximum = %d\n", ans.max_value);
    printf("Comparisons = %llu\n", comparisons);
    printf("Required upper bound 3n/2 = %.1f\n", 1.5 * (double)n);

    if ((double)comparisons <= 1.5 * (double)n)
        printf("Validation: comparison bound satisfied.\n");
    else
        printf("Validation: bound NOT satisfied (unexpected).\n");

    free(a);
    return 0;
}
