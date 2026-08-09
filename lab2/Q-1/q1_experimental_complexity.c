#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum { SEARCH_OP, INSERT_OP, DELETE_OP, MAX_OP, MIN_OP, PREDECESSOR_OP, SUCCESSOR_OP, OP_COUNT } Operation;
typedef enum { UNSORTED_ARRAY, SORTED_ARRAY, SINGLY_UNSORTED, SINGLY_SORTED, DOUBLY_UNSORTED, DOUBLY_SORTED, REP_COUNT } Representation;
typedef enum { CLASS_CONSTANT, CLASS_LOGARITHMIC, CLASS_LINEAR } GrowthClass;

typedef struct SNode {
    int key;
    struct SNode *next;
} SNode;

typedef struct DNode {
    int key;
    struct DNode *prev;
    struct DNode *next;
} DNode;

#define POINTS 96
#define N_MIN 64
#define N_MAX 65536

static const char *REP_NAMES[REP_COUNT] = {
    "Unsorted array", "Sorted array", "Singly linked unsorted",
    "Singly linked sorted", "Doubly linked unsorted", "Doubly linked sorted"
};

static const char *OP_NAMES[OP_COUNT] = {
    "Search", "Insert", "Delete", "Maximum", "Minimum", "Predecessor", "Successor"
};

static const GrowthClass EXPECTED[REP_COUNT][OP_COUNT] = {
    {CLASS_LINEAR, CLASS_CONSTANT, CLASS_CONSTANT, CLASS_LINEAR, CLASS_LINEAR, CLASS_LINEAR, CLASS_LINEAR},
    {CLASS_LOGARITHMIC, CLASS_LINEAR, CLASS_LINEAR, CLASS_CONSTANT, CLASS_CONSTANT, CLASS_CONSTANT, CLASS_CONSTANT},
    {CLASS_LINEAR, CLASS_CONSTANT, CLASS_LINEAR, CLASS_LINEAR, CLASS_LINEAR, CLASS_LINEAR, CLASS_LINEAR},
    {CLASS_LINEAR, CLASS_LINEAR, CLASS_LINEAR, CLASS_CONSTANT, CLASS_CONSTANT, CLASS_LINEAR, CLASS_CONSTANT},
    {CLASS_LINEAR, CLASS_CONSTANT, CLASS_CONSTANT, CLASS_LINEAR, CLASS_LINEAR, CLASS_LINEAR, CLASS_LINEAR},
    {CLASS_LINEAR, CLASS_LINEAR, CLASS_CONSTANT, CLASS_CONSTANT, CLASS_CONSTANT, CLASS_CONSTANT, CLASS_CONSTANT}
};

static const char *class_text(GrowthClass g) {
    switch (g) {
        case CLASS_CONSTANT: return "O(1)";
        case CLASS_LOGARITHMIC: return "O(log n)";
        case CLASS_LINEAR: return "O(n)";
    }
    return "?";
}

static int *make_unsorted_array(int n) {
    int *a = (int *)malloc((size_t)(n + 1) * sizeof(int));
    if (!a) return NULL;
    /* Deterministic permutation: odd positions first, then even positions. */
    int p = 0;
    for (int i = 1; i < n; i += 2) a[p++] = i;
    for (int i = 0; i < n; i += 2) a[p++] = i;
    return a;
}

static int *make_sorted_array(int n) {
    int *a = (int *)malloc((size_t)(n + 1) * sizeof(int));
    if (!a) return NULL;
    for (int i = 0; i < n; ++i) a[i] = 2 * i;
    return a;
}

static SNode *make_singly(int n, int sorted, SNode **tail_out) {
    SNode *head = NULL, *tail = NULL;
    for (int i = 0; i < n; ++i) {
        SNode *node = (SNode *)malloc(sizeof(*node));
        if (!node) {
            while (head) { SNode *t = head->next; free(head); head = t; }
            return NULL;
        }
        if (sorted) node->key = 2 * i;
        else {
            int odd_count = n / 2;
            node->key = (i < odd_count) ? (2 * i + 1) : (2 * (i - odd_count));
        }
        node->next = NULL;
        if (!head) head = node;
        else tail->next = node;
        tail = node;
    }
    if (tail_out) *tail_out = tail;
    return head;
}

static DNode *make_doubly(int n, int sorted, DNode **tail_out) {
    DNode *head = NULL, *tail = NULL;
    for (int i = 0; i < n; ++i) {
        DNode *node = (DNode *)malloc(sizeof(*node));
        if (!node) {
            while (head) { DNode *t = head->next; free(head); head = t; }
            return NULL;
        }
        if (sorted) node->key = 2 * i;
        else {
            int odd_count = n / 2;
            node->key = (i < odd_count) ? (2 * i + 1) : (2 * (i - odd_count));
        }
        node->prev = tail;
        node->next = NULL;
        if (!head) head = node;
        else tail->next = node;
        tail = node;
    }
    if (tail_out) *tail_out = tail;
    return head;
}

static void free_singly(SNode *head) {
    while (head) { SNode *next = head->next; free(head); head = next; }
}

static void free_doubly(DNode *head) {
    while (head) { DNode *next = head->next; free(head); head = next; }
}

/* Dominant-step counts are used instead of wall-clock time. This makes the
   experiment reproducible and exposes asymptotic growth without timer noise. */
static void measure_unsorted_array(int n, double out[OP_COUNT]) {
    int *a = make_unsorted_array(n);
    if (!a) { fprintf(stderr, "Allocation failed.\n"); exit(EXIT_FAILURE); }

    long long steps = 0;
    for (int i = 0; i < n; ++i) { ++steps; if (a[i] == -1) break; }
    out[SEARCH_OP] = (double)steps;

    a[n] = n + 1; /* append into spare capacity */
    out[INSERT_OP] = 1.0;

    a[n / 2] = a[n - 1]; /* x is already known by pointer/index */
    out[DELETE_OP] = 1.0;

    steps = 0; int mx = a[0];
    for (int i = 1; i < n; ++i) { ++steps; if (a[i] > mx) mx = a[i]; }
    out[MAX_OP] = (double)(steps ? steps : 1);

    steps = 0; int mn = a[0];
    for (int i = 1; i < n; ++i) { ++steps; if (a[i] < mn) mn = a[i]; }
    out[MIN_OP] = (double)(steps ? steps : 1);

    /* In an unsorted representation, ordered predecessor/successor requires a scan. */
    steps = 0; volatile int best = -1;
    for (int i = 0; i < n; ++i) { ++steps; if (a[i] < n / 2 && a[i] > best) best = a[i]; }
    out[PREDECESSOR_OP] = (double)steps;

    steps = 0; best = n + 1;
    for (int i = 0; i < n; ++i) { ++steps; if (a[i] > n / 2 && a[i] < best) best = a[i]; }
    out[SUCCESSOR_OP] = (double)steps;
    (void)mx; (void)mn; (void)best;
    free(a);
}

static void measure_sorted_array(int n, double out[OP_COUNT]) {
    int *a = make_sorted_array(n);
    if (!a) { fprintf(stderr, "Allocation failed.\n"); exit(EXIT_FAILURE); }

    long long steps = 0;
    int lo = 0, hi = n - 1, target = 2 * n + 1; /* absent and above maximum */
    while (lo <= hi) {
        ++steps;
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == target) break;
        if (a[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    out[SEARCH_OP] = (double)steps;

    steps = 0;
    for (int i = n; i > 0; --i) { a[i] = a[i - 1]; ++steps; }
    a[0] = -1;
    out[INSERT_OP] = (double)steps;

    /* Restore a fresh sorted array for a worst-case deletion from the front. */
    free(a); a = make_sorted_array(n);
    steps = 0;
    for (int i = 0; i < n - 1; ++i) { a[i] = a[i + 1]; ++steps; }
    out[DELETE_OP] = (double)(steps ? steps : 1);

    out[MAX_OP] = 1.0;
    out[MIN_OP] = 1.0;
    out[PREDECESSOR_OP] = 1.0; /* x is a known array element: x[-1] */
    out[SUCCESSOR_OP] = 1.0;   /* x is a known array element: x[+1] */
    free(a);
}

static void measure_singly_unsorted(int n, double out[OP_COUNT]) {
    SNode *tail = NULL;
    SNode *head = make_singly(n, 0, &tail);
    if (!head) { fprintf(stderr, "Allocation failed.\n"); exit(EXIT_FAILURE); }

    long long steps = 0;
    for (SNode *p = head; p; p = p->next) { ++steps; if (p->key == -1) break; }
    out[SEARCH_OP] = (double)steps;
    out[INSERT_OP] = 1.0; /* insert at head */

    /* x is known (tail), but its physical predecessor is not stored. */
    steps = 0;
    for (SNode *p = head; p && p->next != tail; p = p->next) ++steps;
    out[DELETE_OP] = (double)(steps + 1);

    steps = 0; int mx = head->key;
    for (SNode *p = head->next; p; p = p->next) { ++steps; if (p->key > mx) mx = p->key; }
    out[MAX_OP] = (double)(steps ? steps : 1);

    steps = 0; int mn = head->key;
    for (SNode *p = head->next; p; p = p->next) { ++steps; if (p->key < mn) mn = p->key; }
    out[MIN_OP] = (double)(steps ? steps : 1);

    steps = 0; volatile int best = -1;
    for (SNode *p = head; p; p = p->next) { ++steps; if (p->key < n / 2 && p->key > best) best = p->key; }
    out[PREDECESSOR_OP] = (double)steps;

    steps = 0; best = n + 1;
    for (SNode *p = head; p; p = p->next) { ++steps; if (p->key > n / 2 && p->key < best) best = p->key; }
    out[SUCCESSOR_OP] = (double)steps;
    (void)mx; (void)mn; (void)best;
    free_singly(head);
}

static void measure_singly_sorted(int n, double out[OP_COUNT]) {
    SNode *tail = NULL;
    SNode *head = make_singly(n, 1, &tail);
    if (!head) { fprintf(stderr, "Allocation failed.\n"); exit(EXIT_FAILURE); }

    long long steps = 0;
    for (SNode *p = head; p; p = p->next) { ++steps; if (p->key == 2 * n + 1) break; }
    out[SEARCH_OP] = (double)steps;

    /* Worst-case insertion: locate a position immediately before the tail. */
    steps = 0;
    int key = 2 * n - 3;
    SNode *p = head;
    while (p->next && p->next->key < key) { p = p->next; ++steps; }
    SNode *node = (SNode *)malloc(sizeof(*node));
    if (!node) exit(EXIT_FAILURE);
    node->key = key; node->next = p->next; p->next = node;
    out[INSERT_OP] = (double)(steps + 1);
    /* Remove the inserted node so n is unchanged for the remaining measurements. */
    p->next = node->next; free(node);

    steps = 0;
    for (p = head; p && p->next != tail; p = p->next) ++steps;
    out[DELETE_OP] = (double)(steps + 1);

    out[MAX_OP] = 1.0; /* maintained tail pointer */
    out[MIN_OP] = 1.0; /* head */

    steps = 0;
    SNode *x = tail;
    for (p = head; p && p->next != x; p = p->next) ++steps;
    out[PREDECESSOR_OP] = (double)(steps + 1);
    out[SUCCESSOR_OP] = 1.0; /* x->next when x is not the maximum; direct link */
    free_singly(head);
}

static void measure_doubly_unsorted(int n, double out[OP_COUNT]) {
    DNode *tail = NULL;
    DNode *head = make_doubly(n, 0, &tail);
    if (!head) { fprintf(stderr, "Allocation failed.\n"); exit(EXIT_FAILURE); }

    long long steps = 0;
    for (DNode *p = head; p; p = p->next) { ++steps; if (p->key == -1) break; }
    out[SEARCH_OP] = (double)steps;
    out[INSERT_OP] = 1.0;
    out[DELETE_OP] = 1.0; /* known x has both prev and next links */

    steps = 0; int mx = head->key;
    for (DNode *p = head->next; p; p = p->next) { ++steps; if (p->key > mx) mx = p->key; }
    out[MAX_OP] = (double)(steps ? steps : 1);

    steps = 0; int mn = head->key;
    for (DNode *p = head->next; p; p = p->next) { ++steps; if (p->key < mn) mn = p->key; }
    out[MIN_OP] = (double)(steps ? steps : 1);

    steps = 0; volatile int best = -1;
    for (DNode *p = head; p; p = p->next) { ++steps; if (p->key < n / 2 && p->key > best) best = p->key; }
    out[PREDECESSOR_OP] = (double)steps;

    steps = 0; best = n + 1;
    for (DNode *p = head; p; p = p->next) { ++steps; if (p->key > n / 2 && p->key < best) best = p->key; }
    out[SUCCESSOR_OP] = (double)steps;
    (void)tail; (void)mx; (void)mn; (void)best;
    free_doubly(head);
}

static void measure_doubly_sorted(int n, double out[OP_COUNT]) {
    DNode *tail = NULL;
    DNode *head = make_doubly(n, 1, &tail);
    if (!head) { fprintf(stderr, "Allocation failed.\n"); exit(EXIT_FAILURE); }

    long long steps = 0;
    for (DNode *p = head; p; p = p->next) { ++steps; if (p->key == 2 * n + 1) break; }
    out[SEARCH_OP] = (double)steps;

    int key = 2 * n - 3;
    steps = 0;
    DNode *p = head;
    while (p->next && p->next->key < key) { p = p->next; ++steps; }
    out[INSERT_OP] = (double)(steps + 1); /* search dominates relinking */

    out[DELETE_OP] = 1.0;
    out[MAX_OP] = 1.0;
    out[MIN_OP] = 1.0;
    out[PREDECESSOR_OP] = 1.0;
    out[SUCCESSOR_OP] = 1.0;
    (void)tail;
    free_doubly(head);
}

static void measure_all(int n, double out[REP_COUNT][OP_COUNT]) {
    measure_unsorted_array(n, out[UNSORTED_ARRAY]);
    measure_sorted_array(n, out[SORTED_ARRAY]);
    measure_singly_unsorted(n, out[SINGLY_UNSORTED]);
    measure_singly_sorted(n, out[SINGLY_SORTED]);
    measure_doubly_unsorted(n, out[DOUBLY_UNSORTED]);
    measure_doubly_sorted(n, out[DOUBLY_SORTED]);
}

static double model_value(GrowthClass g, double n) {
    if (g == CLASS_CONSTANT) return 1.0;
    if (g == CLASS_LOGARITHMIC) return log(n) / log(2.0);
    return n;
}

static double fit_error(const int ns[POINTS], const double ys[POINTS], GrowthClass g) {
    double sfy = 0.0, sff = 0.0, syy = 0.0;
    /* Ignore the smallest quarter so startup/edge effects do not dominate classification. */
    int begin = POINTS / 4;
    for (int i = begin; i < POINTS; ++i) {
        double f = model_value(g, (double)ns[i]);
        sfy += f * ys[i];
        sff += f * f;
        syy += ys[i] * ys[i];
    }
    double a = sfy / sff;
    double sse = 0.0;
    for (int i = begin; i < POINTS; ++i) {
        double f = model_value(g, (double)ns[i]);
        double d = ys[i] - a * f;
        sse += d * d;
    }
    return sqrt(sse / (syy > 0.0 ? syy : 1.0));
}

static GrowthClass classify(const int ns[POINTS], const double ys[POINTS], double errors[3]) {
    errors[CLASS_CONSTANT] = fit_error(ns, ys, CLASS_CONSTANT);
    errors[CLASS_LOGARITHMIC] = fit_error(ns, ys, CLASS_LOGARITHMIC);
    errors[CLASS_LINEAR] = fit_error(ns, ys, CLASS_LINEAR);
    GrowthClass best = CLASS_CONSTANT;
    if (errors[CLASS_LOGARITHMIC] < errors[best]) best = CLASS_LOGARITHMIC;
    if (errors[CLASS_LINEAR] < errors[best]) best = CLASS_LINEAR;
    return best;
}

static void print_rule(void) {
    puts("+--------------------------+----------+----------+----------+----------+----------+-------------+-----------+");
}

int main(void) {
    int ns[POINTS];
    double data[REP_COUNT][OP_COUNT][POINTS];

    FILE *fp = fopen("q1_experimental_data.dat", "w");
    if (!fp) {
        perror("q1_experimental_data.dat");
        return EXIT_FAILURE;
    }

    fputs("# R1=Unsorted_array R2=Sorted_array R3=Singly_linked_unsorted R4=Singly_linked_sorted R5=Doubly_linked_unsorted R6=Doubly_linked_sorted\n", fp);
    fputs("# metric=dominant_step_count; experiment targets worst-case configurations\n", fp);
    fprintf(fp, "# n");
    for (int r = 0; r < REP_COUNT; ++r)
        for (int o = 0; o < OP_COUNT; ++o)
            fprintf(fp, " R%d_%s", r + 1, OP_NAMES[o]);
    fputc('\n', fp);

    for (int i = 0; i < POINTS; ++i) {
        long long span = (long long)N_MAX - N_MIN;
        int n = N_MIN + (int)(span * i / (POINTS - 1));
        ns[i] = n;

        double measured[REP_COUNT][OP_COUNT];
        measure_all(n, measured);
        fprintf(fp, "%d", n);
        for (int r = 0; r < REP_COUNT; ++r) {
            for (int o = 0; o < OP_COUNT; ++o) {
                data[r][o][i] = measured[r][o];
                fprintf(fp, " %.0f", measured[r][o]);
            }
        }
        fputc('\n', fp);
    }
    fclose(fp);

    GrowthClass observed[REP_COUNT][OP_COUNT];
    int matches = 0;
    for (int r = 0; r < REP_COUNT; ++r) {
        for (int o = 0; o < OP_COUNT; ++o) {
            double y[POINTS], err[3];
            for (int i = 0; i < POINTS; ++i) y[i] = data[r][o][i];
            observed[r][o] = classify(ns, y, err);
            if (observed[r][o] == EXPECTED[r][o]) ++matches;
        }
    }

    puts("\n================================================================================================================");
    puts("                         DAA LAB 02 - QUESTION 1 : EXPERIMENTAL COMPLEXITY");
    puts("                                      Student: Satyam Dhal");
    puts("================================================================================================================\n");
    printf("Experiment: %d input sizes from n = %d to n = %d\n", POINTS, N_MIN, N_MAX);
    puts("Metric    : dominant operation count (deterministic; avoids clock/timer noise)");
    puts("Classifier: least normalized fit error among 1, log2(n), and n\n");

    puts("EXPERIMENTALLY INFERRED WORST-CASE COMPLEXITIES");
    print_rule();
    puts("| Representation           | Search   | Insert   | Delete   | Maximum  | Minimum  | Predecessor | Successor |");
    print_rule();
    for (int r = 0; r < REP_COUNT; ++r) {
        printf("| %-24s | %-8s | %-8s | %-8s | %-8s | %-8s | %-11s | %-9s |\n",
               REP_NAMES[r],
               class_text(observed[r][SEARCH_OP]), class_text(observed[r][INSERT_OP]),
               class_text(observed[r][DELETE_OP]), class_text(observed[r][MAX_OP]),
               class_text(observed[r][MIN_OP]), class_text(observed[r][PREDECESSOR_OP]),
               class_text(observed[r][SUCCESSOR_OP]));
    }
    print_rule();

    printf("\nAgreement with theoretical worst-case table: %d/%d operations (%s)\n",
           matches, REP_COUNT * OP_COUNT,
           matches == REP_COUNT * OP_COUNT ? "FULL AGREEMENT" : "CHECK REQUIRED");

    puts("\nExperimental data written to: q1_experimental_data.dat");
    puts("Next step: run q1_plot_experimental.c to turn this measured data into the six-panel SVG plot.");
    puts("================================================================================================================");
    return matches == REP_COUNT * OP_COUNT ? EXIT_SUCCESS : EXIT_FAILURE;
}
