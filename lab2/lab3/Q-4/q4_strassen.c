/*
 * DAA Laboratory - Lab 03 - Question 4
 * Square Matrix Multiplication using Strassen's Divide & Conquer method.
 *
 * The input size may be any positive n. If n is not a power of two,
 * both matrices are padded with zeros to the next power of two, Strassen
 * multiplication is performed, and the original n x n result is printed.
 * A classical O(n^3) multiplication is also computed only to validate the
 * numerical result.
 */

#include <stdio.h>
#include <stdlib.h>

typedef long long i64;

typedef struct {
    unsigned long long scalar_multiplications;
    unsigned long long additions_subtractions;
    unsigned long long recursive_calls;
} Metrics;

static i64 *mat_new(int n) {
    return (i64 *)calloc((size_t)n * (size_t)n, sizeof(i64));
}

static void block_copy(const i64 *src, int src_n, int r0, int c0, i64 *dst, int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            dst[i*n + j] = src[(r0+i)*src_n + (c0+j)];
}

static void block_put(i64 *dst, int dst_n, int r0, int c0, const i64 *src, int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            dst[(r0+i)*dst_n + (c0+j)] = src[i*n + j];
}

static void mat_add(const i64 *A, const i64 *B, i64 *C, int n, Metrics *m) {
    for (int i = 0; i < n*n; ++i) {
        C[i] = A[i] + B[i];
        m->additions_subtractions++;
    }
}

static void mat_sub(const i64 *A, const i64 *B, i64 *C, int n, Metrics *m) {
    for (int i = 0; i < n*n; ++i) {
        C[i] = A[i] - B[i];
        m->additions_subtractions++;
    }
}

static i64 *strassen(const i64 *A, const i64 *B, int n, Metrics *m) {
    m->recursive_calls++;
    i64 *C = mat_new(n);
    if (!C) return NULL;

    if (n == 1) {
        C[0] = A[0] * B[0];
        m->scalar_multiplications++;
        return C;
    }

    int h = n / 2;
    i64 *A11=mat_new(h), *A12=mat_new(h), *A21=mat_new(h), *A22=mat_new(h);
    i64 *B11=mat_new(h), *B12=mat_new(h), *B21=mat_new(h), *B22=mat_new(h);
    i64 *T1=mat_new(h), *T2=mat_new(h);
    if(!A11||!A12||!A21||!A22||!B11||!B12||!B21||!B22||!T1||!T2) exit(2);

    block_copy(A,n,0,0,A11,h); block_copy(A,n,0,h,A12,h);
    block_copy(A,n,h,0,A21,h); block_copy(A,n,h,h,A22,h);
    block_copy(B,n,0,0,B11,h); block_copy(B,n,0,h,B12,h);
    block_copy(B,n,h,0,B21,h); block_copy(B,n,h,h,B22,h);

    mat_add(A11,A22,T1,h,m); mat_add(B11,B22,T2,h,m);
    i64 *M1=strassen(T1,T2,h,m);
    mat_add(A21,A22,T1,h,m);
    i64 *M2=strassen(T1,B11,h,m);
    mat_sub(B12,B22,T2,h,m);
    i64 *M3=strassen(A11,T2,h,m);
    mat_sub(B21,B11,T2,h,m);
    i64 *M4=strassen(A22,T2,h,m);
    mat_add(A11,A12,T1,h,m);
    i64 *M5=strassen(T1,B22,h,m);
    mat_sub(A21,A11,T1,h,m); mat_add(B11,B12,T2,h,m);
    i64 *M6=strassen(T1,T2,h,m);
    mat_sub(A12,A22,T1,h,m); mat_add(B21,B22,T2,h,m);
    i64 *M7=strassen(T1,T2,h,m);

    i64 *C11=mat_new(h), *C12=mat_new(h), *C21=mat_new(h), *C22=mat_new(h);
    i64 *U=mat_new(h);
    if(!M1||!M2||!M3||!M4||!M5||!M6||!M7||!C11||!C12||!C21||!C22||!U) exit(2);

    mat_add(M1,M4,U,h,m); mat_sub(U,M5,T1,h,m); mat_add(T1,M7,C11,h,m);
    mat_add(M3,M5,C12,h,m);
    mat_add(M2,M4,C21,h,m);
    mat_sub(M1,M2,U,h,m); mat_add(U,M3,T1,h,m); mat_add(T1,M6,C22,h,m);

    block_put(C,n,0,0,C11,h); block_put(C,n,0,h,C12,h);
    block_put(C,n,h,0,C21,h); block_put(C,n,h,h,C22,h);

    free(A11);free(A12);free(A21);free(A22);free(B11);free(B12);free(B21);free(B22);
    free(T1);free(T2);free(M1);free(M2);free(M3);free(M4);free(M5);free(M6);free(M7);
    free(C11);free(C12);free(C21);free(C22);free(U);
    return C;
}

static void classical(const i64 *A, const i64 *B, i64 *C, int n) {
    for (int i=0;i<n;i++)
        for (int k=0;k<n;k++)
            for (int j=0;j<n;j++)
                C[i*n+j] += A[i*n+k]*B[k*n+j];
}

static int next_power_of_two(int n) {
    int p=1; while(p<n) p<<=1; return p;
}

static void print_matrix(const i64 *A, int n, int stride) {
    for(int i=0;i<n;i++) {
        for(int j=0;j<n;j++) printf("%8lld ", A[i*stride+j]);
        putchar('\n');
    }
}

int main(void) {
    int n;
    printf("============================================================\n");
    printf(" DAA Lab 03 - Q4: Strassen Matrix Multiplication\n");
    printf("============================================================\n");
    printf("Enter matrix order n: ");
    if(scanf("%d",&n)!=1 || n<=0) return 1;

    int p=next_power_of_two(n);
    i64 *A=mat_new(p), *B=mat_new(p), *check=mat_new(n);
    if(!A||!B||!check) return 1;

    printf("Enter matrix A (%d x %d):\n",n,n);
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) scanf("%lld",&A[i*p+j]);
    printf("Enter matrix B (%d x %d):\n",n,n);
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) scanf("%lld",&B[i*p+j]);

    Metrics m={0,0,0};
    i64 *C=strassen(A,B,p,&m);
    if(!C) return 1;

    i64 *Ao=mat_new(n), *Bo=mat_new(n);
    for(int i=0;i<n;i++) for(int j=0;j<n;j++){Ao[i*n+j]=A[i*p+j];Bo[i*n+j]=B[i*p+j];}
    classical(Ao,Bo,check,n);

    int ok=1;
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) if(C[i*p+j]!=check[i*n+j]) ok=0;

    printf("\nA x B using Strassen:\n");
    print_matrix(C,n,p);
    printf("\nPadded order used          : %d\n",p);
    printf("Scalar multiplications     : %llu\n",m.scalar_multiplications);
    printf("Additions/subtractions     : %llu\n",m.additions_subtractions);
    printf("Recursive calls            : %llu\n",m.recursive_calls);
    printf("Classical cross-check      : %s\n",ok?"PASS":"FAIL");
    printf("Complexity                 : T(n)=7T(n/2)+Theta(n^2)=Theta(n^log2(7))\n");
    printf("Numerically log2(7) ~= 2.8074, better than classical Theta(n^3).\n");

    free(A);free(B);free(C);free(check);free(Ao);free(Bo);
    return ok?0:2;
}
