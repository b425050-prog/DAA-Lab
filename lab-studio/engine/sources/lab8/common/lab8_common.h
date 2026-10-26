#ifndef LAB8_COMMON_H
#define LAB8_COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

static inline void fail(const char *message) {
    fprintf(stderr, "Input/computation error: %s\n", message); exit(2);
}
static inline void *allocate(size_t count, size_t size) {
    if (size && count > (size_t)128 * 1024 * 1024 / size) fail("memory limit exceeded");
    void *p = calloc(count ? count : 1, size);
    if (!p) fail("allocation failed");
    return p;
}
static inline int options(int argc, char **argv) {
    if (argc == 1) return 0;
    if (argc == 2 && strcmp(argv[1], "--json") == 0) return 1;
    fail("usage: program [--json]; read the question's sample input from stdin"); return 0;
}
static inline void token(char *s, size_t capacity) {
    int c; size_t n = 0;
    do { c = getchar(); } while (c != EOF && isspace((unsigned char)c));
    if (c == EOF) fail("missing input value");
    do {
        if (n + 1 >= capacity) fail("input token too long");
        s[n++] = (char)c; c = getchar();
    } while (c != EOF && !isspace((unsigned char)c));
    s[n] = '\0';
}
static inline int64_t integer(int64_t low, int64_t high) {
    char s[128], *end; token(s, sizeof s); errno = 0;
    int64_t x = strtoll(s, &end, 10);
    if (errno || *end || x < low || x > high) fail("integer outside documented range");
    return x;
}
static inline uint64_t unsigned_integer(void) {
    char s[128], *end; token(s, sizeof s); errno = 0;
    if (!isdigit((unsigned char)s[0])) fail("expected an unsigned decimal integer");
    uint64_t x = strtoull(s, &end, 10);
    if (errno || *end) fail("unsigned integer out of range");
    return x;
}
static inline double probability(void) {
    char s[128], *end; token(s, sizeof s); errno = 0;
    double x = strtod(s, &end);
    if (errno || *end || !isfinite(x) || x < 0 || x > 1) fail("invalid probability");
    return x;
}
static inline void end_input(void) {
    int c; while ((c = getchar()) != EOF) if (!isspace((unsigned char)c)) fail("unexpected extra input");
}
/* Strings are whole lines; blank lines represent empty strings. */
static inline char *line_input(void) {
    char *s = allocate(2002, 1); size_t n = 0; int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (c == '\r') continue;
        if (c < 32 || c > 126) fail("strings must contain printable ASCII characters");
        if (n == 2000) fail("string length exceeds 2000");
        s[n++] = (char)c;
    }
    if (c == EOF && n == 0) fail("missing string line; use a blank line for an empty string");
    s[n] = '\0'; return s;
}
static inline uint64_t add_u64(uint64_t a, uint64_t b) {
    if (UINT64_MAX - a < b) fail("uint64_t overflow; exact result cannot be represented");
    return a + b;
}
static inline void json_string(const char *s) {
    putchar('"');
    for (; *s; ++s) { if (*s == '"' || *s == '\\') putchar('\\'); putchar(*s); }
    putchar('"');
}
static inline void print_ints(const int *a, size_t n) {
    putchar('['); for (size_t i = 0; i < n; ++i) printf("%s%d", i ? "," : "", a[i]); putchar(']');
}
static inline void print_i64s(const int64_t *a, size_t n) {
    putchar('['); for (size_t i = 0; i < n; ++i) printf("%s%" PRId64, i ? "," : "", a[i]); putchar(']');
}
static inline void print_u64s(const uint64_t *a, size_t n) {
    putchar('['); for (size_t i = 0; i < n; ++i) printf("%s%" PRIu64, i ? "," : "", a[i]); putchar(']');
}
static inline void print_grid(const int *d, size_t rows, size_t cols) {
    putchar('['); for (size_t i = 0; i < rows; ++i) { if (i) putchar(','); print_ints(d + i * cols, cols); } putchar(']');
}
static inline int *coins_input(size_t n) {
    int *c = allocate(n, sizeof *c);
    for (size_t i = 0; i < n; ++i) {
        c[i] = (int)integer(1, INT_MAX);
        for (size_t j = 0; j < i; ++j) if (c[i] == c[j]) fail("coin denominations must be distinct");
    }
    return c;
}
#endif
