#ifndef LAB6_Q6_KNAPSACK_H
#define LAB6_Q6_KNAPSACK_H

#include <stddef.h>
#include <stdlib.h>

typedef struct {
    long long maximum_profit;
    unsigned long long states;
} Q6Result;

static inline int q6_knapsack(const size_t *weights, const long long *profits,
                       size_t n, size_t capacity, unsigned char *selected,
                       Q6Result *result) {
    if (!weights || !profits || !selected || !result || n == 0U) return 0;
    if (capacity == (size_t)-1 || n + 1U > (size_t)-1 / (capacity + 1U)) return 0;
    size_t cols = capacity + 1U, cells = (n + 1U) * cols;
    long long *dp = calloc(cells, sizeof *dp);
    if (!dp) return 0;
    result->states = 0U;
    for (size_t i = 1U; i <= n; ++i) {
        if (weights[i-1U] == 0U) { free(dp); return 0; }
        for (size_t w = 0U; w <= capacity; ++w) {
            long long best = dp[(i-1U)*cols+w];
            if (weights[i-1U] <= w) {
                long long include = profits[i-1U] + dp[(i-1U)*cols+w-weights[i-1U]];
                if (include > best) best = include;
            }
            dp[i*cols+w] = best; ++result->states;
        }
    }
    for (size_t i = 0U; i < n; ++i) selected[i] = 0U;
    size_t w = capacity;
    for (size_t i = n; i > 0U; --i) {
        if (dp[i*cols+w] != dp[(i-1U)*cols+w]) { selected[i-1U]=1U; w -= weights[i-1U]; }
    }
    result->maximum_profit = dp[n*cols+capacity];
    free(dp); return 1;
}

#endif
