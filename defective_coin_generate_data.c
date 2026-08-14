#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    long long *prefix;
    unsigned long long weighings;
} Scale;

static int balance(Scale *s, int l1, int r1, int l2, int r2) {
    long long a = s->prefix[r1 + 1] - s->prefix[l1];
    long long b = s->prefix[r2 + 1] - s->prefix[l2];
    ++s->weighings;
    return (a > b) - (a < b);
}

static int find_lighter(Scale *s, int left, int right, int known_exists) {
    int n = right - left + 1;
    if (n == 1) return known_exists ? left : -1;

    int half = n / 2;
    int l1 = left, r1 = left + half - 1;
    int l2 = r1 + 1, r2 = l2 + half - 1;
    int result = balance(s, l1, r1, l2, r2);

    if (result < 0) return find_lighter(s, l1, r1, 1);
    if (result > 0) return find_lighter(s, l2, r2, 1);
    if (n % 2 == 0) return known_exists ? -2 : -1;

    int leftover = right;
    int single = balance(s, leftover, leftover, l1, l1);
    if (single < 0) return leftover;
    if (single == 0) return known_exists ? -2 : -1;
    return -2;
}

static unsigned long long run_case(int n, int defective_index) {
    long long *prefix = (long long *)malloc((size_t)(n + 1) * sizeof(*prefix));
    if (!prefix) exit(EXIT_FAILURE);

    prefix[0] = 0;
    for (int i = 0; i < n; ++i) {
        long long weight = (i == defective_index) ? 99 : 100;
        prefix[i + 1] = prefix[i] + weight;
    }

    Scale s = {prefix, 0};
    (void)find_lighter(&s, 0, n - 1, 0);
    free(prefix);
    return s.weighings;
}

int main(void) {
    FILE *fp = fopen("defective_coin_data.dat", "w");
    if (!fp) {
        perror("defective_coin_data.dat");
        return 1;
    }

    fprintf(fp, "# n defective_last no_defect log2n_plus_1\n");
    for (int k = 1; k <= 20; ++k) {
        int n = 1 << k;
        unsigned long long defect = run_case(n, n - 1);
        unsigned long long none = run_case(n, -1);
        double bound = log2((double)n) + 1.0;
        fprintf(fp, "%d %llu %llu %.6f\n", n, defect, none, bound);
    }

    fclose(fp);
    printf("Created defective_coin_data.dat\n");
    return 0;
}
