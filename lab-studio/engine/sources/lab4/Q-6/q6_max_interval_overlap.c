/*
 * DAA Laboratory - Lab 04 - Question 6
 * Find the earliest point contained in the maximum number of closed intervals.
 *
 * Endpoint inclusivity is handled explicitly: at one coordinate, add every
 * START before measuring the count, then remove every END.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long long left;
    long long right;
} Interval;

typedef enum { START = 0, END = 1 } EventType;

typedef struct {
    long long coordinate;
    EventType type;
} Event;

typedef struct {
    unsigned long long sort_comparisons;
    unsigned long long endpoint_events;
    unsigned long long coordinate_groups;
} Metrics;

static int event_before(const Event *a, const Event *b, Metrics *m) {
    ++m->sort_comparisons;
    if (a->coordinate != b->coordinate) return a->coordinate < b->coordinate;
    return a->type < b->type; /* START before END for closed intervals. */
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

static void sort_events(Event *a, Event *temp, size_t lo, size_t hi,
                        Metrics *m) {
    if (hi - lo <= 1) return;
    size_t mid = lo + (hi - lo) / 2;
    sort_events(a, temp, lo, mid, m);
    sort_events(a, temp, mid, hi, m);
    merge(a, temp, lo, mid, hi, m);
}

int main(void) {
    size_t n;
    printf("============================================================\n");
    printf(" DAA Lab 04 - Q6: Point of Maximum Interval Overlap\n");
    printf("============================================================\n");
    printf("Enter number of closed intervals: ");
    if (scanf("%zu", &n) != 1 || n == 0 || n > 500000U) {
        fprintf(stderr, "Invalid n.\n");
        return 1;
    }

    Interval *intervals = (Interval *)malloc(n * sizeof *intervals);
    Event *events = (Event *)malloc(2U * n * sizeof *events);
    Event *temp = (Event *)malloc(2U * n * sizeof *temp);
    if (!intervals || !events || !temp) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(intervals); free(events); free(temp);
        return 1;
    }

    printf("Enter each interval as: <left> <right> (left <= right)\n");
    for (size_t i = 0; i < n; ++i) {
        if (scanf("%lld %lld", &intervals[i].left, &intervals[i].right) != 2 ||
            intervals[i].left > intervals[i].right) {
            fprintf(stderr, "Invalid interval %zu.\n", i + 1);
            free(intervals); free(events); free(temp);
            return 1;
        }
        events[2U * i] = (Event){intervals[i].left, START};
        events[2U * i + 1] = (Event){intervals[i].right, END};
    }

    Metrics metrics = {0, 0, 0};
    sort_events(events, temp, 0, 2U * n, &metrics);

    long long active = 0, maximum = 0, best_point = events[0].coordinate;
    size_t i = 0;
    printf("\nGrouped endpoint sweep:\n");
    printf("%-12s %-8s %-8s %-12s\n", "coordinate", "starts", "ends", "at point");
    while (i < 2U * n) {
        long long coordinate = events[i].coordinate;
        size_t starts = 0, ends = 0, j = i;
        while (j < 2U * n && events[j].coordinate == coordinate) {
            if (events[j].type == START) ++starts;
            else ++ends;
            ++j;
            ++metrics.endpoint_events;
        }
        active += (long long)starts;
        long long at_point = active; /* Intervals ending here are still active. */
        if (at_point > maximum) {
            maximum = at_point;
            best_point = coordinate;
        }
        printf("%-12lld %-8zu %-8zu %-12lld\n",
               coordinate, starts, ends, at_point);
        active -= (long long)ends;
        if (active < 0) {
            fprintf(stderr, "Sweep validation failed.\n");
            free(intervals); free(events); free(temp);
            return 2;
        }
        ++metrics.coordinate_groups;
        i = j;
    }
    if (active != 0) {
        fprintf(stderr, "Sweep did not finish at zero.\n");
        free(intervals); free(events); free(temp);
        return 2;
    }

    long long direct_count = 0;
    printf("\nEarliest maximizing point : p = %lld\n", best_point);
    printf("Intervals containing p    : ");
    int first = 1;
    for (size_t index = 0; index < n; ++index) {
        if (intervals[index].left <= best_point &&
            best_point <= intervals[index].right) {
            ++direct_count;
            printf("%sI%zu=(%lld,%lld)", first ? "" : ", ", index + 1,
                   intervals[index].left, intervals[index].right);
            first = 0;
        }
    }
    putchar('\n');
    printf("Maximum overlap            : %lld intervals\n", maximum);
    printf("Direct containment check   : %s\n",
           direct_count == maximum ? "PASS" : "FAIL");
    printf("Tie rule                   : START -> measure -> END\n");
    printf("Complexity                 : O(n log n) time, O(n) space\n");

    free(intervals); free(events); free(temp);
    return direct_count == maximum ? 0 : 2;
}
