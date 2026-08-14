/* DAA Lab 03 Q5 - recurrence-level operation validation for special matrices. */
#include <stdio.h>

typedef struct{unsigned long long mult,addsub,div2;}Count;

static Count count_ops(int n){
    if(n==1){Count c={1,0,0};return c;}
    Count h=count_ops(n/2),c;
    unsigned long long block=(unsigned long long)(n/2)*(unsigned long long)(n/2);
    c.mult=2*h.mult;
    c.addsub=2*h.addsub+6*block; /* 4 pre-recursion + 2 reconstruction */
    c.div2=2*h.div2+2*block;
    return c;
}

int main(void){
    FILE*o=fopen("q5_experimental_data.dat","w");if(!o)return 1;
    fprintf(o,"# n scalar_mult addsub div2 total_arithmetic n_squared classical_mult\n");
    printf("%-7s %-12s %-14s %-12s %-16s %-12s\n","n","mult","add/sub","divide-by-2","total ops","n^2");
    for(int k=0;k<=10;k++){
        int n=1<<k;Count c=count_ops(n);
        unsigned long long total=c.mult+c.addsub+c.div2;
        unsigned long long n2=(unsigned long long)n*n;
        unsigned long long n3=n2*n;
        fprintf(o,"%d %llu %llu %llu %llu %llu %llu\n",n,c.mult,c.addsub,c.div2,total,n2,n3);
        printf("%-7d %-12llu %-14llu %-12llu %-16llu %-12llu\n",n,c.mult,c.addsub,c.div2,total,n2);
    }
    fclose(o);
    printf("\nThe scalar multiplication count is only n; total work is dominated by Theta(n^2)\n");
    printf("block additions/reconstruction and by writing the n^2 output entries.\n");
    return 0;
}
