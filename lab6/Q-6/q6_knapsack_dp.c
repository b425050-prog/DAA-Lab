#include <stdio.h>
#include <stdlib.h>

#include "q6_knapsack.h"

int main(void){
    size_t n=0U,capacity=0U;printf("DAA Lab 06 - Q6: 0/1 Knapsack by Dynamic Programming\nEnter number of items and capacity: ");
    if(scanf("%zu%zu",&n,&capacity)!=2||n==0U||n>10000U||capacity>1000000U)return 1;
    size_t*w=malloc(n*sizeof*w);long long*p=malloc(n*sizeof*p);unsigned char*selected=malloc(n);if(!w||!p||!selected)return 1;
    printf("Enter %zu weights: ",n);for(size_t i=0U;i<n;++i)if(scanf("%zu",&w[i])!=1||w[i]==0U)return 1;
    printf("Enter %zu profits: ",n);for(size_t i=0U;i<n;++i)if(scanf("%lld",&p[i])!=1)return 1;
    Q6Result r;if(!q6_knapsack(w,p,n,capacity,selected,&r)){fprintf(stderr,"DP table allocation failed.\n");return 1;}
    printf("Maximum profit: %lld\nSelected items (1-based):",r.maximum_profit);for(size_t i=0U;i<n;++i)if(selected[i])printf(" %zu",i+1U);putchar('\n');
    printf("DP states: %llu\nTime: Theta(nW); space: Theta(nW), including reconstruction\n",r.states);
    free(w);free(p);free(selected);return 0;
}
