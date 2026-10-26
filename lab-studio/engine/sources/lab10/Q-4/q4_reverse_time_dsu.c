#include "../common/lab10_common.h"
typedef struct { int to, next; } Arc;
int main(void) {
    int n = (int)read_int(1, 200000), m = (int)read_int(0, 200000);
    Node *d = create_dsu(n);
    for (int i = 0; i < n; ++i) d[i].sum = read_int(-1000000000, 1000000000);
    int *head = allocate((size_t)n, sizeof(*head));
    Arc *edge = allocate((size_t)2*m, sizeof(*edge)); int count = 0;
    for (int i = 0; i < n; ++i) head[i] = -1;
    for (int i = 0; i < m; ++i) {
        int u = (int)read_int(1, n)-1, v = (int)read_int(1, n)-1;
        edge[count] = (Arc){v, head[u]}; head[u] = count++;
        edge[count] = (Arc){u, head[v]}; head[v] = count++;
    }
    int *order = allocate((size_t)n, sizeof(*order));
    unsigned char *seen = allocate((size_t)n, 1), *active = allocate((size_t)n, 1);
    for (int i = 0; i < n; ++i) {
        order[i] = (int)read_int(1, n)-1;
        require(!seen[order[i]], "deletion order must be a permutation");
        seen[order[i]] = 1;
    }
    end_input(); Int *answer = allocate((size_t)n, sizeof(*answer));
    Entry *space = allocate((size_t)2*n+1, sizeof(*space)); MaxHeap h = {space, 0};
    for (int i = n-1; i >= 0; --i) {
        /* Before restoring order[i], the state is AFTER its forward deletion. */
        if (!maximum(&h, d, n, &answer[i])) answer[i] = 0;
        int u = order[i]; active[u] = 1; heap_push(&h, (Entry){d[u].sum, u});
        for (int j = head[u]; j >= 0; j = edge[j].next) {
            ++work; int v = edge[j].to;
            if (active[v]) {
                int root = unite(d, u, v, 0);
                if (root >= 0) heap_push(&h, (Entry){d[root].sum, root});
            }
        }
    }
    for (int i = 0; i < n; ++i)
        printf("Delete %d (vertex %d): maximum=%lld%s\n", i+1, order[i]+1,
               answer[i], i == n-1 ? " (empty graph)" : "");
    printf("Work: %llu\n", work);
    free(d); free(head); free(edge); free(order); free(seen); free(active);
    free(answer); free(space); return 0;
}
