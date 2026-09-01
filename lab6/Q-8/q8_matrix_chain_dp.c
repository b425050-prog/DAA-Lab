#include <stdio.h>
#include <stdlib.h>

#include "q8_matrix_chain.h"

int main(void){
    size_t count=0U;printf("DAA Lab 06 - Q8: Matrix-Chain Multiplication\nEnter number of dimensions N (N-1 matrices): ");if(scanf("%zu",&count)!=1||count<2U||count>1000U)return 1;size_t*dims=malloc(count*sizeof*dims),*split=calloc((count-1U)*(count-1U),sizeof*split);if(!dims||!split)return 1;printf("Enter %zu dimensions: ",count);for(size_t i=0U;i<count;++i)if(scanf("%zu",&dims[i])!=1||dims[i]==0U)return 1;Q8Result r;if(!q8_matrix_chain(dims,count,split,&r))return 1;printf("Minimum scalar multiplications: %llu\nOptimal parenthesization      : ",r.minimum_cost);q8_print_parenthesization(stdout,split,count-1U,0U,count-2U);printf("\nCandidate splits examined    : %llu\nTime: Theta(m^3); space: Theta(m^2), m=N-1 matrices\n",r.candidates);free(dims);free(split);return 0;
}
