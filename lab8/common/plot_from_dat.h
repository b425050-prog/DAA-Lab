#ifndef LAB8_PLOT_FROM_DAT_H
#define LAB8_PLOT_FROM_DAT_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef PLOT_DATA_FILE
#error "PLOT_DATA_FILE must be defined"
#endif
#ifndef PLOT_OUTPUT_FILE
#error "PLOT_OUTPUT_FILE must be defined"
#endif
#ifndef PLOT_TITLE
#error "PLOT_TITLE must be defined"
#endif
#ifndef PLOT_SUBTITLE
#define PLOT_SUBTITLE "Deterministic, correctness-checked experiment"
#endif
#ifndef PLOT_BADGE
#define PLOT_BADGE "MEASURED EVIDENCE"
#endif
#ifndef PLOT_X_LABEL
#define PLOT_X_LABEL "Input scale"
#endif
#ifndef PLOT_Y_LABEL
#define PLOT_Y_LABEL "Dominant operations (log2 scale)"
#endif
#ifndef PLOT_FOOTER
#define PLOT_FOOTER "Every point is accepted only after its correctness check passes."
#endif

#ifndef PLOT_REFERENCE_LABEL
#define PLOT_REFERENCE_LABEL "reference"
#endif
#ifndef PLOT_SHOW_REFERENCE
#define PLOT_SHOW_REFERENCE 1
#endif

enum { PLOT_MAX_ROWS = 64, PLOT_W = 1280, PLOT_H = 760 };

typedef struct {
    double scale;
    double measured;
    double reference;
    int valid;
} PlotRow;

static double qplot_map(double value, double low, double high,
                        double pixel_low, double pixel_high) {
    if (fabs(high - low) < 1e-12) return (pixel_low + pixel_high) / 2.0;
    return pixel_low + (value - low) * (pixel_high - pixel_low) / (high - low);
}

static double qplot_log(double value) {
    return log(value < 1.0 ? 1.0 : value) / log(2.0);
}

int main(void) {
    FILE *input = fopen(PLOT_DATA_FILE, "r");
    if (input == NULL) {
        fprintf(stderr, "Run the experimental validator first.\n");
        return 1;
    }

    PlotRow rows[PLOT_MAX_ROWS];
    int count = 0;
    char line[512];
    while (count < PLOT_MAX_ROWS && fgets(line, sizeof line, input) != NULL) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %lf %lf %d", &rows[count].scale,
                   &rows[count].measured, &rows[count].reference,
                   &rows[count].valid) == 4) {
            if (!rows[count].valid) {
                fclose(input);
                fprintf(stderr, "Refusing to plot invalid evidence.\n");
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

    double x_min = qplot_log(rows[0].scale);
    double x_max = qplot_log(rows[count - 1].scale);
    double y_min = qplot_log(rows[0].measured);
    double y_max = y_min;
    for (int i = 0; i < count; ++i) {
        double a = qplot_log(rows[i].measured);
        double b = qplot_log(rows[i].reference);
        if (a < y_min) y_min = a;
        if (b < y_min) y_min = b;
        if (a > y_max) y_max = a;
        if (b > y_max) y_max = b;
    }
    y_min = floor(y_min);
    y_max = ceil(y_max) + 1;

    const double left = 112.0, right = 1188.0, top = 166.0, bottom = 626.0;
    FILE *out = fopen(PLOT_OUTPUT_FILE, "w");
    if (out == NULL) return 1;
    fprintf(out,
        "<svg xmlns='http://www.w3.org/2000/svg' width='1280' height='760' viewBox='0 0 1280 760' role='img' aria-labelledby='title desc'>"
        "<title id='title'>%s</title><desc id='desc'>%s</desc>"
        "<defs><linearGradient id='bg' x1='0' y1='0' x2='1' y2='1'>"
        "<stop stop-color='#071b21'/><stop offset='.5' stop-color='#122b30'/><stop offset='1' stop-color='#2b2139'/></linearGradient>"
        "<linearGradient id='line' x1='0' y1='0' x2='1' y2='0'><stop stop-color='#6ee7b7'/><stop offset='1' stop-color='#fda4af'/></linearGradient>"
        "<filter id='glow'><feGaussianBlur stdDeviation='4' result='b'/><feMerge><feMergeNode in='b'/><feMergeNode in='SourceGraphic'/></feMerge></filter>"
        "<pattern id='dots' width='28' height='28' patternUnits='userSpaceOnUse'><circle cx='2' cy='2' r='1' fill='#94a3b8' opacity='.12'/></pattern></defs>"
        "<rect width='1280' height='760' rx='30' fill='url(#bg)'/><rect width='1280' height='760' rx='30' fill='url(#dots)'/>"
        "<circle cx='1145' cy='45' r='130' fill='#6ee7b7' opacity='.08'/><circle cx='1195' cy='20' r='65' fill='#fda4af' opacity='.10'/>"
        "<text x='64' y='66' fill='#f8fafc' font-family='Segoe UI,Arial' font-size='31' font-weight='800'>%s</text>"
        "<text x='66' y='101' fill='#b8c4db' font-family='Segoe UI,Arial' font-size='17'>%s</text>"
        "<rect x='958' y='54' width='260' height='50' rx='25' fill='#081225' stroke='#6ee7b7' stroke-width='2'/>"
        "<text x='1088' y='86' fill='#67e8f9' font-family='Consolas,monospace' font-size='17' font-weight='700' text-anchor='middle'>%s</text>"
        "<rect x='76' y='138' width='1148' height='516' rx='20' fill='#040914' opacity='.68' stroke='#334155'/>",
        PLOT_TITLE, PLOT_SUBTITLE, PLOT_TITLE, PLOT_SUBTITLE, PLOT_BADGE);

    for (int i = 0; i <= 6; ++i) {
        double x = qplot_map((double)i, 0.0, 6.0, left, right);
        fprintf(out, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='#64748b' opacity='.24'/>", x, top, x, bottom);
    }
    for (int i = 0; i <= 5; ++i) {
        double y = qplot_map((double)i, 0.0, 5.0, bottom, top);
        fprintf(out, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='#64748b' opacity='.24'/>", left, y, right, y);
    }

    for (int tick = 0; tick <= 5; ++tick) {
        double y = qplot_map((double)tick, 0.0, 5.0, bottom, top);
        double exponent = y_min + (y_max - y_min) * tick / 5.0;
        fprintf(out, "<text x='98' y='%.2f' text-anchor='end' fill='#94a3b8' font-family='Consolas,monospace' font-size='12'>%.0f</text>", y + 4, pow(2, exponent));
    }
    if (PLOT_SHOW_REFERENCE) {
    fprintf(out, "<polyline fill='none' stroke='#94a3b8' stroke-width='3' stroke-dasharray='10 8' points='");
    for (int i = 0; i < count; ++i) {
        fprintf(out, "%.2f,%.2f ", qplot_map(qplot_log(rows[i].scale), x_min, x_max, left, right),
                qplot_map(qplot_log(rows[i].reference), y_min, y_max, bottom, top));
    }
    fprintf(out, "'/>");
    }
    fprintf(out, "<polyline fill='none' stroke='url(#line)' stroke-width='6' stroke-linejoin='round' filter='url(#glow)' points='");
    for (int i = 0; i < count; ++i) {
        fprintf(out, "%.2f,%.2f ", qplot_map(qplot_log(rows[i].scale), x_min, x_max, left, right),
                qplot_map(qplot_log(rows[i].measured), y_min, y_max, bottom, top));
    }
    fprintf(out, "'/>");

    for (int i = 0; i < count; ++i) {
        double x = qplot_map(qplot_log(rows[i].scale), x_min, x_max, left, right);
        double y = qplot_map(qplot_log(rows[i].measured), y_min, y_max, bottom, top);
        fprintf(out, "<circle cx='%.2f' cy='%.2f' r='6' fill='#6ee7b7' stroke='#fff' stroke-width='1.5'/>", x, y);
        if (i == 0 || i == count - 1 || i % 2 == 0) {
            fprintf(out, "<text x='%.2f' y='650' fill='#94a3b8' font-family='Segoe UI,Arial' font-size='12' text-anchor='middle'>%.0f</text>", x, rows[i].scale);
        }
    }

    fprintf(out,
        "<line x1='150' y1='191' x2='195' y2='191' stroke='#6ee7b7' stroke-width='6'/><text x='210' y='197' fill='#e2e8f0' font-family='Segoe UI,Arial' font-size='15'>measured</text>");
    if (PLOT_SHOW_REFERENCE) {
        fprintf(out, "<line x1='355' y1='191' x2='400' y2='191' stroke='#94a3b8' stroke-width='3' stroke-dasharray='9 7'/><text x='415' y='197' fill='#e2e8f0' font-family='Segoe UI,Arial' font-size='15'>%s</text>", PLOT_REFERENCE_LABEL);
    } else {
        fprintf(out, "<text x='355' y='197' fill='#94a3b8' font-family='Segoe UI,Arial' font-size='15'>finite observations only</text>");
    }
    fprintf(out,
        "<text x='650' y='688' fill='#cbd5e1' font-family='Segoe UI,Arial' font-size='16' text-anchor='middle'>%s</text>"
        "<text x='36' y='396' fill='#cbd5e1' font-family='Segoe UI,Arial' font-size='16' text-anchor='middle' transform='rotate(-90 36 396)'>%s</text>"
        "<line x1='64' y1='714' x2='1216' y2='714' stroke='#334155'/><text x='640' y='742' fill='#94a3b8' font-family='Segoe UI,Arial' font-size='14' text-anchor='middle'>%s</text></svg>",
        PLOT_X_LABEL, PLOT_Y_LABEL, PLOT_FOOTER);
    fclose(out);
    printf("Created %s\n", PLOT_OUTPUT_FILE);
    return 0;
}

#endif
