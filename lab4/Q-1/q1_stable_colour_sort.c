/*
 * DAA Laboratory - Lab 04 - Question 1
 * Stable O(n) grouping of number-colour pairs: red, blue, yellow.
 *
 * The input is already sorted by number. A stable counting-sort pass groups
 * the three colours without disturbing the relative order inside a colour.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    RED = 0,
    BLUE = 1,
    YELLOW = 2,
    COLOUR_COUNT = 3
} Colour;

typedef struct {
    long long number;
    Colour colour;
    size_t original_position;
} Item;

typedef struct {
    unsigned long long classification_reads;
    unsigned long long output_writes;
} Metrics;

static const char *colour_name(Colour colour) {
    static const char *names[COLOUR_COUNT] = {"red", "blue", "yellow"};
    return names[colour];
}

static int parse_colour(const char *word, Colour *colour) {
    char lower[16];
    size_t length = strlen(word);
    if (length >= sizeof lower) return 0;
    for (size_t i = 0; i <= length; ++i) {
        lower[i] = (char)tolower((unsigned char)word[i]);
    }
    if (strcmp(lower, "red") == 0 || strcmp(lower, "r") == 0) {
        *colour = RED;
        return 1;
    }
    if (strcmp(lower, "blue") == 0 || strcmp(lower, "b") == 0) {
        *colour = BLUE;
        return 1;
    }
    if (strcmp(lower, "yellow") == 0 || strcmp(lower, "y") == 0) {
        *colour = YELLOW;
        return 1;
    }
    return 0;
}

static int numbers_are_sorted(const Item *items, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        if (items[i - 1].number > items[i].number) return 0;
    }
    return 1;
}

static void stable_group_by_colour(const Item *input, Item *output, size_t n,
                                   Metrics *metrics) {
    size_t count[COLOUR_COUNT] = {0, 0, 0};
    size_t next[COLOUR_COUNT];

    for (size_t i = 0; i < n; ++i) {
        ++count[input[i].colour];
        ++metrics->classification_reads;
    }

    next[RED] = 0;
    next[BLUE] = count[RED];
    next[YELLOW] = count[RED] + count[BLUE];

    for (size_t i = 0; i < n; ++i) {
        Colour colour = input[i].colour;
        output[next[colour]++] = input[i];
        ++metrics->output_writes;
    }
}

static int output_is_valid(const Item *input, const Item *output, size_t n) {
    size_t seen[COLOUR_COUNT] = {0, 0, 0};
    size_t expected[COLOUR_COUNT] = {0, 0, 0};
    size_t previous_position[COLOUR_COUNT] = {0, 0, 0};
    int has_previous[COLOUR_COUNT] = {0, 0, 0};

    for (size_t i = 0; i < n; ++i) ++expected[input[i].colour];

    for (size_t i = 0; i < n; ++i) {
        Colour colour = output[i].colour;
        if (i > 0 && output[i - 1].colour > colour) return 0;
        if (has_previous[colour]) {
            if (previous_position[colour] >= output[i].original_position) return 0;
            if (output[i - 1].colour == colour &&
                output[i - 1].number > output[i].number) return 0;
        }
        previous_position[colour] = output[i].original_position;
        has_previous[colour] = 1;
        ++seen[colour];
    }

    for (int colour = 0; colour < COLOUR_COUNT; ++colour) {
        if (seen[colour] != expected[colour]) return 0;
    }
    return 1;
}

int main(void) {
    size_t n;
    printf("============================================================\n");
    printf(" DAA Lab 04 - Q1: Stable Three-Colour Sort in O(n)\n");
    printf("============================================================\n");
    printf("Enter number of pairs: ");
    if (scanf("%zu", &n) != 1 || n == 0 || n > 1000000U) {
        fprintf(stderr, "Invalid n. Enter a value from 1 to 1000000.\n");
        return 1;
    }

    Item *input = (Item *)malloc(n * sizeof *input);
    Item *output = (Item *)malloc(n * sizeof *output);
    if (!input || !output) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(input);
        free(output);
        return 1;
    }

    printf("Enter each pair as: <number> <red|blue|yellow>\n");
    for (size_t i = 0; i < n; ++i) {
        char word[16];
        if (scanf("%lld %15s", &input[i].number, word) != 2 ||
            !parse_colour(word, &input[i].colour)) {
            fprintf(stderr, "Invalid pair at position %zu.\n", i + 1);
            free(input);
            free(output);
            return 1;
        }
        input[i].original_position = i;
    }

    if (!numbers_are_sorted(input, n)) {
        fprintf(stderr, "Precondition failed: numbers must already be sorted.\n");
        free(input);
        free(output);
        return 1;
    }

    Metrics metrics = {0, 0};
    stable_group_by_colour(input, output, n, &metrics);
    int valid = output_is_valid(input, output, n);

    printf("\nStable colour order:\n");
    printf("%-8s %-12s %-18s\n", "Number", "Colour", "Original position");
    for (size_t i = 0; i < n; ++i) {
        printf("%-8lld %-12s %-18zu\n", output[i].number,
               colour_name(output[i].colour), output[i].original_position + 1);
    }

    printf("\nClassification reads : %llu\n", metrics.classification_reads);
    printf("Output writes        : %llu\n", metrics.output_writes);
    printf("Dominant operations  : %llu = 2n\n",
           metrics.classification_reads + metrics.output_writes);
    printf("Validation           : %s\n", valid ? "PASS" : "FAIL");
    printf("Complexity           : Theta(n) time, Theta(n) auxiliary space\n");
    printf("Stability preserves the original number order inside each colour.\n");

    free(input);
    free(output);
    return valid ? 0 : 2;
}
