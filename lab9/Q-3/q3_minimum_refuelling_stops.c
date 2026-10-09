#include "../common/lab9_common.h"
typedef struct { Int distance, fuel; int id; } Station;
static int order(const void *pa, const void *pb) {
    Station a = *(const Station *)pa, b = *(const Station *)pb; ++work;
    if (a.distance != b.distance) return a.distance < b.distance ? -1 : 1;
    return a.id-b.id;
}
int main(int argc, char **argv) {
    int json = json_mode(argc, argv), n = (int)integer(0, 100000);
    Int target = integer(0, 1000000000), reach = integer(0, 1000000000);
    Station *s = memory((size_t)n, sizeof(*s));
    Entry *space = memory((size_t)n+1, sizeof(*space)); Heap h = {space, 0, 1};
    int *chosen = memory((size_t)n, sizeof(*chosen)); int count = 0, next = 0;
    for (int i = 0; i < n; ++i) {
        s[i] = (Station){integer(0, target), integer(0, 1000000000), i+1};
    }
    finish(); qsort(s, (size_t)n, sizeof(*s), order);
    while (reach < target) {
        while (next < n && s[next].distance <= reach) {
            push(&h, (Entry){s[next].fuel, next}); ++next;
        }
        if (!h.n || h.a[1].value == 0) { count = -1; break; }
        Entry best = pop(&h); reach += best.value; chosen[count++] = best.id;
    }
    /* Convert retroactive choices into the actual travel order. */
    Int *travel = memory((size_t)n, sizeof(*travel)); int stops = 0;
    if (count >= 0) {
        int *selected = memory((size_t)n, sizeof(*selected));
        for (int i = 0; i < count; ++i) selected[chosen[i]] = 1;
        for (int i = 0; i < n; ++i) if (selected[i]) travel[stops++] = s[i].id;
        free(selected);
    }
    if (json) {
        printf("{\"stops\":%d,\"stations\":", count); numbers(travel, stops);
        printf(",\"reach\":%lld,\"work\":%llu}\n", reach, work);
    } else {
        printf("Minimum refuelling stops: %d\nTravel-order station IDs: ", count);
        numbers(travel, stops); printf("\nReachable distance: %lld\nWork: %llu\n", reach, work);
    }
    free(s); free(space); free(chosen); free(travel); return 0;
}
