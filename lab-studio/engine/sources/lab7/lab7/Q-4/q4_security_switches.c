#include "../common/lab7_common.h"

static void print_state(uint64_t state, unsigned n) {
    for (unsigned i = n; i > 0; --i)
        putchar((state & (UINT64_C(1) << (i - 1))) ? '1' : '0');
    if (!n) putchar('-');
}

int main(int argc, char **argv) {
    bool count_only = count_mode(argc, argv);
    unsigned n = (unsigned)read_u64(0, 63);
    end_input();
    uint64_t state = (UINT64_C(1) << n) - 1, rank = 0;
    /* Inverse reflected Gray code. The state graph is a single path. */
    for (uint64_t g = state; g; g >>= 1) rank ^= g;
    if (!count_only && rank > UINT64_C(1000000))
        fail("trace exceeds 1000000 moves; rerun with --count");
    printf("Minimum moves: %" PRIu64 "\n", rank);
    if (count_only) return 0;
    printf("Initial: ");
    print_state(state, n);
    putchar('\n');
    uint64_t total = rank;
    while (rank) {
        unsigned bit = 0;
        for (uint64_t x = rank; !(x & 1); x >>= 1) ++bit;
        if (bit && (state & ((UINT64_C(1) << bit) - 1)) !=
                   (UINT64_C(1) << (bit - 1))) fail("illegal switch toggle");
        state ^= UINT64_C(1) << bit;
        --rank;
        printf("Move %" PRIu64 ": switch %u -> ", total - rank, bit + 1);
        print_state(state, n);
        putchar('\n');
    }
    if (state) fail("switches remain on");
    puts("Verified: all switches are off.");
    return 0;
}
