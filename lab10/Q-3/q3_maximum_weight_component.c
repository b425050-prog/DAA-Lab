#include "../common/lab10_common.h"
static void report(int step, MaxHeap *h, Node *d, int limit) {
    Int answer;
    printf("Step %d: maximum=", step);
    if (maximum(h, d, limit, &answer)) printf("%lld\n", answer);
    else puts("NONE");
}
int main(void) {
    int n = (int)read_int(1, 200000), m = (int)read_int(0, 200000);
    int limit = (int)read_int(0, n); Node *d = create_dsu(n);
    Entry *space = allocate((size_t)2*n+1, sizeof(*space));
    MaxHeap h = {space, 0};
    for (int i = 0; i < n; ++i) {
        d[i].sum = read_int(-1000000000, 1000000000);
        if (limit) heap_push(&h, (Entry){d[i].sum, i});
    }
    report(0, &h, d, limit);
    for (int i = 0; i < m; ++i) {
        int u = (int)read_int(1, n)-1, v = (int)read_int(1, n)-1;
        /* Edge weights are labels; connectivity follows arrival order. */
        (void)read_int(-1000000000, 1000000000);
        int root = unite(d, u, v, 0);
        if (root >= 0 && d[root].size <= limit)
            heap_push(&h, (Entry){d[root].sum, root});
        report(i+1, &h, d, limit);
    }
    end_input(); printf("Work: %llu\n", work);
    free(d); free(space); return 0;
}
