#include "../common/lab8_common.h"
typedef struct { uint64_t steps, peak, last; int status; } Result;
static const char *status_name(int status) { return status == 0 ? "reached_1" : status == 1 ? "overflow" : "step_limit"; }
static Result trajectory(uint64_t start, uint64_t cap, uint64_t **values, size_t *length) {
    Result r = {0, start, start, 0}; size_t capacity = 32;
    if (values) { *values = allocate(capacity, sizeof **values); (*values)[0] = start; *length = 1; }
    while (r.last != 1) {
        if (r.steps == cap) { r.status = 2; break; }
        if ((r.last & 1) && r.last > (UINT64_MAX - 1) / 3) { r.status = 1; break; }
        r.last = (r.last & 1) ? 3 * r.last + 1 : r.last / 2; ++r.steps;
        if (r.last > r.peak) r.peak = r.last;
        if (values) {
            if (*length == capacity) {
                capacity *= 2; uint64_t *next = realloc(*values, capacity * sizeof **values);
                if (!next) fail("trajectory allocation failed");
                *values = next;
            }
            (*values)[(*length)++] = r.last;
        }
    }
    return r;
}
int main(int argc, char **argv) {
    int json = options(argc, argv); uint64_t start = unsigned_integer(), a = unsigned_integer(), b = unsigned_integer(), cap = unsigned_integer(); end_input();
    if (!start || !a || a > b || b - a >= 10000 || !cap || cap > 1000000) fail("require n>=1, 1<=a<=b, at most 10000 starts, and 1<=cap<=1000000");
    uint64_t *values = NULL; size_t length = 0; Result single = trajectory(start, cap, &values, &length);
    size_t count = (size_t)(b - a + 1); Result *rows = allocate(count, sizeof *rows);
    uint64_t work = 0, champion = a, longest = 0; size_t completed = 0, overflow = 0, limited = 0;
    for (size_t i = 0; i < count; ++i) {
        rows[i] = trajectory(a + i, cap, NULL, NULL); work += rows[i].steps;
        if (!rows[i].status) { ++completed; if (rows[i].steps > longest) { longest = rows[i].steps; champion = a + i; } }
        else if (rows[i].status == 1) ++overflow; else ++limited;
    }
    if (json) {
        printf("{\"status\":\"%s\",\"steps\":%" PRIu64 ",\"peak\":%" PRIu64 ",\"trajectory\":", status_name(single.status), single.steps, single.peak); print_u64s(values, length);
        printf(",\"work\":%" PRIu64 ",\"completed\":%zu,\"overflow\":%zu,\"limited\":%zu,\"champion\":%" PRIu64 ",\"longest\":%" PRIu64 ",\"interval\":[", work, completed, overflow, limited, champion, longest);
        for (size_t i = 0; i < count; ++i) {
            printf("%s{\"start\":%" PRIu64 ",\"steps\":%" PRIu64 ",\"peak\":%" PRIu64 ",\"status\":\"%s\"}", i ? "," : "", a + i, rows[i].steps, rows[i].peak, status_name(rows[i].status));
        }
        puts("]}");
    } else {
        printf("Start: %" PRIu64 "\nStatus: %s\nTransitions: %" PRIu64 "\nPeak: %" PRIu64 "\nTrajectory: ", start, status_name(single.status), single.steps, single.peak); print_u64s(values, length);
        printf("\nInterval [ %" PRIu64 ", %" PRIu64 " ]: reached_1=%zu overflow=%zu step_limit=%zu\n", a, b, completed, overflow, limited);
        if (completed) printf("Longest completed trajectory: n=%" PRIu64 ", steps=%" PRIu64 "\n", champion, longest);
        printf("Interval transitions: %" PRIu64 "\nFinite simulation is not a proof of the conjecture.\n", work);
    }
    free(values); free(rows); return 0;
}
