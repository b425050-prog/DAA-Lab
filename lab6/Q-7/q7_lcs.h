#ifndef LAB6_Q7_LCS_H
#define LAB6_Q7_LCS_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct { size_t length; unsigned long long states; } Q7Result;

static inline int q7_lcs(const char *a,const char *b,char *sequence,Q7Result *result){
    if(!a||!b||!sequence||!result) return 0;
    size_t m=strlen(a),n=strlen(b),cols=n+1U;
    if(m+1U>(size_t)-1/cols) return 0;
    size_t*cells=calloc((m+1U)*cols,sizeof*cells);
    if(!cells) return 0;
    result->states=0U;
    for(size_t i=1U;i<=m;++i)for(size_t j=1U;j<=n;++j){cells[i*cols+j]=a[i-1U]==b[j-1U]?cells[(i-1U)*cols+j-1U]+1U:(cells[(i-1U)*cols+j]>=cells[i*cols+j-1U]?cells[(i-1U)*cols+j]:cells[i*cols+j-1U]);++result->states;}
    result->length=cells[m*cols+n];sequence[result->length]='\0';size_t i=m,j=n,k=result->length;
    while(i>0U&&j>0U){if(a[i-1U]==b[j-1U]){sequence[--k]=a[i-1U];--i;--j;}else if(cells[(i-1U)*cols+j]>=cells[i*cols+j-1U])--i;else --j;}
    free(cells);return 1;
}

#endif
