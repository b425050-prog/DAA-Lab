#include <inttypes.h>
#include <stdio.h>

#include "q5_fibonacci.h"

int main(void) {
    unsigned int n = 0U; uint64_t value = 0U; Q5Metrics metrics = {0U};
    printf("DAA Lab 06 - Q5: Fibonacci by Dynamic Programming\nEnter n [0..93]: ");
    if (scanf("%u", &n) != 1 || !q5_fibonacci(n, &value, &metrics)) { fprintf(stderr, "n must be in [0,93] for uint64_t.\n"); return 1; }
    printf("F(%u) = %" PRIu64 "\n", n, value);
    printf("DP transitions: %llu\nTime: Theta(n); auxiliary space: Theta(1)\n", metrics.transitions);
    return 0;
}
