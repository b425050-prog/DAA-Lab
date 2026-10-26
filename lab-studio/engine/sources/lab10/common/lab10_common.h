#ifndef LAB10_COMMON_H
#define LAB10_COMMON_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
typedef long long Int;
static unsigned long long work;
static inline void require(int ok, const char *message) {
    if (!ok) { fprintf(stderr, "Input error: %s\n", message); exit(1); }
}
static inline void *allocate(size_t n, size_t size) {
    void *p = calloc(n ? n : 1, size);
    require(p != NULL, "allocation failed"); return p;
}
static inline Int read_int(Int lo, Int hi) {
    char token[128], *end; errno = 0;
    require(scanf("%127s", token) == 1, "missing integer");
    Int x = strtoll(token, &end, 10);
    require(!errno && !*end && x >= lo && x <= hi, "integer out of range");
    return x;
}
static inline void end_input(void) {
    char extra[2]; require(scanf("%1s", extra) != 1, "unexpected extra input");
}
typedef struct { int parent, size, parity; Int sum; } Node;
static inline Node *create_dsu(int n) {
    Node *d = allocate((size_t)n, sizeof(*d));
    for (int i = 0; i < n; ++i) d[i] = (Node){i, 1, 0, 0};
    return d;
}
static int find_root(Node *d, int x) {
    ++work;
    if (d[x].parent != x) {
        int old = d[x].parent; d[x].parent = find_root(d, old);
        d[x].parity ^= d[old].parity;
    }
    return d[x].parent;
}
static inline int unite(Node *d, int u, int v, int opposite) {
    int a = find_root(d, u), b = find_root(d, v);
    if (a == b) return -1;
    int relation = d[u].parity ^ d[v].parity ^ opposite;
    if (d[a].size < d[b].size) { int t = a; a = b; b = t; }
    d[b].parent = a; d[b].parity = relation;
    d[a].size += d[b].size; d[a].sum += d[b].sum;
    return a;
}
/* A lazy max-heap of component snapshots, including negative sums. */
typedef struct { Int value; int root; } Entry;
typedef struct { Entry *a; int n; } MaxHeap;
static inline int greater(Entry a, Entry b) {
    ++work; return a.value != b.value ? a.value > b.value : a.root < b.root;
}
static inline void heap_push(MaxHeap *h, Entry x) {
    int i = ++h->n;
    while (i > 1 && greater(x, h->a[i/2])) {
        h->a[i] = h->a[i/2]; i /= 2;
    }
    h->a[i] = x;
}
static inline void heap_pop(MaxHeap *h) {
    Entry x = h->a[h->n--]; int i = 1;
    while (2*i <= h->n) {
        int j = 2*i;
        if (j < h->n && greater(h->a[j+1], h->a[j])) ++j;
        if (!greater(h->a[j], x)) break;
        h->a[i] = h->a[j]; i = j;
    }
    if (h->n) h->a[i] = x;
}
static inline int maximum(MaxHeap *h, Node *d, int limit, Int *answer) {
    while (h->n) {
        Entry e = h->a[1]; ++work;
        if (d[e.root].parent == e.root && d[e.root].size <= limit &&
            d[e.root].sum == e.value) { *answer = e.value; return 1; }
        heap_pop(h);
    }
    return 0;
}
#endif
