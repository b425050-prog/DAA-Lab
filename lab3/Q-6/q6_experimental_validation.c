/* DAA Lab 03 Q6 - validates best/worst selection-sort comparison counts. */
#include <stdio.h>
#include <stdlib.h>

typedef struct{unsigned long long comparisons,swaps;}Metrics;

static void sort(int a[],int n,Metrics*m){for(int i=0;i<n-1;i++){int p=i;for(int j=i+1;j<n;j++){m->comparisons++;if(a[j]<a[p])p=j;}if(p!=i){int t=a[i];a[i]=a[p];a[p]=t;m->swaps++;}}}
static unsigned int rng=0xC0FFEEu;static unsigned int next_u32(void){rng=1664525u*rng+1013904223u;return rng;}

int main(void){
 FILE*o=fopen("q6_experimental_data.dat","w");if(!o)return 1;fprintf(o,"# n sorted_cmp reverse_cmp random_cmp exact_n_nminus1_over2 sorted_swaps reverse_swaps random_swaps\n");
 printf("%-7s %-12s %-12s %-12s %-12s\n","n","sorted cmp","reverse cmp","random cmp","exact");
 for(int n=8;n<=512;n+=8){
  int *s=malloc((size_t)n*sizeof(int)),*r=malloc((size_t)n*sizeof(int)),*x=malloc((size_t)n*sizeof(int));if(!s||!r||!x)return 1;
  for(int i=0;i<n;i++){s[i]=i;r[i]=n-i;x[i]=(int)(next_u32()%100000u);}Metrics a={0,0},b={0,0},c={0,0};sort(s,n,&a);sort(r,n,&b);sort(x,n,&c);unsigned long long exact=(unsigned long long)n*(n-1)/2;
  if(a.comparisons!=exact||b.comparisons!=exact||c.comparisons!=exact){fprintf(stderr,"Comparison validation failed at n=%d\n",n);return 2;}
  fprintf(o,"%d %llu %llu %llu %llu %llu %llu %llu\n",n,a.comparisons,b.comparisons,c.comparisons,exact,a.swaps,b.swaps,c.swaps);
  if(n<=32||n%64==0)printf("%-7d %-12llu %-12llu %-12llu %-12llu\n",n,a.comparisons,b.comparisons,c.comparisons,exact);
  free(s);free(r);free(x);
 }
 fclose(o);printf("\nAll three input orders perform exactly n(n-1)/2 comparisons.\nBest-case and worst-case running times are therefore both Theta(n^2).\n");return 0;
}
