#include "../common/lab8_common.h"
int main(int argc, char **argv) {
    int json = options(argc, argv);
    size_t n = (size_t)integer(0, 256); int v = (int)integer(0, 200000);
    int *c = coins_input(n); end_input();
    uint64_t *d = allocate((size_t)v + 1, sizeof *d), work = 0; d[0] = 1;
    /* Coin outermost: each multiset has exactly one nondecreasing coin order. */
    for (size_t j = 0; j < n; ++j) for (int x = c[j]; x <= v; ++x) {
        d[x] = add_u64(d[x], d[x - c[j]]); ++work;
    }
    if (json) {
        printf("{\"result\":%" PRIu64 ",\"work\":%" PRIu64 ",\"dp\":", d[v], work);
        print_u64s(d, (size_t)v + 1); puts("}");
    } else printf("Distinct combinations: %" PRIu64 "\nDP additions: %" PRIu64 "\n", d[v], work);
    free(c); free(d); return 0;
}
