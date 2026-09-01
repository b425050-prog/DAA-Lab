#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "q2_algorithms.h"

int main(void) {
    const double test[] = {6,2,1, 2,3,1, 1,1,1};
    const double identity[] = {1,0,0, 0,1,0, 0,0,1};
    double product[9], copy[9], vector[3], eigenvalue = 0.0; size_t iterations = 0U;
    Q2Metrics check = {0U};
    q2_multiply(test, identity, product, 3U, &check);
    int valid = 1;
    for (size_t i = 0U; i < 9U; ++i) if (fabs(product[i] - test[i]) > 1e-10) valid = 0;
    memcpy(copy, test, sizeof test); q2_transpose_in_place(copy, 3U, &check); q2_transpose_in_place(copy, 3U, &check);
    for (size_t i = 0U; i < 9U; ++i) if (fabs(copy[i] - test[i]) > 1e-10) valid = 0;
    if (fabs(q2_determinant(test, 3U, &check) - 9.0) > 1e-9) valid = 0;
    const double diag[] = {9,0,0, 0,4,0, 0,0,1};
    if (!q2_dominant_eigenpair(diag, 3U, 1000U, 1e-12, &eigenvalue, vector, &iterations, &check) || fabs(eigenvalue - 9.0) > 1e-7) valid = 0;
    printf("Identity, double-transpose, determinant and eigenpair checks: %s\n", valid ? "PASS" : "FAIL");

    FILE *data = fopen("q2_experimental_data.dat", "w"); if (!data) return 1;
    fprintf(data, "# n measured_operations cubic_reference valid\n");
    for (size_t n = 4U; n <= 128U; n *= 2U) {
        double *a = calloc(n*n, sizeof *a), *b = calloc(n*n, sizeof *b), *c = calloc(n*n, sizeof *c);
        if (!a || !b || !c) return 1;
        for (size_t i = 0U; i < n; ++i) for (size_t j = 0U; j < n; ++j) {
            a[i*n+j] = i == j ? 3.0 : (double)((i + j) % 5U) / 10.0;
            b[i*n+j] = i == j ? 2.0 : (double)((i * 3U + j) % 7U) / 10.0;
        }
        Q2Metrics m = {0U}; q2_multiply(a, b, c, n, &m); double det = q2_determinant(a, n, &m);
        int row_valid = isfinite(det) && isfinite(c[0]);
        double reference = 3.0 * (double)n * (double)n * (double)n;
        fprintf(data, "%zu %llu %.3f %d\n", n, m.operations, reference, row_valid);
        printf("n=%-4zu work=%-12llu validation=%s\n", n, m.operations, row_valid ? "PASS" : "FAIL");
        valid = valid && row_valid; free(a); free(b); free(c);
    }
    fclose(data); return valid ? 0 : 2;
}
