/* DAA Lab 03 Q1 - Reads q1_experimental_data.dat and writes an SVG plot. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_POINTS 64

typedef struct {
    double n, bp, tp, bc, tc;
} Row;

static double map_x(double n, double nmin, double nmax) {
    double a = log(n) / log(2.0), lo = log(nmin) / log(2.0), hi = log(nmax) / log(2.0);
    return 100.0 + (a - lo) / (hi - lo) * 1000.0;
}

static double map_y(double y, double ymax) {
    return 610.0 - y / ymax * 470.0;
}

int main(void) {
    FILE *in = fopen("q1_experimental_data.dat", "r");
    if (!in) {
        puts("Run q1_experimental_comparison first.");
        return 1;
    }

    Row rows[MAX_POINTS];
    int count = 0;
    char line[256];
    while (fgets(line, sizeof line, in) && count < MAX_POINTS) {
        if (line[0] == '#') continue;
        Row r;
        if (sscanf(line, "%lf %lf %lf %lf %lf", &r.n, &r.bp, &r.tp, &r.bc, &r.tc) == 5)
            rows[count++] = r;
    }
    fclose(in);
    if (count < 2) return 1;

    double ymax = 0.0;
    for (int i = 0; i < count; ++i)
        if (rows[i].tp > ymax) ymax = rows[i].tp;
    ymax *= 1.15;

    FILE *out = fopen("q1_binary_vs_ternary.svg", "w");
    if (!out) return 1;

    fprintf(out, "<svg xmlns='http://www.w3.org/2000/svg' width='1200' height='700' viewBox='0 0 1200 700'>\n");
    fprintf(out, "<rect width='1200' height='700' rx='24' fill='#07111f'/>\n");
    fprintf(out, "<text x='600' y='54' fill='#f8fafc' font-family='Arial' font-size='30' font-weight='700' text-anchor='middle'>Binary vs Ternary Search - Worst-case Array Probes</text>\n");
    fprintf(out, "<text x='600' y='86' fill='#94a3b8' font-family='Arial' font-size='16' text-anchor='middle'>Deterministic unsuccessful searches; x-axis is log2(n)</text>\n");
    fprintf(out, "<line x1='100' y1='610' x2='1100' y2='610' stroke='#64748b' stroke-width='2'/>\n");
    fprintf(out, "<line x1='100' y1='140' x2='100' y2='610' stroke='#64748b' stroke-width='2'/>\n");

    for (int i = 0; i <= 5; ++i) {
        double yv = ymax * i / 5.0;
        double y = map_y(yv, ymax);
        fprintf(out, "<line x1='100' y1='%.2f' x2='1100' y2='%.2f' stroke='#1e293b'/>\n", y, y);
        fprintf(out, "<text x='88' y='%.2f' fill='#94a3b8' font-family='monospace' font-size='13' text-anchor='end'>%.0f</text>\n", y + 4, yv);
    }

    fprintf(out, "<polyline fill='none' stroke='#38bdf8' stroke-width='5' points='");
    for (int i = 0; i < count; ++i) fprintf(out, "%.2f,%.2f ", map_x(rows[i].n, rows[0].n, rows[count-1].n), map_y(rows[i].bp, ymax));
    fprintf(out, "'/>\n");
    fprintf(out, "<polyline fill='none' stroke='#fb7185' stroke-width='5' points='");
    for (int i = 0; i < count; ++i) fprintf(out, "%.2f,%.2f ", map_x(rows[i].n, rows[0].n, rows[count-1].n), map_y(rows[i].tp, ymax));
    fprintf(out, "'/>\n");

    for (int i = 0; i < count; ++i) {
        double x = map_x(rows[i].n, rows[0].n, rows[count-1].n);
        fprintf(out, "<circle cx='%.2f' cy='%.2f' r='4' fill='#38bdf8'/><circle cx='%.2f' cy='%.2f' r='4' fill='#fb7185'/>\n",
                x, map_y(rows[i].bp, ymax), x, map_y(rows[i].tp, ymax));
    }

    fprintf(out, "<rect x='785' y='108' width='300' height='76' rx='14' fill='#0f172a' stroke='#334155'/>\n");
    fprintf(out, "<line x1='810' y1='134' x2='860' y2='134' stroke='#38bdf8' stroke-width='5'/><text x='875' y='140' fill='#e2e8f0' font-family='Arial' font-size='16'>Binary probes</text>\n");
    fprintf(out, "<line x1='810' y1='162' x2='860' y2='162' stroke='#fb7185' stroke-width='5'/><text x='875' y='168' fill='#e2e8f0' font-family='Arial' font-size='16'>Ternary probes</text>\n");
    fprintf(out, "<text x='600' y='660' fill='#cbd5e1' font-family='Arial' font-size='16' text-anchor='middle'>n grows exponentially from 2^3 to 2^20 - straight-ish traces confirm logarithmic growth</text>\n");
    fprintf(out, "</svg>\n");
    fclose(out);

    puts("Created q1_binary_vs_ternary.svg");
    return 0;
}
