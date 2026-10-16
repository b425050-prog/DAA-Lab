#include "../common/lab10_common.h"
typedef struct { int root, index, value; } Record;
static int by_index(const void *pa, const void *pb) {
    const Record *a = pa, *b = pb; ++work;
    return a->root != b->root ? a->root-b->root : a->index-b->index;
}
static int by_value(const void *pa, const void *pb) {
    const Record *a = pa, *b = pb; ++work;
    return a->root != b->root ? a->root-b->root : a->value-b->value;
}
static void sort_records(Record *a, Record *tmp, int lo, int hi, int value_order) {
    if (hi-lo < 2) return;
    int mid = lo+(hi-lo)/2;
    sort_records(a, tmp, lo, mid, value_order);
    sort_records(a, tmp, mid, hi, value_order);
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        int cmp = value_order ? by_value(&a[i], &a[j]) : by_index(&a[i], &a[j]);
        tmp[k++] = cmp <= 0 ? a[i++] : a[j++];
    }
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    for (k = lo; k < hi; ++k) a[k] = tmp[k];
}
int main(void) {
    int n = (int)read_int(1, 200000), m = (int)read_int(0, 200000);
    Node *d = create_dsu(n); int *a = allocate((size_t)n, sizeof(*a));
    unsigned char *seen = allocate((size_t)n+1, 1);
    for (int i = 0; i < n; ++i) {
        a[i] = (int)read_int(1, n);
        require(!seen[a[i]], "A must be a permutation"); seen[a[i]] = 1;
    }
    for (int i = 0; i < m; ++i) {
        int u = (int)read_int(1, n)-1, v = (int)read_int(1, n)-1;
        unite(d, u, v, 0);
    }
    end_input(); Record *positions = allocate((size_t)n, sizeof(*positions));
    Record *values = allocate((size_t)n, sizeof(*values));
    for (int i = 0; i < n; ++i) {
        int root = find_root(d, i); positions[i] = values[i] = (Record){root, i, a[i]};
    }
    Record *tmp = allocate((size_t)n, sizeof(*tmp));
    sort_records(positions, tmp, 0, n, 0);
    sort_records(values, tmp, 0, n, 1);
    for (int i = 0; i < n; ++i) a[positions[i].index] = values[i].value;
    printf("Smallest permutation:");
    for (int i = 0; i < n; ++i) printf(" %d", a[i]);
    printf("\nWork: %llu\n", work);
    free(d); free(a); free(seen); free(positions); free(values); free(tmp); return 0;
}
