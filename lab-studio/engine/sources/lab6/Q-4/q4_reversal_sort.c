#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q4_reversal.h"

static void print_array(const int *a,size_t n){for(size_t i=0U;i<n;++i)printf("%s%d",i?" ":"",a[i]);putchar('\n');}

int main(void){
    size_t n=0U; printf("DAA Lab 06 - Q4: Sorting Only by Reversals\nEnter permutation size n: ");
    if(scanf("%zu",&n)!=1||n==0U||n>1000000U)return 1;
    int *a=malloc(n*sizeof*a),*b=malloc(n*sizeof*b),*seen=calloc(n,sizeof*seen); if(!a||!b||!seen)return 1;
    printf("Enter a permutation of 1..%zu: ",n);
    for(size_t i=0U;i<n;++i){if(scanf("%d",&a[i])!=1||a[i]<1||(size_t)a[i]>n||seen[a[i]-1]){fprintf(stderr,"Not a permutation.\n");return 1;}seen[a[i]-1]=1;}
    memcpy(b,a,n*sizeof*b); Q4Metrics linear={0U,0U,0U},efficient={0U,0U,0U};
    q4_linear_reversal_sort(a,n,&linear); q4_cost_efficient_sort(b,0U,n,&efficient);
    printf("\nDirect placement result : ");print_array(a,n);
    printf("  reversals=%llu (at most n-1), length cost=%llu\n",linear.reversals,linear.reversal_cost);
    printf("Cost-efficient result   : ");print_array(b,n);
    printf("  reversals=%llu, length cost=%llu, comparisons=%llu\n",efficient.reversals,efficient.reversal_cost,efficient.comparisons);
    printf("Proof targets: O(n) reversals for direct placement; O(n log^2 n) reversal-length cost for divide/rotate/merge sort.\n");
    free(a);free(b);free(seen);return 0;
}
