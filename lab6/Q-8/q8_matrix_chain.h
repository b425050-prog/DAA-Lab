#ifndef LAB6_Q8_MATRIX_CHAIN_H
#define LAB6_Q8_MATRIX_CHAIN_H

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { unsigned long long minimum_cost; unsigned long long candidates; } Q8Result;

static inline unsigned long long q8_sat_add(unsigned long long a,unsigned long long b){return ULLONG_MAX-a<b?ULLONG_MAX:a+b;}
static inline unsigned long long q8_sat_mul3(size_t a,size_t b,size_t c){if(a!=0U&&b>(size_t)ULLONG_MAX/a)return ULLONG_MAX;unsigned long long ab=(unsigned long long)a*(unsigned long long)b;if(c!=0U&&ab>ULLONG_MAX/(unsigned long long)c)return ULLONG_MAX;return ab*(unsigned long long)c;}

static inline int q8_matrix_chain(const size_t*dims,size_t dim_count,size_t*split,Q8Result*result){
    if(!dims||!split||!result||dim_count<2U) return 0;
    size_t n=dim_count-1U;
    if(n>(size_t)-1/n) return 0;
    unsigned long long*cost=calloc(n*n,sizeof*cost);
    if(!cost) return 0;
    result->candidates=0U;
    for(size_t len=2U;len<=n;++len)for(size_t i=0U;i+len<=n;++i){size_t j=i+len-1U;cost[i*n+j]=ULLONG_MAX;for(size_t k=i;k<j;++k){unsigned long long candidate=q8_sat_add(q8_sat_add(cost[i*n+k],cost[(k+1U)*n+j]),q8_sat_mul3(dims[i],dims[k+1U],dims[j+1U]));++result->candidates;if(candidate<cost[i*n+j]){cost[i*n+j]=candidate;split[i*n+j]=k;}}}
    result->minimum_cost=cost[n-1U];free(cost);return 1;
}

static inline void q8_print_parenthesization(FILE*out,const size_t*split,size_t n,size_t i,size_t j){if(i==j){fprintf(out,"A%zu",i+1U);return;}size_t k=split[i*n+j];fputc('(',out);q8_print_parenthesization(out,split,n,i,k);fputs(" x ",out);q8_print_parenthesization(out,split,n,k+1U,j);fputc(')',out);}

#endif
