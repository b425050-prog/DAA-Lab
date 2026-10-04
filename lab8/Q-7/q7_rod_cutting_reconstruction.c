#include "../common/lab8_common.h"
int main(int argc, char **argv) {
    int json = options(argc, argv); int n = (int)integer(0, 5000);
    int64_t *p = allocate((size_t)n + 1, sizeof *p), *d = allocate((size_t)n + 1, sizeof *d);
    int *cut = allocate((size_t)n + 1, sizeof *cut);
    /* Signed prices allowed. n * max(abs(price)) cannot exceed INT64_MAX. */
    int64_t bound = n ? INT64_MAX / n : INT64_MAX;
    for (int i = 1; i <= n; ++i) p[i] = integer(-bound, bound);
    end_input(); uint64_t work = 0;
    for (int len = 1; len <= n; ++len) {
        d[len] = INT64_MIN;
        for (int first = 1; first <= len; ++first) {
            int64_t candidate = p[first] + d[len - first]; ++work;
            if (candidate > d[len]) { d[len] = candidate; cut[len] = first; }
        }
    }
    int *pieces = allocate((size_t)n, sizeof *pieces); size_t len = 0;
    for (int x = n; x > 0; x -= cut[x]) pieces[len++] = cut[x];
    if (json) {
        printf("{\"result\":%" PRId64 ",\"work\":%" PRIu64 ",\"pieces\":", d[n], work); print_ints(pieces, len);
        printf(",\"dp\":"); print_i64s(d, (size_t)n + 1); printf(",\"first_cut\":"); print_ints(cut, (size_t)n + 1); puts("}");
    } else { printf("Maximum revenue: %" PRId64 "\nExact piece lengths: ", d[n]); print_ints(pieces, len); printf("\nCut candidates: %" PRIu64 "\n", work); }
    free(p); free(d); free(cut); free(pieces); return 0;
}
