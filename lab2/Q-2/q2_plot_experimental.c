#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define POINTS 80
#define STEP 2000
#define W 1700
#define H 980

typedef struct { unsigned long long comparisons,writes,merges; } Metrics;
typedef struct { int n; double c2,c3,t2,t3; } Row;

static uint32_t rng_state=0xA341316Cu;
static uint32_t xorshift32(void){uint32_t x=rng_state;x^=x<<13;x^=x>>17;x^=x<<5;return rng_state=x;}
static int leq(int a,int b,Metrics*m){++m->comparisons;return a<=b;}
static void merge2(int*a,int*t,int l,int m,int r,Metrics*x){int i=l,j=m,k=l;++x->merges;while(i<m&&j<r){t[k++]=leq(a[i],a[j],x)?a[i++]:a[j++];++x->writes;}while(i<m){t[k++]=a[i++];++x->writes;}while(j<r){t[k++]=a[j++];++x->writes;}for(i=l;i<r;++i){a[i]=t[i];++x->writes;}}
static void sort2(int*a,int*t,int l,int r,Metrics*x){if(r-l<=1)return;int m=l+(r-l)/2;sort2(a,t,l,m,x);sort2(a,t,m,r,x);merge2(a,t,l,m,r,x);}
static int source3(int*a,int i,int e1,int j,int e2,int k,int e3,Metrics*x){int s=-1,best=0;if(i<e1){s=0;best=a[i];}if(j<e2&&(s<0||leq(a[j],best,x))){s=1;best=a[j];}if(k<e3&&(s<0||leq(a[k],best,x)))s=2;return s;}
static void merge3(int*a,int*t,int l,int m1,int m2,int r,Metrics*x){int i=l,j=m1,k=m2,p=l;++x->merges;while(i<m1||j<m2||k<r){int s=source3(a,i,m1,j,m2,k,r,x);if(s==0)t[p++]=a[i++];else if(s==1)t[p++]=a[j++];else t[p++]=a[k++];++x->writes;}for(i=l;i<r;++i){a[i]=t[i];++x->writes;}}
static void sort3(int*a,int*t,int l,int r,Metrics*x){int len=r-l;if(len<=1)return;int q=len/3,rem=len%3,m1=l+q+(rem>0),m2=m1+q+(rem>1);sort3(a,t,l,m1,x);sort3(a,t,m1,m2,x);sort3(a,t,m2,r,x);merge3(a,t,l,m1,m2,r,x);}
static int sorted(const int*a,int n){for(int i=1;i<n;++i)if(a[i-1]>a[i])return 0;return 1;}
static double mapx(double n,double nmax,double x0,double x1){return x0+n/nmax*(x1-x0);}
static double mapy(double y,double ymax,double y0,double y1){return y1-y/ymax*(y1-y0);}

int main(void){
    Row rows[POINTS];
    FILE*d=fopen("q2_experimental_data.dat","w");if(!d){perror("q2_experimental_data.dat");return 1;}
    fputs("# n comparisons_2way comparisons_3way cpu_ms_2way cpu_ms_3way normalized_2way normalized_3way\n",d);
    for(int p=0;p<POINTS;++p){
        int n=(p+1)*STEP;
        int*base=malloc((size_t)n*sizeof(int));int*a=malloc((size_t)n*sizeof(int));int*b=malloc((size_t)n*sizeof(int));int*t1=malloc((size_t)n*sizeof(int));int*t2=malloc((size_t)n*sizeof(int));
        if(!base||!a||!b||!t1||!t2){puts("Allocation failed.");return 1;}
        rng_state=0x9E3779B9u^(uint32_t)n;
        for(int i=0;i<n;++i)base[i]=(int)(xorshift32()&0x7fffffffU);
        for(int i=0;i<n;++i)a[i]=b[i]=base[i];
        Metrics m2={0,0,0},m3={0,0,0};
        clock_t s=clock();sort2(a,t1,0,n,&m2);clock_t e=clock();double ms2=1000.0*(e-s)/CLOCKS_PER_SEC;
        s=clock();sort3(b,t2,0,n,&m3);e=clock();double ms3=1000.0*(e-s)/CLOCKS_PER_SEC;
        if(!sorted(a,n)||!sorted(b,n)){fprintf(stderr,"Verification failed at n=%d\n",n);return 1;}
        double base_nlog=n*(log((double)n)/log(2.0));
        rows[p]=(Row){n,(double)m2.comparisons,(double)m3.comparisons,ms2,ms3};
        fprintf(d,"%d %.0f %.0f %.6f %.6f %.9f %.9f\n",n,rows[p].c2,rows[p].c3,ms2,ms3,rows[p].c2/base_nlog,rows[p].c3/base_nlog);
        free(base);free(a);free(b);free(t1);free(t2);
    }
    fclose(d);

    double cmax=0,normmax=0,nmax=rows[POINTS-1].n;
    for(int i=0;i<POINTS;++i){if(rows[i].c2>cmax)cmax=rows[i].c2;if(rows[i].c3>cmax)cmax=rows[i].c3;double den=rows[i].n*(log((double)rows[i].n)/log(2.0));double q2=rows[i].c2/den,q3=rows[i].c3/den;if(q2>normmax)normmax=q2;if(q3>normmax)normmax=q3;}
    normmax*=1.15;
    FILE*f=fopen("q2_experimental_complexity.svg","w");if(!f){perror("q2_experimental_complexity.svg");return 1;}
    fprintf(f,"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\"><defs><linearGradient id=\"bg\" x1=\"0\" y1=\"0\" x2=\"1\" y2=\"1\"><stop offset=\"0\" stop-color=\"#06101d\"/><stop offset=\"1\" stop-color=\"#161126\"/></linearGradient></defs><rect width=\"100%%\" height=\"100%%\" rx=\"28\" fill=\"url(#bg)\"/>",W,H,W,H);
    fputs("<text x=\"850\" y=\"58\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"34\" font-weight=\"700\" fill=\"#f8fafc\">Q2 · Experimental Merge-Sort Growth</text><text x=\"850\" y=\"94\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"17\" fill=\"#a5b4fc\">80 deterministic random arrays · n = 2,000 … 160,000 · correctness checked at every point</text>",f);
    double ax0=100,ax1=820,bx0=890,bx1=1610,yt=165,yb=760;
    fprintf(f,"<rect x=\"%.0f\" y=\"%.0f\" width=\"%.0f\" height=\"%.0f\" rx=\"20\" fill=\"#0b1220\" stroke=\"#334155\"/><rect x=\"%.0f\" y=\"%.0f\" width=\"%.0f\" height=\"%.0f\" rx=\"20\" fill=\"#0b1220\" stroke=\"#334155\"/>",ax0,yt,ax1-ax0,yb-yt,bx0,yt,bx1-bx0,yb-yt);
    fputs("<text x=\"460\" y=\"145\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"21\" font-weight=\"700\" fill=\"#e2e8f0\">Measured key comparisons</text><text x=\"1250\" y=\"145\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"21\" font-weight=\"700\" fill=\"#e2e8f0\">Comparisons ÷ (n log₂ n)</text>",f);
    for(int k=0;k<=4;++k){double n=nmax*k/4.0,xa=mapx(n,nmax,ax0,ax1),xb=mapx(n,nmax,bx0,bx1);fprintf(f,"<line x1=\"%.1f\" y1=\"%.0f\" x2=\"%.1f\" y2=\"%.0f\" stroke=\"#1e293b\"/><line x1=\"%.1f\" y1=\"%.0f\" x2=\"%.1f\" y2=\"%.0f\" stroke=\"#1e293b\"/><text x=\"%.1f\" y=\"790\" text-anchor=\"middle\" font-family=\"monospace\" font-size=\"13\" fill=\"#94a3b8\">%.0fK</text><text x=\"%.1f\" y=\"790\" text-anchor=\"middle\" font-family=\"monospace\" font-size=\"13\" fill=\"#94a3b8\">%.0fK</text>",xa,yt,xa,yb,xb,yt,xb,yb,xa,n/1000.0,xb,n/1000.0);}
    fputs("<path d=\"",f);for(int i=0;i<POINTS;++i)fprintf(f,"%c%.1f %.1f ",i?'L':'M',mapx(rows[i].n,nmax,ax0,ax1),mapy(rows[i].c2,cmax,yt,yb));fputs("\" fill=\"none\" stroke=\"#38bdf8\" stroke-width=\"4\"/>",f);
    fputs("<path d=\"",f);for(int i=0;i<POINTS;++i)fprintf(f,"%c%.1f %.1f ",i?'L':'M',mapx(rows[i].n,nmax,ax0,ax1),mapy(rows[i].c3,cmax,yt,yb));fputs("\" fill=\"none\" stroke=\"#f472b6\" stroke-width=\"4\" stroke-dasharray=\"11 7\"/>",f);
    fputs("<path d=\"",f);for(int i=0;i<POINTS;++i){double den=rows[i].n*(log((double)rows[i].n)/log(2.0));fprintf(f,"%c%.1f %.1f ",i?'L':'M',mapx(rows[i].n,nmax,bx0,bx1),mapy(rows[i].c2/den,normmax,yt,yb));}fputs("\" fill=\"none\" stroke=\"#38bdf8\" stroke-width=\"4\"/>",f);
    fputs("<path d=\"",f);for(int i=0;i<POINTS;++i){double den=rows[i].n*(log((double)rows[i].n)/log(2.0));fprintf(f,"%c%.1f %.1f ",i?'L':'M',mapx(rows[i].n,nmax,bx0,bx1),mapy(rows[i].c3/den,normmax,yt,yb));}fputs("\" fill=\"none\" stroke=\"#f472b6\" stroke-width=\"4\" stroke-dasharray=\"11 7\"/>",f);
    fputs("<line x1=\"585\" y1=\"842\" x2=\"655\" y2=\"842\" stroke=\"#38bdf8\" stroke-width=\"5\"/><text x=\"675\" y=\"849\" font-family=\"Segoe UI,Arial\" font-size=\"18\" fill=\"#e2e8f0\">2-way merge sort</text><line x1=\"900\" y1=\"842\" x2=\"970\" y2=\"842\" stroke=\"#f472b6\" stroke-width=\"5\" stroke-dasharray=\"11 7\"/><text x=\"990\" y=\"849\" font-family=\"Segoe UI,Arial\" font-size=\"18\" fill=\"#e2e8f0\">3-way merge sort</text>",f);
    fputs("<text x=\"850\" y=\"902\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"17\" fill=\"#cbd5e1\">The normalized curves settle toward constants, experimentally supporting Θ(n log n) for both algorithms.</text><text x=\"850\" y=\"938\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"14\" fill=\"#64748b\">Raw CPU timings are also stored in q2_experimental_data.dat; comparison counts are plotted because they are reproducible across machines.</text></svg>",f);
    fclose(f);puts("Created q2_experimental_data.dat and q2_experimental_complexity.svg");return 0;
}
