/* DAA Lab 04 Q3 - multi-series log-log SVG for generalized k-sum. */

#include <math.h>
#include <stdio.h>

#include "../common/svg_plot.h"

#define MAX_ROWS 64

typedef struct {
    double n, sort_cmp, prefixes, binary_cmp, total, reference, ratio;
    int k, valid;
} Row;

static double lg(double x) { return log(x) / log(2.0); }

int main(void) {
    FILE *input = fopen("q3_experimental_data.dat", "r");
    if (!input) { fprintf(stderr, "Run q3_experimental_validation first.\n"); return 1; }
    Row rows[MAX_ROWS];
    int count = 0;
    char line[300];
    while (count < MAX_ROWS && fgets(line, sizeof line, input)) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %d %lf %lf %lf %lf %lf %lf %d",
                   &rows[count].n, &rows[count].k, &rows[count].sort_cmp,
                   &rows[count].prefixes, &rows[count].binary_cmp,
                   &rows[count].total, &rows[count].reference,
                   &rows[count].ratio, &rows[count].valid) == 9) {
            if (!rows[count].valid) return 2;
            ++count;
        }
    }
    fclose(input);
    if (count < 3) return 1;

    double x_min = lg(16.0), x_max = lg(128.0);
    double y_min = lg(rows[0].total) - 0.6, y_max = y_min;
    for (int i = 0; i < count; ++i) {
        double y = lg(rows[i].total);
        if (y > y_max) y_max = y;
    }
    y_max += 0.7;
    const char *colours[] = {"#22d3ee", "#a78bfa", "#fb7185"};

    FILE *out = fopen("q3_k_sum_growth.svg", "w");
    if (!out) return 1;
    svg_begin(out, "Generalized k-Sum Search Space",
              "Every (k-1)-prefix triggers one logarithmic suffix search",
              "#22d3ee", "#fb7185", "O(n^(k-1) log n)");
    svg_grid(out, 6, 6);

    for (int k = 2; k <= 4; ++k) {
        const char *colour = colours[k - 2];
        fprintf(out, "<polyline fill='none' stroke='%s' stroke-width='5.5' "
                     "stroke-linejoin='round' filter='url(#glow)' points='", colour);
        for (int i = 0; i < count; ++i) {
            if (rows[i].k != k) continue;
            double x = svg_map(lg(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT);
            double y = svg_map(lg(rows[i].total), y_min, y_max, SVG_BOTTOM, SVG_TOP);
            fprintf(out, "%.2f,%.2f ", x, y);
        }
        fprintf(out, "'/>\n");
        for (int i = 0; i < count; ++i) {
            if (rows[i].k != k) continue;
            svg_point(out,
                      svg_map(lg(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT),
                      svg_map(lg(rows[i].total), y_min, y_max, SVG_BOTTOM, SVG_TOP),
                      colour, 4.5);
        }
    }

    static const int n_labels[] = {16, 24, 32, 48, 64, 96, 128};
    for (int i = 0; i < 7; ++i) {
        char label[16];
        snprintf(label, sizeof label, "%d", n_labels[i]);
        svg_label(out, svg_map(lg((double)n_labels[i]), x_min, x_max,
                               SVG_LEFT, SVG_RIGHT),
                  651, label, 12, "#94a3b8", "middle");
    }
    for (int i = 0; i <= 5; ++i) {
        double power = y_min + (y_max - y_min) * i / 5.0;
        char label[24];
        snprintf(label, sizeof label, "2^%.1f", power);
        svg_label(out, 98, svg_map(power, y_min, y_max, SVG_BOTTOM, SVG_TOP) + 5,
                  label, 12, "#94a3b8", "end");
    }
    svg_legend_item(out, 155, 194, colours[0], "k = 2");
    svg_legend_item(out, 330, 194, colours[1], "k = 3");
    svg_legend_item(out, 505, 194, colours[2], "k = 4");
    svg_label(out, 790, 199, "steeper k => one extra factor of n",
              14, "#cbd5e1", "start");
    svg_axis_titles(out, "Set size n (log2 scale)", "Measured comparisons (log2 scale)");
    svg_footer(out,
               "Indices increase through the recursion, so no element can be reused as the final complement.");
    fclose(out);
    printf("Created q3_k_sum_growth.svg\n");
    return 0;
}
