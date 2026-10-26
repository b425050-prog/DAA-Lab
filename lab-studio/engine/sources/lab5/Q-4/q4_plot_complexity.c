/* DAA Lab 05 Q4 - SVG plot for measured heap-sort comparisons. */

#include <math.h>
#include <stdio.h>
#include "../common/svg_plot.h"

#define MAX_ROWS 32

typedef struct {
    double n;
    double build_comparisons;
    double extraction_comparisons;
    double total_comparisons;
    double swaps;
    double reference;
    double ratio;
    int valid;
} Row;

static double log_two(double value) {
    return log(value) / log(2.0);
}

int main(void) {
    FILE *input = fopen("q4_experimental_data.dat", "r");
    if (!input) {
        fputs("Run q4_experimental_validation first.\n", stderr);
        return 1;
    }

    Row rows[MAX_ROWS];
    int count = 0;
    char line[320];
    while (count < MAX_ROWS && fgets(line, sizeof line, input)) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %d",
                   &rows[count].n, &rows[count].build_comparisons,
                   &rows[count].extraction_comparisons,
                   &rows[count].total_comparisons, &rows[count].swaps,
                   &rows[count].reference, &rows[count].ratio,
                   &rows[count].valid) == 8) {
            if (!rows[count].valid) {
                fclose(input);
                return 2;
            }
            ++count;
        }
    }
    fclose(input);
    if (count < 2) return 1;

    double x_min = log_two(rows[0].n);
    double x_max = log_two(rows[count - 1].n);
    double y_min = log_two(rows[0].reference) - 0.6;
    double y_max = log_two(rows[count - 1].total_comparisons) + 0.5;
    FILE *output = fopen("q4_heap_sort_complexity.svg", "w");
    if (!output) return 1;

    svg_begin(output, "Heap Sort of File-Stored Random Values",
              "Linear heap construction followed by n-1 logarithmic root extractions",
              "#fb7185", "#fbbf24", "Theta(n log n)");
    svg_grid(output, count - 1, 5);

    fprintf(output, "<polyline fill='none' stroke='#94a3b8' stroke-width='3' "
                    "stroke-dasharray='10 8' points='");
    for (int i = 0; i < count; ++i) {
        fprintf(output, "%.2f,%.2f ",
                svg_map(log_two(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT),
                svg_map(log_two(rows[i].reference), y_min, y_max,
                        SVG_BOTTOM, SVG_TOP));
    }
    fprintf(output, "'/>\n<polyline fill='none' stroke='url(#lineA)' "
                    "stroke-width='6' stroke-linejoin='round' "
                    "filter='url(#glow)' points='");
    for (int i = 0; i < count; ++i) {
        fprintf(output, "%.2f,%.2f ",
                svg_map(log_two(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT),
                svg_map(log_two(rows[i].total_comparisons), y_min, y_max,
                        SVG_BOTTOM, SVG_TOP));
    }
    fprintf(output, "'/>\n");

    for (int i = 0; i < count; ++i) {
        double x = svg_map(log_two(rows[i].n), x_min, x_max,
                           SVG_LEFT, SVG_RIGHT);
        double y = svg_map(log_two(rows[i].total_comparisons), y_min, y_max,
                           SVG_BOTTOM, SVG_TOP);
        svg_point(output, x, y, "#fb7185", 5.0);
        char label[24];
        snprintf(label, sizeof label, "%.0f", rows[i].n);
        svg_label(output, x, 651.0, label, 12, "#94a3b8", "middle");
    }
    for (int i = 0; i <= 4; ++i) {
        double power = y_min + (y_max - y_min) * (double)i / 4.0;
        char label[24];
        snprintf(label, sizeof label, "2^%.1f", power);
        svg_label(output, 98.0,
                  svg_map(power, y_min, y_max, SVG_BOTTOM, SVG_TOP) + 5.0,
                  label, 12, "#94a3b8", "end");
    }

    svg_legend_item(output, 160.0, 194.0, "#fb7185",
                    "Measured key comparisons");
    svg_legend_item(output, 475.0, 194.0, "#94a3b8",
                    "Reference n log2 n");
    svg_axis_titles(output, "Number of file values n (log2-spaced)",
                    "Key comparisons (log2 scale)");
    svg_footer(output,
               "Floyd heap construction is O(n); repeated root extraction makes the total Theta(n log n).");
    fclose(output);
    puts("Created q4_heap_sort_complexity.svg");
    return 0;
}
