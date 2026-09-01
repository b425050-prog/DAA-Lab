#ifndef LAB6_Q5_FIBONACCI_H
#define LAB6_Q5_FIBONACCI_H

#include <stddef.h>
#include <stdint.h>

typedef struct { unsigned long long transitions; } Q5Metrics;

static inline int q5_fibonacci(unsigned int n, uint64_t *answer, Q5Metrics *metrics) {
    if (n > 93U || answer == NULL) return 0;
    if (n == 0U) { *answer = 0U; return 1; }
    uint64_t previous = 0U, current = 1U;
    for (unsigned int i = 2U; i <= n; ++i) {
        uint64_t next = previous + current;
        previous = current; current = next;
        if (metrics) ++metrics->transitions;
    }
    *answer = current; return 1;
}

#endif
