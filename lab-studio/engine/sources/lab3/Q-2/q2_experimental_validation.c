/* DAA Lab 03 Q2 - validates the logarithmic weighing bound. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct { unsigned long long weighings; } Metrics;

static int balance(const long long w[], int l1, int r1, int l2, int r2, Metrics *m) {
    long long a = 0, b = 0;
    for (int i = l1; i <= r1; ++i) a += w[i];
    for (int i = l2; i <= r2; ++i) b += w[i];
    m->weighings++;
    return (a > b) - (a < b);
}

static int find_defective(const long long w[], int left, int right, int certain, Metrics *m) {
    int n = right - left + 1;
    if (n == 1) return certain ? left : -1;

    int half = n / 2;
    int l1 = left, r1 = left + half - 1;
    int l2 = r1 + 1, r2 = l2 + half - 1;
    int cmp = balance(w, l1, r1, l2, r2, m);

    if (cmp < 0) return find_defective(w, l1, r1, 1, m);
    if (cmp > 0) return find_defective(w, l2, r2, 1, m);

    if (n % 2) {
        m->weighings++;
        return w[right] < w[l1] ? right : -1;
    }
    return -1;
}

static int ceil_log2_int(int n) {
    int p = 0, x = 1;
    while (x < n) { x <<= 1; p++; }
    return p;
}

int main(void) {
    FILE *out = fopen("q2_experimental_data.dat", "w");
    if (!out) return 1;
    fprintf(out, "# n worst_weighings ceil_log2_n bound_plus_1\n");

    printf("%-8s %-17s %-13s %-12s\n", "n", "worst weighings", "ceil(log2 n)", "bound + 1");
    printf("----------------------------------------------------------\n");

    for (int n = 2; n <= 1024; n += (n < 64 ? 1 : 16)) {
        long long *w = (long long *)malloc((size_t)n * sizeof(long long));
        if (!w) return 1;
        unsigned long long worst = 0;

        /* Every possible defective position plus the 'none defective' case. */
        for (int scenario = -1; scenario < n; ++scenario) {
            for (int i = 0; i < n; ++i) w[i] = 100;
            if (scenario >= 0) w[scenario] = 99;
            Metrics m = {0};
            int ans = find_defective(w, 0, n - 1, 0, &m);
            if ((scenario == -1 && ans != -1) || (scenario >= 0 && ans != scenario)) {
                fprintf(stderr, "Validation failed for n=%d scenario=%d\n", n, scenario);
                free(w);
                fclose(out);
                return 2;
            }
            if (m.weighings > worst) worst = m.weighings;
        }

        int lg = ceil_log2_int(n);
        fprintf(out, "%d %llu %d %d\n", n, worst, lg, lg + 1);
        if (n <= 20 || (n & (n - 1)) == 0)
            printf("%-8d %-17llu %-13d %-12d\n", n, worst, lg, lg + 1);

        if (worst > (unsigned long long)(lg + 1)) {
            fprintf(stderr, "Bound violated at n=%d\n", n);
            free(w);
            fclose(out);
            return 3;
        }
        free(w);
    }

    fclose(out);
    printf("\nAll tested cases satisfy worst weighings <= ceil(log2 n) + 1.\n");
    printf("Data written to q2_experimental_data.dat\n");
    return 0;
}
