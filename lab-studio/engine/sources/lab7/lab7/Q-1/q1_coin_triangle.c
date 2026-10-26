#include "../common/lab7_common.h"

typedef struct { int x, y; } Point;

int main(int argc, char **argv) {
    bool count_only = count_mode(argc, argv);
    uint64_t n64 = read_u64(1, count_only ? UINT64_C(1000000000) : 1000);
    end_input();
    uint64_t moves = n64 * (n64 + 1) / 6;
    printf("Coins: %" PRIu64 "\nMinimum moves: %" PRIu64 "\n",
           n64 * (n64 + 1) / 2, moves);
    if (count_only) return 0;

    int n = (int)n64, q = (n - 1) / 3, r = (n - 1) % 3;
    /* Three removed corner triangles have balanced side lengths. */
    int x = q + (r > 0), y = q + (r > 1);
    int a = n - 1 - x, b = n - 1 - y;
    Point *from = allocate((size_t)moves, sizeof(*from));
    Point *to = allocate((size_t)moves, sizeof(*to));
    size_t nf = 0, nt = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n - i; ++j) {
            /* Original S: i,j >= 0, i+j <= n-1. */
            if (i > a || j > b || i + j < a + b - (n - 1))
                from[nf++] = (Point){i, j};
            Point p = {a - i, b - j};
            if (p.x < 0 || p.y < 0 || p.x + p.y > n - 1)
                to[nt++] = p;
        }
    }
    if (nf != (size_t)moves || nt != nf) fail("internal move count mismatch");
    printf("Target offsets: %d %d\n", a, b);
    for (size_t i = 0; i < nf; ++i)
        printf("Move %zu: (%d,%d) -> (%d,%d)\n", i + 1,
               from[i].x, from[i].y, to[i].x, to[i].y);
    puts("Verified: every destination is outside the original triangle.");
    free(from);
    free(to);
    return 0;
}
