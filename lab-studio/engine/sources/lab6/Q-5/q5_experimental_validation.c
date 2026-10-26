#include <inttypes.h>
#include <stdio.h>

#include "q5_fibonacci.h"

int main(void) {
    const uint64_t known[] = {0U,1U,1U,2U,3U,5U,8U,13U,21U,34U,55U};
    int valid = 1; uint64_t value = 0U;
    for (unsigned int n = 0U; n <= 10U; ++n) { Q5Metrics m={0U}; if(!q5_fibonacci(n,&value,&m)||value!=known[n]) valid=0; }
    if (!q5_fibonacci(93U,&value,NULL) || value != UINT64_C(12200160415121876738)) valid=0;
    printf("Known values, recurrence boundary and F(93): %s\n", valid?"PASS":"FAIL");
    FILE *data=fopen("q5_experimental_data.dat","w");if(!data)return 1;
    fprintf(data,"# n transitions linear_reference valid\n");
    for(unsigned int n=8U;n<=88U;n+=8U){Q5Metrics m={0U};int row=q5_fibonacci(n,&value,&m);fprintf(data,"%u %llu %.3f %d\n",n,m.transitions,(double)n,row);printf("n=%-3u F(n)=%-20" PRIu64 " work=%llu\n",n,value,m.transitions);valid=valid&&row;}
    fclose(data);return valid?0:2;
}
