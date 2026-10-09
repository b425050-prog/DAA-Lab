#include "../common/lab9_common.h"
typedef struct { Int weight; int left, right, leaf; } Node;
static void depths(Node *tree, int node, int depth, int *level) {
    if (tree[node].leaf >= 0) level[tree[node].leaf] = depth;
    else {
        depths(tree, tree[node].left, depth+1, level);
        depths(tree, tree[node].right, depth+1, level);
    }
}
static void codewords(Node *tree, int node, char *bits, int depth, int json,
                      const Int *weights, const int *level) {
    if (tree[node].leaf >= 0) {
        int i = tree[node].leaf; bits[depth] = 0;
        if (json) printf("%s{\"id\":%d,\"depth\":%d,\"code\":\"%s\"}",
                         i ? "," : "", i+1, level[i], bits);
        else if (i < 20) printf("Leaf %d: weight %lld, depth %d, code %s\n",
                               i+1, weights[i], level[i], bits);
    } else {
        bits[depth] = '0'; codewords(tree, tree[node].left, bits, depth+1, json, weights, level);
        bits[depth] = '1'; codewords(tree, tree[node].right, bits, depth+1, json, weights, level);
    }
}
int main(int argc, char **argv) {
    int json = json_mode(argc, argv), n = (int)integer(1, 256);
    Node *tree = memory((size_t)2*n, sizeof(*tree));
    Node *alphabetic = memory((size_t)2*n, sizeof(*alphabetic));
    Int weights[256], merge[255][3], cost = 0; int active[256], level[256];
    for (int i = 0; i < n; ++i) {
        weights[i] = integer(1, 1000000000); active[i] = i;
        tree[i] = alphabetic[i] = (Node){weights[i], -1, -1, i};
    }
    finish(); int size = n, count = n;
    while (size > 1) {
        Int best = LLONG_MAX; int left = -1, right = -1;
        /* Original leaves block compatibility. Artificial nodes do not.
           Leftmost pair breaks all equal-weight ties consistently. */
        for (int i = 0; i < size-1; ++i) {
            for (int j = i+1; j < size; ++j) {
                ++work; Int sum = tree[active[i]].weight+tree[active[j]].weight;
                if (sum < best) { best = sum; left = i; right = j; }
                if (tree[active[j]].leaf >= 0) break;
            }
        }
        int a = active[left], b = active[right];
        merge[count-n][0] = tree[a].weight; merge[count-n][1] = tree[b].weight;
        merge[count-n][2] = best; cost += best;
        tree[count] = (Node){best, a, b, -1}; active[left] = count++;
        for (int j = right; j < size-1; ++j) active[j] = active[j+1];
        --size;
    }
    depths(tree, active[0], 0, level);
    /* Rebuild from leaf depths in input order; intermediate edges may cross. */
    int stack[256], stack_level[256], top = 0; count = n;
    for (int i = 0; i < n; ++i) {
        stack[top] = i; stack_level[top++] = level[i];
        while (top >= 2 && stack_level[top-1] == stack_level[top-2]) {
            int a = stack[top-2], b = stack[top-1];
            alphabetic[count] = (Node){alphabetic[a].weight+alphabetic[b].weight,
                                       a, b, -1};
            --top; stack[top-1] = count++; --stack_level[top-1];
        }
    }
    need(top == 1 && stack_level[0] == 0, "invalid alphabetic depth sequence");
    char bits[257];
    Int check = 0; for (int i = 0; i < n; ++i) check += weights[i]*level[i];
    need(check == cost, "merge cost differs from weighted leaf depth");
    if (json) printf("{\"cost\":%lld,\"leaves\":[", cost);
    else printf("Optimal alphabetic cost: %lld\n", cost);
    codewords(alphabetic, stack[0], bits, 0, json, weights, level);
    if (json) {
        printf("],\"merges\":[");
        for (int i = 0; i < n-1; ++i) {
            printf("%s", i ? "," : ""); numbers(merge[i], 3);
        }
        printf("],\"work\":%llu}\n", work);
    } else printf("Compatible-pair checks: %llu\n", work);
    free(tree); free(alphabetic); return 0;
}
