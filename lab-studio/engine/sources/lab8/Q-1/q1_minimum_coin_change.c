#include "../common/lab8_common.h"
int main(int argc, char **argv) {
    int json = options(argc, argv);
    size_t n = (size_t)integer(0, 256); int v = (int)integer(0, 200000);
    int *c = coins_input(n); end_input();
    int *d = allocate((size_t)v + 1, sizeof *d), *pick = allocate((size_t)v + 1, sizeof *pick);
    uint64_t work = 0;
    for (int x = 1; x <= v; ++x) {
        d[x] = v + 1;
        for (size_t j = 0; j < n; ++j) {
            ++work;
            if (c[j] <= x && d[x - c[j]] + 1 < d[x]) { d[x] = d[x - c[j]] + 1; pick[x] = c[j]; }
        }
    }
    int answer = d[v] <= v ? d[v] : -1;
    int *witness = allocate(answer > 0 ? (size_t)answer : 0, sizeof *witness); size_t len = 0;
    if (answer >= 0) for (int x = v; x > 0; x -= pick[x]) witness[len++] = pick[x];
    for (int x = 0; x <= v; ++x) if (d[x] > v) d[x] = -1;
    if (json) {
        printf("{\"result\":%d,\"work\":%" PRIu64 ",\"coins\":", answer, work); print_ints(witness, len);
        printf(",\"dp\":"); print_ints(d, (size_t)v + 1); puts("}");
    } else {
        printf("Minimum coins: %d\nChosen coins: ", answer); print_ints(witness, len);
        printf("\nCandidate checks: %" PRIu64 "\n", work);
    }
    free(c); free(d); free(pick); free(witness); return 0;
}
