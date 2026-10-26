/* DAA Lab 04 Q2 - count merge-sort and binary-search comparisons. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long sort_comparisons;
    unsigned long long search_comparisons;
} Metrics;

static void merge(long long *a, long long *temp, size_t lo, size_t mid,
                  size_t hi, Metrics *m) {
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        ++m->sort_comparisons;
        temp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    }
    while (i < mid) temp[k++] = a[i++];
    while (j < hi) temp[k++] = a[j++];
    for (i = lo; i < hi; ++i) a[i] = temp[i];
}

static void sort_recursive(long long *a, long long *temp, size_t lo,
                           size_t hi, Metrics *m) {
    if (hi - lo <= 1) return;
    size_t mid = lo + (hi - lo) / 2;
    sort_recursive(a, temp, lo, mid, m);
    sort_recursive(a, temp, mid, hi, m);
    merge(a, temp, lo, mid, hi, m);
}

static int binary_search_value(const long long *a, size_t n, long long key,
                               Metrics *m) {
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        ++m->search_comparisons;
        if (a[mid] == key) return 1;
        if (a[mid] < key) lo = mid + 1;
        else hi = mid;
    }
    return 0;
}

static void shuffle(long long *a, size_t n) {
    unsigned int state = 0x51A7E2U;
    for (size_t i = n; i > 1; --i) {
        state = state * 1664525U + 1013904223U;
        size_t j = (size_t)(state % (unsigned int)i);
        long long temporary = a[i - 1];
        a[i - 1] = a[j];
        a[j] = temporary;
    }
}

int main(void) {
    FILE *data = fopen("q2_experimental_data.dat", "w");
    if (!data) return 1;
    fprintf(data, "# n sort_comparisons search_comparisons total n_log2_n ratio valid\n");
    printf("%-8s %-14s %-16s %-14s %-10s\n",
           "n", "sort cmp", "search cmp", "total", "validation");

    for (size_t n = 128; n <= 32768; n *= 2) {
        long long *s1 = (long long *)malloc(n * sizeof *s1);
        long long *s2 = (long long *)malloc(n * sizeof *s2);
        long long *temp = (long long *)malloc(n * sizeof *temp);
        if (!s1 || !s2 || !temp) {
            free(s1); free(s2); free(temp); fclose(data); return 1;
        }
        for (size_t i = 0; i < n; ++i) {
            s1[i] = 2LL * (long long)i;
            s2[i] = 2LL * (long long)(n + i);
        }
        shuffle(s2, n);
        Metrics m = {0, 0};
        sort_recursive(s2, temp, 0, n, &m);
        int sorted = 1;
        for (size_t i = 1; i < n; ++i) {
            if (s2[i - 1] > s2[i]) sorted = 0;
        }
        int found = 0;
        const long long impossible_odd_target = -1;
        for (size_t i = 0; i < n; ++i) {
            found |= binary_search_value(s2, n,
                                         impossible_odd_target - s1[i], &m);
        }
        int ok = sorted && !found;
        unsigned long long total = m.sort_comparisons + m.search_comparisons;
        double reference = (double)n * log2((double)n);
        fprintf(data, "%zu %llu %llu %llu %.3f %.9f %d\n",
                n, m.sort_comparisons, m.search_comparisons, total,
                reference, total / reference, ok);
        printf("%-8zu %-14llu %-16llu %-14llu %-10s\n",
               n, m.sort_comparisons, m.search_comparisons, total,
               ok ? "PASS" : "FAIL");
        free(s1); free(s2); free(temp);
        if (!ok) { fclose(data); return 2; }
    }
    fclose(data);
    printf("\nThe total comparison count stays a constant multiple of n log2(n).\n");
    printf("Data written to q2_experimental_data.dat\n");
    return 0;
}
