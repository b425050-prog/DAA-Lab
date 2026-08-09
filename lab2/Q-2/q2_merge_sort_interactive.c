#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long writes;
    unsigned long long merge_calls;
} Metrics;

static int leq(int a, int b, Metrics *m) {
    ++m->comparisons;
    return a <= b;
}

static void merge2(int a[], int tmp[], int l, int m, int r, Metrics *met) {
    int i=l,j=m,k=l;
    ++met->merge_calls;
    while(i<m && j<r) tmp[k++]=leq(a[i],a[j],met)?a[i++]:a[j++], ++met->writes;
    while(i<m) tmp[k++]=a[i++], ++met->writes;
    while(j<r) tmp[k++]=a[j++], ++met->writes;
    for(i=l;i<r;++i) a[i]=tmp[i], ++met->writes;
}

static void merge_sort2_rec(int a[], int tmp[], int l, int r, Metrics *met) {
    if(r-l<=1) return;
    int m=l+(r-l)/2;
    merge_sort2_rec(a,tmp,l,m,met);
    merge_sort2_rec(a,tmp,m,r,met);
    merge2(a,tmp,l,m,r,met);
}

static int choose_source3(int a[], int i, int e1, int j, int e2, int k, int e3, Metrics *met) {
    int source = -1;
    int best = 0;
    if (i < e1) { source = 0; best = a[i]; }
    if (j < e2 && (source < 0 || leq(a[j], best, met))) { source = 1; best = a[j]; }
    if (k < e3 && (source < 0 || leq(a[k], best, met))) { source = 2; }
    return source;
}

static void merge3(int a[], int tmp[], int l, int m1, int m2, int r, Metrics *met) {
    int i=l,j=m1,k=m2,t=l;
    ++met->merge_calls;
    while(i<m1 || j<m2 || k<r) {
        int source=choose_source3(a,i,m1,j,m2,k,r,met);
        if(source==0) tmp[t++]=a[i++];
        else if(source==1) tmp[t++]=a[j++];
        else tmp[t++]=a[k++];
        ++met->writes;
    }
    for(i=l;i<r;++i) a[i]=tmp[i], ++met->writes;
}

static void merge_sort3_rec(int a[], int tmp[], int l, int r, Metrics *met) {
    int len=r-l;
    if(len<=1) return;
    int q=len/3, rem=len%3;
    int m1=l+q+(rem>0);
    int m2=m1+q+(rem>1);
    merge_sort3_rec(a,tmp,l,m1,met);
    merge_sort3_rec(a,tmp,m1,m2,met);
    merge_sort3_rec(a,tmp,m2,r,met);
    merge3(a,tmp,l,m1,m2,r,met);
}

static int is_sorted(const int a[], int n) {
    for(int i=1;i<n;++i) if(a[i-1]>a[i]) return 0;
    return 1;
}

static void print_array(const int a[], int n) {
    for(int i=0;i<n;++i) printf("%s%d",i?" ":"",a[i]);
    putchar('\n');
}

int main(void) {
    int n, choice;
    puts("\n============================================================");
    puts("        DAA LAB 02 - Q2 : 2-WAY vs 3-WAY MERGE SORT");
    puts("                    Student: Satyam Dhal");
    puts("============================================================");
    printf("Enter number of elements: ");
    if(scanf("%d",&n)!=1 || n<=0){puts("Invalid n.");return EXIT_FAILURE;}

    int *a=(int*)malloc((size_t)n*sizeof(int));
    int *tmp=(int*)malloc((size_t)n*sizeof(int));
    if(!a||!tmp){puts("Memory allocation failed.");free(a);free(tmp);return EXIT_FAILURE;}

    printf("Enter %d integers:\n",n);
    for(int i=0;i<n;++i) if(scanf("%d",&a[i])!=1){puts("Invalid input.");free(a);free(tmp);return EXIT_FAILURE;}

    puts("\nChoose sorting method:");
    puts("  1. Standard merge sort (divide into 2 parts)");
    puts("  2. Modified merge sort (divide into 3 parts)");
    printf("Choice: ");
    if(scanf("%d",&choice)!=1 || (choice!=1 && choice!=2)){puts("Invalid choice.");free(a);free(tmp);return EXIT_FAILURE;}

    Metrics met={0,0,0};
    clock_t begin=clock();
    if(choice==1) merge_sort2_rec(a,tmp,0,n,&met);
    else merge_sort3_rec(a,tmp,0,n,&met);
    clock_t end=clock();
    double ms=1000.0*(double)(end-begin)/CLOCKS_PER_SEC;

    puts("\nSorted array:");
    print_array(a,n);
    printf("\nVerification      : %s\n",is_sorted(a,n)?"PASS - array is nondecreasing":"FAIL");
    printf("Selected algorithm: %s\n",choice==1?"Standard 2-way merge sort":"Modified 3-way merge sort");
    printf("Key comparisons   : %llu\n",met.comparisons);
    printf("Element writes    : %llu\n",met.writes);
    printf("Merge calls       : %llu\n",met.merge_calls);
    printf("Measured CPU time : %.6f ms\n",ms);

    puts("\nWorst-case analysis");
    puts("-------------------");
    puts("2-way recurrence : T(n) = 2T(n/2) + Theta(n)  => Theta(n log n)");
    puts("3-way recurrence : T(n) = 3T(n/3) + Theta(n)  => Theta(n log n)");
    puts("Changing the branching factor changes constants and recursion depth, not the asymptotic class.");
    puts("============================================================");

    free(a); free(tmp);
    return 0;
}
