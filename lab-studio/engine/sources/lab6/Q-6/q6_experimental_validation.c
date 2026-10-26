#include <stdio.h>
#include <stdlib.h>

#include "q6_knapsack.h"

static long long oracle_1d(const size_t*w,const long long*p,size_t n,size_t cap){long long*dp=calloc(cap+1U,sizeof*dp);if(!dp)return -1;for(size_t i=0U;i<n;++i)for(size_t c=cap+1U;c-->w[i];){long long v=p[i]+dp[c-w[i]];if(v>dp[c])dp[c]=v;}long long answer=dp[cap];free(dp);return answer;}

int main(void){
    const size_t sw[]={10,20,30};const long long sp[]={60,100,120};unsigned char ss[3];Q6Result sr={0LL,0U};int valid=q6_knapsack(sw,sp,3U,50U,ss,&sr)&&sr.maximum_profit==220;
    printf("Canonical capacity-50 instance: %s\n",valid?"PASS":"FAIL");
    FILE*data=fopen("q6_experimental_data.dat","w");if(!data)return 1;fprintf(data,"# state_space measured_states linear_in_states_reference valid\n");
    for(size_t n=8U;n<=256U;n*=2U){size_t cap=4U*n;size_t*w=malloc(n*sizeof*w);long long*p=malloc(n*sizeof*p);unsigned char*selected=malloc(n);if(!w||!p||!selected)return 1;for(size_t i=0U;i<n;++i){w[i]=1U+(i*7U)%31U;p[i]=3LL+(long long)((i*17U)%97U);}Q6Result r={0LL,0U};int row=q6_knapsack(w,p,n,cap,selected,&r);long long oracle=oracle_1d(w,p,n,cap);row=row&&r.maximum_profit==oracle;double states=(double)n*(double)(cap+1U);fprintf(data,"%.0f %llu %.3f %d\n",states,r.states,states,row);printf("items=%-4zu W=%-5zu profit=%-8lld oracle=%s\n",n,cap,r.maximum_profit,row?"PASS":"FAIL");valid=valid&&row;free(w);free(p);free(selected);}
    fclose(data);return valid?0:2;
}
