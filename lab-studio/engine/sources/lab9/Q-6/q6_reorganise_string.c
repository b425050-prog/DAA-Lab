#include "../common/lab9_common.h"
int main(int argc, char **argv) {
    int json = json_mode(argc, argv); char *s = memory(100002, 1);
    need(fgets(s, 100002, stdin) != NULL, "missing string line");
    size_t size = strcspn(s, "\r\n");
    need(size <= 100000 && (s[size] || feof(stdin)), "string too long"); s[size] = 0;
    int n = (int)size, k = (int)integer(0, 100000); finish();
    int frequency[128] = {0};
    for (int i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)s[i];
        need(c >= 'a' && c <= 'z', "use lowercase ASCII letters"); ++frequency[c];
    }
    Entry space[129]; Heap h = {space, 0, 1};
    Entry *wait = memory((size_t)n, sizeof(*wait));
    int *release = memory((size_t)n, sizeof(*release)), head = 0, tail = 0;
    char *answer = memory((size_t)n+1, 1); int possible = 1;
    for (int c = 'a'; c <= 'z'; ++c)
        if (frequency[c]) push(&h, (Entry){frequency[c], c});
    for (int t = 0; t < n; ++t) {
        while (head < tail && release[head] <= t) push(&h, wait[head++]);
        if (!h.n) { possible = 0; answer[0] = 0; break; }
        Entry x = pop(&h); answer[t] = (char)x.id;
        if (--x.value) { wait[tail] = x; release[tail++] = t+k; }
    }
    if (json) printf("{\"possible\":%s,\"string\":\"%s\",\"work\":%llu}\n",
                     possible ? "true" : "false", answer, work);
    else printf("K: %d\nReorganised string: %s\nStatus: %s\nWork: %llu\n",
                k, answer, possible ? "valid" : "impossible", work);
    free(s); free(wait); free(release); free(answer); return 0;
}
