#include "../common/lab9_common.h"
int main(int argc, char **argv) {
    int json = json_mode(argc, argv), n = (int)integer(1, 100000);
    Int *original = memory((size_t)n, sizeof(*original));
    Entry *space = memory((size_t)n+1, sizeof(*space)); Heap h = {space, 0, 1};
    Int low = LLONG_MAX;
    for (int i = 0; i < n; ++i) {
        Int x = integer(1, 1000000000); original[i] = (x%2) ? 2*x : x;
        if (original[i] < low) low = original[i];
        push(&h, (Entry){original[i], i});
    }
    finish(); Int best = LLONG_MAX, best_low = 0, best_high = 0; int halvings = 0;
    for (;;) {
        Entry high = h.a[1];
        if (high.value-low < best) {
            best = high.value-low; best_low = low; best_high = high.value;
        }
        if (high.value%2) break;
        high = pop(&h); high.value /= 2; ++halvings;
        if (high.value < low) low = high.value;
        push(&h, high);
    }
    /* Recreate a witness in the best recorded interval without copying per step. */
    for (int i = 0; i < n; ++i) {
        while (original[i] > best_high) original[i] /= 2;
        need(original[i] >= best_low, "invalid reconstructed witness");
    }
    if (json) {
        printf("{\"deviation\":%lld,\"values\":", best); numbers(original, n);
        printf(",\"halvings\":%d,\"work\":%llu}\n", halvings, work);
    } else {
        printf("Minimum deviation: %lld\nWitness array: ", best); numbers(original, n);
        printf("\nBest interval: [%lld,%lld]\nHalvings: %d\nWork: %llu\n",
               best_low, best_high, halvings, work);
    }
    free(original); free(space); return 0;
}
