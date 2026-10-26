/*
 * DAA Laboratory - Lab 03 - Question 2
 * Search the Defective Coin using a balance scale and divide-and-conquer.
 *
 * Assumption from the problem: at most one coin is lighter; no coin is heavier.
 * The program accepts integer weights only to simulate the physical balance scale.
 * The algorithm itself only uses balance outcomes (<, =, >), not the weight values.
 *
 * For n >= 2:
 *   - compare two equal halves,
 *   - recurse into the lighter half if the scale tips,
 *   - if it balances, only an unweighed odd leftover can still be defective;
 *     compare that leftover once with a coin from a balanced group.
 *
 * Worst-case number of weighings is at most ceil(log2 n) + 1.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long weighings;
    unsigned long long recursive_calls;
} Metrics;

static int balance(const long long w[], int l1, int r1, int l2, int r2, Metrics *m) {
    long long s1 = 0, s2 = 0;
    for (int i = l1; i <= r1; ++i) s1 += w[i];
    for (int i = l2; i <= r2; ++i) s2 += w[i];
    m->weighings++;
    if (s1 < s2) return -1;
    if (s1 > s2) return 1;
    return 0;
}

static int compare_single(const long long w[], int a, int good, Metrics *m) {
    m->weighings++;
    if (w[a] < w[good]) return a;
    return -1;
}

static int find_defective(const long long w[], int left, int right,
                          int defect_is_certain, Metrics *m) {
    m->recursive_calls++;
    int n = right - left + 1;

    if (n == 1)
        return defect_is_certain ? left : -1;

    int half = n / 2;
    int l1 = left;
    int r1 = left + half - 1;
    int l2 = r1 + 1;
    int r2 = l2 + half - 1;
    int leftover = (n % 2) ? right : -1;

    int cmp = balance(w, l1, r1, l2, r2, m);
    if (cmp < 0)
        return find_defective(w, l1, r1, 1, m);
    if (cmp > 0)
        return find_defective(w, l2, r2, 1, m);

    /* Both equal groups are certified good when the scale balances. */
    if (leftover != -1)
        return compare_single(w, leftover, l1, m);

    return -1;
}

static int validate_input(const long long w[], int n) {
    long long normal = w[0];
    for (int i = 1; i < n; ++i)
        if (w[i] > normal) normal = w[i];

    int lighter = 0;
    for (int i = 0; i < n; ++i) {
        if (w[i] > normal) return 0;
        if (w[i] < normal) lighter++;
    }
    return lighter <= 1;
}

int main(void) {
    int n;
    printf("============================================================\n");
    printf(" DAA Lab 03 - Q2: Search the Defective Coin\n");
    printf("============================================================\n");
    printf("Enter number of coins (n >= 2): ");
    if (scanf("%d", &n) != 1 || n < 2) {
        printf("Need at least two coins when only a balance scale is available.\n");
        return 1;
    }

    long long *w = (long long *)malloc((size_t)n * sizeof(long long));
    if (!w) return 1;

    printf("Enter %d integer weights (at most one may be lighter):\n", n);
    for (int i = 0; i < n; ++i) {
        if (scanf("%lld", &w[i]) != 1) {
            free(w);
            return 1;
        }
    }

    if (!validate_input(w, n)) {
        printf("Input violates the question's model: at most one coin may be lighter.\n");
        free(w);
        return 1;
    }

    Metrics m = {0, 0};
    int ans = find_defective(w, 0, n - 1, 0, &m);

    printf("\nBalance-scale result:\n");
    if (ans < 0)
        printf("  No lighter coin exists; all coins have identical weight.\n");
    else
        printf("  Coin %d (1-based) is lighter.\n", ans + 1);

    printf("\nValidation metrics:\n");
    printf("  Weighings       : %llu\n", m.weighings);
    printf("  Recursive calls : %llu\n", m.recursive_calls);
    printf("  Required bound  : ceil(log2 n) + c, for a constant c\n");
    printf("\nReason: one weighing discards roughly half the candidates.\n");
    printf("If an odd leftover remains after a balanced weighing, one extra\n");
    printf("comparison against a certified-good coin resolves 'lighter or none'.\n");

    free(w);
    return 0;
}
