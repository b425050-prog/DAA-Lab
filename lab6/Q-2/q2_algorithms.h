#ifndef LAB6_Q2_ALGORITHMS_H
#define LAB6_Q2_ALGORITHMS_H

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct { unsigned long long operations; } Q2Metrics;

static inline void q2_add(const double *a, const double *b, double *c, size_t n,
                   Q2Metrics *m) {
    for (size_t i = 0U; i < n * n; ++i) { c[i] = a[i] + b[i]; if (m) ++m->operations; }
}

static inline void q2_multiply(const double *a, const double *b, double *c, size_t n,
                        Q2Metrics *m) {
    memset(c, 0, n * n * sizeof *c);
    for (size_t i = 0U; i < n; ++i)
        for (size_t k = 0U; k < n; ++k)
            for (size_t j = 0U; j < n; ++j) {
                c[i*n+j] += a[i*n+k] * b[k*n+j];
                if (m) m->operations += 2U;
            }
}

static inline int q2_is_zero(const double *a, size_t n, Q2Metrics *m) {
    for (size_t i = 0U; i < n*n; ++i) { if (m) ++m->operations; if (fabs(a[i]) > 1e-10) return 0; }
    return 1;
}

static inline int q2_is_symmetric(const double *a, size_t n, Q2Metrics *m) {
    for (size_t i = 0U; i < n; ++i)
        for (size_t j = i + 1U; j < n; ++j) {
            if (m) ++m->operations;
            if (fabs(a[i*n+j] - a[j*n+i]) > 1e-10) return 0;
        }
    return 1;
}

static inline void q2_transpose_in_place(double *a, size_t n, Q2Metrics *m) {
    for (size_t i = 0U; i < n; ++i)
        for (size_t j = i + 1U; j < n; ++j) {
            double t = a[i*n+j]; a[i*n+j] = a[j*n+i]; a[j*n+i] = t;
            if (m) m->operations += 3U;
        }
}

static inline double q2_determinant(const double *a, size_t n, Q2Metrics *m) {
    double *work = malloc(n*n*sizeof *work);
    if (!work) return NAN;
    memcpy(work, a, n*n*sizeof *work);
    double det = 1.0;
    for (size_t col = 0U; col < n; ++col) {
        size_t pivot = col;
        for (size_t row = col + 1U; row < n; ++row) {
            if (m) ++m->operations;
            if (fabs(work[row*n+col]) > fabs(work[pivot*n+col])) pivot = row;
        }
        if (fabs(work[pivot*n+col]) < 1e-12) { free(work); return 0.0; }
        if (pivot != col) {
            for (size_t j = 0U; j < n; ++j) {
                double t = work[col*n+j]; work[col*n+j] = work[pivot*n+j]; work[pivot*n+j] = t;
            }
            det = -det;
        }
        double diagonal = work[col*n+col]; det *= diagonal;
        for (size_t row = col + 1U; row < n; ++row) {
            double factor = work[row*n+col] / diagonal;
            for (size_t j = col + 1U; j < n; ++j) {
                work[row*n+j] -= factor * work[col*n+j];
                if (m) m->operations += 3U;
            }
        }
    }
    free(work); return det;
}

static inline int q2_dominant_eigenpair(const double *a, size_t n, size_t max_iter,
                                 double tolerance, double *eigenvalue,
                                 double *vector, size_t *iterations,
                                 Q2Metrics *m) {
    double *next = malloc(n * sizeof *next);
    if (!next) return 0;
    double inv = 1.0 / sqrt((double)n);
    for (size_t i = 0U; i < n; ++i) vector[i] = inv;
    double lambda = 0.0, previous = 0.0;
    for (size_t iter = 1U; iter <= max_iter; ++iter) {
        for (size_t i = 0U; i < n; ++i) {
            next[i] = 0.0;
            for (size_t j = 0U; j < n; ++j) { next[i] += a[i*n+j] * vector[j]; if (m) m->operations += 2U; }
        }
        double norm = 0.0;
        for (size_t i = 0U; i < n; ++i) norm += next[i] * next[i];
        norm = sqrt(norm);
        if (norm < 1e-14) { free(next); return 0; }
        for (size_t i = 0U; i < n; ++i) vector[i] = next[i] / norm;
        lambda = 0.0;
        for (size_t i = 0U; i < n; ++i) {
            double row = 0.0;
            for (size_t j = 0U; j < n; ++j) row += a[i*n+j] * vector[j];
            lambda += vector[i] * row;
        }
        if (iter > 1U && fabs(lambda - previous) <= tolerance * (1.0 + fabs(lambda))) {
            *eigenvalue = lambda; *iterations = iter; free(next); return 1;
        }
        previous = lambda;
    }
    *eigenvalue = lambda; *iterations = max_iter; free(next); return 1;
}

#endif
