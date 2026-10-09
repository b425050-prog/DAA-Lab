#include "../common/lab9_common.h"
int main(int argc, char **argv) {
    int json = json_mode(argc, argv), n = (int)integer(0, 100000);
    Entry *space = memory((size_t)n+1, sizeof(*space)); Heap h = {space, 0, 0};
    Int *steps = memory((size_t)3*n, sizeof(*steps)), total = 0; int count = 0;
    for (int i = 0; i < n; ++i) push(&h, (Entry){integer(1, 1000000000), i});
    finish();
    while (h.n > 1) {
        Entry a = pop(&h), b = pop(&h); Int sum = a.value+b.value;
        steps[3*count] = a.value; steps[3*count+1] = b.value;
        steps[3*count+2] = sum; ++count; total += sum;
        push(&h, (Entry){sum, n+count});
    }
    if (json) {
        printf("{\"cost\":%lld,\"merges\":[", total);
        for (int i = 0; i < count; ++i) {
            printf("%s", i ? "," : ""); numbers(steps+3*i, 3);
        }
        printf("],\"work\":%llu}\n", work);
    } else {
        for (int i = 0; i < count && i < 20; ++i)
            printf("Merge %d: %lld + %lld = %lld\n", i+1,
                   steps[3*i], steps[3*i+1], steps[3*i+2]);
        if (count > 20) puts("Further merges omitted; use --json for the full trace.");
        printf("Minimum total cost: %lld\nWork: %llu\n", total, work);
    }
    free(space); free(steps); return 0;
}
