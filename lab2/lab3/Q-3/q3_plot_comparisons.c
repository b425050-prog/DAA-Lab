/* DAA Lab 03 Q3 - SVG comparison-count plot. */
#include <stdio.h>

#define MAXP 256

typedef struct { double n,c,b,e; } Row;

static double X(double n,double maxn){ return 100.0+n/maxn*1000.0; }
static double Y(double y,double ymax){ return 610.0-y/ymax*460.0; }

int main(void){
    FILE *in=fopen("q3_experimental_data.dat","r"); if(!in){puts("Run q3_experimental_validation first.");return 1;}
    Row a[MAXP];int k=0;char s[256]; while(fgets(s,sizeof s,in)&&k<MAXP){if(s[0]=='#')continue;if(sscanf(s,"%lf %lf %lf %lf",&a[k].n,&a[k].c,&a[k].b,&a[k].e)==4)k++;} fclose(in); if(k<2)return 1;
    double maxn=a[k-1].n,ymax=a[k-1].b*1.05;
    FILE *o=fopen("q3_max_min_comparisons.svg","w");if(!o)return 1;
    fprintf(o,"<svg xmlns='http://www.w3.org/2000/svg' width='1200' height='700'><rect width='1200' height='700' rx='24' fill='#111827'/><text x='600' y='58' fill='#fff' font-family='Arial' font-size='30' font-weight='700' text-anchor='middle'>Max + Min with Divide &amp; Conquer</text><text x='600' y='90' fill='#c4b5fd' font-family='Arial' font-size='16' text-anchor='middle'>Measured comparisons stay below the requested 3n/2 ceiling</text>");
    fprintf(o,"<line x1='100' y1='610' x2='1100' y2='610' stroke='#64748b' stroke-width='2'/><line x1='100' y1='150' x2='100' y2='610' stroke='#64748b' stroke-width='2'/>");
    for(int i=0;i<=5;i++){double v=ymax*i/5,y=Y(v,ymax);fprintf(o,"<line x1='100' y1='%.2f' x2='1100' y2='%.2f' stroke='#1f2937'/><text x='88' y='%.2f' fill='#9ca3af' font-family='monospace' font-size='13' text-anchor='end'>%.0f</text>",y,y,y+4,v);}
    fprintf(o,"<polyline fill='none' stroke='#8b5cf6' stroke-width='5' points='");for(int i=0;i<k;i++)fprintf(o,"%.2f,%.2f ",X(a[i].n,maxn),Y(a[i].c,ymax));fprintf(o,"'/><polyline fill='none' stroke='#f59e0b' stroke-width='3' stroke-dasharray='10 8' points='");for(int i=0;i<k;i++)fprintf(o,"%.2f,%.2f ",X(a[i].n,maxn),Y(a[i].b,ymax));fprintf(o,"'/>");
    fprintf(o,"<rect x='760' y='110' width='320' height='82' rx='14' fill='#0f172a' stroke='#334155'/><line x1='785' y1='139' x2='840' y2='139' stroke='#8b5cf6' stroke-width='5'/><text x='855' y='145' fill='#fff' font-family='Arial' font-size='15'>Actual D&amp;C comparisons</text><line x1='785' y1='169' x2='840' y2='169' stroke='#f59e0b' stroke-width='3' stroke-dasharray='10 8'/><text x='855' y='175' fill='#fff' font-family='Arial' font-size='15'>3n/2 upper bound</text><text x='600' y='660' fill='#ddd6fe' font-family='Arial' font-size='16' text-anchor='middle'>For n = 2^k: exact count = 3n/2 - 2 comparisons.</text></svg>");fclose(o);puts("Created q3_max_min_comparisons.svg");return 0;
}
