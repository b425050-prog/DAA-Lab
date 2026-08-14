#include <stdio.h>
#include <stdlib.h>

typedef long long ll;

typedef struct {
    int n;
    ll *v;
} Matrix;

static unsigned long long scalar_mult = 0;
static unsigned long long scalar_add_sub = 0;
static unsigned long long scalar_div2 = 0;

static Matrix matrix_new(int n) {
    Matrix m = {n, (ll *)calloc((size_t)n * (size_t)n, sizeof(ll))};
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

static int is_power_of_two(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

static int is_special_region(const Matrix *m, int row, int col, int n) {
    if (n == 1) return 1;
    int h = n / 2;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < h; ++j) {
            if (get(m, row + i, col + j) != get(m, row + h + i, col + h + j))
                return 0;
            if (get(m, row + i, col + h + j) != get(m, row + h + i, col + j))
                return 0;
        }
    }

    return is_special_region(m, row, col, h) &&
           is_special_region(m, row, col + h, h);
}

static Matrix block(const Matrix *m, int row0, int col0, int n) {
    Matrix b = matrix_new(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            set(&b, i, j, get(m, row0 + i, col0 + j));
    return b;
}

static Matrix add_or_sub(const Matrix *a, const Matrix *b, int sign) {
    Matrix c = matrix_new(a->n);
    int total = a->n * a->n;
    for (int i = 0; i < total; ++i) {
        c.v[i] = a->v[i] + (ll)sign * b->v[i];
        ++scalar_add_sub;
    }
    return c;
}

static void place(Matrix *dst, const Matrix *src, int row0, int col0) {
    for (int i = 0; i < src->n; ++i)
        for (int j = 0; j < src->n; ++j)
            set(dst, row0 + i, col0 + j, get(src, i, j));
}

/*
   A = [A1 A2]   B = [B1 B2]
       [A2 A1]       [B2 B1]

   P = (A1 + A2)(B1 + B2) = C1 + C2
   Q = (A1 - A2)(B1 - B2) = C1 - C2

   Hence C1 = (P + Q)/2 and C2 = (P - Q)/2.
   Only TWO recursive multiplications are needed.
*/
static Matrix special_multiply(const Matrix *a, const Matrix *b) {
    int n = a->n;
    Matrix c = matrix_new(n);

    if (n == 1) {
        c.v[0] = a->v[0] * b->v[0];
        ++scalar_mult;
        return c;
    }

    int h = n / 2;
    Matrix A1 = block(a, 0, 0, h);
    Matrix A2 = block(a, 0, h, h);
    Matrix B1 = block(b, 0, 0, h);
    Matrix B2 = block(b, 0, h, h);

    Matrix Ap = add_or_sub(&A1, &A2, +1);
    Matrix Am = add_or_sub(&A1, &A2, -1);
    Matrix Bp = add_or_sub(&B1, &B2, +1);
    Matrix Bm = add_or_sub(&B1, &B2, -1);

    Matrix P = special_multiply(&Ap, &Bp);
    Matrix Q = special_multiply(&Am, &Bm);

    Matrix C1 = matrix_new(h);
    Matrix C2 = matrix_new(h);
    for (int i = 0; i < h * h; ++i) {
        ll sum = P.v[i] + Q.v[i];
        ll diff = P.v[i] - Q.v[i];
        scalar_add_sub += 2;

        C1.v[i] = sum / 2;
        C2.v[i] = diff / 2;
        scalar_div2 += 2;
    }

    place(&c, &C1, 0, 0);
    place(&c, &C2, 0, h);
    place(&c, &C2, h, 0);
    place(&c, &C1, h, h);

    Matrix *all[] = {&A1,&A2,&B1,&B2,&Ap,&Am,&Bp,&Bm,&P,&Q,&C1,&C2};
    int count = (int)(sizeof(all) / sizeof(all[0]));
    for (int i = 0; i < count; ++i) matrix_free(all[i]);

    return c;
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

static int equal_matrix(const Matrix *a, const Matrix *b) {
    for (int i = 0; i < a->n * a->n; ++i)
        if (a->v[i] != b->v[i]) return 0;
    return 1;
}

static void print_matrix(const Matrix *m) {
    for (int i = 0; i < m->n; ++i) {
        for (int j = 0; j < m->n; ++j)
            printf("%lld%c", get(m, i, j), j + 1 == m->n ? '\n' : ' ');
    }
}

int main(void) {
    int n;
    printf("Special-Pattern Matrix Multiplication using Divide and Conquer\n");
    printf("Enter n (power of 2): ");
    if (scanf("%d", &n) != 1 || !is_power_of_two(n)) {
        fprintf(stderr, "n must be a positive power of 2.\n");
        return 1;
    }

    Matrix A = matrix_new(n), B = matrix_new(n);
    printf("Enter special-pattern matrix A (%d x %d):\n", n, n);
    for (int i = 0; i < n * n; ++i)
        if (scanf("%lld", &A.v[i]) != 1) { fprintf(stderr, "Invalid input.\n"); return 1; }

    printf("Enter special-pattern matrix B (%d x %d):\n", n, n);
    for (int i = 0; i < n * n; ++i)
        if (scanf("%lld", &B.v[i]) != 1) { fprintf(stderr, "Invalid input.\n"); return 1; }

    if (!is_special_region(&A, 0, 0, n) || !is_special_region(&B, 0, 0, n)) {
        fprintf(stderr, "At least one input matrix does not satisfy the required recursive pattern.\n");
        matrix_free(&A); matrix_free(&B);
        return 1;
    }

    scalar_mult = scalar_add_sub = scalar_div2 = 0;
    Matrix C = special_multiply(&A, &B);
    Matrix check = classical_multiply(&A, &B);

    printf("\nProduct A x B:\n");
    print_matrix(&C);
    printf("\nValidation against classical multiplication: %s\n",
           equal_matrix(&C, &check) ? "PASSED" : "FAILED");
    printf("Recursive scalar multiplications = %llu\n", scalar_mult);
    printf("Scalar additions/subtractions = %llu\n", scalar_add_sub);
    printf("Divisions by 2 = %llu\n", scalar_div2);
    printf("Recurrence: T(n) = 2T(n/2) + Theta(n^2) = Theta(n^2).\n");

    matrix_free(&A); matrix_free(&B); matrix_free(&C); matrix_free(&check);
    return 0;
}
