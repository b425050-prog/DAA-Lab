/* DAA Lab 04 Q5 - deterministic merge-interval complexity experiment. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { long long l, r; } Interval;
typedef struct { unsigned long long sort_cmp, scan; } Metrics;

static int before(const Interval *a, const Interval *b, Metrics *m) {
    ++m->sort_cmp;
    return a->l != b->l ? a->l < b->l : a->r <= b->r;
}
static void merge(Interval *a, Interval *tmp, size_t lo, size_t mid, size_t hi,
                  Metrics *m) {
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) tmp[k++] = before(&a[i], &a[j], m) ? a[i++] : a[j++];
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    for (i = lo; i < hi; ++i) a[i] = tmp[i];
}
static void sort_rec(Interval *a, Interval *tmp, size_t lo, size_t hi, Metrics *m) {
    if (hi - lo <= 1) return;
    size_t mid = lo + (hi - lo) / 2;
    sort_rec(a, tmp, lo, mid, m); sort_rec(a, tmp, mid, hi, m);
    merge(a, tmp, lo, mid, hi, m);
}
static size_t combine(const Interval *a, size_t n, Interval *out, Metrics *m) {
    size_t used = 1; out[0] = a[0];
    for (size_t i = 1; i < n; ++i) {
        ++m->scan;
        if (a[i].l <= out[used - 1].r) {
            if (a[i].r > out[used - 1].r) out[used - 1].r = a[i].r;
        } else out[used++] = a[i];
    }
    return used;
}
static void shuffle(Interval *a, size_t n, unsigned int seed) {
    unsigned int state = seed;
    for (size_t i = n; i > 1; --i) {
        state = state * 1664525U + 1013904223U;
        size_t j = (size_t)(state % (unsigned int)i);
        Interval t = a[i - 1]; a[i - 1] = a[j]; a[j] = t;
    }
}

int main(void) {
    FILE *data = fopen("q5_experimental_data.dat", "w"); if (!data) return 1;
    fprintf(data, "# n sort_cmp scan total reference_nlog2n ratio merged_count valid\n");
    printf("%-8s %-14s %-10s %-14s %-12s %-8s\n",
           "n", "sort cmp", "scan", "total", "merged", "check");
    for (size_t n = 128; n <= 65536; n *= 2) {
        Interval *a = (Interval *)malloc(n * sizeof *a);
        Interval *tmp = (Interval *)malloc(n * sizeof *tmp);
        Interval *out = (Interval *)malloc(n * sizeof *out);
        if (!a || !tmp || !out) { free(a); free(tmp); free(out); fclose(data); return 1; }
        for (size_t i = 0; i < n; ++i) {
            a[i].l = 3LL * (long long)i;
            a[i].r = a[i].l + 2LL + (long long)(i % 5U);
        }
        shuffle(a, n, 0x1A7E2U + (unsigned int)n);
        Metrics m = {0, 0}; sort_rec(a, tmp, 0, n, &m);
        size_t used = combine(a, n, out, &m);
        int ok = used > 0;
        for (size_t i = 1; i < n; ++i) if (a[i - 1].l > a[i].l) ok = 0;
        for (size_t i = 1; i < used; ++i) if (out[i - 1].r >= out[i].l) ok = 0;
        size_t p = 0;
        for (size_t i = 0; i < n; ++i) {
            while (p < used && out[p].r < a[i].l) ++p;
            if (p == used || out[p].l > a[i].l || out[p].r < a[i].r) ok = 0;
        }
        unsigned long long total = m.sort_cmp + m.scan;
        double reference = (double)n * log2((double)n);
        fprintf(data, "%zu %llu %llu %llu %.3f %.9f %zu %d\n",
                n, m.sort_cmp, m.scan, total, reference, total / reference, used, ok);
        printf("%-8zu %-14llu %-10llu %-14llu %-12zu %-8s\n",
               n, m.sort_cmp, m.scan, total, used, ok ? "PASS" : "FAIL");
        free(a); free(tmp); free(out); if (!ok) { fclose(data); return 2; }
    }
    fclose(data);
    printf("\nEvery output is sorted, pairwise disjoint, and covers every input interval.\n");
    printf("Data written to q5_experimental_data.dat\n"); return 0;
}
