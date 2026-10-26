/* DAA Lab 04 Q1 - SVG plot of measured 2n work. */

#include <stdio.h>

#include "../common/svg_plot.h"

#define MAX_ROWS 32

typedef struct {
    double n;
    double reads;
    double writes;
    double total;
    double reference;
    int valid;
} Row;

int main(void) {
    FILE *input = fopen("q1_experimental_data.dat", "r");
    if (!input) {
        fprintf(stderr, "Run q1_experimental_validation first.\n");
        return 1;
    }
    Row rows[MAX_ROWS];
    int count = 0;
    char line[256];
    while (count < MAX_ROWS && fgets(line, sizeof line, input)) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %lf %lf %lf %lf %d",
                   &rows[count].n, &rows[count].reads, &rows[count].writes,
                   &rows[count].total, &rows[count].reference,
                   &rows[count].valid) == 6) {
            if (!rows[count].valid) {
                fclose(input);
                fprintf(stderr, "Dataset contains a failed validation row.\n");
                return 2;
            }
            ++count;
        }
    }
    fclose(input);
    if (count < 2) return 1;

    FILE *out = fopen("q1_stable_colour_sort.svg", "w");
    if (!out) return 1;
    svg_begin(out, "Stable Three-Colour Grouping", 
              "Measured classifications and placements on already number-sorted pairs",
              "#fb7185", "#facc15", "Theta(n) | exact 2n");
    svg_grid(out, 8, 5);

    double max_n = rows[count - 1].n;
    double max_y = rows[count - 1].reference * 1.08;
    fprintf(out, "<polyline fill='none' stroke='#94a3b8' stroke-width='3' "
                 "stroke-dasharray='10 9' points='");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(rows[i].n, 0.0, max_n, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(rows[i].reference, 0.0, max_y, SVG_BOTTOM, SVG_TOP);
        fprintf(out, "%.2f,%.2f ", x, y);
    }
    fprintf(out, "'/>\n<polyline fill='none' stroke='url(#lineA)' stroke-width='6' "
                 "stroke-linecap='round' stroke-linejoin='round' filter='url(#glow)' points='");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(rows[i].n, 0.0, max_n, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(rows[i].total, 0.0, max_y, SVG_BOTTOM, SVG_TOP);
        fprintf(out, "%.2f,%.2f ", x, y);
    }
    fprintf(out, "'/>\n");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(rows[i].n, 0.0, max_n, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(rows[i].total, 0.0, max_y, SVG_BOTTOM, SVG_TOP);
        svg_point(out, x, y, "#fb7185", 4.5);
    }

    for (int i = 0; i <= 4; ++i) {
        char label[32];
        double value = max_n * i / 4.0;
        snprintf(label, sizeof label, "%.0f", value);
        svg_label(out, svg_map(value, 0.0, max_n, SVG_LEFT, SVG_RIGHT),
                  651, label, 13, "#94a3b8", "middle");
        snprintf(label, sizeof label, "%.0f", max_y * i / 4.0);
        svg_label(out, 98,
                  svg_map(max_y * i / 4.0, 0.0, max_y, SVG_BOTTOM, SVG_TOP) + 5,
                  label, 13, "#94a3b8", "end");
    }
    svg_legend_item(out, 160, 194, "#fb7185", "Measured reads + writes");
    svg_legend_item(out, 430, 194, "#94a3b8", "Reference 2n (coincident)");
    svg_axis_titles(out, "Input pairs n", "Dominant operations");
    svg_footer(out,
               "One counting pass + one stable placement pass; order inside each colour is preserved.");
    fclose(out);
    printf("Created q1_stable_colour_sort.svg\n");
    return 0;
}
