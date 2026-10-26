#include "../common/lab7_common.h"

int main(void) {
    unsigned n = (unsigned)read_u64(2, 2000);
    end_input();
    unsigned shots = n == 2 ? 2 : 2 * (n - 2);
    bool *possible = allocate(n, sizeof(*possible));
    bool *next = allocate(n, sizeof(*next));
    for (unsigned i = 0; i < n; ++i) possible[i] = true;
    printf("Guaranteed shots: %u\n", shots);
    unsigned survivors = n;
    for (unsigned t = 0; t < shots; ++t) {
        unsigned spot;
        if (n == 2) spot = 1;
        else if (t < n - 2) spot = t + 2;
        else spot = n - 1 - (t - (n - 2));
        possible[spot - 1] = false;
        survivors = 0;
        for (unsigned i = 0; i < n; ++i) survivors += possible[i] ? 1U : 0U;
        printf("Shot %u: spot %u; surviving positions: %u\n", t + 1, spot, survivors);
        if (t + 1 < shots) {
            for (unsigned i = 0; i < n; ++i)
                next[i] = (i > 0 && possible[i - 1]) ||
                          (i + 1 < n && possible[i + 1]);
            bool *temp = possible;
            possible = next;
            next = temp;
        }
    }
    if (survivors) fail("strategy did not eliminate every possible path");
    puts("Verified: no target path can survive the complete schedule.");
    free(possible);
    free(next);
    return 0;
}
