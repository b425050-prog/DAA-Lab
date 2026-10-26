#ifndef LAB5_Q3_QUICKSORT_H
#define LAB5_Q3_QUICKSORT_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t comparisons;
    uint64_t swaps;
    uint64_t partitions;
    uint64_t partitioned_values;
    size_t max_stack_frames;
} Q3Metrics;

typedef struct {
    size_t count;
    uint64_t value_sum;
    uint64_t mixed_xor;
    uint64_t mixed_sum;
} Q3Fingerprint;

/* SplitMix64 is small, deterministic, and identical on every C17 platform
   that provides uint64_t. It is used for both data and pivot generation. */
static uint64_t q3_splitmix64_next(uint64_t *state) {
    uint64_t z;
    *state += UINT64_C(0x9e3779b97f4a7c15);
    z = *state;
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}

static uint64_t q3_mix64(uint64_t z) {
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}

static uint64_t q3_data_state(uint64_t seed, size_t n) {
    return q3_mix64(seed ^ q3_mix64((uint64_t)n +
                                    UINT64_C(0xd1b54a32d192ed03)));
}

static uint64_t q3_sort_state(uint64_t seed, size_t n) {
    return q3_mix64(seed ^ q3_mix64((uint64_t)n +
                                    UINT64_C(0xa0761d6478bd642f)));
}

/* Rejection sampling removes the tiny modulo bias in pivot selection. */
static size_t q3_bounded_random(uint64_t *state, size_t bound) {
    uint64_t limit = (uint64_t)bound;
    uint64_t threshold = (uint64_t)(0 - limit) % limit;
    uint64_t value;
    do {
        value = q3_splitmix64_next(state);
    } while (value < threshold);
    return (size_t)(value % limit);
}

static void q3_generate_values(long long *values, size_t n, uint64_t seed) {
    uint64_t state = q3_data_state(seed, n);
    for (size_t i = 0; i < n; ++i) {
        uint64_t raw = q3_splitmix64_next(&state);
        if ((raw & UINT64_C(7)) == 0) {
            /* A small duplicate pool intentionally exercises 3-way partitioning. */
            values[i] = (long long)((raw >> 8) % UINT64_C(41)) - 20LL;
        } else {
            values[i] = (long long)((raw >> 8) % UINT64_C(2000000001)) -
                        1000000000LL;
        }
    }
}

static uint64_t q3_rotate_left(uint64_t value, unsigned shift) {
    return shift == 0 ? value : (value << shift) | (value >> (64U - shift));
}

static Q3Fingerprint q3_fingerprint(const long long *values, size_t n) {
    Q3Fingerprint result = {n, 0, 0, 0};
    for (size_t i = 0; i < n; ++i) {
        uint64_t bits = (uint64_t)values[i];
        uint64_t mixed = q3_mix64(bits ^ UINT64_C(0x243f6a8885a308d3));
        result.value_sum += bits;
        result.mixed_sum += mixed;
        result.mixed_xor ^= q3_rotate_left(mixed, (unsigned)(mixed & 63U));
    }
    return result;
}

static int q3_same_fingerprint(Q3Fingerprint a, Q3Fingerprint b) {
    return a.count == b.count && a.value_sum == b.value_sum &&
           a.mixed_xor == b.mixed_xor && a.mixed_sum == b.mixed_sum;
}

static int q3_is_sorted(const long long *values, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        if (values[i - 1] > values[i]) return 0;
    }
    return 1;
}

static void q3_swap(long long *values, size_t first, size_t second,
                    Q3Metrics *metrics) {
    if (first != second) {
        long long temporary = values[first];
        values[first] = values[second];
        values[second] = temporary;
        ++metrics->swaps;
    }
}

/* Sort half-open range [lo, hi). Dijkstra's partition produces
   [lo, lt) < pivot, [lt, gt) == pivot, and [gt, hi) > pivot. */
static void q3_quicksort_range(long long *values, size_t lo, size_t hi,
                               uint64_t *pivot_state, Q3Metrics *metrics,
                               size_t stack_frames) {
    if (stack_frames > metrics->max_stack_frames) {
        metrics->max_stack_frames = stack_frames;
    }

    while (hi - lo > 1) {
        size_t length = hi - lo;
        size_t pivot_index = lo + q3_bounded_random(pivot_state, length);
        long long pivot = values[pivot_index];
        size_t lt = lo;
        size_t scan = lo;
        size_t gt = hi;

        ++metrics->partitions;
        metrics->partitioned_values += (uint64_t)length;

        while (scan < gt) {
            ++metrics->comparisons;
            if (values[scan] < pivot) {
                q3_swap(values, lt, scan, metrics);
                ++lt;
                ++scan;
            } else {
                ++metrics->comparisons;
                if (values[scan] > pivot) {
                    --gt;
                    q3_swap(values, scan, gt, metrics);
                } else {
                    ++scan;
                }
            }
        }

        /* Recurse only into the smaller strict partition. The larger one is
           handled by this loop, which bounds stack use independently of pivots. */
        if (lt - lo < hi - gt) {
            if (lt - lo > 1) {
                q3_quicksort_range(values, lo, lt, pivot_state, metrics,
                                   stack_frames + 1);
            }
            lo = gt;
        } else {
            if (hi - gt > 1) {
                q3_quicksort_range(values, gt, hi, pivot_state, metrics,
                                   stack_frames + 1);
            }
            hi = lt;
        }
    }
}

static void q3_quicksort(long long *values, size_t n, uint64_t seed,
                         Q3Metrics *metrics) {
    uint64_t pivot_state = q3_sort_state(seed, n);
    *metrics = (Q3Metrics){0, 0, 0, 0, 0};
    if (n == 0) return;
    q3_quicksort_range(values, 0, n, &pivot_state, metrics, 1);
}

static size_t q3_stack_frame_bound(size_t n) {
    size_t frames = 0;
    while (n > 0) {
        ++frames;
        n /= 2;
    }
    return frames;
}

#endif
