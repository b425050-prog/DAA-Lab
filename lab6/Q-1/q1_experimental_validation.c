#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "q1_algorithms.h"

static uint64_t next_u64(uint64_t *s) {
    *s ^= *s << 13U; *s ^= *s >> 7U; *s ^= *s << 17U; return *s;
}

static int valid_partition(const long long *a, size_t n, size_t split,
                           long long pivot) {
    for (size_t i = 0U; i < split; ++i) if (a[i] < pivot) return 0;
    for (size_t i = split; i < n; ++i) if (a[i] >= pivot) return 0;
    return 1;
}

int main(void) {
    FILE *data = fopen("q1_experimental_data.dat", "w");
    if (!data) return 1;
    fprintf(data, "# n measured_operations n_log2_n_reference valid\n");
    uint64_t state = UINT64_C(0xDAA60001);
    int all_valid = 1;
    for (size_t n = 64U; n <= 16384U; n *= 2U) {
        long long *a = malloc(n * sizeof *a), *sorted = malloc(n * sizeof *a);
        long long *unique = malloc(n * sizeof *a), *work = malloc(n * sizeof *a);
        if (!a || !sorted || !unique || !work) return 1;
        for (size_t i = 0U; i < n; ++i) a[i] = (long long)(next_u64(&state) % 257U) - 128LL;
        Q1Summary s; size_t unique_count = 0U;
        int valid = q1_analyse(a, n, &s, sorted, unique, &unique_count);
        for (size_t i = 1U; valid && i < n; ++i) if (sorted[i - 1U] > sorted[i]) valid = 0;
        q1_reverse(a, n, work);
        for (size_t i = 0U; valid && i < n; ++i) if (work[i] != a[n - 1U - i]) valid = 0;
        size_t pivot_index = n / 3U;
        size_t split = q1_partition_ge_first(a, n, pivot_index, work);
        valid = valid && valid_partition(work, n, split, a[pivot_index]);
        double reference = 4.0 * (double)n * (log((double)n) / log(2.0));
        fprintf(data, "%zu %llu %.3f %d\n", n, s.operations, reference, valid);
        printf("n=%-6zu work=%-10llu validation=%s\n", n, s.operations, valid ? "PASS" : "FAIL");
        all_valid = all_valid && valid;
        free(a); free(sorted); free(unique); free(work);
    }
    fclose(data);
    printf("Array invariants and every partition: %s\n", all_valid ? "PASS" : "FAIL");
    return all_valid ? 0 : 2;
}
