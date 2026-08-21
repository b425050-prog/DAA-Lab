/* DAA Lab 04 Q6 - deterministic closed-endpoint sweep experiment. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { long long x; int type; } Event;
typedef struct { long long l, r; } Interval;
typedef struct { unsigned long long sort_cmp, events, groups; } Metrics;

static int before(const Event *a, const Event *b, Metrics *m) {
    ++m->sort_cmp;
    if (a->x != b->x) return a->x < b->x;
    return a->type < b->type;
}
static void merge(Event *a, Event *tmp, size_t lo, size_t mid, size_t hi,
                  Metrics *m) {
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) tmp[k++] = before(&a[i], &a[j], m) ? a[i++] : a[j++];
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    for (i = lo; i < hi; ++i) a[i] = tmp[i];
}
static void sort_rec(Event *a, Event *tmp, size_t lo, size_t hi, Metrics *m) {
    if (hi - lo <= 1) return;
    size_t mid = lo + (hi-lo)/2; sort_rec(a,tmp,lo,mid,m); sort_rec(a,tmp,mid,hi,m);
    merge(a,tmp,lo,mid,hi,m);
}
static void shuffle_intervals(Interval *a, size_t n, unsigned int seed) {
    unsigned int state = seed;
    for (size_t i = n; i > 1; --i) {
        state = state * 1664525U + 1013904223U;
        size_t j = (size_t)(state % (unsigned int)i);
        Interval t = a[i-1]; a[i-1] = a[j]; a[j] = t;
    }
}

int main(void) {
    FILE *data = fopen("q6_experimental_data.dat", "w"); if (!data) return 1;
    fprintf(data, "# n endpoints sort_cmp sweep_work total reference_2nlog2_2n ratio peak valid\n");
    printf("%-8s %-10s %-14s %-12s %-14s %-8s %-8s\n",
           "n", "endpoints", "sort cmp", "sweep work", "total", "peak", "check");
    for (size_t n = 128; n <= 65536; n *= 2) {
        Interval *iv = (Interval *)malloc(n*sizeof *iv);
        Event *e = (Event *)malloc(2U*n*sizeof *e), *tmp = (Event *)malloc(2U*n*sizeof *tmp);
        if (!iv || !e || !tmp) { free(iv); free(e); free(tmp); fclose(data); return 1; }
        size_t span = n/8U + 3U;
        for (size_t i=0;i<n;++i) {
            iv[i].l = (long long)(i % span);
            iv[i].r = iv[i].l + (long long)(span/2U + (i % 11U));
        }
        shuffle_intervals(iv,n,0xC105EDU+(unsigned int)n);
        for (size_t i=0;i<n;++i) { e[2U*i]=(Event){iv[i].l,0}; e[2U*i+1]=(Event){iv[i].r,1}; }
        Metrics m={0,0,0}; sort_rec(e,tmp,0,2U*n,&m);
        long long active=0,peak=0,best=e[0].x; size_t i=0; int ok=1;
        while (i < 2U * n) {
            long long x = e[i].x;
            size_t starts = 0, ends = 0, j = i;
            while (j < 2U * n && e[j].x == x) {
                if (e[j].type == 0) ++starts;
                else ++ends;
                ++j;
                ++m.events;
            }
            active += (long long)starts;
            if (active > peak) {
                peak = active;
                best = x;
            }
            active -= (long long)ends;
            if (active < 0) ok = 0;
            ++m.groups;
            i = j;
        }
        if (active != 0) ok = 0;
        long long direct = 0;
        for (size_t q = 0; q < n; ++q) {
            if (iv[q].l <= best && best <= iv[q].r) ++direct;
        }
        if (direct != peak) ok = 0;
        unsigned long long sweep_work = m.events + m.groups;
        unsigned long long total = m.sort_cmp + sweep_work;
        double ref = 2.0 * (double)n * log2(2.0 * (double)n);
        fprintf(data, "%zu %zu %llu %llu %llu %.3f %.9f %lld %d\n",
                n, 2U * n, m.sort_cmp, sweep_work, total, ref,
                total / ref, peak, ok);
        printf("%-8zu %-10zu %-14llu %-12llu %-14llu %-8lld %-8s\n",
               n, 2U * n, m.sort_cmp, sweep_work, total, peak,
               ok ? "PASS" : "FAIL");
        free(iv);
        free(e);
        free(tmp);
        if (!ok) {
            fclose(data);
            return 2;
        }
    }
    fclose(data);
    printf("\nTied endpoints were grouped with closed-interval START-before-END semantics.\n");
    printf("Data written to q6_experimental_data.dat\n"); return 0;
}
