/* DAA Lab 03 Q4 - exact scalar-multiplication recurrence for Strassen. */
#include <stdio.h>
#include <math.h>

static unsigned long long ipow(unsigned long long a,int e){unsigned long long r=1;while(e--){r*=a;}return r;}

int main(void){
    FILE *o=fopen("q4_experimental_data.dat","w"); if(!o)return 1;
    fprintf(o,"# n strassen_scalar_mult classical_scalar_mult ratio_classical_over_strassen\n");
    printf("%-8s %-20s %-20s %-12s\n","n","Strassen mult","Classical mult","speedup count");
    for(int k=0;k<=10;k++){
        unsigned long long n=1ULL<<k;
        unsigned long long s=ipow(7,k);
        unsigned long long c=n*n*n;
        double ratio=(double)c/(double)s;
        fprintf(o,"%llu %llu %llu %.9f\n",n,s,c,ratio);
        printf("%-8llu %-20llu %-20llu %-12.4f\n",n,s,c,ratio);
    }
    fclose(o);
    printf("\nAt n=2^k, Strassen performs exactly 7^k = n^(log2 7) scalar multiplications.\n");
    printf("Data written to q4_experimental_data.dat\n");
    return 0;
}
