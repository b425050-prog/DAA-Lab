/*
 * DAA Laboratory - Lab 05 - Question 3
 * Generate N reproducible random values, store/read them through a file,
 * sort with seeded randomized 3-way Quick Sort, and verify every stage.
 */

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q3_quicksort.h"

enum { PATH_CAPACITY = 512, LINE_CAPACITY = 512 };
static const size_t MAX_N = 1000000U;

static int read_line(char *buffer, size_t capacity) {
    if (!fgets(buffer, (int)capacity, stdin)) return 0;
    char *newline = strchr(buffer, '\n');
    if (newline) {
        *newline = '\0';
    } else if (!feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        return -1;
    }
    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\r') buffer[length - 1] = '\0';
    return 1;
}

static int parse_unsigned(const char *text, uint64_t *value) {
    while (isspace((unsigned char)*text)) ++text;
    if (*text == '-' || *text == '\0') return 0;
    errno = 0;
    char *end;
    uintmax_t parsed = strtoumax(text, &end, 10);
    if (errno == ERANGE || end == text || parsed > UINT64_MAX) return 0;
    while (isspace((unsigned char)*end)) ++end;
    if (*end != '\0') return 0;
    *value = (uint64_t)parsed;
    return 1;
}

static int prompt_size(size_t *n) {
    char line[LINE_CAPACITY];
    uint64_t parsed;
    printf("Enter N (1 to %zu): ", MAX_N);
    fflush(stdout);
    if (read_line(line, sizeof line) != 1 || !parse_unsigned(line, &parsed) ||
        parsed == 0 || parsed > (uint64_t)MAX_N) {
        fprintf(stderr, "Invalid N. Enter an integer from 1 to %zu.\n", MAX_N);
        return 0;
    }
    *n = (size_t)parsed;
    return 1;
}

static int prompt_seed(uint64_t *seed) {
    char line[LINE_CAPACITY];
    printf("Enter deterministic seed (0 to %" PRIu64 "): ", UINT64_MAX);
    fflush(stdout);
    if (read_line(line, sizeof line) != 1 || !parse_unsigned(line, seed)) {
        fprintf(stderr, "Invalid seed. Enter an unsigned 64-bit integer.\n");
        return 0;
    }
    return 1;
}

static int prompt_path(const char *prompt, const char *default_path,
                       char *path, size_t capacity) {
    char line[LINE_CAPACITY];
    printf("%s [%s]: ", prompt, default_path);
    fflush(stdout);
    int status = read_line(line, sizeof line);
    if (status != 1) {
        fprintf(stderr, status == 0 ? "Unexpected end of input.\n"
                                    : "File path is too long.\n");
        return 0;
    }

    char *start = line;
    while (isspace((unsigned char)*start)) ++start;
    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)end[-1])) --end;
    *end = '\0';
    if (*start == '\0') start = (char *)default_path;
    if (strlen(start) >= capacity) {
        fprintf(stderr, "File path must be shorter than %zu characters.\n",
                capacity);
        return 0;
    }
    strcpy(path, start);
    return 1;
}

static int write_values(const char *path, const long long *values, size_t n,
                        uint64_t seed, const char *description) {
    FILE *file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "Cannot open '%s' for writing: %s\n", path,
                strerror(errno));
        return 0;
    }
    fprintf(file, "# DAA Lab 05 Q3 - %s\n", description);
    fprintf(file, "# count=%zu seed=%" PRIu64 " generator=SplitMix64\n", n,
            seed);
    fprintf(file, "# one signed 64-bit integer per following line\n");
    for (size_t i = 0; i < n; ++i) fprintf(file, "%lld\n", values[i]);

    int ok = !ferror(file);
    if (fclose(file) != 0) ok = 0;
    if (!ok) fprintf(stderr, "Failed while writing '%s'.\n", path);
    return ok;
}

static int read_values(const char *path, long long *values, size_t expected) {
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Cannot open '%s' for reading: %s\n", path,
                strerror(errno));
        return 0;
    }

    char line[LINE_CAPACITY];
    size_t used = 0;
    size_t line_number = 0;
    int ok = 1;
    while (fgets(line, sizeof line, file)) {
        ++line_number;
        if (!strchr(line, '\n') && !feof(file)) {
            fprintf(stderr, "Line %zu in '%s' is too long.\n", line_number,
                    path);
            ok = 0;
            break;
        }
        char *start = line;
        while (isspace((unsigned char)*start)) ++start;
        if (*start == '\0' || *start == '#') continue;

        errno = 0;
        char *end;
        long long value = strtoll(start, &end, 10);
        if (errno == ERANGE || end == start) {
            fprintf(stderr, "Invalid integer on line %zu in '%s'.\n",
                    line_number, path);
            ok = 0;
            break;
        }
        while (isspace((unsigned char)*end)) ++end;
        if (*end != '\0') {
            fprintf(stderr, "Unexpected text on line %zu in '%s'.\n",
                    line_number, path);
            ok = 0;
            break;
        }
        if (used == expected) {
            fprintf(stderr, "File '%s' contains more than %zu values.\n", path,
                    expected);
            ok = 0;
            break;
        }
        values[used++] = value;
    }
    if (ferror(file)) ok = 0;
    if (fclose(file) != 0) ok = 0;
    if (ok && used != expected) {
        fprintf(stderr, "File '%s' contains %zu values; expected %zu.\n", path,
                used, expected);
        ok = 0;
    }
    return ok;
}

static void print_values_preview(const char *label, const long long *values,
                                 size_t n) {
    const size_t side = 8;
    printf("%s", label);
    if (n <= 2 * side) {
        for (size_t i = 0; i < n; ++i) printf("%s%lld", i ? "  " : "", values[i]);
    } else {
        for (size_t i = 0; i < side; ++i) printf("%s%lld", i ? "  " : "", values[i]);
        printf("  ...");
        for (size_t i = n - side; i < n; ++i) printf("  %lld", values[i]);
    }
    putchar('\n');
}

static void print_fingerprint(Q3Fingerprint fingerprint) {
    printf("%016" PRIx64 "-%016" PRIx64 "-%016" PRIx64,
           fingerprint.value_sum, fingerprint.mixed_xor,
           fingerprint.mixed_sum);
}

int main(void) {
    size_t n;
    uint64_t seed;
    char input_path[PATH_CAPACITY];
    char output_path[PATH_CAPACITY];

    printf("============================================================\n");
    printf(" DAA Lab 05 - Q3: Random-File Three-Way Quick Sort\n");
    printf("============================================================\n");
    if (!prompt_size(&n) || !prompt_seed(&seed) ||
        !prompt_path("Random-data file", "q3_random_input.txt", input_path,
                     sizeof input_path) ||
        !prompt_path("Sorted-output file", "q3_sorted_output.txt", output_path,
                     sizeof output_path)) {
        return 1;
    }
    if (strcmp(input_path, output_path) == 0) {
        fprintf(stderr, "Input and output paths must be different.\n");
        return 1;
    }

    if (n > SIZE_MAX / sizeof(long long)) {
        fprintf(stderr, "N is too large for this platform.\n");
        return 1;
    }
    long long *generated = (long long *)malloc(n * sizeof *generated);
    long long *values = (long long *)malloc(n * sizeof *values);
    if (!generated || !values) {
        fprintf(stderr, "Memory allocation failed for %zu values.\n", n);
        free(generated);
        free(values);
        return 1;
    }

    q3_generate_values(generated, n, seed);
    Q3Fingerprint original = q3_fingerprint(generated, n);
    if (!write_values(input_path, generated, n, seed,
                      "reproducible unsorted random values") ||
        !read_values(input_path, values, n)) {
        free(generated);
        free(values);
        return 1;
    }

    Q3Fingerprint input_round_trip = q3_fingerprint(values, n);
    int input_ok = q3_same_fingerprint(original, input_round_trip);
    print_values_preview("Input preview  : ", values, n);
    if (!input_ok) {
        fprintf(stderr, "Input file round-trip changed the generated multiset.\n");
        free(generated);
        free(values);
        return 2;
    }

    Q3Metrics metrics;
    q3_quicksort(values, n, seed, &metrics);
    Q3Fingerprint sorted_memory = q3_fingerprint(values, n);
    int memory_sorted = q3_is_sorted(values, n);
    int memory_multiset = q3_same_fingerprint(original, sorted_memory);
    if (!memory_sorted || !memory_multiset ||
        !write_values(output_path, values, n, seed,
                      "ascending three-way Quick Sort result") ||
        !read_values(output_path, generated, n)) {
        fprintf(stderr, "Sorting or sorted-output validation failed.\n");
        free(generated);
        free(values);
        return 2;
    }

    Q3Fingerprint output_round_trip = q3_fingerprint(generated, n);
    int output_sorted = q3_is_sorted(generated, n);
    int output_multiset = q3_same_fingerprint(original, output_round_trip);
    int stack_ok = metrics.max_stack_frames <= q3_stack_frame_bound(n);
    int all_ok = input_ok && memory_sorted && memory_multiset && output_sorted &&
                 output_multiset && stack_ok;

    print_values_preview("Sorted preview : ", generated, n);
    printf("\nFiles\n");
    printf("  Random input  : %s\n", input_path);
    printf("  Sorted output : %s\n", output_path);
    printf("\nQuick Sort metrics\n");
    printf("  Pivot comparisons : %" PRIu64 "\n", metrics.comparisons);
    printf("  Swaps             : %" PRIu64 "\n", metrics.swaps);
    printf("  Partitions         : %" PRIu64 "\n", metrics.partitions);
    printf("  Values partitioned : %" PRIu64 "\n", metrics.partitioned_values);
    printf("  Maximum stack      : %zu frame(s), bound %zu\n",
           metrics.max_stack_frames, q3_stack_frame_bound(n));
    printf("\nValidation\n");
    printf("  Input round-trip   : %s\n", input_ok ? "PASS" : "FAIL");
    printf("  Ascending order    : %s\n", output_sorted ? "PASS" : "FAIL");
    printf("  Multiset checksum  : %s (", output_multiset ? "PASS" : "FAIL");
    print_fingerprint(output_round_trip);
    printf(")\n");
    printf("  Output round-trip  : %s\n",
           output_sorted && output_multiset ? "PASS" : "FAIL");
    printf("  Stack bound        : %s\n", stack_ok ? "PASS" : "FAIL");
    printf("  Overall            : %s\n", all_ok ? "PASS" : "FAIL");
    printf("\nExpected sorting time: Theta(n log n); worst case: Theta(n^2).\n");
    printf("3-way partitioning processes an all-equal range in Theta(n).\n");
    printf("Quick Sort stack space is O(log n) because only the smaller side recurses.\n");

    free(generated);
    free(values);
    return all_ok ? 0 : 2;
}
