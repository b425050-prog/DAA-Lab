#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int index;
    unsigned long long probes;
} SearchResult;

static SearchResult binary_search_counted(const int a[], int n, int x) {
    int low = 0, high = n - 1;
    SearchResult r = {-1, 0};
    while (low <= high) {
        int mid = low + (high - low) / 2;
        ++r.probes;
        if (a[mid] == x) { r.index = mid; return r; }
        if (a[mid] < x) low = mid + 1;
        else high = mid - 1;
    }
    return r;
}

static SearchResult ternary_search_counted(const int a[], int n, int x) {
    int low = 0, high = n - 1;
    SearchResult r = {-1, 0};
    while (low <= high) {
        int third = (high - low) / 3;
        int mid1 = low + third;
        int mid2 = high - third;

        ++r.probes;
        if (a[mid1] == x) { r.index = mid1; return r; }
        if (mid2 != mid1) {
            ++r.probes;
            if (a[mid2] == x) { r.index = mid2; return r; }
        }

        if (x < a[mid1]) high = mid1 - 1;
        else if (x > a[mid2]) low = mid2 + 1;
        else { low = mid1 + 1; high = mid2 - 1; }
    }
    return r;
}

int main(void) {
    FILE *fp = fopen("binary_ternary_data.dat", "w");
    if (!fp) {
        perror("binary_ternary_data.dat");
        return 1;
    }

    fprintf(fp, "# n binary_probes ternary_probes\n");
    for (int n = 16; n <= 1048576; n *= 2) {
        int *a = (int *)malloc((size_t)n * sizeof(*a));
        if (!a) {
            fprintf(stderr, "Memory allocation failed at n=%d\n", n);
            fclose(fp);
            return 1;
        }

        for (int i = 0; i < n; ++i) a[i] = 2 * i;

        /* x is larger than every key: a clean unsuccessful worst-direction search. */
        int x = 2 * n + 1;
        SearchResult b = binary_search_counted(a, n, x);
        SearchResult t = ternary_search_counted(a, n, x);
        fprintf(fp, "%d %llu %llu\n", n, b.probes, t.probes);
        free(a);
    }

    fclose(fp);
    printf("Created binary_ternary_data.dat\n");
    return 0;
}
