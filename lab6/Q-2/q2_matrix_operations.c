#include <stdio.h>
#include <stdlib.h>

#include "q2_algorithms.h"

static void print_matrix(const double *a, size_t n) {
    for (size_t i = 0U; i < n; ++i) { for (size_t j = 0U; j < n; ++j) printf("%9.3f", a[i*n+j]); putchar('\n'); }
}

int main(void) {
    size_t n = 0U;
    printf("DAA Lab 06 - Q2: Square Matrix Operations\nEnter matrix order n: ");
    if (scanf("%zu", &n) != 1 || n == 0U || n > 100U) return 1;
    double *a = malloc(n*n*sizeof *a), *b = malloc(n*n*sizeof *b);
    double *c = malloc(n*n*sizeof *c), *copy = malloc(n*n*sizeof *copy);
    double *eigenvector = malloc(n*sizeof *eigenvector);
    if (!a || !b || !c || !copy || !eigenvector) return 1;
    printf("Enter matrix A (%zu values): ", n*n);
    for (size_t i = 0U; i < n*n; ++i) if (scanf("%lf", &a[i]) != 1) return 1;
    printf("Enter matrix B (%zu values): ", n*n);
    for (size_t i = 0U; i < n*n; ++i) if (scanf("%lf", &b[i]) != 1) return 1;
    Q2Metrics m = {0U};
    q2_add(a, b, c, n, &m); printf("\nA + B:\n"); print_matrix(c, n);
    q2_multiply(a, b, c, n, &m); printf("A x B:\n"); print_matrix(c, n);
    printf("A is zero      : %s\n", q2_is_zero(a, n, &m) ? "yes" : "no");
    printf("A is symmetric : %s\n", q2_is_symmetric(a, n, &m) ? "yes" : "no");
    printf("det(A)         : %.8g\n", q2_determinant(a, n, &m));
    memcpy(copy, a, n*n*sizeof *copy); q2_transpose_in_place(copy, n, &m);
    printf("transpose(A):\n"); print_matrix(copy, n);
    double eigenvalue = 0.0; size_t iterations = 0U;
    if (q2_dominant_eigenpair(a, n, 1000U, 1e-10, &eigenvalue, eigenvector, &iterations, &m)) {
        printf("Dominant eigenvalue (power iteration): %.8g\nEigenvector: ", eigenvalue);
        for (size_t i = 0U; i < n; ++i) printf("%s%.6f", i ? " " : "", eigenvector[i]);
        printf("\nIterations: %zu\n", iterations);
    } else printf("Dominant eigenpair: undefined for the zero iterate.\n");
    printf("Complexities: add/zero/symmetry/transpose Theta(n^2); multiply/determinant Theta(n^3); power iteration Theta(k n^2)\n");
    printf("Counted arithmetic/comparison work: %llu\n", m.operations);
    free(a); free(b); free(c); free(copy); free(eigenvector); return 0;
}
