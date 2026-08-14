#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long swaps;
} Metrics;

static void selection_sort(int a[], int n, Metrics *m) {
    m->comparisons = m->swaps = 0;
    for (int i = 0; i < n - 1; ++i) {
        int min_index = i;
        for (int j = i + 1; j < n; ++j) {
            ++m->comparisons;
            if (a[j] < a[min_index]) min_index = j;
        }
        if (min_index != i) {
            int t = a[i]; a[i] = a[min_index]; a[min_index] = t;
            ++m->swaps;
        }
    }
}

int main(void) {
    FILE *fp = fopen("selection_sort_data.dat", "w");
    if (!fp) {
        perror("selection_sort_data.dat");
        return 1;
    }

    fprintf(fp, "# n sorted_comparisons reverse_comparisons random_comparisons theoretical\n");
    for (int n = 50; n <= 2000; n += 50) {
        int *sorted = (int *)malloc((size_t)n * sizeof(*sorted));
        int *reverse = (int *)malloc((size_t)n * sizeof(*reverse));
        int *random = (int *)malloc((size_t)n * sizeof(*random));
        if (!sorted || !reverse || !random) return 1;

        unsigned int state = 123456789u + (unsigned int)n;
        for (int i = 0; i < n; ++i) {
            sorted[i] = i;
            reverse[i] = n - i;
            state = 1664525u * state + 1013904223u;
            random[i] = (int)(state % 100000u);
        }

        Metrics a, b, c;
        selection_sort(sorted, n, &a);
        selection_sort(reverse, n, &b);
        selection_sort(random, n, &c);

        unsigned long long theory = (unsigned long long)n * (unsigned long long)(n - 1) / 2ULL;
        fprintf(fp, "%d %llu %llu %llu %llu\n",
                n, a.comparisons, b.comparisons, c.comparisons, theory);

        free(sorted); free(reverse); free(random);
    }

    fclose(fp);
    printf("Created selection_sort_data.dat\n");
    return 0;
}
