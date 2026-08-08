#include <stdio.h>

typedef enum {
    GROWTH_CONSTANT,
    GROWTH_LOGARITHMIC,
    GROWTH_LINEAR
} Growth;

typedef struct {
    const char *representation;
    Growth search;
    Growth insert;
    Growth delete_op;
    Growth maximum;
    Growth minimum;
    Growth predecessor;
    Growth successor;
} ComplexityRow;

static const char *growth_text(Growth growth) {
    switch (growth) {
        case GROWTH_CONSTANT:    return "O(1)";
        case GROWTH_LOGARITHMIC: return "O(log n)";
        case GROWTH_LINEAR:      return "O(n)";
    }
    return "-";
}

static void print_rule(void) {
    puts("+--------------------------+----------+----------+----------+----------+----------+-------------+-----------+");
}

static void print_table(const ComplexityRow rows[], int count) {
    print_rule();
    puts("| Representation           | Search   | Insert   | Delete   | Maximum  | Minimum  | Predecessor | Successor |");
    print_rule();

    for (int i = 0; i < count; ++i) {
        printf("| %-24s | %-8s | %-8s | %-8s | %-8s | %-8s | %-11s | %-9s |\n",
               rows[i].representation,
               growth_text(rows[i].search),
               growth_text(rows[i].insert),
               growth_text(rows[i].delete_op),
               growth_text(rows[i].maximum),
               growth_text(rows[i].minimum),
               growth_text(rows[i].predecessor),
               growth_text(rows[i].successor));
    }

    print_rule();
}

int main(void) {
    const ComplexityRow rows[] = {
        {"Unsorted array",         GROWTH_LINEAR,      GROWTH_CONSTANT, GROWTH_CONSTANT, GROWTH_LINEAR,   GROWTH_LINEAR,   GROWTH_LINEAR, GROWTH_LINEAR},
        {"Sorted array",           GROWTH_LOGARITHMIC, GROWTH_LINEAR,   GROWTH_LINEAR,   GROWTH_CONSTANT, GROWTH_CONSTANT, GROWTH_CONSTANT, GROWTH_CONSTANT},
        {"Singly linked unsorted", GROWTH_LINEAR,      GROWTH_CONSTANT, GROWTH_LINEAR,   GROWTH_LINEAR,   GROWTH_LINEAR,   GROWTH_LINEAR, GROWTH_LINEAR},
        {"Singly linked sorted",   GROWTH_LINEAR,      GROWTH_LINEAR,   GROWTH_LINEAR,   GROWTH_CONSTANT, GROWTH_CONSTANT, GROWTH_LINEAR, GROWTH_CONSTANT},
        {"Doubly linked unsorted", GROWTH_LINEAR,      GROWTH_CONSTANT, GROWTH_CONSTANT, GROWTH_LINEAR,   GROWTH_LINEAR,   GROWTH_LINEAR, GROWTH_LINEAR},
        {"Doubly linked sorted",   GROWTH_LINEAR,      GROWTH_LINEAR,   GROWTH_CONSTANT, GROWTH_CONSTANT, GROWTH_CONSTANT, GROWTH_CONSTANT, GROWTH_CONSTANT}
    };

    const int row_count = (int)(sizeof(rows) / sizeof(rows[0]));

    puts("");
    puts("================================================================================================================");
    puts("                         DAA LAB 02 - QUESTION 1 : DICTIONARY OPERATIONS");
    puts("                                      Student: Satyam Dhal");
    puts("================================================================================================================");
    puts("");

    print_table(rows, row_count);

    puts("");
    puts("Key reasoning");
    puts("-------------");
    puts("1. Unsorted arrays support O(1) insertion and O(1) deletion of a known item by replacing the hole with the last item.");
    puts("2. Sorted arrays support O(log n) search through binary search, but insertion and deletion can shift O(n) items.");
    puts("3. A singly linked list needs O(n) worst-case time to delete a known node because its physical predecessor may have to be found.");
    puts("4. A doubly linked list stores the predecessor link explicitly, so deletion of a known node is O(1).");
    puts("5. Sorting a linked list does not enable binary search, so search remains O(n).");
    puts("6. For sorted linked lists, minimum is at the head and maximum can be maintained by a tail pointer.");
    puts("7. In a singly linked sorted list, predecessor is O(n), while successor is the next link and is O(1).");

    puts("");
    puts("Final observation");
    puts("-----------------");
    puts("No single representation is best for every dictionary operation; faster queries generally trade against update cost.");
    puts("================================================================================================================");

    return 0;
}
