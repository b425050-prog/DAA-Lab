#include <stdio.h>
#include <stdlib.h>

typedef long long ll;

typedef struct {
    int n;
    ll *v;
} Matrix;

static unsigned long long scalar_multiplications = 0;
static unsigned long long scalar_add_sub = 0;

static Matrix matrix_new(int n) {
    Matrix m;
    m.n = n;
    m.v = (ll *)calloc((size_t)n * (size_t)n, sizeof(ll));
    if (!m.v) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    return m;
}

static void matrix_free(Matrix *m) {
    free(m->v);
    m->v = NULL;
    m->n = 0;
}

static ll get(const Matrix *m, int r, int c) {
    return m->v[(size_t)r * (size_t)m->n + (size_t)c];
}

static void set(Matrix *m, int r, int c, ll x) {
    m->v[(size_t)r * (size_t)m->n + (size_t)c] = x;
}

static Matrix add(const Matrix *a, const Matrix *b, int sign) {
    Matrix c = matrix_new(a->n);
    int n = a->n;
    for (int i = 0; i < n * n; ++i) {
        c.v[i] = a->v[i] + (ll)sign * b->v[i];
        ++scalar_add_sub;
    }
    return c;
}

static Matrix block(const Matrix *a, int row0, int col0, int size) {
    Matrix b = matrix_new(size);
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            set(&b, i, j, get(a, row0 + i, col0 + j));
    return b;
}

static void place_block(Matrix *dst, const Matrix *src, int row0, int col0) {
    for (int i = 0; i < src->n; ++i)
        for (int j = 0; j < src->n; ++j)
            set(dst, row0 + i, col0 + j, get(src, i, j));
}

static Matrix strassen(const Matrix *a, const Matrix *b) {
    int n = a->n;
    Matrix c = matrix_new(n);

    if (n == 1) {
        c.v[0] = a->v[0] * b->v[0];
        ++scalar_multiplications;
        return c;
    }

    int h = n / 2;
    Matrix A11 = block(a, 0, 0, h);
    Matrix A12 = block(a, 0, h, h);
    Matrix A21 = block(a, h, 0, h);
    Matrix A22 = block(a, h, h, h);
    Matrix B11 = block(b, 0, 0, h);
    Matrix B12 = block(b, 0, h, h);
    Matrix B21 = block(b, h, 0, h);
    Matrix B22 = block(b, h, h, h);

    Matrix T1, T2;

    T1 = add(&A11, &A22, +1);
    T2 = add(&B11, &B22, +1);
    Matrix M1 = strassen(&T1, &T2);
    matrix_free(&T1); matrix_free(&T2);

    T1 = add(&A21, &A22, +1);
    Matrix M2 = strassen(&T1, &B11);
    matrix_free(&T1);

    T1 = add(&B12, &B22, -1);
    Matrix M3 = strassen(&A11, &T1);
    matrix_free(&T1);

    T1 = add(&B21, &B11, -1);
    Matrix M4 = strassen(&A22, &T1);
    matrix_free(&T1);

    T1 = add(&A11, &A12, +1);
    Matrix M5 = strassen(&T1, &B22);
    matrix_free(&T1);

    T1 = add(&A21, &A11, -1);
    T2 = add(&B11, &B12, +1);
    Matrix M6 = strassen(&T1, &T2);
    matrix_free(&T1); matrix_free(&T2);

    T1 = add(&A12, &A22, -1);
    T2 = add(&B21, &B22, +1);
    Matrix M7 = strassen(&T1, &T2);
    matrix_free(&T1); matrix_free(&T2);

    Matrix U1 = add(&M1, &M4, +1);
    Matrix U2 = add(&U1, &M5, -1);
    Matrix C11 = add(&U2, &M7, +1);
    matrix_free(&U1); matrix_free(&U2);

    Matrix C12 = add(&M3, &M5, +1);
    Matrix C21 = add(&M2, &M4, +1);

    U1 = add(&M1, &M2, -1);
    U2 = add(&U1, &M3, +1);
    Matrix C22 = add(&U2, &M6, +1);
    matrix_free(&U1); matrix_free(&U2);

    place_block(&c, &C11, 0, 0);
    place_block(&c, &C12, 0, h);
    place_block(&c, &C21, h, 0);
    place_block(&c, &C22, h, h);

    Matrix *all[] = {&A11,&A12,&A21,&A22,&B11,&B12,&B21,&B22,
                     &M1,&M2,&M3,&M4,&M5,&M6,&M7,&C11,&C12,&C21,&C22};
    int count = (int)(sizeof(all) / sizeof(all[0]));
    for (int i = 0; i < count; ++i) matrix_free(all[i]);

    return c;
}

static int next_power_of_two(int n) {
    int p = 1;
    while (p < n) p <<= 1;
    return p;
}

static Matrix padded_copy(const Matrix *src, int p) {
    Matrix dst = matrix_new(p);
    for (int i = 0; i < src->n; ++i)
        for (int j = 0; j < src->n; ++j)
            set(&dst, i, j, get(src, i, j));
    return dst;
}

static Matrix classical_multiply(const Matrix *a, const Matrix *b) {
    Matrix c = matrix_new(a->n);
    int n = a->n;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            ll sum = 0;
            for (int k = 0; k < n; ++k)
                sum += get(a, i, k) * get(b, k, j);
            set(&c, i, j, sum);
        }
    return c;
}

static int same_top_left(const Matrix *a, const Matrix *b, int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (get(a, i, j) != get(b, i, j)) return 0;
    return 1;
}

static void print_top_left(const Matrix *a, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            printf("%lld%c", get(a, i, j), j + 1 == n ? '\n' : ' ');
    }
}

int main(void) {
    int n;
    printf("Matrix Multiplication using Strassen's Method\n");
    printf("Enter square-matrix size n: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Invalid size.\n");
        return 1;
    }

    Matrix A = matrix_new(n), B = matrix_new(n);
    printf("Enter matrix A (%d x %d):\n", n, n);
    for (int i = 0; i < n * n; ++i)
        if (scanf("%lld", &A.v[i]) != 1) { fprintf(stderr, "Invalid input.\n"); return 1; }

    printf("Enter matrix B (%d x %d):\n", n, n);
    for (int i = 0; i < n * n; ++i)
        if (scanf("%lld", &B.v[i]) != 1) { fprintf(stderr, "Invalid input.\n"); return 1; }

    int p = next_power_of_two(n);
    Matrix PA = padded_copy(&A, p);
    Matrix PB = padded_copy(&B, p);

    scalar_multiplications = 0;
    scalar_add_sub = 0;
    Matrix PC = strassen(&PA, &PB);

    printf("\nProduct A x B:\n");
    print_top_left(&PC, n);

    Matrix check = classical_multiply(&A, &B);
    printf("\nValidation against classical multiplication: %s\n",
           same_top_left(&PC, &check, n) ? "PASSED" : "FAILED");
    printf("Padded working size: %d\n", p);
    printf("Strassen scalar multiplications: %llu\n", scalar_multiplications);
    printf("Strassen scalar additions/subtractions: %llu\n", scalar_add_sub);
    printf("Asymptotic recurrence: T(n) = 7T(n/2) + Theta(n^2) = Theta(n^log2(7)).\n");

    matrix_free(&A); matrix_free(&B); matrix_free(&PA); matrix_free(&PB);
    matrix_free(&PC); matrix_free(&check);
    return 0;
}
