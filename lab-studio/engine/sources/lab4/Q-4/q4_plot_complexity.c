/* DAA Lab 04 Q4 - SVG plot for the party event sweep. */

#include <math.h>
#include <stdio.h>

#include "../common/svg_plot.h"

#define MAX_ROWS 32
typedef struct {
    double n, events, sort_cmp, sweep, total, reference, ratio, peak;
    int valid;
} Row;
static double lg(double x) { return log(x) / log(2.0); }

int main(void) {
    FILE *input = fopen("q4_experimental_data.dat", "r");
    if (!input) { fprintf(stderr, "Run q4_experimental_validation first.\n"); return 1; }
    Row rows[MAX_ROWS]; int count = 0; char line[300];
    while (count < MAX_ROWS && fgets(line, sizeof line, input)) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %d",
                   &rows[count].n, &rows[count].events, &rows[count].sort_cmp,
                   &rows[count].sweep, &rows[count].total, &rows[count].reference,
                   &rows[count].ratio, &rows[count].peak, &rows[count].valid) == 9) {
            if (!rows[count].valid) return 2;
            ++count;
        }
    }
    fclose(input); if (count < 2) return 1;
    double x_min = lg(rows[0].n), x_max = lg(rows[count - 1].n);
    double y_min = lg(rows[0].reference) - 0.5;
    double y_max = lg(rows[count - 1].total) + 0.6;
    FILE *out = fopen("q4_party_sweep_complexity.svg", "w"); if (!out) return 1;
    svg_begin(out, "Peak Party Attendance Sweep",
              "Two events per person; sorting dominates one linear chronological scan",
              "#34d399", "#22d3ee", "O(n log n)");
    svg_grid(out, count - 1, 5);
    fprintf(out, "<polyline fill='none' stroke='#94a3b8' stroke-width='3' "
                 "stroke-dasharray='10 8' points='");
    for (int i = 0; i < count; ++i) fprintf(out, "%.2f,%.2f ",
        svg_map(lg(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT),
        svg_map(lg(rows[i].reference), y_min, y_max, SVG_BOTTOM, SVG_TOP));
    fprintf(out, "'/>\n<polyline fill='none' stroke='url(#lineA)' stroke-width='6' "
                 "stroke-linejoin='round' filter='url(#glow)' points='");
    for (int i = 0; i < count; ++i) fprintf(out, "%.2f,%.2f ",
        svg_map(lg(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT),
        svg_map(lg(rows[i].total), y_min, y_max, SVG_BOTTOM, SVG_TOP));
    fprintf(out, "'/>\n");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(lg(rows[i].n), x_min, x_max, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(lg(rows[i].total), y_min, y_max, SVG_BOTTOM, SVG_TOP);
        svg_point(out, x, y, "#34d399", 5.0);
        char label[24]; snprintf(label, sizeof label, "%.0f", rows[i].n);
        svg_label(out, x, 651, label, 12, "#94a3b8", "middle");
    }
    for (int i = 0; i <= 4; ++i) {
        double p = y_min + (y_max - y_min) * i / 4.0; char label[24];
        snprintf(label, sizeof label, "2^%.1f", p);
        svg_label(out, 98, svg_map(p, y_min, y_max, SVG_BOTTOM, SVG_TOP) + 5,
                  label, 12, "#94a3b8", "end");
    }
    svg_legend_item(out, 160, 194, "#34d399", "Measured sort + sweep");
    svg_legend_item(out, 455, 194, "#94a3b8", "Reference 2n log2(2n)");
    svg_axis_titles(out, "Number of persons n (log2-spaced)", "Operations (log2 scale)");
    svg_footer(out,
               "With distinct times, every event has a single unambiguous +1 or -1 attendance transition.");
    fclose(out); puts("Created q4_party_sweep_complexity.svg"); return 0;
}
