#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long writes;
} MergeStats;

typedef struct {
    int *data;
    int length;
} Run;

static void fail_allocation(void) {
    fputs("Memory allocation failed.\n", stderr);
    exit(EXIT_FAILURE);
}

static int *merge_two_arrays(const int a[], int na,
                             const int b[], int nb,
                             MergeStats *stats) {
    int *merged = malloc((size_t)(na + nb) * sizeof(*merged));
    if (merged == NULL) {
        fail_allocation();
    }

    int i = 0;
    int j = 0;
    int out = 0;

    while (i < na && j < nb) {
        ++stats->comparisons;
        merged[out++] = (a[i] <= b[j]) ? a[i++] : b[j++];
        ++stats->writes;
    }
    while (i < na) {
        merged[out++] = a[i++];
        ++stats->writes;
    }
    while (j < nb) {
        merged[out++] = b[j++];
        ++stats->writes;
    }
    return merged;
}

static int **create_sorted_arrays(int k, int n) {
    if (k < 1 || n < 1) {
        fputs("k and n must both be positive.\n", stderr);
        exit(EXIT_FAILURE);
    }

    int **arrays = malloc((size_t)k * sizeof(*arrays));
    if (arrays == NULL) {
        fail_allocation();
    }

    for (int a = 0; a < k; ++a) {
        arrays[a] = malloc((size_t)n * sizeof(**arrays));
        if (arrays[a] == NULL) {
            fail_allocation();
        }
        for (int i = 0; i < n; ++i) {
            arrays[a][i] = i * k + a;
        }
    }
    return arrays;
}

static void free_sorted_arrays(int **arrays, int k) {
    for (int i = 0; i < k; ++i) {
        free(arrays[i]);
    }
    free(arrays);
}

static int *merge_sequential(int **arrays, int k, int n, MergeStats *stats) {
    int current_length = n;
    int *current = malloc((size_t)n * sizeof(*current));
    if (current == NULL) {
        fail_allocation();
    }
    memcpy(current, arrays[0], (size_t)n * sizeof(*current));

    for (int i = 1; i < k; ++i) {
        int *next = merge_two_arrays(current, current_length, arrays[i], n, stats);
        free(current);
        current = next;
        current_length += n;
    }
    return current;
}

static int *merge_balanced(int **arrays, int k, int n, MergeStats *stats) {
    Run *runs = malloc((size_t)k * sizeof(*runs));
    if (runs == NULL) {
        fail_allocation();
    }

    for (int i = 0; i < k; ++i) {
        runs[i].data = malloc((size_t)n * sizeof(*runs[i].data));
        if (runs[i].data == NULL) {
            fail_allocation();
        }
        runs[i].length = n;
        memcpy(runs[i].data, arrays[i], (size_t)n * sizeof(*runs[i].data));
    }

    int active = k;
    while (active > 1) {
        int next_count = 0;
        for (int i = 0; i < active; i += 2) {
            if (i + 1 == active) {
                runs[next_count++] = runs[i];
                continue;
            }

            int *merged = merge_two_arrays(runs[i].data, runs[i].length,
                                           runs[i + 1].data, runs[i + 1].length,
                                           stats);
            const int merged_length = runs[i].length + runs[i + 1].length;
            free(runs[i].data);
            free(runs[i + 1].data);
            runs[next_count].data = merged;
            runs[next_count].length = merged_length;
            ++next_count;
        }
        active = next_count;
    }

    int *answer = runs[0].data;
    free(runs);
    return answer;
}

static int is_sorted(const int array[], int n) {
    for (int i = 1; i < n; ++i) {
        if (array[i - 1] > array[i]) {
            return 0;
        }
    }
    return 1;
}

static void measure(int k, int n,
                    unsigned long long *sequential_writes,
                    unsigned long long *balanced_writes) {
    int **arrays = create_sorted_arrays(k, n);
    MergeStats sequential_stats = {0, 0};
    MergeStats balanced_stats = {0, 0};
    int *sequential = merge_sequential(arrays, k, n, &sequential_stats);
    int *balanced = merge_balanced(arrays, k, n, &balanced_stats);
    const int total = k * n;

    if (!is_sorted(sequential, total) ||
        !is_sorted(balanced, total) ||
        memcmp(sequential, balanced, (size_t)total * sizeof(*sequential)) != 0) {
        fputs("Merge verification failed.\n", stderr);
        exit(EXIT_FAILURE);
    }

    *sequential_writes = sequential_stats.writes;
    *balanced_writes = balanced_stats.writes;

    free(sequential);
    free(balanced);
    free_sorted_arrays(arrays, k);
}

int main(void) {
    const int k_values[] = {2, 4, 8, 16, 32, 64, 128};
    const int n_values[] = {32, 64, 128, 256, 512, 1024, 2048};
    const int count = (int)(sizeof(k_values) / sizeof(k_values[0]));
    const char *data_file = "q3_merge_k_arrays.dat";
    const char *plot_file = "q3_merge_k_sorted_arrays.gp";
    unsigned long long rows[7][6];
    FILE *file = fopen(data_file, "w");

    if (file == NULL) {
        perror(data_file);
        return EXIT_FAILURE;
    }

    fprintf(file, "# k sequential_k balanced_k n sequential_n balanced_n\n");

    for (int i = 0; i < count; ++i) {
        unsigned long long sequential_k;
        unsigned long long balanced_k;
        unsigned long long sequential_n;
        unsigned long long balanced_n;

        measure(k_values[i], 256, &sequential_k, &balanced_k);
        measure(16, n_values[i], &sequential_n, &balanced_n);

        rows[i][0] = (unsigned long long)k_values[i];
        rows[i][1] = sequential_k;
        rows[i][2] = balanced_k;
        rows[i][3] = (unsigned long long)n_values[i];
        rows[i][4] = sequential_n;
        rows[i][5] = balanced_n;
    }

    for (int i = 0; i < count; ++i) {
        fprintf(file, "%llu %llu %llu %llu %llu %llu\n",
                rows[i][0], rows[i][1], rows[i][2],
                rows[i][3], rows[i][4], rows[i][5]);
    }

    fclose(file);

    puts("Q3 measurements completed and verified.");
    puts("Generating q3_merge_k_sorted_arrays.svg with GNUPlot…");

    char command[256];
    snprintf(command, sizeof(command), "gnuplot %s", plot_file);
    const int status = system(command);

    if (status != 0) {
        puts("GNUPlot could not be started. Keep the generated .dat file and run the .gp script after GNUPlot is available.");
        return EXIT_FAILURE;
    }

    remove(data_file);
    puts("Graph generated successfully: q3_merge_k_sorted_arrays.svg");
    return 0;
}
