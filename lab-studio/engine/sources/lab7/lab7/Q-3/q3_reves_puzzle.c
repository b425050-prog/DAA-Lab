#include "../common/lab7_common.h"

static uint64_t best[65], emitted;
static unsigned split[65], peg[4][64], height[4];

static void move_disk(unsigned from, unsigned to, unsigned disk) {
    if (!height[from] || peg[from][height[from] - 1] != disk ||
        (height[to] && peg[to][height[to] - 1] < disk))
        fail("illegal Hanoi move");
    --height[from];
    peg[to][height[to]++] = disk;
    printf("Move %" PRIu64 ": disk %u %c -> %c\n", ++emitted, disk,
           (char)('A' + from), (char)('A' + to));
}

static void hanoi3(unsigned n, unsigned offset, unsigned from, unsigned to,
                   unsigned spare) {
    if (!n) return;
    hanoi3(n - 1, offset, from, spare, to);
    move_disk(from, to, offset + n);
    hanoi3(n - 1, offset, spare, to, from);
}

static void hanoi4(unsigned n, unsigned offset, unsigned from, unsigned to,
                   unsigned spare1, unsigned spare2) {
    if (!n) return;
    unsigned k = split[n];
    hanoi4(k, offset, from, spare1, to, spare2);
    hanoi3(n - k, offset + k, from, to, spare2);
    hanoi4(k, offset, spare1, to, from, spare2);
}

int main(int argc, char **argv) {
    bool count_only = count_mode(argc, argv);
    unsigned n = (unsigned)read_u64(0, 64);
    end_input();
    for (unsigned i = 1; i <= n; ++i) {
        best[i] = UINT64_MAX;
        for (unsigned k = 0; k < i; ++k) {
            unsigned m = i - k;
            uint64_t three = m == 64 ? UINT64_MAX : (UINT64_C(1) << m) - 1;
            if (best[k] > (UINT64_MAX - three) / 2) continue;
            uint64_t candidate = 2 * best[k] + three;
            if (candidate < best[i]) {
                best[i] = candidate;
                split[i] = k;
            }
        }
    }
    printf("Minimum moves: %" PRIu64 "\n", best[n]);
    if (n) printf("Top-level split: %u small disks\n", split[n]);
    if (count_only) return 0;
    for (unsigned i = 0; i < n; ++i) peg[0][i] = n - i;
    height[0] = n;
    hanoi4(n, 0, 0, 3, 1, 2);
    if (height[3] != n || emitted != best[n]) fail("incomplete Hanoi transfer");
    puts("Verified: all disks legally transferred from A to D.");
    return 0;
}
