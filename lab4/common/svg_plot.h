#ifndef LAB4_SVG_PLOT_H
#define LAB4_SVG_PLOT_H

#include <math.h>
#include <stdio.h>

enum {
    SVG_WIDTH = 1280,
    SVG_HEIGHT = 760,
    SVG_LEFT = 112,
    SVG_RIGHT = 1188,
    SVG_TOP = 166,
    SVG_BOTTOM = 626
};

static double svg_map(double value, double low, double high,
                      double pixel_low, double pixel_high) {
    if (fabs(high - low) < 1e-12) {
        return (pixel_low + pixel_high) / 2.0;
    }
    return pixel_low + (value - low) * (pixel_high - pixel_low) / (high - low);
}

static void svg_begin(FILE *out, const char *title, const char *subtitle,
                      const char *accent_a, const char *accent_b,
                      const char *badge) {
    fprintf(out,
        "<svg xmlns='http://www.w3.org/2000/svg' width='%d' height='%d' "
        "viewBox='0 0 %d %d' role='img' aria-labelledby='title desc'>\n"
        "<title id='title'>%s</title><desc id='desc'>%s</desc>\n"
        "<defs>"
        "<linearGradient id='bg' x1='0' y1='0' x2='1' y2='1'>"
        "<stop stop-color='#071426'/><stop offset='.48' stop-color='#111337'/>"
        "<stop offset='1' stop-color='#230b2f'/></linearGradient>"
        "<linearGradient id='lineA' x1='0' y1='0' x2='1' y2='0'>"
        "<stop stop-color='%s'/><stop offset='1' stop-color='%s'/></linearGradient>"
        "<filter id='glow' x='-50%%' y='-50%%' width='200%%' height='200%%'>"
        "<feGaussianBlur stdDeviation='5' result='b'/><feMerge><feMergeNode in='b'/>"
        "<feMergeNode in='SourceGraphic'/></feMerge></filter>"
        "<pattern id='dots' width='28' height='28' patternUnits='userSpaceOnUse'>"
        "<circle cx='2' cy='2' r='1' fill='#94a3b8' opacity='.11'/></pattern>"
        "</defs>\n"
        "<rect width='%d' height='%d' rx='30' fill='url(#bg)'/>"
        "<rect width='%d' height='%d' rx='30' fill='url(#dots)'/>"
        "<circle cx='1110' cy='72' r='118' fill='%s' opacity='.09'/>"
        "<circle cx='1155' cy='28' r='58' fill='%s' opacity='.12'/>"
        "<text x='64' y='66' fill='#f8fafc' font-family='Inter,Segoe UI,Arial,sans-serif' "
        "font-size='31' font-weight='800'>%s</text>"
        "<text x='66' y='101' fill='#b8c4db' font-family='Inter,Segoe UI,Arial,sans-serif' "
        "font-size='17'>%s</text>"
        "<rect x='955' y='54' width='263' height='50' rx='25' fill='#0b1227' "
        "stroke='%s' stroke-width='2'/><text x='1086.5' y='86' fill='%s' "
        "font-family='ui-monospace,SFMono-Regular,Consolas,monospace' font-size='18' "
        "font-weight='700' text-anchor='middle'>%s</text>\n"
        "<rect x='%d' y='%d' width='%d' height='%d' rx='18' fill='#050b18' "
        "opacity='.66' stroke='#334155'/>\n",
        SVG_WIDTH, SVG_HEIGHT, SVG_WIDTH, SVG_HEIGHT, title, subtitle,
        accent_a, accent_b, SVG_WIDTH, SVG_HEIGHT, SVG_WIDTH, SVG_HEIGHT,
        accent_a, accent_b, title, subtitle, accent_a, accent_b, badge,
        SVG_LEFT - 36, SVG_TOP - 28,
        SVG_RIGHT - SVG_LEFT + 72, SVG_BOTTOM - SVG_TOP + 56);
}

static void svg_grid(FILE *out, int x_divisions, int y_divisions) {
    for (int i = 0; i <= x_divisions; ++i) {
        double x = svg_map((double)i, 0.0, (double)x_divisions,
                           SVG_LEFT, SVG_RIGHT);
        fprintf(out,
                "<line x1='%.2f' y1='%d' x2='%.2f' y2='%d' "
                "stroke='#64748b' stroke-width='1' opacity='.22'/>\n",
                x, SVG_TOP, x, SVG_BOTTOM);
    }
    for (int i = 0; i <= y_divisions; ++i) {
        double y = svg_map((double)i, 0.0, (double)y_divisions,
                           SVG_BOTTOM, SVG_TOP);
        fprintf(out,
                "<line x1='%d' y1='%.2f' x2='%d' y2='%.2f' "
                "stroke='#64748b' stroke-width='1' opacity='.22'/>\n",
                SVG_LEFT, y, SVG_RIGHT, y);
    }
    fprintf(out,
            "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='#cbd5e1' "
            "stroke-width='1.5' opacity='.75'/>"
            "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='#cbd5e1' "
            "stroke-width='1.5' opacity='.75'/>\n",
            SVG_LEFT, SVG_BOTTOM, SVG_RIGHT, SVG_BOTTOM,
            SVG_LEFT, SVG_TOP, SVG_LEFT, SVG_BOTTOM);
}

static void svg_label(FILE *out, double x, double y, const char *text,
                      int size, const char *colour, const char *anchor) {
    fprintf(out,
            "<text x='%.2f' y='%.2f' fill='%s' "
            "font-family='Inter,Segoe UI,Arial,sans-serif' font-size='%d' "
            "text-anchor='%s'>%s</text>\n",
            x, y, colour, size, anchor, text);
}

static void svg_point(FILE *out, double x, double y, const char *colour,
                      double radius) {
    fprintf(out,
            "<circle cx='%.2f' cy='%.2f' r='%.1f' fill='%s' "
            "stroke='#f8fafc' stroke-width='1.5' filter='url(#glow)'/>\n",
            x, y, radius, colour);
}

static void svg_legend_item(FILE *out, double x, double y,
                            const char *colour, const char *text) {
    fprintf(out,
            "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s' "
            "stroke-width='5' stroke-linecap='round'/><circle cx='%.2f' "
            "cy='%.2f' r='5' fill='%s'/><text x='%.2f' y='%.2f' "
            "fill='#e2e8f0' font-family='Inter,Segoe UI,Arial,sans-serif' "
            "font-size='15'>%s</text>\n",
            x, y, x + 42.0, y, colour, x + 21.0, y, colour,
            x + 53.0, y + 5.0, text);
}

static void svg_axis_titles(FILE *out, const char *x_title,
                            const char *y_title) {
    svg_label(out, (SVG_LEFT + SVG_RIGHT) / 2.0, 685.0, x_title,
              16, "#cbd5e1", "middle");
    fprintf(out,
            "<text x='35' y='396' fill='#cbd5e1' "
            "font-family='Inter,Segoe UI,Arial,sans-serif' font-size='16' "
            "text-anchor='middle' transform='rotate(-90 35 396)'>%s</text>\n",
            y_title);
}

static void svg_footer(FILE *out, const char *text) {
    fprintf(out,
            "<line x1='64' y1='714' x2='1216' y2='714' stroke='#334155'/>"
            "<text x='640' y='741' fill='#94a3b8' "
            "font-family='Inter,Segoe UI,Arial,sans-serif' font-size='14' "
            "text-anchor='middle'>%s</text></svg>\n",
            text);
}

#endif
