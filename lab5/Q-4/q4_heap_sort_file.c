/*
 * DAA Laboratory - Lab 05 - Question 4
 * Generate random integers in a file, read them back, and heap-sort them.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_N 1000000U
#define PATH_CAPACITY 260U

typedef struct {
    unsigned long long key_comparisons;
    unsigned long long swaps;
    unsigned long long sift_calls;
} Metrics;

typedef struct {
    uint64_t sum;
    uint64_t hash_sum;
    uint64_t hash_xor;
} Fingerprint;

static uint64_t mix64(uint64_t value) {
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static uint64_t next_random(uint64_t *state) {
    *state += UINT64_C(0x9e3779b97f4a7c15);
    return mix64(*state);
}

static long long random_value(uint64_t *state) {
    return (long long)(next_random(state) % UINT64_C(200001)) - 100000LL;
}

static Fingerprint fingerprint(const long long *values, size_t n) {
    Fingerprint result = {0, 0, 0};
    for (size_t i = 0; i < n; ++i) {
        uint64_t bits = (uint64_t)values[i];
        uint64_t hash = mix64(bits + UINT64_C(0x517cc1b727220a95));
        result.sum += bits;
        result.hash_sum += hash;
        result.hash_xor ^= hash;
    }
    return result;
}

static int same_fingerprint(Fingerprint a, Fingerprint b) {
    return a.sum == b.sum && a.hash_sum == b.hash_sum &&
           a.hash_xor == b.hash_xor;
}

static void swap_values(long long *a, long long *b, Metrics *metrics) {
    long long temporary = *a;
    *a = *b;
    *b = temporary;
    ++metrics->swaps;
}

static void sift_down(long long *values, size_t heap_size, size_t root,
                      Metrics *metrics) {
    ++metrics->sift_calls;
    while (root < heap_size / 2U) {
        size_t child = 2U * root + 1U;
        if (child + 1U < heap_size) {
            ++metrics->key_comparisons;
            if (values[child] < values[child + 1U]) {
                ++child;
            }
        }

        ++metrics->key_comparisons;
        if (values[root] >= values[child]) {
            break;
        }

        swap_values(&values[root], &values[child], metrics);
        root = child;
    }
}

static int is_max_heap(const long long *values, size_t n) {
    for (size_t child = 1; child < n; ++child) {
        size_t parent = (child - 1U) / 2U;
        if (values[parent] < values[child]) {
            return 0;
        }
    }
    return 1;
}

static int heap_sort(long long *values, size_t n, Metrics *metrics) {
    for (size_t start = n / 2U; start > 0; --start) {
        sift_down(values, n, start - 1U, metrics);
    }
    int heap_valid = is_max_heap(values, n);

    for (size_t end = n; end > 1U; --end) {
        swap_values(&values[0], &values[end - 1U], metrics);
        sift_down(values, end - 1U, 0, metrics);
    }
    return heap_valid;
}

static int is_sorted(const long long *values, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        if (values[i - 1U] > values[i]) {
            return 0;
        }
    }
    return 1;
}

static int write_random_file(const char *path, size_t n, uint64_t seed) {
    FILE *file = fopen(path, "w");
    if (!file) {
        return 0;
    }

    fprintf(file, "%zu\n", n);
    uint64_t state = seed;
    for (size_t i = 0; i < n; ++i) {
        fprintf(file, "%lld%c", random_value(&state),
                (i + 1U) % 12U == 0U || i + 1U == n ? '\n' : ' ');
    }

    return fclose(file) == 0;
}

static int read_number_file(const char *path, long long **values, size_t *n) {
    FILE *file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    size_t count;
    if (fscanf(file, "%zu", &count) != 1 || count == 0U || count > MAX_N) {
        fclose(file);
        return 0;
    }

    long long *array = (long long *)malloc(count * sizeof *array);
    if (!array) {
        fclose(file);
        return 0;
    }

    for (size_t i = 0; i < count; ++i) {
        if (fscanf(file, "%lld", &array[i]) != 1) {
            free(array);
            fclose(file);
            return 0;
        }
    }

    char extra;
    if (fscanf(file, " %c", &extra) == 1) {
        free(array);
        fclose(file);
        return 0;
    }

    fclose(file);
    *values = array;
    *n = count;
    return 1;
}

static int write_sorted_file(const char *path, const long long *values,
                             size_t n) {
    FILE *file = fopen(path, "w");
    if (!file) {
        return 0;
    }

    fprintf(file, "%zu\n", n);
    for (size_t i = 0; i < n; ++i) {
        fprintf(file, "%lld%c", values[i],
                (i + 1U) % 12U == 0U || i + 1U == n ? '\n' : ' ');
    }
    return fclose(file) == 0;
}

static void print_preview(const char *label, const long long *values,
                          size_t n) {
    const size_t limit = n < 12U ? n : 12U;
    printf("%-22s", label);
    for (size_t i = 0; i < limit; ++i) {
        printf("%s%lld", i == 0U ? "" : " ", values[i]);
    }
    if (limit < n) {
        printf(" ...");
    }
    putchar('\n');
}

int main(void) {
    size_t requested_n;
    uint64_t seed;
    char input_path[PATH_CAPACITY];
    char output_path[PATH_CAPACITY];

    printf("============================================================\n");
    printf(" DAA Lab 05 - Q4: Heap Sort of File-Stored Random Numbers\n");
    printf("============================================================\n");
    printf("Enter N (1..1000000): ");
    if (scanf("%zu", &requested_n) != 1 || requested_n == 0U ||
        requested_n > MAX_N) {
        fprintf(stderr, "Invalid N.\n");
        return 1;
    }

    printf("Enter an unsigned seed: ");
    if (scanf("%" SCNu64, &seed) != 1) {
        fprintf(stderr, "Invalid seed.\n");
        return 1;
    }

    printf("Input file name: ");
    if (scanf("%259s", input_path) != 1) {
        fprintf(stderr, "Invalid input file name.\n");
        return 1;
    }
    printf("Sorted output file name: ");
    if (scanf("%259s", output_path) != 1) {
        fprintf(stderr, "Invalid output file name.\n");
        return 1;
    }

    if (!write_random_file(input_path, requested_n, seed)) {
        fprintf(stderr, "Could not create '%s'.\n", input_path);
        return 1;
    }

    long long *values = NULL;
    size_t n = 0;
    if (!read_number_file(input_path, &values, &n) || n != requested_n) {
        fprintf(stderr, "Could not read back a valid dataset from '%s'.\n",
                input_path);
        free(values);
        return 1;
    }

    print_preview("Generated preview:", values, n);
    Fingerprint before = fingerprint(values, n);
    Metrics metrics = {0, 0, 0};
    int heap_valid = heap_sort(values, n, &metrics);
    Fingerprint after = fingerprint(values, n);
    int sorted = is_sorted(values, n);
    int same_values = same_fingerprint(before, after);

    print_preview("Sorted preview:", values, n);
    if (!write_sorted_file(output_path, values, n)) {
        fprintf(stderr, "Could not create '%s'.\n", output_path);
        free(values);
        return 1;
    }

    printf("\nGenerated dataset      : %s (%zu values)\n", input_path, n);
    printf("Sorted dataset         : %s\n", output_path);
    printf("Key comparisons        : %llu\n", metrics.key_comparisons);
    printf("Swaps                  : %llu\n", metrics.swaps);
    printf("Sift-down calls        : %llu\n", metrics.sift_calls);
    printf("Max-heap after build   : %s\n", heap_valid ? "PASS" : "FAIL");
    printf("Nondecreasing output   : %s\n", sorted ? "PASS" : "FAIL");
    printf("Multiset fingerprint   : %s\n", same_values ? "PASS" : "FAIL");
    printf("Heap-sort complexity   : Theta(n log n) time, O(1) extra space\n");
    printf("Whole file pipeline    : Theta(n log n) time, Theta(n) array space\n");

    free(values);
    return heap_valid && sorted && same_values ? 0 : 2;
}
