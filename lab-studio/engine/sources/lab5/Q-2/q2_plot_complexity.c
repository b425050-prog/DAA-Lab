/* DAA Lab 05 Q2 - SVG plot of deterministic BFPRT comparison growth. */

#include <math.h>
#include <stdio.h>

#include "../common/svg_plot.h"

enum { MAX_ROWS = 32 };

typedef struct {
    double n;
    int trials;
    double average_comparisons;
    double comparisons_per_n;
    double average_partitions;
    double max_depth;
    double linear_reference;
    int valid;
} Row;

static double log2_safe(double value) {
    return log(value) / log(2.0);
}

int main(void) {
    FILE *input = fopen("q2_experimental_data.dat", "r");
    if (input == NULL) {
        fprintf(stderr, "Run q2_experimental_validation first.\n");
        return 1;
    }

    Row rows[MAX_ROWS];
    int count = 0;
    char line[256];
    while (count < MAX_ROWS && fgets(line, sizeof line, input) != NULL) {
        if (line[0] == '#') {
            continue;
        }
        if (sscanf(line, "%lf %d %lf %lf %lf %lf %lf %d",
                   &rows[count].n, &rows[count].trials,
                   &rows[count].average_comparisons,
                   &rows[count].comparisons_per_n,
                   &rows[count].average_partitions,
                   &rows[count].max_depth, &rows[count].linear_reference,
                   &rows[count].valid) == 8) {
            if (!rows[count].valid) {
                fclose(input);
                return 2;
            }
            ++count;
        }
    }
    fclose(input);
    if (count < 2) {
        fprintf(stderr, "Not enough valid experiment rows.\n");
        return 1;
    }

    double x_min = log2_safe(rows[0].n);
    double x_max = log2_safe(rows[count - 1].n);
    double y_low_value = rows[0].average_comparisons;
    double y_high_value = rows[0].average_comparisons;
    for (int i = 0; i < count; ++i) {
        if (rows[i].average_comparisons < y_low_value) {
            y_low_value = rows[i].average_comparisons;
        }
        if (rows[i].linear_reference < y_low_value) {
            y_low_value = rows[i].linear_reference;
        }
        if (rows[i].average_comparisons > y_high_value) {
            y_high_value = rows[i].average_comparisons;
        }
        if (rows[i].linear_reference > y_high_value) {
            y_high_value = rows[i].linear_reference;
        }
    }
    double y_min = log2_safe(y_low_value) - 0.35;
    double y_max = log2_safe(y_high_value) + 0.35;

    FILE *output = fopen("q2_bfprt_linear_growth.svg", "w");
    if (output == NULL) {
        return 1;
    }

    svg_begin(output, "BFPRT Selection: Measured Linear Growth",
              "Five ranks per size; deterministic data with duplicates and signed extremes",
              "#2dd4bf", "#a78bfa", "Theta(n) WORST CASE");
    svg_grid(output, count - 1, 5);

    fprintf(output,
            "<polyline fill='none' stroke='#94a3b8' stroke-width='3' "
            "stroke-dasharray='10 8' points='");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(log2_safe(rows[i].n), x_min, x_max,
                           SVG_LEFT, SVG_RIGHT);
        double y = svg_map(log2_safe(rows[i].linear_reference), y_min, y_max,
                           SVG_BOTTOM, SVG_TOP);
        fprintf(output, "%.2f,%.2f ", x, y);
    }
    fprintf(output, "'/>\n");

    fprintf(output,
            "<polyline fill='none' stroke='url(#lineA)' stroke-width='6' "
            "stroke-linejoin='round' filter='url(#glow)' points='");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(log2_safe(rows[i].n), x_min, x_max,
                           SVG_LEFT, SVG_RIGHT);
        double y = svg_map(log2_safe(rows[i].average_comparisons),
                           y_min, y_max, SVG_BOTTOM, SVG_TOP);
        fprintf(output, "%.2f,%.2f ", x, y);
    }
    fprintf(output, "'/>\n");

    for (int i = 0; i < count; ++i) {
        double x = svg_map(log2_safe(rows[i].n), x_min, x_max,
                           SVG_LEFT, SVG_RIGHT);
        double y = svg_map(log2_safe(rows[i].average_comparisons),
                           y_min, y_max, SVG_BOTTOM, SVG_TOP);
        svg_point(output, x, y, "#2dd4bf", 5.0);
        char label[24];
        snprintf(label, sizeof label, "%.0f", rows[i].n);
        svg_label(output, x, 651.0, label, 12, "#94a3b8", "middle");
    }

    for (int i = 0; i <= 4; ++i) {
        double power = y_min + (y_max - y_min) * (double)i / 4.0;
        char label[32];
        snprintf(label, sizeof label, "2^%.1f", power);
        svg_label(output, 98.0,
                  svg_map(power, y_min, y_max, SVG_BOTTOM, SVG_TOP) + 5.0,
                  label, 12, "#94a3b8", "end");
    }

    svg_legend_item(output, 160.0, 194.0, "#2dd4bf",
                    "Measured average comparisons");
    svg_legend_item(output, 515.0, 194.0, "#94a3b8",
                    "Reference 10n");
    svg_axis_titles(output, "Input size n (powers of two)",
                    "Comparisons (log2 scale)");
    svg_footer(output,
               "A near-constant comparisons/n ratio is the experimental signature of linear growth.");
    fclose(output);
    printf("Created q2_bfprt_linear_growth.svg\n");
    return 0;
}
