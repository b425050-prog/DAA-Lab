/* DAA Lab 03 Q3 - validates the <= 3n/2 comparison bound. */
#include <stdio.h>
#include <stdlib.h>

typedef struct { unsigned long long comparisons; } Metrics;

static int dc_min(const int a[],int l,int r,Metrics*m){if(l==r)return a[l];int mid=l+(r-l)/2;int x=dc_min(a,l,mid,m),y=dc_min(a,mid+1,r,m);m->comparisons++;return x<y?x:y;}
static int dc_max(const int a[],int l,int r,Metrics*m){if(l==r)return a[l];int mid=l+(r-l)/2;int x=dc_max(a,l,mid,m),y=dc_max(a,mid+1,r,m);m->comparisons++;return x>y?x:y;}

static void solve(const int a[],int n,Metrics*m,int*mn,int*mx){
 int c=(n+1)/2,*lo=malloc((size_t)c*sizeof(int)),*hi=malloc((size_t)c*sizeof(int));if(!lo||!hi)exit(2);int k=0;
 for(int i=0;i+1<n;i+=2){m->comparisons++;if(a[i]<a[i+1]){lo[k]=a[i];hi[k]=a[i+1];}else{lo[k]=a[i+1];hi[k]=a[i];}k++;}
 if(n%2){lo[k]=hi[k]=a[n-1];k++;}
 *mn=dc_min(lo,0,k-1,m);*mx=dc_max(hi,0,k-1,m);free(lo);free(hi);
}

int main(void){
 FILE*out=fopen("q3_experimental_data.dat","w");if(!out)return 1;fprintf(out,"# n comparisons three_n_over_2 exact_formula\n");
 int *a=malloc(4096*sizeof(int));if(!a)return 1;for(int i=0;i<4096;i++)a[i]=(i*7919)%1000003;
 printf("%-8s %-14s %-14s %-14s\n","n","comparisons","3n/2","exact formula");
 for(int n=1;n<=4096;n+=(n<64?1:32)){
  Metrics m={0};int mn,mx;solve(a,n,&m,&mn,&mx);(void)mn;(void)mx;double bound=1.5*n;
  unsigned long long exact=(n%2==0)?((3ULL*n)/2ULL-2ULL):(3ULL*(n-1))/2ULL;
  if(n==1) exact=0;
  fprintf(out,"%d %llu %.1f %llu\n",n,m.comparisons,bound,exact);
  if(m.comparisons!=exact||(double)m.comparisons>bound){fprintf(stderr,"Validation failed at n=%d\n",n);return 2;}
  if(n<=16||(n&(n-1))==0)printf("%-8d %-14llu %-14.1f %-14llu\n",n,m.comparisons,bound,exact);
 }
 free(a);fclose(out);printf("\nEvery tested n satisfies comparisons <= 3n/2 and matches the exact pairwise-tournament formula.\n");return 0;
}
