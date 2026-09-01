#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q7_lcs.h"

static size_t oracle_length(const char*a,const char*b){size_t m=strlen(a),n=strlen(b);size_t*prev=calloc(n+1U,sizeof*prev),*cur=calloc(n+1U,sizeof*cur);if(!prev||!cur)return 0U;for(size_t i=1U;i<=m;++i){for(size_t j=1U;j<=n;++j)cur[j]=a[i-1U]==b[j-1U]?prev[j-1U]+1U:(prev[j]>=cur[j-1U]?prev[j]:cur[j-1U]);size_t*t=prev;prev=cur;cur=t;memset(cur,0,(n+1U)*sizeof*cur);}size_t r=prev[n];free(prev);free(cur);return r;}
static int is_subsequence(const char*s,const char*t){size_t j=0U;for(size_t i=0U;t[i]&&s[j];++i)if(s[j]==t[i])++j;return s[j]=='\0';}

int main(void){
    char sequence[128];Q7Result known;int valid=q7_lcs("AGGTAB","GXTXAYB",sequence,&known)&&known.length==4U&&is_subsequence(sequence,"AGGTAB")&&is_subsequence(sequence,"GXTXAYB");printf("Canonical LCS instance: %s (%s)\n",valid?"PASS":"FAIL",sequence);
    FILE*data=fopen("q7_experimental_data.dat","w");if(!data)return 1;fprintf(data,"# length measured_states quadratic_reference valid\n");
    for(size_t n=16U;n<=2048U;n*=2U){char*a=malloc(n+1U),*b=malloc(n+1U),*out=malloc(n+1U);if(!a||!b||!out)return 1;for(size_t i=0U;i<n;++i){a[i]="ALGORITHM"[i%9U];b[i]="LOGARITHM"[(i*5U+3U)%9U];}a[n]=b[n]='\0';Q7Result r;int row=q7_lcs(a,b,out,&r)&&r.length==oracle_length(a,b)&&is_subsequence(out,a)&&is_subsequence(out,b);double ref=(double)n*(double)n;fprintf(data,"%zu %llu %.3f %d\n",n,r.states,ref,row);printf("length=%-5zu LCS=%-5zu validation=%s\n",n,r.length,row?"PASS":"FAIL");valid=valid&&row;free(a);free(b);free(out);}
    fclose(data);return valid?0:2;
}
