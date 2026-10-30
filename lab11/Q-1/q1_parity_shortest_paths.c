#include "../common/lab11_common.h"
enum { INF = 1000000000 };
typedef struct { int to, weight, next; } Edge;
typedef struct { int state, distance; } Entry;
int main(void) {
    int n = (int)number(1, 200000), m = (int)number(0, 400000);
    int source = (int)number(1, n)-1;
    long long penalty = number(0, 1000000000000LL);
    int *head = memory((size_t)n, sizeof(*head));
    Edge *edge = memory((size_t)m, sizeof(*edge));
    for (int i = 0; i < n; ++i) { head[i] = -1; ++work; }
    for (int i = 0; i < m; ++i) {
        int u = (int)number(1, n)-1, v = (int)number(1, n)-1;
        int w = (int)number(0, 1);
        edge[i] = (Edge){v, w, head[u]}; head[u] = i;
    }
    finish();
    int states = 2*n, capacity = 2*m+2, first = 0, last = 0;
    int *distance = memory((size_t)states, sizeof(*distance));
    unsigned char *settled = memory((size_t)states, sizeof(*settled));
    Entry *deque = memory((size_t)capacity, sizeof(*deque));
    for (int i = 0; i < states; ++i) { distance[i] = INF; ++work; }
    distance[2*source] = 0;
    deque[last++] = (Entry){2*source, 0}; ++work;
    while (first != last) {
        Entry item = deque[first]; first = (first+1)%capacity; ++work;
        int s = item.state;
        if (settled[s] || item.distance != distance[s]) continue;
        settled[s] = 1;
        for (int i = head[s/2]; i != -1; i = edge[i].next) {
            ++work;
            int w = edge[i].weight, t = 2*edge[i].to+((s%2)^w);
            int next = distance[s]+w;
            if (next >= distance[t]) continue;
            distance[t] = next;
            if (!w) { first = (first+capacity-1)%capacity;
                deque[first] = (Entry){t, next}; }
            else { deque[last] = (Entry){t, next}; last = (last+1)%capacity; }
            ++work;
        }
    }
    puts("Vertex  Even  Odd  Minimum effective cost");
    for (int v = 0; v < n; ++v) {
        ++work; int even = distance[2*v], odd = distance[2*v+1];
        printf("%d", v+1);
        if (even == INF) printf("  INF"); else printf("  %d", even);
        if (odd == INF) printf("  INF"); else printf("  %d", odd);
        if (even == INF && odd == INF) puts("  UNREACHABLE");
        else {
            long long best = even == INF ? odd+penalty : even;
            if (odd != INF && odd+penalty < best) best = odd+penalty;
            printf("  %lld\n", best);
        }
    }
    printf("Work: %llu\n", work);
    free(head); free(edge); free(distance); free(settled); free(deque);
    return 0;
}
