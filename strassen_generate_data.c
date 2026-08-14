#include <stdio.h>
#include <math.h>

static unsigned long long pow_u64(unsigned long long base, int exp) {
    unsigned long long result = 1;
    while (exp-- > 0) result *= base;
    return result;
}

int main(void) {
    FILE *fp = fopen("strassen_data.dat", "w");
    if (!fp) {
        perror("strassen_data.dat");
        return 1;
    }

    fprintf(fp, "# n strassen_scalar_mult classical_scalar_mult n_pow_log2_7\n");
    for (int k = 0; k <= 10; ++k) {
        unsigned long long n = 1ULL << k;
        unsigned long long strassen_mult = pow_u64(7ULL, k);
        unsigned long long classic_mult = n * n * n;
        double theoretical = pow((double)n, log(7.0) / log(2.0));
        fprintf(fp, "%llu %llu %llu %.3f\n", n, strassen_mult, classic_mult, theoretical);
    }

    fclose(fp);
    printf("Created strassen_data.dat\n");
    return 0;
}
