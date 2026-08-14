/*
 * DAA Laboratory - Lab 03 - Question 5
 * O(n^2) multiplication for recursively structured matrices
 *
 * Every matrix has the form
 *              [ M1  M2 ]
 *          M = [ M2  M1 ]
 * and M1, M2 recursively have the same form.
 *
 * For A=[A1 A2; A2 A1] and B=[B1 B2; B2 B1],
 * the product is C=[C1 C2; C2 C1], where
 *   C1 = A1B1 + A2B2
 *   C2 = A1B2 + A2B1.
 *
 * Define only TWO recursive products:
 *   P = (A1 + A2)(B1 + B2) = C1 + C2
 *   Q = (A1 - A2)(B1 - B2) = C1 - C2
 * Then
 *   C1 = (P + Q)/2,
 *   C2 = (P - Q)/2.
 *
 * Recurrence: T(n)=2T(n/2)+Theta(n^2)=Theta(n^2).
 */

#include <stdio.h>
#include <stdlib.h>

typedef long long i64;

typedef struct {
    unsigned long long scalar_multiplications;
    unsigned long long additions_subtractions;
    unsigned long long divisions_by_two;
    unsigned long long recursive_calls;
} Metrics;

static i64 *new_matrix(int n) {
    return (i64 *)calloc((size_t)n * (size_t)n, sizeof(i64));
}

static int is_power_of_two(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

static void copy_block(const i64 *src,int sn,int r0,int c0,i64 *dst,int n){
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) dst[i*n+j]=src[(r0+i)*sn+c0+j];
}

static void put_block(i64 *dst,int dn,int r0,int c0,const i64 *src,int n){
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) dst[(r0+i)*dn+c0+j]=src[i*n+j];
}

static void add_mat(const i64 *A,const i64 *B,i64 *C,int n,Metrics *m){
    for(int i=0;i<n*n;i++){C[i]=A[i]+B[i];m->additions_subtractions++;}
}

static void sub_mat(const i64 *A,const i64 *B,i64 *C,int n,Metrics *m){
    for(int i=0;i<n*n;i++){C[i]=A[i]-B[i];m->additions_subtractions++;}
}

static int check_special(const i64 *M,int n){
    if(n==1) return 1;
    int h=n/2;
    for(int i=0;i<h;i++) for(int j=0;j<h;j++){
        if(M[i*n+j]!=M[(i+h)*n+(j+h)]) return 0;
        if(M[i*n+(j+h)]!=M[(i+h)*n+j]) return 0;
    }
    i64 *M1=new_matrix(h),*M2=new_matrix(h); if(!M1||!M2) exit(2);
    copy_block(M,n,0,0,M1,h); copy_block(M,n,0,h,M2,h);
    int ok=check_special(M1,h)&&check_special(M2,h);
    free(M1);free(M2);return ok;
}

static i64 *special_multiply(const i64 *A,const i64 *B,int n,Metrics *m){
    m->recursive_calls++;
    i64 *C=new_matrix(n); if(!C)return NULL;
    if(n==1){C[0]=A[0]*B[0];m->scalar_multiplications++;return C;}

    int h=n/2;
    i64 *A1=new_matrix(h),*A2=new_matrix(h),*B1=new_matrix(h),*B2=new_matrix(h);
    i64 *SA=new_matrix(h),*SB=new_matrix(h),*DA=new_matrix(h),*DB=new_matrix(h);
    if(!A1||!A2||!B1||!B2||!SA||!SB||!DA||!DB)exit(2);
    copy_block(A,n,0,0,A1,h);copy_block(A,n,0,h,A2,h);
    copy_block(B,n,0,0,B1,h);copy_block(B,n,0,h,B2,h);

    add_mat(A1,A2,SA,h,m); add_mat(B1,B2,SB,h,m);
    sub_mat(A1,A2,DA,h,m); sub_mat(B1,B2,DB,h,m);
    i64 *P=special_multiply(SA,SB,h,m);
    i64 *Q=special_multiply(DA,DB,h,m);
    i64 *C1=new_matrix(h),*C2=new_matrix(h);
    if(!P||!Q||!C1||!C2)exit(2);

    for(int i=0;i<h*h;i++){
        i64 sum=P[i]+Q[i];
        i64 dif=P[i]-Q[i];
        m->additions_subtractions+=2;
        if((sum&1LL)||(dif&1LL)){
            fprintf(stderr,"Internal parity error: exact division by 2 expected.\n");
            exit(3);
        }
        C1[i]=sum/2; C2[i]=dif/2;
        m->divisions_by_two+=2;
    }

    put_block(C,n,0,0,C1,h); put_block(C,n,h,h,C1,h);
    put_block(C,n,0,h,C2,h); put_block(C,n,h,0,C2,h);

    free(A1);free(A2);free(B1);free(B2);free(SA);free(SB);free(DA);free(DB);
    free(P);free(Q);free(C1);free(C2);
    return C;
}

static void classical(const i64 *A,const i64 *B,i64 *C,int n){
    for(int i=0;i<n;i++)for(int k=0;k<n;k++)for(int j=0;j<n;j++)C[i*n+j]+=A[i*n+k]*B[k*n+j];
}

static void print_matrix(const i64 *A,int n){
    for(int i=0;i<n;i++){for(int j=0;j<n;j++)printf("%8lld ",A[i*n+j]);putchar('\n');}
}

int main(void){
    int n;
    printf("============================================================\n");
    printf(" DAA Lab 03 - Q5: Special-Pattern Matrix Multiplication\n");
    printf("============================================================\n");
    printf("Enter n (power of two): ");
    if(scanf("%d",&n)!=1||!is_power_of_two(n)){printf("n must be a positive power of two.\n");return 1;}

    i64 *A=new_matrix(n),*B=new_matrix(n),*D=new_matrix(n);if(!A||!B||!D)return 1;
    printf("Enter special-pattern matrix A (%d x %d):\n",n,n);
    for(int i=0;i<n*n;i++)scanf("%lld",&A[i]);
    printf("Enter special-pattern matrix B (%d x %d):\n",n,n);
    for(int i=0;i<n*n;i++)scanf("%lld",&B[i]);

    if(!check_special(A,n)||!check_special(B,n)){
        printf("Input rejected: one or both matrices do not satisfy the recursive pattern.\n");
        free(A);free(B);free(D);return 1;
    }

    Metrics m={0,0,0,0};
    i64 *C=special_multiply(A,B,n,&m); classical(A,B,D,n);
    int ok=1;for(int i=0;i<n*n;i++)if(C[i]!=D[i])ok=0;

    printf("\nProduct matrix:\n");print_matrix(C,n);
    printf("\nScalar multiplications : %llu\n",m.scalar_multiplications);
    printf("Add/sub operations     : %llu\n",m.additions_subtractions);
    printf("Exact /2 operations    : %llu\n",m.divisions_by_two);
    printf("Recursive calls        : %llu\n",m.recursive_calls);
    printf("Classical cross-check  : %s\n",ok?"PASS":"FAIL");
    printf("Complexity             : T(n)=2T(n/2)+Theta(n^2)=Theta(n^2)\n");
    printf("The full n x n output itself contains n^2 entries, so Theta(n^2) is output-optimal.\n");

    free(A);free(B);free(C);free(D);return ok?0:2;
}
