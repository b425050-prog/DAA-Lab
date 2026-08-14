/*
 * DAA Laboratory - Lab 03 - Question 6
 * Selection Sort + Loop Invariant Validation.
 *
 * Loop invariant at the start of iteration i (0-based):
 *   A[0..i-1] contains the i smallest elements of the original array,
 *   in sorted order, and every element in that prefix is <= every element
 *   still in A[i..n-1].
 *
 * The outer loop stops at n-2 because after placing the first n-1 smallest
 * elements, the only remaining element must already be the largest.
 * Comparisons are always n(n-1)/2, so best and worst cases are both Theta(n^2).
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long comparisons;
    unsigned long long swaps;
} Metrics;

static void print_array(const int a[], int n) {
    for (int i=0;i<n;i++) printf("%d%s",a[i],i+1==n?"\n":" ");
}

static int invariant_holds(const int a[], int n, int i) {
    for (int p=1;p<i;p++)
        if (a[p-1] > a[p]) return 0;
    for (int p=0;p<i;p++)
        for (int q=i;q<n;q++)
            if (a[p] > a[q]) return 0;
    return 1;
}

static void selection_sort(int a[], int n, Metrics *m, int trace) {
    for (int i=0;i<n-1;i++) {
        if (trace) {
            printf("\nStart iteration i=%d: invariant %s\n", i+1,
                   invariant_holds(a,n,i)?"HOLDS":"FAILS");
            printf("Current array: "); print_array(a,n);
        }

        int min_index=i;
        for (int j=i+1;j<n;j++) {
            m->comparisons++;
            if (a[j] < a[min_index]) min_index=j;
        }
        if (min_index!=i) {
            int t=a[i];a[i]=a[min_index];a[min_index]=t;
            m->swaps++;
        }

        if (trace) {
            printf("Placed minimum of suffix at position %d.\n",i+1);
            printf("After exchange: "); print_array(a,n);
            printf("Invariant for next iteration: %s\n",
                   invariant_holds(a,n,i+1)?"HOLDS":"FAILS");
        }
    }
}

int main(void) {
    int n, trace;
    printf("============================================================\n");
    printf(" DAA Lab 03 - Q6: Selection Sort and Loop Invariant\n");
    printf("============================================================\n");
    printf("Enter array size: ");
    if(scanf("%d",&n)!=1||n<=0)return 1;
    int *a=(int*)malloc((size_t)n*sizeof(int));if(!a)return 1;
    printf("Enter %d integers:\n",n);for(int i=0;i<n;i++)scanf("%d",&a[i]);
    printf("Show invariant trace? (1=yes, 0=no): ");scanf("%d",&trace);

    Metrics m={0,0}; selection_sort(a,n,&m,trace!=0);
    printf("\nSorted array: ");print_array(a,n);
    unsigned long long exact=(unsigned long long)n*(n-1)/2;
    printf("\nComparisons : %llu (exact formula n(n-1)/2 = %llu)\n",m.comparisons,exact);
    printf("Swaps       : %llu\n",m.swaps);
    printf("Worst case  : Theta(n^2)\n");
    printf("Best case   : Theta(n^2) - comparisons do not depend on input order\n");

    printf("\nPseudocode:\n");
    printf("SELECTION-SORT(A, n)\n");
    printf("  for i = 1 to n-1\n");
    printf("      min = i\n");
    printf("      for j = i+1 to n\n");
    printf("          if A[j] < A[min]\n");
    printf("              min = j\n");
    printf("      exchange A[i] and A[min]\n");

    printf("\nLoop invariant:\n");
    printf("At the start of each outer-loop iteration i, A[1..i-1]\n");
    printf("contains the i-1 smallest original elements in sorted order.\n");
    printf("Initialization: before i=1 the prefix is empty, so the claim is true.\n");
    printf("Maintenance   : selecting the minimum of A[i..n] and placing it at A[i]\n");
    printf("                extends the sorted-smallest prefix by one element.\n");
    printf("Termination   : after i=n-1, the first n-1 positions are correct; the\n");
    printf("                last remaining element must be the largest.\n");

    free(a);return 0;
}
