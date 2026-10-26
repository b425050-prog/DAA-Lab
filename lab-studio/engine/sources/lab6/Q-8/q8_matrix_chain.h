#ifndef LAB6_Q8_MATRIX_CHAIN_H
#define LAB6_Q8_MATRIX_CHAIN_H
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct { unsigned long long minimum_cost; unsigned long long candidates; } Q8Result;
static inline unsigned long long q8_sat_add(unsigned long long a,unsigned long long b){return ULLONG_MAX-a<b?ULLONG_MAX:a+b;}
static inline unsigned long long q8_sat_mul3(size_t a,size_t b,size_t c){if(a!=0U&&b>(size_t)ULLONG_MAX/a)return ULLONG_MAX;unsigned long long ab=(unsigned long long)a*(unsigned long long)b;if(c!=0U&&ab>ULLONG_MAX/(unsigned long long)c)return ULLONG_MAX;return ab*(unsigned long long)c;}

/* Validity is separate from cost: ULLONG_MAX is itself a representable answer. */
static inline int q8_matrix_chain(const size_t *dims, size_t dim_count,
                                 size_t *split, Q8Result *result) {
    if (!dims || !split || !result || dim_count < 2U) return 0;
    size_t n = dim_count - 1U;
    if (n > (size_t)-1 / n || n*n > (size_t)-1 / sizeof(unsigned long long)) return 0;
    for (size_t i=0; i<dim_count; ++i) if (!dims[i]) return 0;
    unsigned long long *cost = calloc(n*n, sizeof *cost);
    unsigned char *valid = calloc(n*n, 1);
    if (!cost || !valid) { free(cost); free(valid); return 0; }
    for (size_t i=0; i<n; ++i) valid[i*n+i] = 1;
    result->candidates = 0;
    for (size_t len=2; len<=n; ++len) for (size_t i=0; i+len<=n; ++i) {
        size_t j=i+len-1;
        for (size_t k=i; k<j; ++k) {
            ++result->candidates;
            if (!valid[i*n+k] || !valid[(k+1)*n+j]) continue;
            unsigned long long a=dims[i], b=dims[k+1], c=dims[j+1];
            if (a && b>ULLONG_MAX/a) continue;
            unsigned long long product=a*b;
            if (c && product>ULLONG_MAX/c) continue;
            product*=c;
            unsigned long long left=cost[i*n+k], right=cost[(k+1)*n+j];
            if (ULLONG_MAX-left<right) continue;
            unsigned long long sum=left+right;
            if (ULLONG_MAX-sum<product) continue;
            unsigned long long candidate=sum+product;
            if (!valid[i*n+j] || candidate<cost[i*n+j]) {
                valid[i*n+j]=1; cost[i*n+j]=candidate; split[i*n+j]=k;
            }
        }
    }
    int ok=valid[n-1];
    if (ok) result->minimum_cost=cost[n-1];
    free(cost); free(valid); return ok;
}
static inline void q8_print_parenthesization(FILE*out,const size_t*split,size_t n,size_t i,size_t j){if(i==j){fprintf(out,"A%zu",i+1U);return;}size_t k=split[i*n+j];fputc('(',out);q8_print_parenthesization(out,split,n,i,k);fputs(" x ",out);q8_print_parenthesization(out,split,n,k+1U,j);fputc(')',out);}
#endif
