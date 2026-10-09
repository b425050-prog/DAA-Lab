#ifndef LAB9_COMMON_H
#define LAB9_COMMON_H
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef long long Int;
static unsigned long long work;
static inline void need(int ok, const char *message) {
    if (!ok) { fprintf(stderr, "Input error: %s\n", message); exit(1); }
}
static inline void *memory(size_t count, size_t size) {
    void *p = calloc(count ? count : 1, size);
    need(p != NULL, "allocation failed"); return p;
}
static inline Int integer(Int lo, Int hi) {
    char token[128], *end; errno = 0;
    need(scanf("%127s", token) == 1, "missing integer");
    Int x = strtoll(token, &end, 10);
    need(!errno && !*end && x >= lo && x <= hi, "integer out of range");
    return x;
}
static inline double real(double lo, double hi) {
    char token[128], *end; errno = 0;
    need(scanf("%127s", token) == 1, "missing real number");
    double x = strtod(token, &end);
    need(!errno && !*end && isfinite(x) && x >= lo && x <= hi,
         "real number out of range"); return x;
}
static inline void finish(void) {
    char extra[2]; need(scanf("%1s", extra) != 1, "unexpected extra input");
}
static inline int json_mode(int argc, char **argv) {
    need(argc == 1 || (argc == 2 && !strcmp(argv[1], "--json")),
         "use no argument or --json"); return argc == 2;
}
typedef struct { Int value; int id; } Entry;
typedef struct { Entry *a; int n, max; } Heap;
static inline int before(Entry a, Entry b, int max) {
    ++work;
    return a.value != b.value ? (max ? a.value > b.value : a.value < b.value)
                             : a.id < b.id;
}
static inline void push(Heap *h, Entry x) {
    int i = ++h->n;
    while (i > 1 && before(x, h->a[i/2], h->max)) {
        h->a[i] = h->a[i/2]; i /= 2;
    }
    h->a[i] = x;
}
static inline Entry pop(Heap *h) {
    need(h->n > 0, "empty heap"); Entry result = h->a[1], x = h->a[h->n--];
    int i = 1;
    while (2*i <= h->n) {
        int j = 2*i;
        if (j < h->n && before(h->a[j+1], h->a[j], h->max)) ++j;
        if (!before(h->a[j], x, h->max)) break;
        h->a[i] = h->a[j]; i = j;
    }
    if (h->n) h->a[i] = x;
    return result;
}
static inline void numbers(const Int *a, int n) {
    putchar('[');
    for (int i = 0; i < n; ++i) printf("%s%lld", i ? "," : "", a[i]);
    putchar(']');
}
#endif
