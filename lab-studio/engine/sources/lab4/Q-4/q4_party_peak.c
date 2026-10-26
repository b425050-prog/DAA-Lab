/*
 * DAA Laboratory - Lab 04 - Question 4
 * Earliest time of maximum simultaneous party attendance in O(n log n).
 *
 * Convert each visit [entry, exit) into two events, sort all 2n distinct
 * times, and sweep: +1 at entry, -1 at exit.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long long entry;
    long long exit;
} Person;

typedef struct {
    long long time;
    int delta;
    size_t person;
} Event;

typedef struct {
    unsigned long long sort_comparisons;
    unsigned long long sweep_events;
} Metrics;

static int event_before(const Event *a, const Event *b, Metrics *m) {
    ++m->sort_comparisons;
    return a->time < b->time;
}

static void merge(Event *a, Event *temp, size_t lo, size_t mid, size_t hi,
                  Metrics *m) {
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        temp[k++] = event_before(&a[i], &a[j], m) ? a[i++] : a[j++];
    }
    while (i < mid) temp[k++] = a[i++];
    while (j < hi) temp[k++] = a[j++];
    for (i = lo; i < hi; ++i) a[i] = temp[i];
}

static void merge_sort_events(Event *a, Event *temp, size_t lo, size_t hi,
                              Metrics *m) {
    if (hi - lo <= 1) return;
    size_t mid = lo + (hi - lo) / 2;
    merge_sort_events(a, temp, lo, mid, m);
    merge_sort_events(a, temp, mid, hi, m);
    merge(a, temp, lo, mid, hi, m);
}

int main(void) {
    size_t n;
    printf("============================================================\n");
    printf(" DAA Lab 04 - Q4: Peak Party Attendance\n");
    printf("============================================================\n");
    printf("Enter number of persons: ");
    if (scanf("%zu", &n) != 1 || n == 0 || n > 500000U) {
        fprintf(stderr, "Invalid n.\n");
        return 1;
    }

    Person *people = (Person *)malloc(n * sizeof *people);
    Event *events = (Event *)malloc(2U * n * sizeof *events);
    Event *temp = (Event *)malloc(2U * n * sizeof *temp);
    if (!people || !events || !temp) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(people); free(events); free(temp);
        return 1;
    }

    printf("Enter each person's distinct entry and exit times (entry < exit):\n");
    for (size_t i = 0; i < n; ++i) {
        printf("P%zu: ", i + 1);
        if (scanf("%lld %lld", &people[i].entry, &people[i].exit) != 2 ||
            people[i].entry >= people[i].exit) {
            fprintf(stderr, "Invalid interval for P%zu.\n", i + 1);
            free(people); free(events); free(temp);
            return 1;
        }
        events[2U * i] = (Event){people[i].entry, +1, i};
        events[2U * i + 1] = (Event){people[i].exit, -1, i};
    }

    Metrics metrics = {0, 0};
    merge_sort_events(events, temp, 0, 2U * n, &metrics);
    for (size_t i = 1; i < 2U * n; ++i) {
        if (events[i - 1].time == events[i].time) {
            fprintf(stderr,
                    "Precondition failed: every entry and exit time must be distinct.\n");
            free(people); free(events); free(temp);
            return 1;
        }
    }

    long long current = 0, maximum = 0;
    size_t peak_event = 0;
    for (size_t i = 0; i < 2U * n; ++i) {
        current += events[i].delta;
        ++metrics.sweep_events;
        if (current > maximum) {
            maximum = current;
            peak_event = i;
        }
        if (current < 0) {
            fprintf(stderr, "Invalid event timeline.\n");
            free(people); free(events); free(temp);
            return 2;
        }
    }
    if (current != 0 || peak_event + 1 >= 2U * n) {
        fprintf(stderr, "Timeline validation failed.\n");
        free(people); free(events); free(temp);
        return 2;
    }

    long long peak_start = events[peak_event].time;
    long long peak_end = events[peak_event + 1].time;
    printf("\nChronological event sweep:\n");
    current = 0;
    for (size_t i = 0; i < 2U * n; ++i) {
        current += events[i].delta;
        printf("t=%-8lld  %-5s P%-4zu  present=%lld\n",
               events[i].time, events[i].delta > 0 ? "ENTER" : "EXIT",
               events[i].person + 1, current);
    }

    printf("\nMaximum attendance : %lld people\n", maximum);
    printf("Earliest peak time : %lld\n", peak_start);
    printf("Peak remains on    : [%lld, %lld)\n", peak_start, peak_end);
    printf("People at the peak : ");
    int first = 1;
    for (size_t i = 0; i < n; ++i) {
        if (people[i].entry <= peak_start && peak_start < people[i].exit) {
            printf("%sP%zu", first ? "" : ", ", i + 1);
            first = 0;
        }
    }
    putchar('\n');
    printf("Sort comparisons   : %llu\n", metrics.sort_comparisons);
    printf("Sweep events       : %llu\n", metrics.sweep_events);
    printf("Complexity         : O(n log n) time, O(n) auxiliary space\n");

    free(people); free(events); free(temp);
    return 0;
}
