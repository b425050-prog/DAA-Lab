#include "../common/lab11_common.h"
typedef struct { int to, next; } Edge;
int main(void) {
    int n = (int)number(1, 200000), m = (int)number(0, 400000);
    int k = (int)number(0, n), radius = (int)number(0, 200000);
    int *head = memory((size_t)n, sizeof(*head));
    int *owner = memory((size_t)n, sizeof(*owner));
    int *distance = memory((size_t)n, sizeof(*distance));
    int *queue = memory((size_t)n, sizeof(*queue));
    Edge *edge = memory((size_t)2*m, sizeof(*edge));
    int first = 0, last = 0;
    for (int i = 0; i < n; ++i) {
        head[i] = owner[i] = distance[i] = -1; ++work;
    }
    /* Ordered seeding makes equal-distance ties favour the smaller hub index. */
    for (int i = 1; i <= k; ++i) {
        int h = (int)number(1, n)-1; ++work;
        need(owner[h] == -1, "hub vertices must be distinct");
        owner[h] = i; distance[h] = 0; queue[last++] = h; ++work;
    }
    for (int i = 0; i < m; ++i) {
        int u = (int)number(1, n)-1, v = (int)number(1, n)-1;
        edge[2*i] = (Edge){v, head[u]}; head[u] = 2*i;
        edge[2*i+1] = (Edge){u, head[v]}; head[v] = 2*i+1;
    }
    finish();
    while (first < last) {
        int u = queue[first++]; ++work;
        if (distance[u] == radius) continue;
        for (int i = head[u]; i != -1; i = edge[i].next) {
            int v = edge[i].to; ++work;
            if (owner[v] != -1) continue;
            owner[v] = owner[u]; distance[v] = distance[u]+1;
            queue[last++] = v; ++work;
        }
    }
    puts("Vertex  Owner  Distance");
    for (int i = 0; i < n; ++i) {
        ++work; printf("%d  %d  %d\n", i+1, owner[i], distance[i]);
    }
    printf("Work: %llu\n", work);
    free(head); free(owner); free(distance); free(queue); free(edge);
    return 0;
}
