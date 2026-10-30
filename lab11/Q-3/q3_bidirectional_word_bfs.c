#include "../common/lab11_common.h"
#include <stdint.h>
static uint64_t *word;
static int *table, capacity, length;
static uint64_t read_word(void) {
    char s[12]; need(scanf("%11s", s) == 1, "missing word");
    need((int)strlen(s) == length, "word length must equal L");
    uint64_t code = 0;
    for (int i = 0; i < length; ++i) {
        need(s[i] >= 'a' && s[i] <= 'z', "use lowercase English letters");
        code = 26*code+(unsigned)(s[i]-'a'); ++work;
    }
    return code;
}
static int slot(uint64_t code) {
    uint64_t x = code+UINT64_C(0x9e3779b97f4a7c15);
    x = (x^(x>>30))*UINT64_C(0xbf58476d1ce4e5b9);
    x = (x^(x>>27))*UINT64_C(0x94d049bb133111eb); x ^= x>>31;
    int p = (int)(x & (unsigned)(capacity-1));
    for (;;) {
        ++work; if (!table[p] || word[table[p]-1] == code) return p;
        p = (p+1)&(capacity-1);
    }
}
static void print_word(uint64_t code) {
    char s[11]; s[length] = '\0';
    for (int i = length-1; i >= 0; --i) { s[i] = 'a'+(int)(code%26); code /= 26; }
    printf("%s", s);
}
int main(void) {
    int n = (int)number(1, 100000); length = (int)number(1, 10);
    uint64_t a = read_word(), b = read_word(), power[10]; power[0] = 1;
    for (int j = 1; j < length; ++j) power[j] = 26*power[j-1];
    capacity = 1; while (capacity < 4*(n+1)) capacity *= 2;
    word = memory((size_t)n+1, sizeof(*word));
    table = memory((size_t)capacity, sizeof(*table));
    for (int i = 0; i < n; ++i) {
        word[i] = read_word(); int p = slot(word[i]);
        need(!table[p], "dictionary words must be unique"); table[p] = i+1;
    }
    finish(); int target = table[slot(b)]-1, source = table[slot(a)]-1;
    if (source < 0) { source = n++; word[source] = a; table[slot(a)] = source+1; }
    int *distance[2], *parent[2], *queue[2], head[2] = {0,0}, tail[2] = {0,0};
    for (int s = 0; s < 2; ++s) {
        distance[s] = memory((size_t)n, sizeof(int));
        parent[s] = memory((size_t)n, sizeof(int)); queue[s] = memory((size_t)n, sizeof(int));
        for (int i = 0; i < n; ++i) { distance[s][i] = parent[s][i] = -1; ++work; }
    }
    int meet = -1, best = 1000000000;
    if (target >= 0) {
        distance[0][source] = distance[1][target] = 0;
        queue[0][tail[0]++] = source; queue[1][tail[1]++] = target; work += 2;
        if (source == target) { meet = source; best = 0; }
    }
    while (meet < 0 && head[0] < tail[0] && head[1] < tail[1]) {
        int s = tail[0]-head[0] <= tail[1]-head[1] ? 0 : 1, end = tail[s];
        /* Finish the entire chosen layer before accepting its best meeting. */
        while (head[s] < end) {
            int u = queue[s][head[s]++]; ++work;
            for (int j = 0; j < length; ++j) {
                int old = (int)(word[u]/power[j]%26);
                for (int c = 0; c < 26; ++c) if (c != old) {
                    ++work;
                    uint64_t code = word[u]-old*power[j]+c*power[j];
                    int v = table[slot(code)]-1; if (v < 0) continue;
                    if (distance[s][v] < 0) {
                        distance[s][v] = distance[s][u]+1; parent[s][v] = u;
                        queue[s][tail[s]++] = v; ++work;
                    }
                    int other = distance[1-s][v];
                    if (other >= 0 && distance[s][v]+other < best) {
                        best = distance[s][v]+other; meet = v;
                    }
                }
            }
        }
    }
    printf("Minimum mutations: %d\n", meet < 0 ? -1 : best);
    if (meet >= 0) {
        int *path = memory((size_t)n, sizeof(*path)), count = 0;
        for (int u = meet; u >= 0; u = parent[0][u]) path[count++] = u;
        printf("Sequence: ");
        for (int i = count-1; i >= 0; --i) { print_word(word[path[i]]); if (i) printf(" -> "); }
        for (int u = parent[1][meet]; u >= 0; u = parent[1][u]) { printf(" -> "); print_word(word[u]); }
        puts(""); free(path);
    } else puts("Sequence: NONE");
    printf("Work: %llu\n", work);
    for (int s = 0; s < 2; ++s) { free(distance[s]); free(parent[s]); free(queue[s]); }
    free(word); free(table); return 0;
}
