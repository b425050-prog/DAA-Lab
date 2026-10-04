#include "../common/lab8_common.h"
int main(int argc, char **argv) {
    int json = options(argc, argv); size_t n = (size_t)integer(0, 10000);
    int64_t *a = allocate(n, sizeof *a); for (size_t i = 0; i < n; ++i) a[i] = integer(INT64_MIN, INT64_MAX); end_input();
    int *d = allocate(n, sizeof *d), *parent = allocate(n, sizeof *parent); uint64_t work = 0;
    int best = -1;
    for (size_t i = 0; i < n; ++i) {
        d[i] = 1; parent[i] = -1;
        for (size_t j = 0; j < i; ++j) {
            ++work;
            if (a[j] < a[i] && d[j] + 1 > d[i]) { d[i] = d[j] + 1; parent[i] = (int)j; }
        }
        if (best < 0 || d[i] > d[best]) best = (int)i;
    }
    int answer = best < 0 ? 0 : d[best]; int64_t *s = allocate((size_t)answer, sizeof *s); int k = answer;
    for (int i = best; i >= 0; i = parent[i]) s[--k] = a[i];
    if (json) {
        printf("{\"result\":%d,\"work\":%" PRIu64 ",\"subsequence\":", answer, work); print_i64s(s, (size_t)answer);
        printf(",\"dp\":"); print_ints(d, n); printf(",\"parent\":"); print_ints(parent, n); puts("}");
    } else { printf("LIS length: %d\nStrictly increasing subsequence: ", answer); print_i64s(s, (size_t)answer); printf("\nPair checks: %" PRIu64 "\n", work); }
    free(a); free(d); free(parent); free(s); return 0;
}
