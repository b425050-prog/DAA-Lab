#include "../common/lab9_common.h"
typedef struct { Int start, end; int id; } Meeting;
static int order(const void *pa, const void *pb) {
    Meeting a = *(const Meeting *)pa, b = *(const Meeting *)pb; ++work;
    if (a.start != b.start) return a.start < b.start ? -1 : 1;
    if (a.end != b.end) return a.end < b.end ? -1 : 1;
    return a.id-b.id;
}
int main(int argc, char **argv) {
    int json = json_mode(argc, argv), n = (int)integer(0, 100000);
    Meeting *meetings = memory((size_t)n, sizeof(*meetings));
    Int *assignment = memory((size_t)n, sizeof(*assignment));
    Entry *space = memory((size_t)n+1, sizeof(*space)); Heap h = {space, 0, 0};
    for (int i = 0; i < n; ++i) {
        Int start = integer(0, 1000000000), end = integer(start, 1000000000);
        meetings[i] = (Meeting){start, end, i};
    }
    finish(); qsort(meetings, (size_t)n, sizeof(*meetings), order); int rooms = 0;
    for (int i = 0; i < n; ++i) {
        Meeting m = meetings[i]; if (m.start == m.end) continue;
        int room = h.n && h.a[1].value <= m.start ? pop(&h).id : ++rooms;
        assignment[m.id] = room; push(&h, (Entry){m.end, room});
    }
    if (json) {
        printf("{\"rooms\":%d,\"assignment\":", rooms); numbers(assignment, n);
        printf(",\"work\":%llu}\n", work);
    } else {
        printf("Minimum meeting rooms: %d\nRoom IDs in input order: ", rooms);
        numbers(assignment, n); printf("\nIntervals: [start,end); zero length uses no room.\nWork: %llu\n", work);
    }
    free(meetings); free(assignment); free(space); return 0;
}
