#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define W 1600
#define H 940
#define LEFT 125.0
#define RIGHT 70.0
#define TOP 150.0
#define BOTTOM 120.0

static double sx(double n) {
    double xmin = log10(2.0), xmax = log10(65536.0);
    return LEFT + (log10(n) - xmin) / (xmax - xmin) * (W - LEFT - RIGHT);
}

static double sy(double y) {
    double ymin = 0.0, ymax = log10(65536.0);
    return TOP + (ymax - log10(y)) / (ymax - ymin) * (H - TOP - BOTTOM);
}

static void path(FILE *f, int kind, const char *color, const char *dash) {
    fprintf(f, "<path d=\"");
    for (int i = 0; i <= 500; ++i) {
        double t = (double)i / 500.0;
        double n = pow(10.0, log10(2.0) + t * (log10(65536.0) - log10(2.0)));
        double y = kind == 0 ? 1.0 : (kind == 1 ? log(n) / log(2.0) : n);
        fprintf(f, "%c%.2f %.2f ", i ? 'L' : 'M', sx(n), sy(y));
    }
    fprintf(f, "\" fill=\"none\" stroke=\"%s\" stroke-width=\"5\" stroke-linecap=\"round\"", color);
    if (dash) fprintf(f, " stroke-dasharray=\"%s\"", dash);
    fprintf(f, "/>\n");
}

int main(void) {
    FILE *f = fopen("q1_theoretical_complexity.svg", "w");
    if (!f) { perror("q1_theoretical_complexity.svg"); return EXIT_FAILURE; }

    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\">\n", W,H,W,H);
    fputs("<defs><linearGradient id=\"bg\" x1=\"0\" y1=\"0\" x2=\"1\" y2=\"1\"><stop offset=\"0\" stop-color=\"#07111f\"/><stop offset=\"1\" stop-color=\"#111827\"/></linearGradient><filter id=\"glow\"><feGaussianBlur stdDeviation=\"4\" result=\"b\"/><feMerge><feMergeNode in=\"b\"/><feMergeNode in=\"SourceGraphic\"/></feMerge></filter></defs>\n", f);
    fprintf(f, "<rect width=\"%d\" height=\"%d\" rx=\"28\" fill=\"url(#bg)\"/>\n", W,H);
    fputs("<text x=\"800\" y=\"64\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"34\" font-weight=\"700\" fill=\"#f8fafc\">Q1 · Theoretical Growth Signatures</text>\n", f);
    fputs("<text x=\"800\" y=\"104\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"19\" fill=\"#94a3b8\">Log–log axes keep O(1), O(log n), and O(n) visibly separated over a large range</text>\n", f);

    double x0=LEFT, x1=W-RIGHT, y0=TOP, y1=H-BOTTOM;
    fprintf(f, "<rect x=\"%.0f\" y=\"%.0f\" width=\"%.0f\" height=\"%.0f\" rx=\"18\" fill=\"#0b1220\" stroke=\"#334155\"/>\n", x0,y0,x1-x0,y1-y0);

    int xticks[] = {2,4,16,64,256,1024,4096,16384,65536};
    int nx = (int)(sizeof xticks/sizeof xticks[0]);
    for (int i=0;i<nx;++i) {
        double x=sx((double)xticks[i]);
        fprintf(f,"<line x1=\"%.2f\" y1=\"%.0f\" x2=\"%.2f\" y2=\"%.0f\" stroke=\"#1e293b\"/><text x=\"%.2f\" y=\"%.0f\" text-anchor=\"middle\" font-family=\"monospace\" font-size=\"14\" fill=\"#94a3b8\">%d</text>\n",x,y0,x,y1,x,y1+28,xticks[i]);
    }
    int yticks[] = {1,2,4,16,64,256,1024,4096,16384,65536};
    int ny = (int)(sizeof yticks/sizeof yticks[0]);
    for (int i=0;i<ny;++i) {
        double y=sy((double)yticks[i]);
        fprintf(f,"<line x1=\"%.0f\" y1=\"%.2f\" x2=\"%.0f\" y2=\"%.2f\" stroke=\"#1e293b\"/><text x=\"%.0f\" y=\"%.2f\" text-anchor=\"end\" dominant-baseline=\"middle\" font-family=\"monospace\" font-size=\"14\" fill=\"#94a3b8\">%d</text>\n",x0,y,x1,y,x0-14,y,yticks[i]);
    }

    path(f,0,"#22c55e","14 10");
    path(f,1,"#38bdf8",NULL);
    path(f,2,"#f97316","4 8");

    fputs("<g font-family=\"Segoe UI,Arial\" font-size=\"19\"><rect x=\"1040\" y=\"180\" width=\"420\" height=\"155\" rx=\"18\" fill=\"#0f172a\" fill-opacity=\".94\" stroke=\"#334155\"/><line x1=\"1075\" y1=\"220\" x2=\"1140\" y2=\"220\" stroke=\"#22c55e\" stroke-width=\"5\" stroke-dasharray=\"14 10\"/><text x=\"1160\" y=\"227\" fill=\"#e2e8f0\">O(1) · constant</text><line x1=\"1075\" y1=\"260\" x2=\"1140\" y2=\"260\" stroke=\"#38bdf8\" stroke-width=\"5\"/><text x=\"1160\" y=\"267\" fill=\"#e2e8f0\">O(log n) · logarithmic</text><line x1=\"1075\" y1=\"300\" x2=\"1140\" y2=\"300\" stroke=\"#f97316\" stroke-width=\"5\" stroke-dasharray=\"4 8\"/><text x=\"1160\" y=\"307\" fill=\"#e2e8f0\">O(n) · linear</text></g>\n", f);

    fputs("<text x=\"800\" y=\"885\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"17\" fill=\"#94a3b8\">x-axis: input size n (log scale) · y-axis: normalized dominant work (log scale)</text>\n", f);
    fputs("<text x=\"800\" y=\"915\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"15\" fill=\"#64748b\">The curves are growth models, not machine-time predictions.</text>\n", f);
    fputs("</svg>\n", f);
    fclose(f);
    puts("Created q1_theoretical_complexity.svg");
    return EXIT_SUCCESS;
}
