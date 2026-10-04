#include "../common/lab8_common.h"
int main(int argc, char **argv) {
    int json = options(argc, argv); char *a = line_input(), *b = line_input(); end_input();
    size_t m = strlen(a), n = strlen(b), cols = n + 1;
    int *d = allocate((m + 1) * cols, sizeof *d); uint64_t work = 0;
    for (size_t i = 1; i <= m; ++i) for (size_t j = 1; j <= n; ++j) {
        int up = d[(i - 1) * cols + j], left = d[i * cols + j - 1];
        d[i * cols + j] = a[i - 1] == b[j - 1] ? d[(i - 1) * cols + j - 1] + 1 : (up >= left ? up : left); ++work;
    }
    int answer = d[m * cols + n]; char *s = allocate((size_t)answer + 1, 1); int k = answer;
    size_t i = m, j = n;
    while (i && j) {
        if (a[i - 1] == b[j - 1]) { s[--k] = a[--i]; --j; }
        else if (d[(i - 1) * cols + j] >= d[i * cols + j - 1]) --i;
        else --j;
    }
    if (json) {
        printf("{\"result\":%d,\"work\":%" PRIu64 ",\"subsequence\":", answer, work); json_string(s);
        printf(",\"dp\":"); print_grid(d, m + 1, cols); puts("}");
    } else printf("LCS length: %d\nSubsequence: %s\nPrefix-pair states: %" PRIu64 "\n", answer, s, work);
    free(a); free(b); free(d); free(s); return 0;
}
