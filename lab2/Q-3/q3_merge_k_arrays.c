#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long writes;
    unsigned long long merge_calls;
    unsigned long long rounds;
    double cpu_ms;
} Metrics;

typedef struct {
    int *data;
    int size;
} Block;

static double now_ms(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

static int is_sorted(const int a[], int n) {
    for (int i=1;i<n;++i) if (a[i-1] > a[i]) return 0;
    return 1;
}

static int *merge_two(const int a[], int na, const int b[], int nb, Metrics *m) {
    int *out=(int*)malloc((size_t)(na+nb)*sizeof(int));
    if(!out) return NULL;
    int i=0,j=0,k=0;
    ++m->merge_calls;
    while(i<na && j<nb){
        ++m->comparisons;
        if(a[i] <= b[j]) out[k++]=a[i++];
        else out[k++]=b[j++];
        ++m->writes;
    }
    while(i<na){out[k++]=a[i++];++m->writes;}
    while(j<nb){out[k++]=b[j++];++m->writes;}
    return out;
}

static int *method1_sequential(int **arrays, int k, int n, Metrics *m) {
    int *acc=(int*)malloc((size_t)n*sizeof(int));
    if(!acc) return NULL;
    for(int i=0;i<n;++i) acc[i]=arrays[0][i];
    int acc_size=n;
    for(int a=1;a<k;++a){
        int *next=merge_two(acc,acc_size,arrays[a],n,m);
        if(!next){free(acc);return NULL;}
        free(acc);
        acc=next;
        acc_size+=n;
    }
    m->rounds = (unsigned long long)(k>1 ? k-1 : 0); /* sequential merge stages */
    return acc;
}

static int *copy_array(const int *a,int n){int *b=(int*)malloc((size_t)n*sizeof(int));if(!b)return NULL;for(int i=0;i<n;++i)b[i]=a[i];return b;}

static int *method2_balanced(int **arrays, int k, int n, Metrics *m) {
    Block *blocks=(Block*)calloc((size_t)k,sizeof(Block));
    if(!blocks) return NULL;
    for(int i=0;i<k;++i){blocks[i].data=copy_array(arrays[i],n);blocks[i].size=n;if(!blocks[i].data){for(int z=0;z<i;++z)free(blocks[z].data);free(blocks);return NULL;}}
    int count=k;
    while(count>1){
        ++m->rounds;
        int next_count=(count+1)/2;
        Block *next=(Block*)calloc((size_t)next_count,sizeof(Block));
        if(!next){for(int i=0;i<count;++i)free(blocks[i].data);free(blocks);return NULL;}
        int q=0;
        for(int i=0;i<count;i+=2){
            if(i+1<count){
                next[q].data=merge_two(blocks[i].data,blocks[i].size,blocks[i+1].data,blocks[i+1].size,m);
                next[q].size=blocks[i].size+blocks[i+1].size;
                if(!next[q].data){
                    for(int z=0;z<q;++z) free(next[z].data);
                    for(int z=i;z<count;++z) free(blocks[z].data);
                    free(next);
                    free(blocks);
                    return NULL;
                }
                free(blocks[i].data);
                free(blocks[i+1].data);
            } else {
                next[q]=blocks[i]; /* carry odd block unchanged to next round */
            }
            ++q;
        }
        free(blocks);
        blocks=next;
        count=next_count;
    }
    int *result=blocks[0].data;
    free(blocks);
    return result;
}

static int arrays_equal(const int *a,const int *b,int n){for(int i=0;i<n;++i)if(a[i]!=b[i])return 0;return 1;}
static void print_array(const int *a,int n){for(int i=0;i<n;++i)printf("%s%d",i?" ":"",a[i]);putchar('\n');}

static unsigned long long method1_worst_comparisons(int n,int k){
    if(k<=1) return 0;
    unsigned long long sum=(unsigned long long)k*(k+1)/2ULL - 1ULL;
    return (unsigned long long)n*sum - (unsigned long long)(k-1);
}

static unsigned long long method1_output_writes(int n,int k){
    if(k<=1) return 0;
    unsigned long long sum=(unsigned long long)k*(k+1)/2ULL - 1ULL;
    return (unsigned long long)n*sum;
}

static unsigned int ceil_log2_int(int k){unsigned int p=0;int v=1;while(v<k){v*=2;++p;}return p;}

int main(void){
    int k,n,choice;
    puts("\n================================================================================");
    puts("               DAA LAB 02 - Q3 : MERGING k SORTED ARRAYS");
    puts("                           Student: Satyam Dhal");
    puts("================================================================================");
    printf("Enter number of sorted arrays k: ");
    if(scanf("%d",&k)!=1 || k<=0){puts("Invalid k.");return EXIT_FAILURE;}
    printf("Enter number of elements in each array n: ");
    if(scanf("%d",&n)!=1 || n<=0){puts("Invalid n.");return EXIT_FAILURE;}
    if(k > INT_MAX / n){puts("Input is too large: k*n exceeds the supported integer range.");return EXIT_FAILURE;}

    int **arrays=(int**)calloc((size_t)k,sizeof(int*));
    if(!arrays){puts("Allocation failed.");return EXIT_FAILURE;}
    for(int i=0;i<k;++i){
        arrays[i]=(int*)malloc((size_t)n*sizeof(int));
        if(!arrays[i]){
            puts("Allocation failed.");
            for(int z=0;z<i;++z) free(arrays[z]);
            free(arrays);
            return EXIT_FAILURE;
        }
        printf("Enter %d sorted integers for array %d:\n",n,i+1);
        for(int j=0;j<n;++j) {
            if(scanf("%d",&arrays[i][j])!=1){
                puts("Invalid input.");
                for(int z=0;z<=i;++z) free(arrays[z]);
                free(arrays);
                return EXIT_FAILURE;
            }
        }
        if(!is_sorted(arrays[i],n)){
            printf("Error: array %d is not sorted in nondecreasing order. The problem assumes sorted input arrays.\n",i+1);
            for(int z=0;z<k;++z) free(arrays[z]);
            free(arrays);
            return EXIT_FAILURE;
        }
    }

    puts("\nChoose the method whose merged result you want displayed:");
    puts("  1. Method 1 - sequential repeated merge");
    puts("  2. Method 2 - balanced pairwise merge");
    printf("Choice: ");
    if(scanf("%d",&choice)!=1 || (choice!=1 && choice!=2)){
        puts("Invalid choice.");
        for(int i=0;i<k;++i) free(arrays[i]);
        free(arrays);
        return EXIT_FAILURE;
    }

    Metrics m1={0},m2={0};
    double t0=now_ms();
    int *r1=method1_sequential(arrays,k,n,&m1);
    m1.cpu_ms=now_ms()-t0;
    t0=now_ms();
    int *r2=method2_balanced(arrays,k,n,&m2);
    m2.cpu_ms=now_ms()-t0;
    if(!r1||!r2){
        puts("Merge failed because memory allocation failed.");
        free(r1); free(r2);
        for(int i=0;i<k;++i) free(arrays[i]);
        free(arrays);
        return EXIT_FAILURE;
    }

    int total=k*n;
    const int *selected=choice==1?r1:r2;
    puts("\nMerged output:");
    print_array(selected,total);
    printf("\nSelected method verification : %s\n",is_sorted(selected,total)?"PASS - output is sorted":"FAIL");
    printf("Cross-method verification    : %s\n",arrays_equal(r1,r2,total)?"PASS - both methods produce identical output":"FAIL");

    puts("\nMeasured details on your input");
    puts("+---------------------------+----------------------+----------------------+");
    puts("| Metric                    | Method 1: Sequential | Method 2: Balanced   |");
    puts("+---------------------------+----------------------+----------------------+");
    printf("| Key comparisons           | %20llu | %20llu |\n",m1.comparisons,m2.comparisons);
    printf("| Output element writes     | %20llu | %20llu |\n",m1.writes,m2.writes);
    printf("| Merge calls               | %20llu | %20llu |\n",m1.merge_calls,m2.merge_calls);
    printf("| Stages / rounds           | %20llu | %20llu |\n",m1.rounds,m2.rounds);
    printf("| Elapsed time (ms)         | %20.6f | %20.6f |\n",m1.cpu_ms,m2.cpu_ms);
    puts("+---------------------------+----------------------+----------------------+");

    unsigned int levels=ceil_log2_int(k);
    unsigned long long m1wc=method1_worst_comparisons(n,k);
    unsigned long long m1w=method1_output_writes(n,k);
    unsigned long long m2bound=(unsigned long long)n*k*levels;

    puts("\nWorst-case analysis requested by the problem");
    puts("--------------------------------------------");
    printf("Method 1 sequential: merge lengths 2n, 3n, ..., kn.\n");
    printf("  Worst-case comparisons = n * (k(k+1)/2 - 1) - (k - 1) = %llu for this n,k.\n",m1wc);
    printf("  Output writes across merges = n * (k(k+1)/2 - 1) = %llu for this n,k.\n",m1w);
    puts("  Therefore the running time is Theta(n k^2).\n");

    printf("Method 2 balanced: at most ceil(log2 k) = %u merge rounds.\n",levels);
    printf("  At each round, at most nk = %d elements participate in merging.\n",n*k);
    printf("  Hence work is at most proportional to nk * ceil(log2 k) = %llu element-level units.\n",m2bound);
    puts("  Therefore the running time is Theta(n k log k).");
    if((k & (k-1))==0){
        unsigned long long exact=(unsigned long long)n*k*levels-(unsigned long long)(k-1);
        printf("  Because k is a power of two, exact worst-case key comparisons are nk log2(k) - (k-1) = %llu.\n",exact);
    }

    puts("\nConclusion");
    puts("----------");
    puts("Both methods are correct, but balanced pairwise merging avoids repeatedly reprocessing one ever-growing accumulator.");
    puts("For large k, Theta(n k log k) is asymptotically better than Theta(n k^2).");
    puts("================================================================================");

    free(r1);free(r2);for(int i=0;i<k;++i)free(arrays[i]);free(arrays);
    return 0;
}
