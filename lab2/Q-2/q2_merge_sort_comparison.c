#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long writes;
} SortStats;

static void fail_allocation(void) {
    fputs("Memory allocation failed.\n", stderr);
    exit(EXIT_FAILURE);
}

static void merge_two(int array[], int temp[], int left, int middle, int right, SortStats *stats) {
    int i = left;
    int j = middle;
    int out = left;

    while (i < middle && j < right) {
        ++stats->comparisons;
        if (array[i] <= array[j]) {
            temp[out++] = array[i++];
        } else {
            temp[out++] = array[j++];
        }
        ++stats->writes;
    }

    while (i < middle) {
        temp[out++] = array[i++];
        ++stats->writes;
    }

    while (j < right) {
        temp[out++] = array[j++];
        ++stats->writes;
    }

    for (i = left; i < right; ++i) {
        array[i] = temp[i];
    }
}

static void merge_sort_two_recursive(int array[], int temp[], int left, int right, SortStats *stats) {
    if (right - left <= 1) {
        return;
    }

    const int middle = left + (right - left) / 2;
    merge_sort_two_recursive(array, temp, left, middle, stats);
    merge_sort_two_recursive(array, temp, middle, right, stats);
    merge_two(array, temp, left, middle, right, stats);
}

static SortStats merge_sort_two(int array[], int n) {
    SortStats stats = {0, 0};
    if (n <= 1) {
        return stats;
    }

    int *temp = malloc((size_t)n * sizeof(*temp));
    if (temp == NULL) {
        fail_allocation();
    }

    merge_sort_two_recursive(array, temp, 0, n, &stats);
    free(temp);
    return stats;
}

static int choose_smallest(const int array[],
                           int i, int end1,
                           int j, int end2,
                           int k, int end3,
                           SortStats *stats) {
    int chosen = -1;

    if (i < end1) {
        chosen = 0;
    }

    if (j < end2) {
        if (chosen == -1) {
            chosen = 1;
        } else {
            ++stats->comparisons;
            if (array[j] < array[i]) {
                chosen = 1;
            }
        }
    }

    if (k < end3) {
        if (chosen == -1) {
            chosen = 2;
        } else {
            const int current_value = (chosen == 0) ? array[i] : array[j];
            ++stats->comparisons;
            if (array[k] < current_value) {
                chosen = 2;
            }
        }
    }

    return chosen;
}

static void merge_three(int array[], int temp[],
                        int left, int middle1, int middle2, int right,
                        SortStats *stats) {
    int i = left;
    int j = middle1;
    int k = middle2;
    int out = left;

    while (i < middle1 || j < middle2 || k < right) {
        const int chosen = choose_smallest(array, i, middle1, j, middle2, k, right, stats);

        if (chosen == 0) {
            temp[out++] = array[i++];
        } else if (chosen == 1) {
            temp[out++] = array[j++];
        } else {
            temp[out++] = array[k++];
        }
        ++stats->writes;
    }

    for (i = left; i < right; ++i) {
        array[i] = temp[i];
    }
}

static void merge_sort_three_recursive(int array[], int temp[], int left, int right, SortStats *stats) {
    const int length = right - left;
    if (length <= 1) {
        return;
    }

    const int middle1 = left + length / 3;
    const int middle2 = left + (2 * length) / 3;

    merge_sort_three_recursive(array, temp, left, middle1, stats);
    merge_sort_three_recursive(array, temp, middle1, middle2, stats);
    merge_sort_three_recursive(array, temp, middle2, right, stats);
    merge_three(array, temp, left, middle1, middle2, right, stats);
}

static SortStats merge_sort_three(int array[], int n) {
    SortStats stats = {0, 0};
    if (n <= 1) {
        return stats;
    }

    int *temp = malloc((size_t)n * sizeof(*temp));
    if (temp == NULL) {
        fail_allocation();
    }

    merge_sort_three_recursive(array, temp, 0, n, &stats);
    free(temp);
    return stats;
}

static int is_sorted(const int array[], int n) {
    for (int i = 1; i < n; ++i) {
        if (array[i - 1] > array[i]) {
            return 0;
        }
    }
    return 1;
}

static void print_array(const int array[], int n) {
    putchar('[');
    for (int i = 0; i < n; ++i) {
        if (i > 0) {
            fputs(", ", stdout);
        }
        printf("%d", array[i]);
    }
    putchar(']');
}

int main(void) {
    const int input[] = {
        42, 7, 19, 3, 88, 15, 61, 2, 31,
        54, 12, 73, 9, 27, 65, 1, 48, 36
    };
    const int n = (int)(sizeof(input) / sizeof(input[0]));
    int two_way[sizeof(input) / sizeof(input[0])];
    int three_way[sizeof(input) / sizeof(input[0])];

    memcpy(two_way, input, sizeof(input));
    memcpy(three_way, input, sizeof(input));

    const SortStats two_stats = merge_sort_two(two_way, n);
    const SortStats three_stats = merge_sort_three(three_way, n);

    puts("");
    puts("========================================================================================");
    puts("                 DAA LAB 02 - QUESTION 2 : 2-WAY vs 3-WAY MERGE SORT");
    puts("                                  Student: Satyam Dhal");
    puts("========================================================================================");
    puts("");

    puts("Worst-case analysis");
    puts("-------------------");
    puts("Standard merge sort : T₂(n) = 2T₂(n/2) + Θ(n)  ⇒  Θ(n log n)");
    puts("Three-way merge sort: T₃(n) = 3T₃(n/3) + Θ(n)  ⇒  Θ(n log n)");
    puts("");
    puts("Both recurrences perform Θ(n) merging work per recursion level.");
    puts("Changing the number of subproblems changes the recursion depth and constant factors, not the asymptotic class.");

    puts("");
    fputs("Input             : ", stdout);
    print_array(input, n);
    puts("");
    fputs("2-way sorted      : ", stdout);
    print_array(two_way, n);
    puts("");
    fputs("3-way sorted      : ", stdout);
    print_array(three_way, n);
    puts("");

    puts("");
    puts("Measured work on the same input");
    puts("--------------------------------");
    printf("2-way merge sort  : %llu comparisons, %llu merge writes\n", two_stats.comparisons, two_stats.writes);
    printf("3-way merge sort  : %llu comparisons, %llu merge writes\n", three_stats.comparisons, three_stats.writes);
    printf("2-way verification: %s\n", is_sorted(two_way, n) ? "SORTED" : "FAILED");
    printf("3-way verification: %s\n", is_sorted(three_way, n) ? "SORTED" : "FAILED");

    puts("");
    puts("Final conclusion");
    puts("----------------");
    puts("Standard merge sort and the modified three-way merge sort both have worst-case running time Θ(n log n).");
    puts("========================================================================================");

    return 0;
}
