#include <stdio.h>
#include <stdlib.h>

static unsigned long long comparisons;

static int dc_min(const int a[], int left, int right) {
    if (left == right) return a[left];
    int mid = left + (right - left) / 2;
    int x = dc_min(a, left, mid);
    int y = dc_min(a, mid + 1, right);
    ++comparisons;
    return x < y ? x : y;
}

static int dc_max(const int a[], int left, int right) {
    if (left == right) return a[left];
    int mid = left + (right - left) / 2;
    int x = dc_max(a, left, mid);
    int y = dc_max(a, mid + 1, right);
    ++comparisons;
    return x > y ? x : y;
}

static void run_algorithm(const int a[], int n) {
    if (n == 1) return;
    int pairs = n / 2;
    int *low = (int *)malloc((size_t)pairs * sizeof(*low));
    int *high = (int *)malloc((size_t)pairs * sizeof(*high));
    if (!low || !high) exit(EXIT_FAILURE);

    for (int i = 0; i < pairs; ++i) {
        int x = a[2 * i], y = a[2 * i + 1];
        ++comparisons;
        if (x < y) { low[i] = x; high[i] = y; }
        else { low[i] = y; high[i] = x; }
    }

    (void)dc_min(low, 0, pairs - 1);
    (void)dc_max(high, 0, pairs - 1);

    if (n % 2 == 1) comparisons += 2;

    free(low);
    free(high);
}

int main(void) {
    FILE *fp = fopen("max_min_data.dat", "w");
    if (!fp) {
        perror("max_min_data.dat");
        return 1;
    }

    fprintf(fp, "# n comparisons 1.5n exact_formula\n");
    for (int n = 2; n <= 4096; n += 17) {
        int *a = (int *)malloc((size_t)n * sizeof(*a));
        if (!a) return 1;
        for (int i = 0; i < n; ++i) a[i] = (i * 7919) % 1000003;

        comparisons = 0;
        run_algorithm(a, n);
        double exact = (n % 2 == 0) ? (1.5 * (double)n - 2.0)
                                    : (1.5 * (double)(n - 1));
        fprintf(fp, "%d %llu %.1f %.1f\n", n, comparisons, 1.5 * (double)n, exact);
        free(a);
    }

    fclose(fp);
    printf("Created max_min_data.dat\n");
    return 0;
}
