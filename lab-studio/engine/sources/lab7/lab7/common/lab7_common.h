#ifndef DAA_COMMON_H
#define DAA_COMMON_H

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void fail(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

/* Bounded token parsing avoids scanf's undefined behavior on numeric overflow. */
static inline void token(char out[128]) {
    int c;
    size_t n = 0;
    do { c = getchar(); } while (c != EOF && isspace((unsigned char)c));
    if (c == EOF) fail("missing input");
    do {
        if (n == 127) fail("input token exceeds 127 characters");
        out[n++] = (char)c;
        c = getchar();
    } while (c != EOF && !isspace((unsigned char)c));
    out[n] = '\0';
}

static inline uint64_t read_u64(uint64_t low, uint64_t high) {
    char s[128], *end;
    token(s);
    if (s[0] == '-') fail("expected a nonnegative integer");
    errno = 0;
    uintmax_t v = strtoumax(s, &end, 10);
    if (errno || end == s || *end || v < low || v > high)
        fail("integer outside the documented range");
    return (uint64_t)v;
}

static inline int64_t read_i64(void) {
    char s[128], *end;
    token(s);
    errno = 0;
    intmax_t v = strtoimax(s, &end, 10);
    if (errno || end == s || *end || v < INT64_MIN || v > INT64_MAX)
        fail("invalid signed 64-bit year");
    return (int64_t)v;
}

static inline void end_input(void) {
    int c;
    while ((c = getchar()) != EOF)
        if (!isspace((unsigned char)c)) fail("unexpected extra input");
    if (ferror(stdin)) fail("could not read input");
}

static inline void *allocate(size_t n, size_t size) {
    if (size && n > SIZE_MAX / size) fail("allocation size overflow");
    void *p = calloc(n ? n : 1, size);
    if (!p) fail("out of memory");
    return p;
}

static inline bool count_mode(int argc, char **argv) {
    if (argc == 1) return false;
    if (argc == 2 && strcmp(argv[1], "--count") == 0) return true;
    fail("usage: executable [--count]");
    return false;
}

#endif
