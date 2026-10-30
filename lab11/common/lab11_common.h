#ifndef LAB11_COMMON_H
#define LAB11_COMMON_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned long long work;
static void need(int ok, const char *message) {
    if (!ok) { fprintf(stderr, "Input error: %s\n", message); exit(EXIT_FAILURE); }
}
static long long number(long long low, long long high) {
    char token[80], *end; errno = 0;
    need(scanf("%79s", token) == 1, "missing integer");
    long long value = strtoll(token, &end, 10);
    need(!errno && end != token && !*end && value >= low && value <= high,
         "integer outside allowed range");
    return value;
}
static void *memory(size_t n, size_t size) {
    void *p = calloc(n ? n : 1, size);
    need(p != NULL, "allocation failed"); return p;
}
static void finish(void) {
    char extra; need(scanf(" %c", &extra) == EOF, "unexpected trailing input");
}
#endif
