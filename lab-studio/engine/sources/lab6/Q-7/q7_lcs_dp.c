#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q7_lcs.h"

int main(void){
    char*a=malloc(10001U),*b=malloc(10001U);if(!a||!b)return 1;printf("DAA Lab 06 - Q7: Longest Common Subsequence\nEnter first string (no spaces): ");if(scanf("%10000s",a)!=1)return 1;printf("Enter second string (no spaces): ");if(scanf("%10000s",b)!=1)return 1;
    size_t cap=strlen(a)<strlen(b)?strlen(a):strlen(b);char*sequence=malloc(cap+1U);if(!sequence)return 1;Q7Result r;if(!q7_lcs(a,b,sequence,&r))return 1;
    printf("LCS length   : %zu\nLCS sequence : %s\nDP states    : %llu\nTime: Theta(mn); space: Theta(mn) for reconstruction\n",r.length,sequence,r.states);
    free(a);free(b);free(sequence);return 0;
}
