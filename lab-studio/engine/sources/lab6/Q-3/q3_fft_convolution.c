#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "q3_fft.h"

int main(void) {
    size_t m = 0U, n = 0U;
    printf("DAA Lab 06 - Q3: FFT Convolution\nEnter m and n (n >= m): ");
    if (scanf("%zu%zu", &m, &n) != 2 || m == 0U || n < m || n > 1000000U) return 1;
    double *a = malloc(m*sizeof *a), *b = malloc(n*sizeof *b), *c = malloc((m+n-1U)*sizeof *c);
    if (!a || !b || !c) return 1;
    printf("Enter vector A: "); for (size_t i = 0U; i < m; ++i) if (scanf("%lf", &a[i]) != 1) return 1;
    printf("Enter vector B: "); for (size_t i = 0U; i < n; ++i) if (scanf("%lf", &b[i]) != 1) return 1;
    Q3Metrics metrics = {0U}; if (!q3_convolve(a, m, b, n, c, &metrics)) return 1;
    printf("Convolution C: ");
    for (size_t i = 0U; i < m+n-1U; ++i) {
        double value = fabs(c[i] - round(c[i])) < 1e-9 ? round(c[i]) : c[i];
        printf("%s%.10g", i ? " " : "", value);
    }
    printf("\nFFT butterflies: %llu\nTime: O((m+n) log(m+n)); space: O(m+n)\n", metrics.butterflies);
    free(a); free(b); free(c); return 0;
}
