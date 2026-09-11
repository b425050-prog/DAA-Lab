#include "../common/lab7_common.h"

int main(void) {
    unsigned eggs = (unsigned)read_u64(1, 64);
    uint64_t floors = read_u64(0, UINT64_C(1000000000000));
    end_input();
    uint64_t reach[65] = {0}, drops = 0, first = 0;
    if (eggs == 1) {
        drops = floors;
        first = floors ? 1 : 0;
    } else {
        while (reach[eggs] < floors) {
            first = reach[eggs - 1] + 1;
            ++drops;
            /* Descending order preserves both entries from the previous row. */
            for (unsigned e = eggs; e > 0; --e) {
                uint64_t next = reach[e] + reach[e - 1] + 1;
                reach[e] = next < floors ? next : floors;
            }
        }
    }
    printf("Minimum worst-case drops: %" PRIu64 "\n", drops);
    if (floors) printf("One optimal first drop: floor %" PRIu64 "\n", first);
    else puts("No drops required.");
    return 0;
}
