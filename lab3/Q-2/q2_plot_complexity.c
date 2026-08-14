/* DAA Lab 03 Q2 - SVG plot of balance-scale weighings. */
#include <stdio.h>
#include <math.h>

#define MAXP 256

typedef struct { double n, w, lg, bound; } Row;

static double X(double n, double maxn) { return 105.0 + log(n)/log(maxn) * 980.0; }
static double Y(double v, double ymax) { return 610.0 - v/ymax * 450.0; }

int main(void) {
    FILE *in = fopen("q2_experimental_data.dat", "r");
    if (!in) { puts("Run q2_experimental_validation first."); return 1; }
    Row a[MAXP]; int c = 0; char line[256];
    while (fgets(line, sizeof line, in) && c < MAXP) {
        if (line[0] == '#') continue;
        if (sscanf(line, "%lf %lf %lf %lf", &a[c].n, &a[c].w, &a[c].lg, &a[c].bound) == 4) c++;
    }
    fclose(in); if (c < 2) return 1;
    double ymax = a[c-1].bound + 2.0, maxn = a[c-1].n;

    FILE *o = fopen("q2_defective_coin_complexity.svg", "w");
    if (!o) return 1;
    fprintf(o, "<svg xmlns='http://www.w3.org/2000/svg' width='1200' height='700'>\n");
    fprintf(o, "<defs><linearGradient id='g' x1='0' x2='1'><stop stop-color='#052e16'/><stop offset='1' stop-color='#0f172a'/></linearGradient></defs>\n");
    fprintf(o, "<rect width='1200' height='700' rx='24' fill='url(#g)'/>\n");
    fprintf(o, "<text x='600' y='55' fill='white' font-family='Arial' font-size='30' font-weight='700' text-anchor='middle'>Defective Coin - Divide &amp; Conquer Validation</text>\n");
    fprintf(o, "<text x='600' y='88' fill='#bbf7d0' font-family='Arial' font-size='16' text-anchor='middle'>Worst observed balance weighings stay below ceil(log2 n) + 1</text>\n");
    fprintf(o, "<line x1='105' y1='610' x2='1085' y2='610' stroke='#64748b' stroke-width='2'/><line x1='105' y1='160' x2='105' y2='610' stroke='#64748b' stroke-width='2'/>\n");
    for (int i=0;i<=5;i++){ double v=ymax*i/5.0,y=Y(v,ymax); fprintf(o,"<line x1='105' y1='%.2f' x2='1085' y2='%.2f' stroke='#1f2937'/><text x='92' y='%.2f' fill='#cbd5e1' font-family='monospace' font-size='13' text-anchor='end'>%.1f</text>\n",y,y,y+4,v); }
    fprintf(o, "<polyline fill='none' stroke='#22c55e' stroke-width='5' points='");
    for(int i=0;i<c;i++) fprintf(o,"%.2f,%.2f ",X(a[i].n,maxn),Y(a[i].w,ymax));
    fprintf(o,"'/><polyline fill='none' stroke='#fbbf24' stroke-width='3' stroke-dasharray='10 8' points='");
    for(int i=0;i<c;i++) fprintf(o,"%.2f,%.2f ",X(a[i].n,maxn),Y(a[i].bound,ymax));
    fprintf(o,"'/>\n");
    fprintf(o,"<rect x='760' y='112' width='310' height='80' rx='14' fill='#0f172a' stroke='#475569'/><line x1='785' y1='140' x2='835' y2='140' stroke='#22c55e' stroke-width='5'/><text x='850' y='146' fill='white' font-family='Arial' font-size='15'>Measured worst case</text><line x1='785' y1='169' x2='835' y2='169' stroke='#fbbf24' stroke-width='3' stroke-dasharray='10 8'/><text x='850' y='175' fill='white' font-family='Arial' font-size='15'>ceil(log2 n) + 1</text>\n");
    fprintf(o,"<text x='600' y='660' fill='#d1fae5' font-family='Arial' font-size='16' text-anchor='middle'>Each unbalanced weighing keeps only one half; a balanced odd split needs at most one final leftover check.</text></svg>\n");
    fclose(o); puts("Created q2_defective_coin_complexity.svg"); return 0;
}
