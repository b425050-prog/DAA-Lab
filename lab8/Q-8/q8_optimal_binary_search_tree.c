#include "../common/lab8_common.h"
static void tree(const int *r, size_t cols, size_t i, size_t j) {
    if (i == j) { printf("d%zu", i); return; }
    int k = r[i * cols + j]; printf("(k%d ", k + 1);
    tree(r, cols, i, (size_t)k); putchar(' '); tree(r, cols, (size_t)k + 1, j); putchar(')');
}
int main(int argc, char **argv) {
    int json = options(argc, argv); size_t n = (size_t)integer(0, 200), cols = n + 1;
    int64_t *keys = allocate(n, sizeof *keys);
    for (size_t i = 0; i < n; ++i) {
        keys[i] = integer(INT64_MIN, INT64_MAX);
        if (i && keys[i] <= keys[i - 1]) fail("keys must be distinct and sorted");
    }
    double *p = allocate(n, sizeof *p), *q = allocate(n + 1, sizeof *q); double total = 0;
    for (size_t i = 0; i < n; ++i) { p[i] = probability(); total += p[i]; }
    for (size_t i = 0; i <= n; ++i) { q[i] = probability(); total += q[i]; }
    end_input(); if (fabs(total - 1) > 1e-8) fail("p and q must sum to 1 within 1e-8");
    double *e = allocate(cols * cols, sizeof *e), *w = allocate(cols * cols, sizeof *w);
    int *r = allocate(cols * cols, sizeof *r); uint64_t work = 0;
    for (size_t i = 0; i <= n; ++i) { e[i * cols + i] = w[i * cols + i] = q[i]; r[i * cols + i] = -1; }
    for (size_t len = 1; len <= n; ++len) for (size_t i = 0; i + len <= n; ++i) {
        size_t j = i + len; w[i * cols + j] = w[i * cols + j - 1] + p[j - 1] + q[j]; e[i * cols + j] = INFINITY;
        for (size_t k = i; k < j; ++k) {
            double x = e[i * cols + k] + e[(k + 1) * cols + j] + w[i * cols + j]; ++work;
            if (x < e[i * cols + j]) { e[i * cols + j] = x; r[i * cols + j] = (int)k; }
        }
    }
    if (json) {
        printf("{\"result\":%.12g,\"work\":%" PRIu64 ",\"tree\":\"", e[n], work); tree(r, cols, 0, n);
        printf("\",\"roots\":"); print_grid(r, cols, cols); printf(",\"costs\":[");
        for (size_t i = 0; i <= n; ++i) { if (i) putchar(','); putchar('['); for (size_t j = 0; j <= n; ++j) printf("%s%.12g", j ? "," : "", e[i * cols + j]); putchar(']'); } puts("]}");
    } else {
        printf("Minimum expected cost (dummy-leaf depths included): %.10f\nTree: ", e[n]); tree(r, cols, 0, n);
        printf("\nKey mapping:"); for (size_t i = 0; i < n; ++i) printf(" k%zu=%" PRId64, i + 1, keys[i]);
        printf("\nCandidate roots: %" PRIu64 "\n", work);
    }
    free(keys); free(p); free(q); free(e); free(w); free(r); return 0;
}
