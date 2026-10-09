#include "../common/lab9_common.h"
int main(int argc, char **argv) {
    int json = json_mode(argc, argv), n = (int)integer(0, 100000);
    Int *rating = memory((size_t)n, sizeof(*rating));
    Int *candy = memory((size_t)n, sizeof(*candy)); Int total = 0;
    for (int i = 0; i < n; ++i) { rating[i] = integer(-1000000000, 1000000000); candy[i] = 1; }
    finish();
    for (int i = 1; i < n; ++i) {
        ++work; if (rating[i] > rating[i-1]) candy[i] = candy[i-1]+1;
    }
    for (int i = n-2; i >= 0; --i) {
        ++work;
        if (rating[i] > rating[i+1] && candy[i] <= candy[i+1])
            candy[i] = candy[i+1]+1;
    }
    for (int i = 0; i < n; ++i) total += candy[i];
    if (json) {
        printf("{\"total\":%lld,\"candies\":", total); numbers(candy, n);
        printf(",\"work\":%llu}\n", work);
    } else {
        printf("Ratings: "); numbers(rating, n);
        printf("\nCandies: "); numbers(candy, n);
        printf("\nMinimum total candies: %lld\nWork: %llu\n", total, work);
    }
    free(rating); free(candy); return 0;
}
