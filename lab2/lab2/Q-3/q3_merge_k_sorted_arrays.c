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
        if (a[i] <= b[j]) {
            merged[out++] = a[i++];
        } else {
            merged[out++] = b[j++];
        }
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

    for (int array_index = 0; array_index < k; ++array_index) {
        arrays[array_index] = malloc((size_t)n * sizeof(**arrays));
        if (arrays[array_index] == NULL) {
            fail_allocation();
        }

        for (int i = 0; i < n; ++i) {
            arrays[array_index][i] = i * k + array_index;
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
    const int k = 4;
    const int n = 5;
    const int total = k * n;

    int **arrays = create_sorted_arrays(k, n);
    MergeStats sequential_stats = {0, 0};
    MergeStats balanced_stats = {0, 0};
    int *sequential = merge_sequential(arrays, k, n, &sequential_stats);
    int *balanced = merge_balanced(arrays, k, n, &balanced_stats);

    puts("");
    puts("========================================================================================");
    puts("                    DAA LAB 02 - QUESTION 3 : MERGING k SORTED ARRAYS");
    puts("                                      Student: Satyam Dhal");
    puts("========================================================================================");
    puts("");

    puts("Method 1 - sequential merging");
    puts("-----------------------------");
    puts("Merge sizes: 2n, 3n, 4n, …, kn");
    puts("Total work : n(2 + 3 + … + k) = n[k(k + 1)/2 − 1]  ⇒  Θ(nk²)");
    puts("");

    puts("Method 2 - balanced pairwise merging");
    puts("------------------------------------");
    puts("Work per level : Θ(nk)");
    puts("Number of levels: Θ(log k)");
    puts("Total work      : Θ(nk log k)");
    puts("");

    printf("Correctness demonstration with k = %d and n = %d\n", k, n);
    puts("-------------------------------------------------");
    for (int i = 0; i < k; ++i) {
        printf("Array %d: ", i + 1);
        print_array(arrays[i], n);
        putchar('\n');
    }

    fputs("\nSequential result: ", stdout);
    print_array(sequential, total);
    fputs("\nBalanced result  : ", stdout);
    print_array(balanced, total);
    puts("");

    puts("");
    printf("Sequential merge : %llu comparisons, %llu element writes\n",
           sequential_stats.comparisons, sequential_stats.writes);
    printf("Balanced merge   : %llu comparisons, %llu element writes\n",
           balanced_stats.comparisons, balanced_stats.writes);
    printf("Outputs identical: %s\n",
           memcmp(sequential, balanced, (size_t)total * sizeof(*sequential)) == 0 ? "YES" : "NO");
    printf("Sorted correctly : %s\n",
           is_sorted(sequential, total) && is_sorted(balanced, total) ? "YES" : "NO");

    puts("");
    puts("Final conclusion");
    puts("----------------");
    puts("Method 2 is asymptotically better as k grows: Θ(nk log k) instead of Θ(nk²).");
    puts("========================================================================================");

    free(sequential);
    free(balanced);
    free_sorted_arrays(arrays, k);
    return 0;
}
