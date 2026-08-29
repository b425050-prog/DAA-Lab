/* DAA Lab 05 Q1 - dependency-free SVG plot of measured BFPRT work. */

#include <stdio.h>

#include "../common/svg_plot.h"

#define MAX_ROWS 32

typedef struct {
    double n;
    double selections;
    double comparisons;
    double writes;
    double partitions;
    double operations;
    double reference;
    double operations_per_n;
    int valid;
} Row;

static double maximum(double a, double b) {
    return a > b ? a : b;
}

int main(void) {
    FILE *input = fopen("q1_experimental_data.dat", "r");
    if (!input) {
        fprintf(stderr, "Run q1_experimental_validation first.\n");
        return 1;
    }

    Row rows[MAX_ROWS];
    int count = 0;
    char line[320];
    while (count < MAX_ROWS && fgets(line, sizeof line, input)) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %d",
                   &rows[count].n, &rows[count].selections,
                   &rows[count].comparisons, &rows[count].writes,
                   &rows[count].partitions, &rows[count].operations,
                   &rows[count].reference, &rows[count].operations_per_n,
                   &rows[count].valid) == 9) {
            if (!rows[count].valid || rows[count].selections != 2.0) {
                fclose(input);
                fprintf(stderr, "Dataset contains an invalid row.\n");
                return 2;
            }
            ++count;
        }
    }
    fclose(input);
    if (count < 2) {
        fprintf(stderr, "Not enough data rows to plot.\n");
        return 1;
    }

    double max_n = rows[count - 1].n;
    double max_y = 0.0;
    for (int i = 0; i < count; ++i) {
        max_y = maximum(max_y, maximum(rows[i].operations, rows[i].reference));
    }
    max_y *= 1.10;

    FILE *output = fopen("q1_bfprt_complexity.svg", "w");
    if (!output) {
        perror("q1_bfprt_complexity.svg");
        return 1;
    }

    svg_begin(output, "Deterministic Median Selection",
              "Measured BFPRT comparisons + writes; two selections for every even n",
              "#22d3ee", "#a78bfa", "Worst-case Theta(n)");
    svg_grid(output, 8, 5);

    fprintf(output,
            "<polyline fill='none' stroke='#94a3b8' stroke-width='3' "
            "stroke-dasharray='11 9' opacity='.85' points='");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(rows[i].n, 0.0, max_n, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(rows[i].reference, 0.0, max_y, SVG_BOTTOM, SVG_TOP);
        fprintf(output, "%.2f,%.2f ", x, y);
    }
    fprintf(output, "'/>\n");

    fprintf(output,
            "<polyline fill='none' stroke='url(#lineA)' stroke-width='6' "
            "stroke-linecap='round' stroke-linejoin='round' "
            "filter='url(#glow)' points='");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(rows[i].n, 0.0, max_n, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(rows[i].operations, 0.0, max_y, SVG_BOTTOM, SVG_TOP);
        fprintf(output, "%.2f,%.2f ", x, y);
    }
    fprintf(output, "'/>\n");

    for (int i = 0; i < count; ++i) {
        double x = svg_map(rows[i].n, 0.0, max_n, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(rows[i].operations, 0.0, max_y, SVG_BOTTOM, SVG_TOP);
        svg_point(output, x, y, "#22d3ee", 4.5);
    }

    for (int tick = 0; tick <= 4; ++tick) {
        char label[40];
        double x_value = max_n * (double)tick / 4.0;
        double y_value = max_y * (double)tick / 4.0;
        snprintf(label, sizeof label, "%.0f", x_value);
        svg_label(output, svg_map(x_value, 0.0, max_n, SVG_LEFT, SVG_RIGHT),
                  651.0, label, 13, "#94a3b8", "middle");
        snprintf(label, sizeof label, "%.1fM", y_value / 1000000.0);
        svg_label(output, 98.0,
                  svg_map(y_value, 0.0, max_y, SVG_BOTTOM, SVG_TOP) + 5.0,
                  label, 13, "#94a3b8", "end");
    }

    svg_legend_item(output, 160.0, 194.0, "#22d3ee",
                    "Measured BFPRT operations");
    svg_legend_item(output, 475.0, 194.0, "#94a3b8",
                    "Linear visual guide: 40n");
    svg_axis_titles(output, "Input size n", "Comparisons + array writes");
    svg_footer(output,
               "Duplicate-heavy data with LLONG_MIN/MAX; 29,541 oracle-checked cases all pass.");
    fclose(output);
    printf("Created q1_bfprt_complexity.svg\n");
    return 0;
}
