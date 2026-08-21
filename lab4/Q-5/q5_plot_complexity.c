/* DAA Lab 04 Q5 - SVG plot for sorting plus linear interval merge. */

#include <math.h>
#include <stdio.h>
#include "../common/svg_plot.h"

#define MAX_ROWS 32
typedef struct { double n, sort_cmp, scan, total, reference, ratio, merged; int valid; } Row;
static double lg(double x) { return log(x) / log(2.0); }

int main(void) {
    FILE *in = fopen("q5_experimental_data.dat", "r");
    if (!in) { fputs("Run q5_experimental_validation first.\n", stderr); return 1; }
    Row a[MAX_ROWS]; int count = 0; char line[280];
    while (count < MAX_ROWS && fgets(line, sizeof line, in)) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %d",
                   &a[count].n, &a[count].sort_cmp, &a[count].scan,
                   &a[count].total, &a[count].reference, &a[count].ratio,
                   &a[count].merged, &a[count].valid) == 8) {
            if (!a[count].valid) return 2;
            ++count;
        }
    }
    fclose(in); if (count < 2) return 1;
    double xmin = lg(a[0].n), xmax = lg(a[count - 1].n);
    double ymin = lg(a[0].reference) - .5, ymax = lg(a[count - 1].total) + .6;
    FILE *out = fopen("q5_merge_intervals_complexity.svg", "w"); if (!out) return 1;
    svg_begin(out, "Merge Overlapping Intervals",
              "After endpoint-ordering, every interval is consumed exactly once by the union scan",
              "#f59e0b", "#fb7185", "O(n log n)");
    svg_grid(out, count - 1, 5);
    fprintf(out, "<polyline fill='none' stroke='#94a3b8' stroke-width='3' "
                 "stroke-dasharray='10 8' points='");
    for (int i = 0; i < count; ++i) fprintf(out, "%.2f,%.2f ",
        svg_map(lg(a[i].n), xmin, xmax, SVG_LEFT, SVG_RIGHT),
        svg_map(lg(a[i].reference), ymin, ymax, SVG_BOTTOM, SVG_TOP));
    fprintf(out, "'/>\n<polyline fill='none' stroke='url(#lineA)' stroke-width='6' "
                 "stroke-linejoin='round' filter='url(#glow)' points='");
    for (int i = 0; i < count; ++i) fprintf(out, "%.2f,%.2f ",
        svg_map(lg(a[i].n), xmin, xmax, SVG_LEFT, SVG_RIGHT),
        svg_map(lg(a[i].total), ymin, ymax, SVG_BOTTOM, SVG_TOP));
    fprintf(out, "'/>\n");
    for (int i = 0; i < count; ++i) {
        double x = svg_map(lg(a[i].n), xmin, xmax, SVG_LEFT, SVG_RIGHT);
        double y = svg_map(lg(a[i].total), ymin, ymax, SVG_BOTTOM, SVG_TOP);
        svg_point(out, x, y, "#f59e0b", 5);
        char label[24]; snprintf(label, sizeof label, "%.0f", a[i].n);
        svg_label(out, x, 651, label, 12, "#94a3b8", "middle");
    }
    for (int i = 0; i <= 4; ++i) { double p = ymin + (ymax-ymin)*i/4.0; char s[24];
        snprintf(s, sizeof s, "2^%.1f", p);
        svg_label(out, 98, svg_map(p, ymin, ymax, SVG_BOTTOM, SVG_TOP)+5,
                  s, 12, "#94a3b8", "end"); }
    svg_legend_item(out, 160, 194, "#f59e0b", "Measured sort + union scan");
    svg_legend_item(out, 490, 194, "#94a3b8", "Reference n log2(n)");
    svg_axis_titles(out, "Input intervals n (log2-spaced)", "Operations (log2 scale)");
    svg_footer(out,
               "Closed intervals merge when the next left endpoint is not beyond the current right endpoint.");
    fclose(out); puts("Created q5_merge_intervals_complexity.svg"); return 0;
}
