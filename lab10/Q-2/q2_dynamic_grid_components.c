#include "../common/lab10_common.h"
int main(void) {
    int rows = (int)read_int(1, 1000000), cols = (int)read_int(1, 1000000);
    require((Int)rows*cols <= 1000000, "grid may have at most 1000000 cells");
    int k = (int)read_int(0, 200000), n = rows*cols, islands = 0;
    Node *d = create_dsu(n); unsigned char *land = allocate((size_t)n, 1);
    const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
    for (int i = 0; i < k; ++i) {
        int r = (int)read_int(0, rows-1), c = (int)read_int(0, cols-1);
        int id = r*cols+c, duplicate = land[id];
        if (!duplicate) {
            land[id] = 1; ++islands;
            for (int j = 0; j < 4; ++j) {
                ++work; int nr = r+dr[j], nc = c+dc[j];
                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
                int next = nr*cols+nc;
                if (land[next] && unite(d, id, next, 0) >= 0) --islands;
            }
        }
        printf("Add %d (%d,%d): islands=%d%s\n", i+1, r, c, islands,
               duplicate ? " (duplicate)" : "");
    }
    end_input(); printf("Work: %llu\n", work);
    free(d); free(land); return 0;
}
