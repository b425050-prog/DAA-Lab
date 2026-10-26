/* DAA Lab 04 Q3 - measured growth for k = 2, 3 and 4. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long sort_cmp;
    unsigned long long prefixes;
    unsigned long long binary_cmp;
} Metrics;

static void merge(long long *a, long long *tmp, size_t lo, size_t mid,
                  size_t hi, Metrics *m) {
    size_t i = lo, j = mid, p = lo;
    while (i < mid && j < hi) {
        ++m->sort_cmp;
        tmp[p++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    }
    while (i < mid) tmp[p++] = a[i++];
    while (j < hi) tmp[p++] = a[j++];
    for (i = lo; i < hi; ++i) a[i] = tmp[i];
}

static void sort_rec(long long *a, long long *tmp, size_t lo, size_t hi,
                     Metrics *m) {
    if (hi - lo <= 1) return;
    size_t mid = lo + (hi - lo) / 2;
    sort_rec(a, tmp, lo, mid, m);
    sort_rec(a, tmp, mid, hi, m);
    merge(a, tmp, lo, mid, hi, m);
}

static int binary_search_suffix(const long long *a, size_t lo, size_t hi,
                                long long key, Metrics *m) {
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        ++m->binary_cmp;
        if (a[mid] == key) return 1;
        if (a[mid] < key) lo = mid + 1;
        else hi = mid;
    }
    return 0;
}

static int enumerate(const long long *a, size_t n, int k, int depth,
                     size_t start, long long sum, long long target, Metrics *m) {
    if (depth == k - 1) {
        ++m->prefixes;
        return binary_search_suffix(a, start, n, target - sum, m);
    }
    size_t still_needed = (size_t)(k - depth);
    if (n - start < still_needed) return 0;
    size_t last = n - still_needed;
    int found = 0;
    for (size_t i = start; i <= last; ++i) {
        found |= enumerate(a, n, k, depth + 1, i + 1,
                           sum + a[i], target, m);
    }
    return found;
}

static void shuffle(long long *a, size_t n, unsigned int seed) {
    unsigned int state = seed;
    for (size_t i = n; i > 1; --i) {
        state = state * 1664525U + 1013904223U;
        size_t j = (size_t)(state % (unsigned int)i);
        long long t = a[i - 1]; a[i - 1] = a[j]; a[j] = t;
    }
}

int main(void) {
    static const size_t sizes[] = {16, 24, 32, 48, 64, 96, 128};
    FILE *data = fopen("q3_experimental_data.dat", "w");
    if (!data) return 1;
    fprintf(data, "# n k sort_cmp prefixes binary_cmp total reference ratio valid\n");
    printf("%-5s %-3s %-11s %-13s %-13s %-13s %-8s\n",
           "n", "k", "sort cmp", "prefixes", "binary cmp", "total", "check");

    for (int k = 2; k <= 4; ++k) {
        for (size_t row = 0; row < sizeof sizes / sizeof sizes[0]; ++row) {
            size_t n = sizes[row];
            long long *a = (long long *)malloc(n * sizeof *a);
            long long *tmp = (long long *)malloc(n * sizeof *tmp);
            if (!a || !tmp) { free(a); free(tmp); fclose(data); return 1; }
            for (size_t i = 0; i < n; ++i) a[i] = 2LL * (long long)i + 2LL;
            shuffle(a, n, 0xA11CEU + (unsigned int)(31 * k + row));
            Metrics m = {0, 0, 0};
            sort_rec(a, tmp, 0, n, &m);
            int found = enumerate(a, n, k, 0, 0, 0, -1, &m);
            int sorted = 1;
            for (size_t i = 1; i < n; ++i) if (a[i - 1] >= a[i]) sorted = 0;
            int ok = sorted && !found;
            unsigned long long total = m.sort_cmp + m.binary_cmp;
            double reference = pow((double)n, (double)(k - 1)) * log2((double)n);
            fprintf(data, "%zu %d %llu %llu %llu %llu %.3f %.12f %d\n",
                    n, k, m.sort_cmp, m.prefixes, m.binary_cmp, total,
                    reference, total / reference, ok);
            printf("%-5zu %-3d %-11llu %-13llu %-13llu %-13llu %-8s\n",
                   n, k, m.sort_cmp, m.prefixes, m.binary_cmp, total,
                   ok ? "PASS" : "FAIL");
            free(a); free(tmp);
            if (!ok) { fclose(data); return 2; }
        }
    }
    fclose(data);
    printf("\nAll impossible-target runs exhausted every (k-1)-prefix.\n");
    printf("Data written to q3_experimental_data.dat\n");
    return 0;
}
