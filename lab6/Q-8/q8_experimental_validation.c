#include <stdio.h>
#include <stdlib.h>

#include "q8_matrix_chain.h"

static unsigned long long brute(const size_t*d,size_t i,size_t j){if(i==j)return 0U;unsigned long long best=ULLONG_MAX;for(size_t k=i;k<j;++k){unsigned long long v=q8_sat_add(q8_sat_add(brute(d,i,k),brute(d,k+1U,j)),q8_sat_mul3(d[i],d[k+1U],d[j+1U]));if(v<best)best=v;}return best;}

int main(void){
    const size_t sample[]={10U,30U,5U,60U};size_t sample_split[9]={0U};Q8Result sample_result={0U,0U};int valid=q8_matrix_chain(sample,4U,sample_split,&sample_result)&&sample_result.minimum_cost==4500U;printf("Required sample {10,30,5,60} -> 4500: %s\n",valid?"PASS":"FAIL");
    const size_t clrs[]={30U,35U,15U,5U,10U,20U,25U};size_t clrs_split[36]={0U};Q8Result clrs_result={0U,0U};valid=valid&&q8_matrix_chain(clrs,7U,clrs_split,&clrs_result)&&clrs_result.minimum_cost==15125U&&clrs_result.minimum_cost==brute(clrs,0U,5U);printf("CLRS oracle -> 15125: %s\n",valid?"PASS":"FAIL");
    FILE*data=fopen("q8_experimental_data.dat","w");if(!data)return 1;fprintf(data,"# matrix_count measured_candidates cubic_reference valid\n");
    for(size_t n=4U;n<=128U;n*=2U){size_t*dims=malloc((n+1U)*sizeof*dims),*split=calloc(n*n,sizeof*split);if(!dims||!split)return 1;for(size_t i=0U;i<=n;++i)dims[i]=2U+(i*11U)%31U;Q8Result r={0U,0U};int row=q8_matrix_chain(dims,n+1U,split,&r)&&r.minimum_cost<ULLONG_MAX;double ref=(double)n*(double)n*(double)n/6.0;fprintf(data,"%zu %llu %.3f %d\n",n,r.candidates,ref,row);printf("matrices=%-4zu candidates=%-10llu cost=%llu %s\n",n,r.candidates,r.minimum_cost,row?"PASS":"FAIL");valid=valid&&row;free(dims);free(split);}
    fclose(data);return valid?0:2;
}
