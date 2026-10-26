#include "../common/lab10_common.h"
int main(void) {
    int n = (int)read_int(1, 200000), m = (int)read_int(0, 200000);
    Node *d = create_dsu(n); int accepted = 0;
    for (int i = 0; i < m; ++i) {
        int u = (int)read_int(1, n)-1, v = (int)read_int(1, n)-1;
        int a = find_root(d, u), b = find_root(d, v);
        int ok = a != b || (d[u].parity ^ d[v].parity) == 1;
        if (ok) {
            if (a != b) unite(d, u, v, 1);
            ++accepted;
        }
        printf("Edge %d (%d,%d): %s\n", i+1, u+1, v+1,
               ok ? "ACCEPTED" : "REJECTED (odd cycle)");
    }
    end_input();
    printf("Accepted arrivals: %d of %d\nFinal colours:", accepted, m);
    for (int i = 0; i < n; ++i) {
        find_root(d, i); printf(" %d", d[i].parity);
    }
    printf("\nWork: %llu\n", work); free(d); return 0;
}
