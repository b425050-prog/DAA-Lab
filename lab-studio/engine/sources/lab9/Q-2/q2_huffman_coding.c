#include "../common/lab9_common.h"
typedef struct { Int weight; int left, right, symbol; } Node;
typedef struct { int symbol, length; } Code;
static void lengths(Node *tree, int node, int depth, Code *codes) {
    if (tree[node].symbol >= 0) {
        int s = tree[node].symbol; codes[s] = (Code){s, depth ? depth : 1};
    } else {
        lengths(tree, tree[node].left, depth+1, codes);
        lengths(tree, tree[node].right, depth+1, codes);
    }
}
static int order(const void *pa, const void *pb) {
    Code a = *(const Code *)pa, b = *(const Code *)pb;
    ++work;
    return a.length != b.length ? a.length-b.length : a.symbol-b.symbol;
}
int main(int argc, char **argv) {
    int json = json_mode(argc, argv), n = (int)integer(1, 128);
    Node *tree = memory((size_t)2*n, sizeof(*tree));
    Entry *space = memory((size_t)n+1, sizeof(*space));
    Heap heap = {space, 0, 0}; Code *codes = memory((size_t)n, sizeof(*codes));
    char symbols[128]; Int frequency[128], by_symbol[128] = {0}, total = 0, cost = 0;
    int seen[128] = {0}, count = n;
    for (int i = 0; i < n; ++i) {
        char token[3]; need(scanf("%2s", token) == 1 && strlen(token) == 1,
                            "symbol must be one printable nonspace byte");
        unsigned char s = (unsigned char)token[0];
        need(s >= 33 && s <= 126 && s != '"' && s != '\\' && !seen[s],
             "duplicate or unsupported symbol"); seen[s] = 1;
        symbols[i] = (char)s; frequency[i] = integer(1, 1000000000);
        by_symbol[s] = frequency[i];
        total += frequency[i]; tree[i] = (Node){frequency[i], -1, -1, i};
        push(&heap, (Entry){frequency[i], i});
    }
    finish();
    while (heap.n > 1) {
        Entry a = pop(&heap), b = pop(&heap);
        tree[count] = (Node){a.value+b.value, a.id, b.id, -1};
        push(&heap, (Entry){a.value+b.value, count++});
    }
    lengths(tree, heap.a[1].id, 0, codes);
    /* Symbol order, rather than input order, breaks equal-length ties. */
    for (int i = 0; i < n; ++i) codes[i].symbol = (unsigned char)symbols[i];
    qsort(codes, (size_t)n, sizeof(*codes), order);
    char bits[129] = {0}; int length = codes[0].length;
    memset(bits, '0', (size_t)length);
    if (json) printf("{\"codes\":[");
    else puts("Canonical codebook (length, then symbol):");
    for (int i = 0; i < n; ++i) {
        if (i) {
            int j = length-1;
            while (j >= 0 && bits[j] == '1') bits[j--] = '0';
            need(j >= 0, "invalid Huffman lengths"); bits[j] = '1';
            while (length < codes[i].length) bits[length++] = '0';
            bits[length] = 0;
        }
        Int f = by_symbol[codes[i].symbol]; cost += f*length;
        if (json) printf("%s{\"symbol\":\"%c\",\"frequency\":%lld,"
                         "\"length\":%d,\"code\":\"%s\"}",
                         i ? "," : "", codes[i].symbol, f, length, bits);
        else printf("%c: %s (%d bits)\n", codes[i].symbol, bits, length);
    }
    if (json) printf("],\"cost\":%lld,\"work\":%llu}\n", cost, work);
    else printf("Weighted bits: %lld\nAverage bits: %.6f\nWork: %llu\n",
                cost, (double)cost/(double)total, work);
    free(tree); free(space); free(codes); return 0;
}
