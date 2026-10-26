#ifndef LAB6_Q4_REVERSAL_H
#define LAB6_Q4_REVERSAL_H

#include <stddef.h>

typedef struct {
    unsigned long long reversals;
    unsigned long long reversal_cost;
    unsigned long long comparisons;
} Q4Metrics;

static inline void q4_reverse(int *a, size_t first, size_t last, Q4Metrics *m) {
    if (last - first < 2U) return;
    if (m) { ++m->reversals; m->reversal_cost += (unsigned long long)(last-first); }
    for (size_t i = first, j = last - 1U; i < j; ++i, --j) { int t=a[i];a[i]=a[j];a[j]=t; }
}

static inline size_t q4_lower_bound(const int *a, size_t first, size_t last, int value, Q4Metrics *m) {
    while (first < last) { size_t mid=first+(last-first)/2U; if(m)++m->comparisons; if(a[mid]<value) first=mid+1U; else last=mid; } return first;
}
static inline size_t q4_upper_bound(const int *a, size_t first, size_t last, int value, Q4Metrics *m) {
    while (first < last) { size_t mid=first+(last-first)/2U; if(m)++m->comparisons; if(a[mid]<=value) first=mid+1U; else last=mid; } return first;
}

static inline void q4_rotate(int *a, size_t first, size_t middle, size_t last, Q4Metrics *m) {
    if (first==middle || middle==last) return;
    q4_reverse(a,first,middle,m); q4_reverse(a,middle,last,m); q4_reverse(a,first,last,m);
}

static inline void q4_inplace_merge(int *a, size_t first, size_t middle, size_t last, Q4Metrics *m) {
    if (first>=middle || middle>=last) return;
    if (last-first==2U) { if(m)++m->comparisons; if(a[middle]<a[first]) q4_reverse(a,first,last,m); return; }
    size_t first_cut, second_cut;
    if (middle-first > last-middle) {
        first_cut=first+(middle-first)/2U;
        second_cut=q4_lower_bound(a,middle,last,a[first_cut],m);
    } else {
        second_cut=middle+(last-middle)/2U;
        first_cut=q4_upper_bound(a,first,middle,a[second_cut],m);
    }
    size_t new_middle=first_cut+(second_cut-middle);
    q4_rotate(a,first_cut,middle,second_cut,m);
    q4_inplace_merge(a,first,first_cut,new_middle,m);
    q4_inplace_merge(a,new_middle,second_cut,last,m);
}

static inline void q4_cost_efficient_sort(int *a, size_t first, size_t last, Q4Metrics *m) {
    if (last-first<2U) return;
    size_t middle=first+(last-first)/2U;
    q4_cost_efficient_sort(a,first,middle,m); q4_cost_efficient_sort(a,middle,last,m);
    q4_inplace_merge(a,first,middle,last,m);
}

static inline void q4_linear_reversal_sort(int *a, size_t n, Q4Metrics *m) {
    for (size_t target=0U; target<n; ++target) {
        size_t at=target; while(at<n && a[at]!=(int)target+1){++at;if(m)++m->comparisons;}
        if (at<n && at!=target) q4_reverse(a,target,at+1U,m);
    }
}

static inline int q4_is_sorted(const int *a, size_t n) { for(size_t i=1U;i<n;++i) if(a[i-1U]>a[i]) return 0; return 1; }

#endif
