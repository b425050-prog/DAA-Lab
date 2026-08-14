#include <stdio.h>

int main(void) {
    FILE *fp = fopen("special_matrix_data.dat", "w");
    if (!fp) {
        perror("special_matrix_data.dat");
        return 1;
    }

    fprintf(fp, "# n scalar_mult add_sub div2 total_arithmetic n_squared n_cubed\n");
    for (unsigned long long n = 1; n <= 4096; n *= 2) {
        unsigned long long mult = n;
        unsigned long long add_sub = 3ULL * n * n - 3ULL * n;
        unsigned long long div2 = n * n - n;
        unsigned long long total = mult + add_sub + div2;
        unsigned long long n2 = n * n;
        unsigned long long n3 = n2 * n;
        fprintf(fp, "%llu %llu %llu %llu %llu %llu %llu\n",
                n, mult, add_sub, div2, total, n2, n3);
    }

    fclose(fp);
    printf("Created special_matrix_data.dat\n");
    return 0;
}
