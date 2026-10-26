#include "../common/lab7_common.h"

static uint64_t cost[200][200];
static bool known[200][200];
static unsigned split[200][200];

static bool multiply(uint64_t a, uint64_t b, uint64_t *result) {
    if (b && a > UINT64_MAX / b) return false;
    *result = a * b;
    return true;
}

static bool add(uint64_t a, uint64_t b, uint64_t *result) {
    if (a > UINT64_MAX - b) return false;
    *result = a + b;
    return true;
}

static void ordering(unsigned i, unsigned j) {
    if (i == j) { printf("A%u", i + 1); return; }
    putchar('(');
    ordering(i, split[i][j]);
    printf(" x ");
    ordering(split[i][j] + 1, j);
    putchar(')');
}

int main(void) {
    unsigned n = (unsigned)read_u64(1, 200);
    uint64_t p[201];
    for (unsigned i = 0; i <= n; ++i) p[i] = read_u64(1, UINT64_C(1000000000));
    end_input();
    for (unsigned i = 0; i < n; ++i) known[i][i] = true;
    for (unsigned length = 2; length <= n; ++length) {
        for (unsigned i = 0; i + length <= n; ++i) {
            unsigned j = i + length - 1;
            for (unsigned k = i; k < j; ++k) {
                uint64_t product, candidate;
                if (!known[i][k] || !known[k + 1][j] ||
                    !multiply(p[i], p[k + 1], &product) ||
                    !multiply(product, p[j + 1], &product) ||
                    !add(cost[i][k], cost[k + 1][j], &candidate) ||
                    !add(candidate, product, &candidate)) continue;
                if (!known[i][j] || candidate < cost[i][j]) {
                    known[i][j] = true;
                    cost[i][j] = candidate;
                    split[i][j] = k;
                }
            }
        }
    }
    if (!known[0][n - 1]) fail("minimum cost exceeds unsigned 64-bit range");
    printf("Minimum scalar multiplications: %" PRIu64 "\n", cost[0][n - 1]);
    printf("Optimal ordering: ");
    ordering(0, n - 1);
    putchar('\n');
    return 0;
}
