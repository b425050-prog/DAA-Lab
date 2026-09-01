#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q4_reversal.h"

static uint64_t next_u64(uint64_t*s){*s^=*s<<13U;*s^=*s>>7U;*s^=*s<<17U;return*s;}
static void shuffle(int*a,size_t n,uint64_t*s){for(size_t i=n;i>1U;--i){size_t j=(size_t)(next_u64(s)%i);int t=a[i-1U];a[i-1U]=a[j];a[j]=t;}}

int main(void){
    FILE*data=fopen("q4_experimental_data.dat","w");if(!data)return 1;
    fprintf(data,"# n reversal_length_cost n_log2_squared_reference valid\n");
    uint64_t state=UINT64_C(0xDAA60004);int all_valid=1;
    for(size_t n=16U;n<=8192U;n*=2U){
        int*a=malloc(n*sizeof*a),*b=malloc(n*sizeof*b);if(!a||!b)return 1;
        for(size_t i=0U;i<n;++i) {
            a[i]=(int)i+1;
        }
        shuffle(a,n,&state);
        memcpy(b,a,n*sizeof*b);
        Q4Metrics efficient={0U,0U,0U};q4_cost_efficient_sort(a,0U,n,&efficient);
        Q4Metrics linear={0U,0U,0U};q4_linear_reversal_sort(b,n,&linear);
        int valid=q4_is_sorted(a,n)&&q4_is_sorted(b,n)&&linear.reversals<=n-1U;
        double lg=log((double)n)/log(2.0),reference=(double)n*lg*lg;
        fprintf(data,"%zu %llu %.3f %d\n",n,efficient.reversal_cost,reference,valid);
        printf("n=%-5zu efficient-cost=%-11llu direct-reversals=%-6llu %s\n",n,efficient.reversal_cost,linear.reversals,valid?"PASS":"FAIL");
        all_valid=all_valid&&valid;free(a);free(b);
    }
    fclose(data);return all_valid?0:2;
}
