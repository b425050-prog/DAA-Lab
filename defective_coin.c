#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long long *prefix;
    unsigned long long weighings;
} Scale;

/* Simulates one balance-scale operation on two equal-sized contiguous groups.
   Return -1 if left is lighter, 0 if equal, +1 if right is lighter. */
static int balance(Scale *scale, int l1, int r1, int l2, int r2) {
    long long left_sum = scale->prefix[r1 + 1] - scale->prefix[l1];
    long long right_sum = scale->prefix[r2 + 1] - scale->prefix[l2];
    ++scale->weighings;

    if (left_sum < right_sum) return -1;
    if (left_sum > right_sum) return +1;
    return 0;
}

/* known_exists = 1 means an earlier unbalanced weighing has already proved
   that a lighter coin exists somewhere in [left, right]. */
static int find_lighter(Scale *scale, int left, int right, int known_exists) {
    int n = right - left + 1;

    if (n == 1) {
        return known_exists ? left : -1;
    }

    int half = n / 2;
    int l1 = left;
    int r1 = left + half - 1;
    int l2 = r1 + 1;
    int r2 = l2 + half - 1;
    int result = balance(scale, l1, r1, l2, r2);

    if (result < 0)
        return find_lighter(scale, l1, r1, 1);
    if (result > 0)
        return find_lighter(scale, l2, r2, 1);

    /* Equal pans: both weighed groups contain only normal coins. */
    if (n % 2 == 0) {
        return known_exists ? -2 : -1; /* -2 means inconsistent input. */
    }

    /* One coin was left out. Compare it with a coin just proved normal. */
    int leftover = right;
    int single = balance(scale, leftover, leftover, l1, l1);
    if (single < 0) return leftover;
    if (single == 0) return known_exists ? -2 : -1;
    return -2; /* The possible defective coin was promised never to be heavier. */
}

int main(void) {
    int n;
    printf("Search the Defective (Lighter) Coin\n");
    printf("Enter number of coins (n >= 2): ");
    if (scanf("%d", &n) != 1 || n < 2) {
        fprintf(stderr, "Please enter n >= 2.\n");
        return 1;
    }

    long long *w = (long long *)malloc((size_t)n * sizeof(*w));
    long long *prefix = (long long *)malloc((size_t)(n + 1) * sizeof(*prefix));
    if (!w || !prefix) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(w);
        free(prefix);
        return 1;
    }

    printf("Enter the %d integer coin weights (at most one may be lighter):\n", n);
    prefix[0] = 0;
    for (int i = 0; i < n; ++i) {
        if (scanf("%lld", &w[i]) != 1) {
            fprintf(stderr, "Invalid input.\n");
            free(w);
            free(prefix);
            return 1;
        }
        prefix[i + 1] = prefix[i] + w[i];
    }

    Scale scale = {prefix, 0};
    int ans = find_lighter(&scale, 0, n - 1, 0);

    if (ans >= 0)
        printf("\nDefective coin: position %d (1-based), weight = %lld\n", ans + 1, w[ans]);
    else if (ans == -1)
        printf("\nNo lighter coin exists.\n");
    else
        printf("\nInput is inconsistent with the promise: at most one coin is lighter and none is heavier.\n");

    printf("Balance-scale weighings used: %llu\n", scale.weighings);
    printf("The divide-and-conquer search uses O(log2 n) scale operations; an odd split can add one final check.\n");

    free(w);
    free(prefix);
    return 0;
}
