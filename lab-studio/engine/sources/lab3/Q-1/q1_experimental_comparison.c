/*
 * DAA Laboratory - Lab 03 - Question 1
 * Deterministic worst-case experiment for Binary vs Ternary Search.
 *
 * We search for a value larger than every array element, forcing a miss
 * down the rightmost branch. The experiment records array probes and key
 * comparisons; operation counts are used instead of noisy wall-clock time.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long probes;
    unsigned long long comparisons;
} Metrics;

static int binary_search_rec(const int a[], int l, int r, int x, Metrics *m) {
    if (l > r) return -1;
    int mid = l + (r - l) / 2;
    m->probes++;
    m->comparisons++;
    if (a[mid] == x) return mid;
    m->comparisons++;
    if (x < a[mid]) return binary_search_rec(a, l, mid - 1, x, m);
    return binary_search_rec(a, mid + 1, r, x, m);
}

static int ternary_search_rec(const int a[], int l, int r, int x, Metrics *m) {
    if (l > r) return -1;
    int third = (r - l) / 3;
    int m1 = l + third;
    int m2 = r - third;

    m->probes++;
    m->comparisons++;
    if (a[m1] == x) return m1;

    if (m2 != m1) {
        m->probes++;
        m->comparisons++;
        if (a[m2] == x) return m2;
    }

    m->comparisons++;
    if (x < a[m1]) return ternary_search_rec(a, l, m1 - 1, x, m);
    m->comparisons++;
    if (x > a[m2]) return ternary_search_rec(a, m2 + 1, r, x, m);
    return ternary_search_rec(a, m1 + 1, m2 - 1, x, m);
}

int main(void) {
    const char *out_name = "q1_experimental_data.dat";
    FILE *out = fopen(out_name, "w");
    if (!out) {
        perror(out_name);
        return 1;
    }

    fprintf(out, "# n binary_probes ternary_probes binary_comparisons ternary_comparisons\n");
    printf("Q1 deterministic worst-case comparison\n");
    printf("Searching for x > maximum element (unsuccessful search).\n\n");
    printf("%-10s %-14s %-15s %-16s %-17s %-10s\n",
           "n", "Bin probes", "Ter probes", "Bin comparisons", "Ter comparisons", "Winner");

    for (int p = 3; p <= 20; ++p) {
        int n = 1 << p;
        int *a = (int *)malloc((size_t)n * sizeof(int));
        if (!a) {
            fclose(out);
            return 1;
        }
        for (int i = 0; i < n; ++i) a[i] = 2 * i;
        int x = 2 * n + 1;

        Metrics b = {0, 0}, t = {0, 0};
        (void)binary_search_rec(a, 0, n - 1, x, &b);
        (void)ternary_search_rec(a, 0, n - 1, x, &t);

        fprintf(out, "%d %llu %llu %llu %llu\n",
                n, b.probes, t.probes, b.comparisons, t.comparisons);
        printf("%-10d %-14llu %-15llu %-16llu %-17llu %-10s\n",
               n, b.probes, t.probes, b.comparisons, t.comparisons,
               b.probes <= t.probes ? "Binary" : "Ternary");
        free(a);
    }

    fclose(out);
    printf("\nData written to %s\n", out_name);
    printf("Conclusion: both grow logarithmically, but binary search uses the smaller\n");
    printf("worst-case probe constant for this comparison model.\n");
    return 0;
}
