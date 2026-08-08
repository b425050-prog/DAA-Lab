#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long writes;
} SortStats;

static uint32_t random_state = 0xC0FFEEu;

static uint32_t next_random(void) {
    uint32_t x = random_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    random_state = x;
    return x;
}

static void fill_random(int array[], int n) {
    for (int i = 0; i < n; ++i) {
        array[i] = (int)(next_random() & 0x7fffffffU);
    }
}

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
        temp[out++] = (array[i] <= array[j]) ? array[i++] : array[j++];
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

static void sort_two_recursive(int array[], int temp[], int left, int right, SortStats *stats) {
    if (right - left <= 1) {
        return;
    }
    const int middle = left + (right - left) / 2;
    sort_two_recursive(array, temp, left, middle, stats);
    sort_two_recursive(array, temp, middle, right, stats);
    merge_two(array, temp, left, middle, right, stats);
}

static SortStats sort_two(int array[], int n) {
    SortStats stats = {0, 0};
    int *temp = malloc((size_t)n * sizeof(*temp));
    if (temp == NULL) {
        fail_allocation();
    }
    sort_two_recursive(array, temp, 0, n, &stats);
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
            const int current = (chosen == 0) ? array[i] : array[j];
            ++stats->comparisons;
            if (array[k] < current) {
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

static void sort_three_recursive(int array[], int temp[], int left, int right, SortStats *stats) {
    const int length = right - left;
    if (length <= 1) {
        return;
    }

    const int middle1 = left + length / 3;
    const int middle2 = left + (2 * length) / 3;
    sort_three_recursive(array, temp, left, middle1, stats);
    sort_three_recursive(array, temp, middle1, middle2, stats);
    sort_three_recursive(array, temp, middle2, right, stats);
    merge_three(array, temp, left, middle1, middle2, right, stats);
}

static SortStats sort_three(int array[], int n) {
    SortStats stats = {0, 0};
    int *temp = malloc((size_t)n * sizeof(*temp));
    if (temp == NULL) {
        fail_allocation();
    }
    sort_three_recursive(array, temp, 0, n, &stats);
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

static int log2_for_power_of_two(int n) {
    int level = 0;
    while (n > 1) {
        n /= 2;
        ++level;
    }
    return level;
}

int main(void) {
    const int sizes[] = {256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
    const int count = (int)(sizeof(sizes) / sizeof(sizes[0]));
    const char *data_file = "q2_merge_sort.dat";
    const char *plot_file = "q2_merge_sort_comparison.gp";
    FILE *file = fopen(data_file, "w");

    if (file == NULL) {
        perror(data_file);
        return EXIT_FAILURE;
    }

    fprintf(file, "# n work_2way work_3way n_log2_n comparisons_2way comparisons_3way\n");

    for (int s = 0; s < count; ++s) {
        const int n = sizes[s];
        int *base = malloc((size_t)n * sizeof(*base));
        int *two_way = malloc((size_t)n * sizeof(*two_way));
        int *three_way = malloc((size_t)n * sizeof(*three_way));

        if (base == NULL || two_way == NULL || three_way == NULL) {
            fail_allocation();
        }

        fill_random(base, n);
        memcpy(two_way, base, (size_t)n * sizeof(*two_way));
        memcpy(three_way, base, (size_t)n * sizeof(*three_way));

        const SortStats two = sort_two(two_way, n);
        const SortStats three = sort_three(three_way, n);

        if (!is_sorted(two_way, n) || !is_sorted(three_way, n)) {
            fputs("Sort verification failed.\n", stderr);
            return EXIT_FAILURE;
        }

        const unsigned long long work_two = two.comparisons + two.writes;
        const unsigned long long work_three = three.comparisons + three.writes;
        const unsigned long long reference = (unsigned long long)n * (unsigned long long)log2_for_power_of_two(n);

        fprintf(file, "%d %llu %llu %llu %llu %llu\n",
                n, work_two, work_three, reference, two.comparisons, three.comparisons);

        free(base);
        free(two_way);
        free(three_way);
    }

    fclose(file);

    puts("Q2 measurements completed and verified.");
    puts("Generating q2_merge_sort_comparison.svg with GNUPlot…");

    char command[256];
    snprintf(command, sizeof(command), "gnuplot %s", plot_file);
    const int status = system(command);

    if (status != 0) {
        puts("GNUPlot could not be started. Keep the generated .dat file and run the .gp script after GNUPlot is available.");
        return EXIT_FAILURE;
    }

    remove(data_file);
    puts("Graph generated successfully: q2_merge_sort_comparison.svg");
    return 0;
}
