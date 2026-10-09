#include "../common/lab9_common.h"
/* Explicit model: consume one weight unit per time unit, without idle time.
   Integrate the given density over consumption. Skipping material is allowed.
   Greedy lambda order is optimal for fixed quantities; choices require a QP. */
typedef struct { double rho, weight, rate; int id; } Item;
static Item item[8]; static double h[8][8], best_x[8], capacity, best_value;
static int n;
static int order(const void *pa, const void *pb) {
    Item a = *(const Item *)pa, b = *(const Item *)pb; ++work;
    return a.rate != b.rate ? (a.rate > b.rate ? -1 : 1) : a.id-b.id;
}
static int solve(double a[9][10], int m, double *x) {
    for (int k = 0; k < m; ++k) {
        int pivot = k;
        for (int i = k+1; i < m; ++i)
            if (fabs(a[i][k]) > fabs(a[pivot][k])) pivot = i;
        if (fabs(a[pivot][k]) < 1e-12) return 0;
        for (int j = k; j <= m; ++j) {
            double t = a[k][j]; a[k][j] = a[pivot][j]; a[pivot][j] = t;
        }
        double divisor = a[k][k];
        for (int j = k; j <= m; ++j) a[k][j] /= divisor;
        for (int i = 0; i < m; ++i) if (i != k) {
            double factor = a[i][k];
            for (int j = k; j <= m; ++j) a[i][j] -= factor*a[k][j];
        }
    }
    for (int i = 0; i < m; ++i) x[i] = a[i][m];
    return 1;
}
static void candidate(int state, int full) {
    double x[8] = {0}, matrix[9][10] = {{0}}, solution[9], fixed = 0;
    int free_id[8], count = 0; ++work;
    for (int i = 0; i < n; ++i, state /= 3) {
        int status = state%3;
        if (status == 1) x[i] = item[i].weight;
        if (status == 2) free_id[count++] = i;
        fixed += x[i];
    }
    if (fixed > capacity+1e-8) return;
    int m = count+full;
    if (count) {
        for (int r = 0; r < count; ++r) {
            int i = free_id[r]; double rhs = item[i].rho;
            for (int j = 0; j < n; ++j) rhs -= h[i][j]*x[j];
            for (int c = 0; c < count; ++c) matrix[r][c] = h[i][free_id[c]];
            if (full) { matrix[r][count] = 1; matrix[count][r] = 1; }
            matrix[r][m] = rhs;
        }
        if (full) matrix[count][m] = capacity-fixed;
        if (!solve(matrix, m, solution)) return;
        for (int r = 0; r < count; ++r) x[free_id[r]] = solution[r];
    } else if (full && fabs(fixed-capacity) > 1e-8) return;
    double used = 0, value = 0;
    for (int i = 0; i < n; ++i) {
        if (x[i] < -1e-8 || x[i] > item[i].weight+1e-8) return;
        x[i] = fmax(0, fmin(item[i].weight, x[i])); used += x[i];
        value += item[i].rho*x[i];
        for (int j = 0; j < n; ++j) value -= 0.5*h[i][j]*x[i]*x[j];
    }
    if (used <= capacity+1e-8 && value > best_value+1e-10) {
        best_value = value; memcpy(best_x, x, sizeof(best_x));
    }
}
int main(int argc, char **argv) {
    int json = json_mode(argc, argv); n = (int)integer(1, 8);
    capacity = real(0, 10000);
    for (int i = 0; i < n; ++i) {
        double value = real(0, 10000), weight = real(0.001, 10000);
        item[i] = (Item){value/weight, weight, real(0.001, 10000), i+1};
    }
    finish(); qsort(item, (size_t)n, sizeof(*item), order);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) h[i][j] = fmin(item[i].rate, item[j].rate);
    int states = 1; for (int i = 0; i < n; ++i) states *= 3;
    for (int state = 0; state < states; ++state) {
        candidate(state, 0); candidate(state, 1);
    }
    double time = 0;
    if (json) printf("{\"value\":%.10f,\"schedule\":[", best_value);
    else puts("Model: continuous consumption, 1 weight unit per time unit.");
    for (int i = 0; i < n; ++i) {
        if (json) printf("%s{\"id\":%d,\"amount\":%.10f,\"fraction\":%.10f,"
                         "\"start\":%.10f,\"rate\":%.10f}", i ? "," : "",
                         item[i].id, best_x[i], best_x[i]/item[i].weight,
                         time, item[i].rate);
        else if (best_x[i] > 1e-8)
            printf("Item %d: amount %.6f, fraction %.6f, start %.6f\n",
                   item[i].id, best_x[i], best_x[i]/item[i].weight, time);
        time += best_x[i];
    }
    if (json) printf("],\"used\":%.10f,\"work\":%llu}\n", time, work);
    else printf("Maximum integrated value: %.6f\nUsed capacity: %.6f\nWork: %llu\n",
                best_value, time, work);
    return 0;
}
