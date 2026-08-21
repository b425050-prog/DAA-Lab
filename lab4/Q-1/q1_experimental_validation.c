/* DAA Lab 04 Q1 - deterministic validation of the stable linear algorithm. */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long long number;
    int colour;
    size_t original_position;
} Item;

typedef struct {
    unsigned long long reads;
    unsigned long long writes;
} Metrics;

static void stable_group(const Item *input, Item *output, size_t n, Metrics *m) {
    size_t count[3] = {0, 0, 0};
    size_t next[3];
    for (size_t i = 0; i < n; ++i) {
        ++count[input[i].colour];
        ++m->reads;
    }
    next[0] = 0;
    next[1] = count[0];
    next[2] = count[0] + count[1];
    for (size_t i = 0; i < n; ++i) {
        output[next[input[i].colour]++] = input[i];
        ++m->writes;
    }
}

static int valid(const Item *output, size_t n) {
    size_t previous[3] = {0, 0, 0};
    int seen[3] = {0, 0, 0};
    for (size_t i = 0; i < n; ++i) {
        int colour = output[i].colour;
        if (i > 0 && output[i - 1].colour > colour) return 0;
        if (seen[colour] && previous[colour] >= output[i].original_position) return 0;
        previous[colour] = output[i].original_position;
        seen[colour] = 1;
    }
    return 1;
}

int main(void) {
    FILE *data = fopen("q1_experimental_data.dat", "w");
    if (!data) {
        perror("q1_experimental_data.dat");
        return 1;
    }
    fprintf(data, "# n classification_reads output_writes total reference_2n valid\n");
    printf("%-9s %-14s %-14s %-14s %-10s\n",
           "n", "reads", "writes", "total", "validation");

    for (size_t n = 128; n <= 131072; n *= 2) {
        Item *input = (Item *)malloc(n * sizeof *input);
        Item *output = (Item *)malloc(n * sizeof *output);
        if (!input || !output) {
            free(input);
            free(output);
            fclose(data);
            return 1;
        }
        unsigned int state = 0xC0FFEEU;
        for (size_t i = 0; i < n; ++i) {
            state = state * 1664525U + 1013904223U;
            input[i].number = (long long)(2U * i);
            input[i].colour = (int)((state >> 16) % 3U);
            input[i].original_position = i;
        }
        Metrics m = {0, 0};
        stable_group(input, output, n, &m);
        int ok = valid(output, n);
        unsigned long long total = m.reads + m.writes;
        fprintf(data, "%zu %llu %llu %llu %zu %d\n",
                n, m.reads, m.writes, total, 2U * n, ok);
        printf("%-9zu %-14llu %-14llu %-14llu %-10s\n",
               n, m.reads, m.writes, total, ok ? "PASS" : "FAIL");
        free(input);
        free(output);
        if (!ok) {
            fclose(data);
            return 2;
        }
    }
    fclose(data);
    printf("\nEvery run performs exactly n classifications + n placements = 2n.\n");
    printf("Data written to q1_experimental_data.dat\n");
    return 0;
}
