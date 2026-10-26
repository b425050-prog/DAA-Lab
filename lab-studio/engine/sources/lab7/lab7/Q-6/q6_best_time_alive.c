#include "../common/lab7_common.h"

typedef struct { int64_t year; int delta; } Event;

static bool before(Event a, Event b) {
    return a.year < b.year || (a.year == b.year && a.delta <= b.delta);
}

static void sort(Event *a, Event *temp, size_t lo, size_t hi) {
    if (hi - lo < 2) return;
    size_t mid = lo + (hi - lo) / 2;
    sort(a, temp, lo, mid);
    sort(a, temp, mid, hi);
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) temp[k++] = before(a[i], a[j]) ? a[i++] : a[j++];
    while (i < mid) temp[k++] = a[i++];
    while (j < hi) temp[k++] = a[j++];
    for (i = lo; i < hi; ++i) a[i] = temp[i];
}

int main(void) {
    size_t n = (size_t)read_u64(0, 100000), used = 0;
    Event *events = allocate(2 * n, sizeof(*events));
    Event *temp = allocate(2 * n, sizeof(*temp));
    for (size_t i = 0; i < n; ++i) {
        char name[128];
        token(name);
        int64_t birth = read_i64(), death = read_i64();
        if (birth > death) fail("birth year is after death year");
        /* [birth, death): equal endpoints contribute no year interval. */
        if (birth != death) {
            events[used++] = (Event){birth, 1};
            events[used++] = (Event){death, -1};
        }
    }
    end_input();
    sort(events, temp, 0, used);
    size_t groups = 0;
    int alive = 0, peak = 0;
    for (size_t i = 0; i < used;) {
        int64_t year = events[i].year;
        do { alive += events[i++].delta; } while (i < used && events[i].year == year);
        /* Store the count valid from this year up to the next event year. */
        temp[groups++] = (Event){year, alive};
        if (alive > peak) peak = alive;
    }
    printf("Maximum scientists alive: %d\n", peak);
    if (!peak) puts("No nonempty lifetime intervals.");
    else {
        bool pending = false;
        int64_t start = 0, end = 0;
        for (size_t i = 0; i + 1 < groups; ++i) {
            if (temp[i].delta == peak) {
                if (!pending) { start = temp[i].year; pending = true; }
                end = temp[i + 1].year;
            } else if (pending) {
                printf("Best interval: [%" PRId64 ", %" PRId64 ")\n", start, end);
                pending = false;
            }
        }
        if (pending) printf("Best interval: [%" PRId64 ", %" PRId64 ")\n", start, end);
    }
    free(events);
    free(temp);
    return 0;
}
