/*
 * DAA Laboratory - Lab 04 - Question 5
 * Merge a list of closed, overlapping intervals in O(n log n).
 *
 * Sort by left endpoint (then right endpoint), seed the output with the first
 * interval, and either extend the current union or start a new component.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long long left;
    long long right;
} Interval;

typedef struct {
    unsigned long long sort_comparisons;
    unsigned long long scan_comparisons;
    unsigned long long extensions;
    unsigned long long emitted_intervals;
} Metrics;

static int comes_first(const Interval *a, const Interval *b, Metrics *m) {
    ++m->sort_comparisons;
    if (a->left != b->left) return a->left < b->left;
    return a->right <= b->right;
}

static void merge_sort_step(Interval *a, Interval *temp, size_t lo, size_t mid,
                            size_t hi, Metrics *m) {
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        temp[k++] = comes_first(&a[i], &a[j], m) ? a[i++] : a[j++];
    }
    while (i < mid) temp[k++] = a[i++];
    while (j < hi) temp[k++] = a[j++];
    for (i = lo; i < hi; ++i) a[i] = temp[i];
}

static void merge_sort_intervals(Interval *a, Interval *temp, size_t lo,
                                 size_t hi, Metrics *m) {
    if (hi - lo <= 1) return;
    size_t mid = lo + (hi - lo) / 2;
    merge_sort_intervals(a, temp, lo, mid, m);
    merge_sort_intervals(a, temp, mid, hi, m);
    merge_sort_step(a, temp, lo, mid, hi, m);
}

static size_t combine_overlaps(const Interval *sorted, size_t n,
                               Interval *output, Metrics *m) {
    size_t used = 1;
    output[0] = sorted[0];
    m->emitted_intervals = 1;
    for (size_t i = 1; i < n; ++i) {
        ++m->scan_comparisons;
        if (sorted[i].left <= output[used - 1].right) {
            if (sorted[i].right > output[used - 1].right) {
                output[used - 1].right = sorted[i].right;
                ++m->extensions;
            }
        } else {
            output[used++] = sorted[i];
            ++m->emitted_intervals;
        }
    }
    return used;
}

static int result_is_valid(const Interval *sorted, size_t n,
                           const Interval *merged, size_t used) {
    for (size_t i = 1; i < used; ++i) {
        if (merged[i - 1].left > merged[i].left) return 0;
        if (merged[i - 1].right >= merged[i].left) return 0;
    }
    size_t out = 0;
    for (size_t i = 0; i < n; ++i) {
        while (out < used && merged[out].right < sorted[i].left) ++out;
        if (out == used || merged[out].left > sorted[i].left ||
            merged[out].right < sorted[i].right) return 0;
    }
    return 1;
}

int main(void) {
    size_t n;
    printf("============================================================\n");
    printf(" DAA Lab 04 - Q5: Merge Overlapping Intervals\n");
    printf("============================================================\n");
    printf("Enter number of intervals: ");
    if (scanf("%zu", &n) != 1 || n == 0 || n > 1000000U) {
        fprintf(stderr, "Invalid n.\n");
        return 1;
    }

    Interval *intervals = (Interval *)malloc(n * sizeof *intervals);
    Interval *temp = (Interval *)malloc(n * sizeof *temp);
    Interval *result = (Interval *)malloc(n * sizeof *result);
    if (!intervals || !temp || !result) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(intervals); free(temp); free(result);
        return 1;
    }

    printf("Enter each closed interval as: <left> <right>\n");
    for (size_t i = 0; i < n; ++i) {
        if (scanf("%lld %lld", &intervals[i].left, &intervals[i].right) != 2 ||
            intervals[i].left > intervals[i].right) {
            fprintf(stderr, "Invalid interval %zu. Require left <= right.\n", i + 1);
            free(intervals); free(temp); free(result);
            return 1;
        }
    }

    Metrics metrics = {0, 0, 0, 0};
    merge_sort_intervals(intervals, temp, 0, n, &metrics);
    size_t used = combine_overlaps(intervals, n, result, &metrics);
    int valid = result_is_valid(intervals, n, result, used);

    printf("\nSorted intervals : ");
    for (size_t i = 0; i < n; ++i) {
        printf("%s(%lld, %lld)", i ? ", " : "{", intervals[i].left,
               intervals[i].right);
    }
    printf("}\nMerged intervals : ");
    for (size_t i = 0; i < used; ++i) {
        printf("%s(%lld, %lld)", i ? ", " : "{", result[i].left,
               result[i].right);
    }
    printf("}\n");
    printf("Output size      : %zu\n", used);
    printf("Sort comparisons : %llu\n", metrics.sort_comparisons);
    printf("Linear scan tests: %llu\n", metrics.scan_comparisons);
    printf("Validation       : %s\n", valid ? "PASS" : "FAIL");
    printf("Complexity       : O(n log n) time, O(n) auxiliary space\n");

    free(intervals); free(temp); free(result);
    return valid ? 0 : 2;
}
