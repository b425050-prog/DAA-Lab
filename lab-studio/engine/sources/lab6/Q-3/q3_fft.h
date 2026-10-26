#ifndef LAB6_Q3_FFT_H
#define LAB6_Q3_FFT_H

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct { double re, im; } Q3Complex;
typedef struct { unsigned long long butterflies; } Q3Metrics;

static inline Q3Complex q3_add(Q3Complex a, Q3Complex b) { Q3Complex c = {a.re+b.re,a.im+b.im}; return c; }
static inline Q3Complex q3_sub(Q3Complex a, Q3Complex b) { Q3Complex c = {a.re-b.re,a.im-b.im}; return c; }
static inline Q3Complex q3_mul(Q3Complex a, Q3Complex b) { Q3Complex c = {a.re*b.re-a.im*b.im,a.re*b.im+a.im*b.re}; return c; }

static inline void q3_fft(Q3Complex *a, size_t n, int inverse, Q3Metrics *metrics) {
    for (size_t i = 1U, j = 0U; i < n; ++i) {
        size_t bit = n >> 1U;
        for (; j & bit; bit >>= 1U) j ^= bit;
        j ^= bit;
        if (i < j) { Q3Complex t = a[i]; a[i] = a[j]; a[j] = t; }
    }
    const double pi = acos(-1.0);
    for (size_t len = 2U; len <= n; len <<= 1U) {
        double angle = 2.0 * pi / (double)len * (inverse ? -1.0 : 1.0);
        Q3Complex root = {cos(angle), sin(angle)};
        for (size_t i = 0U; i < n; i += len) {
            Q3Complex w = {1.0, 0.0};
            for (size_t j = 0U; j < len/2U; ++j) {
                Q3Complex even = a[i+j], odd = q3_mul(a[i+j+len/2U], w);
                a[i+j] = q3_add(even, odd); a[i+j+len/2U] = q3_sub(even, odd);
                w = q3_mul(w, root); if (metrics) ++metrics->butterflies;
            }
        }
    }
    if (inverse) for (size_t i = 0U; i < n; ++i) { a[i].re /= (double)n; a[i].im /= (double)n; }
}

static inline size_t q3_next_power_of_two(size_t n) { size_t p = 1U; while (p < n) p <<= 1U; return p; }

static inline int q3_convolve(const double *a, size_t m, const double *b, size_t n,
                       double *out, Q3Metrics *metrics) {
    if (!a || !b || !out || m == 0U || n == 0U) return 0;
    size_t length = m + n - 1U, padded = q3_next_power_of_two(length);
    Q3Complex *fa = calloc(padded, sizeof *fa), *fb = calloc(padded, sizeof *fb);
    if (!fa || !fb) { free(fa); free(fb); return 0; }
    for (size_t i = 0U; i < m; ++i) fa[i].re = a[i];
    for (size_t i = 0U; i < n; ++i) fb[i].re = b[i];
    q3_fft(fa, padded, 0, metrics); q3_fft(fb, padded, 0, metrics);
    for (size_t i = 0U; i < padded; ++i) fa[i] = q3_mul(fa[i], fb[i]);
    q3_fft(fa, padded, 1, metrics);
    for (size_t i = 0U; i < length; ++i) out[i] = fa[i].re;
    free(fa); free(fb); return 1;
}

#endif
