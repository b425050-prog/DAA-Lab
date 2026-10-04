#include "../common/lab8_common.h"
int main(int argc, char **argv) {
    int json = options(argc, argv); size_t n = (size_t)integer(0, 10000);
    uint64_t *a = allocate(n, sizeof *a), *d = allocate(n, sizeof *d); int *parent = allocate(n, sizeof *parent);
    for (size_t i = 0; i < n; ++i) { a[i] = unsigned_integer(); if (!a[i]) fail("MSIS values must be positive"); } end_input();
    int best = -1; uint64_t work = 0;
    for (size_t i = 0; i < n; ++i) {
        d[i] = a[i]; parent[i] = -1;
        for (size_t j = 0; j < i; ++j) {
            ++work;
            if (a[j] < a[i]) {
                uint64_t candidate = add_u64(d[j], a[i]);
                if (candidate > d[i]) { d[i] = candidate; parent[i] = (int)j; }
            }
        }
        if (best < 0 || d[i] > d[best]) best = (int)i;
    }
    uint64_t answer = best < 0 ? 0 : d[best]; size_t len = 0;
    for (int i = best; i >= 0; i = parent[i]) ++len;
    uint64_t *s = allocate(len, sizeof *s); size_t k = len;
    for (int i = best; i >= 0; i = parent[i]) s[--k] = a[i];
    if (json) {
        printf("{\"result\":%" PRIu64 ",\"work\":%" PRIu64 ",\"subsequence\":", answer, work); print_u64s(s, len);
        printf(",\"dp\":"); print_u64s(d, n); printf(",\"parent\":"); print_ints(parent, n); puts("}");
    } else { printf("Maximum increasing sum: %" PRIu64 "\nChosen subsequence: ", answer); print_u64s(s, len); printf("\nPair checks: %" PRIu64 "\n", work); }
    free(a); free(d); free(parent); free(s); return 0;
}
