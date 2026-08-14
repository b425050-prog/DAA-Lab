/*
 * DAA Laboratory - Lab 03 - Question 3
 * Maximum and Minimum using a Divide & Conquer tournament.
 *
 * Step 1 (divide into pairs): compare each pair once. Put the smaller
 * element in a MIN-candidate array and the larger in a MAX-candidate array.
 * If n is odd, the unpaired element is a candidate for both.
 *
 * Step 2 (conquer recursively): find the minimum of the MIN candidates
 * and the maximum of the MAX candidates using binary divide-and-conquer.
 *
 * Comparisons:
 *   even n: n/2 + (n/2-1) + (n/2-1) = 3n/2 - 2
 *   odd  n: (n-1)/2 + (n-1)/2 + (n-1)/2 = 3(n-1)/2
 * Hence the number of comparisons is always <= 3n/2.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int minimum;
    int maximum;
} MinMax;

typedef struct {
    unsigned long long comparisons;
    unsigned long long recursive_calls;
} Metrics;

static int dc_min(const int a[], int left, int right, Metrics *m) {
    m->recursive_calls++;
    if (left == right) return a[left];
    int mid = left + (right-left)/2;
    int x = dc_min(a,left,mid,m);
    int y = dc_min(a,mid+1,right,m);
    m->comparisons++;
    return x < y ? x : y;
}

static int dc_max(const int a[], int left, int right, Metrics *m) {
    m->recursive_calls++;
    if (left == right) return a[left];
    int mid = left + (right-left)/2;
    int x = dc_max(a,left,mid,m);
    int y = dc_max(a,mid+1,right,m);
    m->comparisons++;
    return x > y ? x : y;
}

static MinMax find_min_max(const int a[], int n, Metrics *m) {
    int candidate_count = (n + 1) / 2;
    int *mins = (int *)malloc((size_t)candidate_count * sizeof(int));
    int *maxs = (int *)malloc((size_t)candidate_count * sizeof(int));
    if (!mins || !maxs) exit(2);

    int k = 0;
    for (int i = 0; i + 1 < n; i += 2) {
        m->comparisons++;
        if (a[i] < a[i+1]) {
            mins[k] = a[i];
            maxs[k] = a[i+1];
        } else {
            mins[k] = a[i+1];
            maxs[k] = a[i];
        }
        k++;
    }
    if (n % 2) {
        mins[k] = maxs[k] = a[n-1];
        k++;
    }

    MinMax ans;
    ans.minimum = dc_min(mins, 0, k-1, m);
    ans.maximum = dc_max(maxs, 0, k-1, m);
    free(mins);
    free(maxs);
    return ans;
}

int main(void) {
    int n;
    printf("============================================================\n");
    printf(" DAA Lab 03 - Q3: Maximum and Minimum using D&C\n");
    printf("============================================================\n");
    printf("Enter array size: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid size.\n");
        return 1;
    }

    int *a = (int *)malloc((size_t)n * sizeof(int));
    if (!a) return 1;
    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; ++i) {
        if (scanf("%d", &a[i]) != 1) {
            free(a);
            return 1;
        }
    }

    Metrics m = {0, 0};
    MinMax ans = find_min_max(a, n, &m);
    double bound = 1.5 * (double)n;
    unsigned long long expected = (n % 2 == 0)
        ? (n == 1 ? 0ULL : (3ULL*(unsigned long long)n)/2ULL - 2ULL)
        : (3ULL*(unsigned long long)(n-1))/2ULL;

    printf("\nMinimum : %d\n", ans.minimum);
    printf("Maximum : %d\n", ans.maximum);
    printf("\nComparisons       : %llu\n", m.comparisons);
    printf("Expected exact    : %llu\n", expected);
    printf("3n/2 bound        : %.1f\n", bound);
    printf("Bound satisfied   : %s\n", (double)m.comparisons <= bound ? "YES" : "NO");
    printf("Recursive D&C calls: %llu\n", m.recursive_calls);

    free(a);
    return 0;
}
