#include "../common/lab9_common.h"
enum { MAX_N = 12, MAX_WORD = 64, MAX_TEXT = 769 };
static int overlap(const char *a, const char *b) {
    int x = (int)strlen(a), y = (int)strlen(b);
    for (int k = x < y ? x : y; k > 0; --k) {
        ++work; if (!memcmp(a+x-k, b, (size_t)k)) return k;
    }
    return 0;
}
int main(int argc, char **argv) {
    int json = json_mode(argc, argv), input_n = (int)integer(1, MAX_N), n = 0;
    char original[MAX_N][MAX_WORD+2], word[MAX_N][MAX_TEXT];
    for (int i = 0; i < input_n; ++i) {
        need(scanf("%65s", original[i]) == 1 && strlen(original[i]) <= MAX_WORD,
             "use words of length 1..64");
        for (size_t j = 0; original[i][j]; ++j)
            need(original[i][j] >= 'a' && original[i][j] <= 'z',
                 "use lowercase ASCII letters");
    }
    finish();
    for (int i = 0; i < input_n; ++i) {
        int contained = 0;
        for (int j = 0; j < input_n; ++j) if (i != j && strstr(original[j], original[i])) {
            if (strcmp(original[i], original[j]) || j < i) contained = 1;
        }
        if (!contained) strcpy(word[n++], original[i]);
    }
    char greedy[MAX_N][MAX_TEXT]; memcpy(greedy, word, sizeof(word));
    Int steps[MAX_N-1][3]; int size = n, step = 0;
    while (size > 1) {
        int left = 0, right = 1, best = -1;
        for (int i = 0; i < size; ++i) for (int j = 0; j < size; ++j) if (i != j) {
            int k = overlap(greedy[i], greedy[j]);
            if (k > best) { best = k; left = i; right = j; }
        }
        char merged[MAX_TEXT]; strcpy(merged, greedy[left]);
        strcat(merged, greedy[right]+best);
        steps[step][0] = (Int)strlen(greedy[left]);
        steps[step][1] = (Int)strlen(greedy[right]); steps[step++][2] = best;
        strcpy(greedy[left], merged);
        for (int j = right; j < size-1; ++j) strcpy(greedy[j], greedy[j+1]);
        --size;
    }
    int ov[MAX_N][MAX_N] = {{0}}, states = 1<<n;
    for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j)
        if (i != j) ov[i][j] = overlap(word[i], word[j]);
    int *dp = memory((size_t)states*n, sizeof(*dp));
    int *parent = memory((size_t)states*n, sizeof(*parent));
    for (int i = 0; i < states*n; ++i) { dp[i] = INT_MAX/2; parent[i] = -1; }
    for (int j = 0; j < n; ++j) dp[(1<<j)*n+j] = (int)strlen(word[j]);
    unsigned long long dp_work = 0;
    for (int mask = 1; mask < states; ++mask) for (int j = 0; j < n; ++j) {
        if (!(mask & (1<<j))) continue;
        int previous = mask^(1<<j); if (!previous) continue;
        for (int i = 0; i < n; ++i) if (previous & (1<<i)) {
            ++dp_work; int value = dp[previous*n+i]+(int)strlen(word[j])-ov[i][j];
            if (value < dp[mask*n+j]) { dp[mask*n+j] = value; parent[mask*n+j] = i; }
        }
    }
    int last = 0, mask = states-1, path[MAX_N], count = 0;
    for (int j = 1; j < n; ++j) if (dp[mask*n+j] < dp[mask*n+last]) last = j;
    int optimal = dp[mask*n+last];
    while (last >= 0) {
        path[count++] = last; int next = parent[mask*n+last];
        mask ^= 1<<last; last = next;
    }
    char exact[MAX_TEXT]; strcpy(exact, word[path[count-1]]);
    for (int i = count-2; i >= 0; --i) strcat(exact, word[path[i]]+ov[path[i+1]][path[i]]);
    for (int i = 0; i < input_n; ++i) {
        need(strstr(greedy[0], original[i]) != NULL && strstr(exact, original[i]) != NULL,
             "superstring lost an input");
    }
    if (json) {
        printf("{\"greedy\":\"%s\",\"exact\":\"%s\",\"optimal\":%d,"
               "\"work\":%llu,\"dp_work\":%llu,\"merges\":[",
               greedy[0], exact, optimal, work, dp_work);
        for (int i = 0; i < step; ++i) { printf("%s", i ? "," : ""); numbers(steps[i], 3); }
        puts("]}");
    } else printf("Greedy superstring: %s\nExact superstring: %s\nGreedy length: %zu\n"
                  "Optimal length: %d\nObserved ratio: %.6f\n"
                  "Greedy is a heuristic; this is a finite-instance comparison.\n"
                  "Overlap checks: %llu\nExact DP transitions: %llu\n",
                  greedy[0], exact, strlen(greedy[0]), optimal,
                  (double)strlen(greedy[0])/optimal, work, dp_work);
    free(dp); free(parent); return 0;
}
