/* DAA Lab 04 Q2 - log-log SVG for the O(n log n) pair-sum algorithm. */

#include <math.h>
#include <stdio.h>

#include "../common/svg_plot.h"

#define MAX_ROWS 32

typedef struct {
    double n, sort_cmp, search_cmp, total, reference, ratio;
    int valid;
} Row;

static double log2_safe(double value) { return log(value) / log(2.0); }

int main(void) {
    FILE *input = fopen("q2_experimental_data.dat", "r");
    if (!input) { fprintf(stderr, "Run q2_experimental_validation first.\n"); return 1; }
    Row rows[MAX_ROWS];
    int count = 0;
    char line[256];
    while (count < MAX_ROWS && fgets(line, sizeof line, input)) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %lf %lf %lf %lf %lf %d",
                   &rows[count].n, &rows[count].sort_cmp,
                   &rows[count].search_cmp, &rows[count].total,
                   &rows[count].reference, &rows[count].ratio,
                   &rows[count].valid) == 7) {
            if (!rows[count].valid) return 2;
            ++count;
        }
    }
    fclose(input);
    if (count < 2) return 1;

    double x_min = log2_safe(rows[0].n), x_max = log2_safe(rows[count - 1].n);
    double y_min = log2_safe(rows[0].reference) - 0.5;
    double y_max = log2_safe(rows[count - 1].total) + 0.5;
    FILE *out = fopen("q2_pair_sum_complexity.svg", "w");
    if (!out) return 1;
    svg_begin(out, "Cross-Set Pair Sum", 
              "Worst-case comparison growth: sort S2, then binary-search one complement per S1 value",
              "#22d3ee", "#a78bfa", "O(n log n)");
    svg_grid(out, count - 1, 5);

    fprintf(out, "<polyline fill='none' stroke='#94a3b8' stroke-width='3' "
                 "stroke-dasharray='10 8' points='");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(log2_safe(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(log2_safe(rows[i].reference), y_min, y_max, SVG_BOTTOM, SVG_TOP);
        fprintf(out, "%.2f,%.2f ", x, y);
    }
    fprintf(out, "'/>\n<polyline fill='none' stroke='url(#lineA)' stroke-width='6' "
                 "stroke-linejoin='round' filter='url(#glow)' points='");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(log2_safe(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(log2_safe(rows[i].total), y_min, y_max, SVG_BOTTOM, SVG_TOP);
        fprintf(out, "%.2f,%.2f ", x, y);
    }
    fprintf(out, "'/>\n");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(log2_safe(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(log2_safe(rows[i].total), y_min, y_max, SVG_BOTTOM, SVG_TOP);
        svg_point(out, x, y, "#22d3ee", 5.0);
        char label[24];
        snprintf(label, sizeof label, "%.0f", rows[i].n);
        svg_label(out, x, 651, label, 12, "#94a3b8", "middle");
    }
    for (int i = 0; i <= 4; ++i) {
        double power = y_min + (y_max - y_min) * i / 4.0;
        char label[32];
        snprintf(label, sizeof label, "2^%.1f", power);
        svg_label(out, 98, svg_map(power, y_min, y_max, SVG_BOTTOM, SVG_TOP) + 5,
                  label, 12, "#94a3b8", "end");
    }
    svg_legend_item(out, 160, 194, "#22d3ee", "Measured total comparisons");
    svg_legend_item(out, 470, 194, "#94a3b8", "Reference n log2(n)");
    svg_axis_titles(out, "Set size n (log2-spaced)", "Comparisons (log2 scale)");
    svg_footer(out,
               "Merge sort gives a worst-case guarantee; each of n complements costs at most logarithmic search.");
    fclose(out);
    printf("Created q2_pair_sum_complexity.svg\n");
    return 0;
}
