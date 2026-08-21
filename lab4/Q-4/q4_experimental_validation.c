/* DAA Lab 04 Q4 - deterministic event-sweep complexity experiment. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { long long time; int delta; } Event;
typedef struct { unsigned long long sort_cmp, sweep; } Metrics;

static void merge(Event *a, Event *tmp, size_t lo, size_t mid, size_t hi,
                  Metrics *m) {
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        ++m->sort_cmp;
        tmp[k++] = (a[i].time <= a[j].time) ? a[i++] : a[j++];
    }
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    for (i = lo; i < hi; ++i) a[i] = tmp[i];
}

static void sort_rec(Event *a, Event *tmp, size_t lo, size_t hi, Metrics *m) {
    if (hi - lo <= 1) return;
    size_t mid = lo + (hi - lo) / 2;
    sort_rec(a, tmp, lo, mid, m);
    sort_rec(a, tmp, mid, hi, m);
    merge(a, tmp, lo, mid, hi, m);
}

static void shuffle_size_t(size_t *a, size_t n, unsigned int seed) {
    unsigned int state = seed;
    for (size_t i = n; i > 1; --i) {
        state = state * 1664525U + 1013904223U;
        size_t j = (size_t)(state % (unsigned int)i);
        size_t t = a[i - 1]; a[i - 1] = a[j]; a[j] = t;
    }
}

int main(void) {
    FILE *data = fopen("q4_experimental_data.dat", "w");
    if (!data) return 1;
    fprintf(data, "# n events sort_cmp sweep total reference_2nlog2_2n ratio peak valid\n");
    printf("%-8s %-10s %-14s %-10s %-14s %-8s %-8s\n",
           "persons", "events", "sort cmp", "sweep", "total", "peak", "check");

    for (size_t n = 128; n <= 65536; n *= 2) {
        size_t *entry_order = (size_t *)malloc(n * sizeof *entry_order);
        size_t *exit_order = (size_t *)malloc(n * sizeof *exit_order);
        Event *events = (Event *)malloc(2U * n * sizeof *events);
        Event *tmp = (Event *)malloc(2U * n * sizeof *tmp);
        if (!entry_order || !exit_order || !events || !tmp) {
            free(entry_order); free(exit_order); free(events); free(tmp);
            fclose(data); return 1;
        }
        for (size_t i = 0; i < n; ++i) entry_order[i] = exit_order[i] = i;
        shuffle_size_t(entry_order, n, 0xE117U + (unsigned int)n);
        shuffle_size_t(exit_order, n, 0xE817U + (unsigned int)n);
        for (size_t person = 0; person < n; ++person) {
            events[2U * person] = (Event){(long long)entry_order[person], +1};
            events[2U * person + 1] =
                (Event){(long long)(n + exit_order[person]), -1};
        }
        Metrics m = {0, 0};
        sort_rec(events, tmp, 0, 2U * n, &m);
        long long active = 0, peak = 0;
        int ok = 1;
        for (size_t i = 0; i < 2U * n; ++i) {
            if (i > 0 && events[i - 1].time >= events[i].time) ok = 0;
            active += events[i].delta;
            ++m.sweep;
            if (active < 0) ok = 0;
            if (active > peak) peak = active;
        }
        if (active != 0 || peak != (long long)n) ok = 0;
        unsigned long long total = m.sort_cmp + m.sweep;
        double reference = 2.0 * (double)n * log2(2.0 * (double)n);
        fprintf(data, "%zu %zu %llu %llu %llu %.3f %.9f %lld %d\n",
                n, 2U * n, m.sort_cmp, m.sweep, total, reference,
                total / reference, peak, ok);
        printf("%-8zu %-10zu %-14llu %-10llu %-14llu %-8lld %-8s\n",
               n, 2U * n, m.sort_cmp, m.sweep, total, peak,
               ok ? "PASS" : "FAIL");
        free(entry_order); free(exit_order); free(events); free(tmp);
        if (!ok) { fclose(data); return 2; }
    }
    fclose(data);
    printf("\nEvery timeline sorted 2n distinct events, reached peak n, and ended at zero.\n");
    printf("Data written to q4_experimental_data.dat\n");
    return 0;
}
