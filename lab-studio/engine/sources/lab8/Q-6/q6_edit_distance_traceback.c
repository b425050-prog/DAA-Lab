#include "../common/lab8_common.h"
int main(int argc, char **argv) {
    int json = options(argc, argv); char *a = line_input(), *b = line_input(); end_input();
    size_t m = strlen(a), n = strlen(b), cols = n + 1;
    int *d = allocate((m + 1) * cols, sizeof *d);
    for (size_t i = 0; i <= m; ++i) d[i * cols] = (int)i;
    for (size_t j = 0; j <= n; ++j) d[j] = (int)j;
    uint64_t work = 0;
    for (size_t i = 1; i <= m; ++i) for (size_t j = 1; j <= n; ++j) {
        int x = d[(i - 1) * cols + j - 1] + (a[i - 1] != b[j - 1]);
        int del = d[(i - 1) * cols + j] + 1, ins = d[i * cols + j - 1] + 1;
        if (del < x) x = del;
        if (ins < x) x = ins;
        d[i * cols + j] = x; ++work;
    }
    /* Trace columns use explicit op codes, so literal '-' in an input is unambiguous. */
    char *ops = allocate(m + n + 1, 1), *from = allocate(m + n + 1, 1), *to = allocate(m + n + 1, 1);
    size_t i = m, j = n, len = 0;
    while (i || j) {
        if (i && j && d[i * cols + j] == d[(i - 1) * cols + j - 1] + (a[i - 1] != b[j - 1])) {
            ops[len] = a[i - 1] == b[j - 1] ? 'M' : 'S'; from[len] = a[--i]; to[len] = b[--j];
        } else if (i && d[i * cols + j] == d[(i - 1) * cols + j] + 1) { ops[len] = 'D'; from[len] = a[--i]; to[len] = '-'; }
        else { ops[len] = 'I'; from[len] = '-'; to[len] = b[--j]; }
        ++len;
    }
    for (size_t k = 0; k < len / 2; ++k) {
        size_t r = len - k - 1; char tmp = ops[k]; ops[k] = ops[r]; ops[r] = tmp;
        tmp = from[k]; from[k] = from[r]; from[r] = tmp; tmp = to[k]; to[k] = to[r]; to[r] = tmp;
    }
    int answer = d[m * cols + n];
    if (json) {
        printf("{\"result\":%d,\"work\":%" PRIu64 ",\"ops\":", answer, work); json_string(ops);
        printf(",\"from\":"); json_string(from); printf(",\"to\":"); json_string(to);
        printf(",\"dp\":"); print_grid(d, m + 1, cols); puts("}");
    } else {
        printf("Edit distance: %d\nTraceback (forward; M=match S=substitute D=delete I=insert):\n", answer);
        for (size_t k = 0; k < len; ++k) printf("%c: %c -> %c\n", ops[k], from[k], to[k]);
        printf("Prefix-pair states: %" PRIu64 "\n", work);
    }
    free(a); free(b); free(d); free(ops); free(from); free(to); return 0;
}
