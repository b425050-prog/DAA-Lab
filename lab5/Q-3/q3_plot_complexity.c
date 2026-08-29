/* DAA Lab 05 Q3 - log-log SVG of measured randomized Quick Sort work. */

#include <math.h>
#include <stdio.h>

#include "../common/svg_plot.h"

#define MAX_ROWS 32

typedef struct {
    double n;
    double comparisons;
    double swaps;
    double partitions;
    double reference;
    double ratio;
    size_t max_stack;
    int valid;
} Row;

static double log2_safe(double value) { return log(value) / log(2.0); }

static void write_series(FILE *out, const Row *rows, int count, int field,
                         double x_min, double x_max, double y_min,
                         double y_max, const char *stroke, double width,
                         const char *dash) {
    fprintf(out,
            "<polyline fill='none' stroke='%s' stroke-width='%.1f' %s "
            "stroke-linecap='round' stroke-linejoin='round' points='",
            stroke, width, dash);
    for (int i = 0; i < count; ++i) {
        double value = field == 0 ? rows[i].comparisons
                       : field == 1 ? rows[i].swaps
                                    : rows[i].reference;
        double x = svg_map(log2_safe(rows[i].n), x_min, x_max, SVG_LEFT,
                           SVG_RIGHT);
        double y = svg_map(log2_safe(value), y_min, y_max, SVG_BOTTOM, SVG_TOP);
        fprintf(out, "%.2f,%.2f ", x, y);
    }
    fprintf(out, "'/>\n");
}

int main(void) {
    FILE *input = fopen("q3_experimental_data.dat", "r");
    if (!input) {
        fprintf(stderr, "Run q3_experimental_validation first.\n");
        return 1;
    }
    Row rows[MAX_ROWS];
    int count = 0;
    char line[256];
    while (count < MAX_ROWS && fgets(line, sizeof line, input)) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %lf %lf %lf %lf %lf %zu %d", &rows[count].n,
                   &rows[count].comparisons, &rows[count].swaps,
                   &rows[count].partitions, &rows[count].reference,
                   &rows[count].ratio, &rows[count].max_stack,
                   &rows[count].valid) == 8) {
            if (!rows[count].valid || rows[count].comparisons <= 0.0 ||
                rows[count].swaps <= 0.0) {
                fclose(input);
                fprintf(stderr, "Dataset contains an invalid row.\n");
                return 2;
            }
            ++count;
        }
    }
    fclose(input);
    if (count < 2) return 1;

    double x_min = log2_safe(rows[0].n);
    double x_max = log2_safe(rows[count - 1].n);
    double y_min = log2_safe(rows[0].swaps) - 0.5;
    double largest = rows[count - 1].comparisons > rows[count - 1].reference
                         ? rows[count - 1].comparisons
                         : rows[count - 1].reference;
    double y_max = log2_safe(largest) + 0.5;

    FILE *out = fopen("q3_quicksort_complexity.svg", "w");
    if (!out) return 1;
    svg_begin(out, "Seeded Three-Way Quick Sort",
              "Five-trial deterministic mean on generated file data; both axes use log2 scale",
              "#22d3ee", "#a78bfa", "expected Θ(n log n)");
    svg_grid(out, count - 1, 5);
    write_series(out, rows, count, 2, x_min, x_max, y_min, y_max,
                 "#94a3b8", 3.0, "stroke-dasharray='10 8'");
    write_series(out, rows, count, 1, x_min, x_max, y_min, y_max,
                 "#fb7185", 4.0, "");
    write_series(out, rows, count, 0, x_min, x_max, y_min, y_max,
                 "url(#lineA)", 6.0, "filter='url(#glow)'");

    for (int i = 0; i < count; ++i) {
        double x = svg_map(log2_safe(rows[i].n), x_min, x_max, SVG_LEFT,
                           SVG_RIGHT);
        double y = svg_map(log2_safe(rows[i].comparisons), y_min, y_max,
                           SVG_BOTTOM, SVG_TOP);
        svg_point(out, x, y, "#22d3ee", 4.6);
        char label[24];
        if (rows[i].n < 1000.0) {
            snprintf(label, sizeof label, "%.0f", rows[i].n);
        } else {
            snprintf(label, sizeof label, "%.0fk", rows[i].n / 1024.0);
        }
        svg_label(out, x, 651, label, 11, "#94a3b8", "middle");
    }
    for (int i = 0; i <= 4; ++i) {
        double power = y_min + (y_max - y_min) * i / 4.0;
        char label[32];
        snprintf(label, sizeof label, "2^%.1f", power);
        svg_label(out, 98,
                  svg_map(power, y_min, y_max, SVG_BOTTOM, SVG_TOP) + 5,
                  label, 12, "#94a3b8", "end");
    }
    svg_legend_item(out, 148, 194, "#22d3ee", "Pivot comparisons");
    svg_legend_item(out, 410, 194, "#fb7185", "Swaps");
    svg_legend_item(out, 590, 194, "#94a3b8", "n log2(n) reference");
    svg_axis_titles(out, "Number of values n (log2-spaced)",
                    "Measured operations (log2 scale)");
    svg_footer(out,
               "Random pivots give expected Θ(n log n); 3-way equal blocks are finished in one partition.");
    if (fclose(out) != 0) return 1;
    printf("Created q3_quicksort_complexity.svg\n");
    return 0;
}
