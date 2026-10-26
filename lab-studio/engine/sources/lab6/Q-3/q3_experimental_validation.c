#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "q3_fft.h"

static uint64_t next_u64(uint64_t *s) { *s ^= *s << 13U; *s ^= *s >> 7U; *s ^= *s << 17U; return *s; }

int main(void) {
    FILE *data = fopen("q3_experimental_data.dat", "w"); if (!data) return 1;
    fprintf(data, "# padded_scale measured_butterflies n_log2_n_reference valid\n");
    uint64_t state = UINT64_C(0xDAA60003); int all_valid = 1;
    for (size_t n = 32U; n <= 16384U; n *= 2U) {
        size_t m = n/2U, length = m+n-1U;
        double *a = malloc(m*sizeof *a), *b = malloc(n*sizeof *b), *fast = malloc(length*sizeof *fast), *slow = calloc(length,sizeof *slow);
        if (!a || !b || !fast || !slow) return 1;
        for (size_t i = 0U; i < m; ++i) a[i] = (double)((int)(next_u64(&state)%17U)-8);
        for (size_t i = 0U; i < n; ++i) b[i] = (double)((int)(next_u64(&state)%17U)-8);
        Q3Metrics metrics = {0U}; int valid = q3_convolve(a,m,b,n,fast,&metrics);
        for (size_t i = 0U; i < m; ++i) for (size_t j = 0U; j < n; ++j) slow[i+j] += a[i]*b[j];
        for (size_t i = 0U; valid && i < length; ++i) if (fabs(fast[i]-slow[i]) > 1e-6*(1.0+fabs(slow[i]))) valid = 0;
        size_t padded = q3_next_power_of_two(length);
        double ref = 1.5*(double)padded*(log((double)padded)/log(2.0));
        fprintf(data, "%zu %llu %.3f %d\n", padded, metrics.butterflies, ref, valid);
        printf("m=%-6zu n=%-6zu padded=%-6zu oracle=%s\n", m,n,padded,valid?"PASS":"FAIL");
        all_valid = all_valid && valid; free(a); free(b); free(fast); free(slow);
    }
    fclose(data); printf("FFT vs direct convolution: %s\n", all_valid?"PASS":"FAIL"); return all_valid?0:2;
}
