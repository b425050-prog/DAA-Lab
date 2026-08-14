/* DAA Lab 03 Q4 - log-log SVG of Strassen vs classical scalar multiplications. */
#include <stdio.h>
#include <math.h>
#define MAXP 32

typedef struct{double n,s,c,r;}Row;
static double lx(double x){return log(x)/log(2.0);} 
int main(void){
 FILE*in=fopen("q4_experimental_data.dat","r");if(!in){puts("Run q4_experimental_validation first.");return 1;}Row a[MAXP];int k=0;char z[256];while(fgets(z,sizeof z,in)&&k<MAXP){if(z[0]=='#')continue;if(sscanf(z,"%lf %lf %lf %lf",&a[k].n,&a[k].s,&a[k].c,&a[k].r)==4)k++;}fclose(in);if(k<2)return 1;
 double xmax=lx(a[k-1].n),ymax=log(a[k-1].c)/log(10.0); FILE*o=fopen("q4_strassen_vs_classical.svg","w");if(!o)return 1;
 fprintf(o,"<svg xmlns='http://www.w3.org/2000/svg' width='1200' height='700'><defs><linearGradient id='b' x1='0' y1='0' x2='1' y2='1'><stop stop-color='#1e1b4b'/><stop offset='1' stop-color='#020617'/></linearGradient></defs><rect width='1200' height='700' rx='24' fill='url(#b)'/><text x='600' y='58' fill='white' font-family='Arial' font-size='30' font-weight='700' text-anchor='middle'>Strassen: 7 Recursive Products instead of 8</text><text x='600' y='90' fill='#c7d2fe' font-family='Arial' font-size='16' text-anchor='middle'>Scalar multiplication growth on log scale</text><line x1='100' y1='610' x2='1100' y2='610' stroke='#64748b' stroke-width='2'/><line x1='100' y1='150' x2='100' y2='610' stroke='#64748b' stroke-width='2'/>");
 fprintf(o,"<polyline fill='none' stroke='#22d3ee' stroke-width='5' points='");for(int i=0;i<k;i++){double x=100+lx(a[i].n)/xmax*1000,y=610-(log(a[i].s)/log(10.0))/ymax*460;fprintf(o,"%.2f,%.2f ",x,y);}fprintf(o,"'/><polyline fill='none' stroke='#f97316' stroke-width='5' points='");for(int i=0;i<k;i++){double x=100+lx(a[i].n)/xmax*1000,y=610-(log(a[i].c)/log(10.0))/ymax*460;fprintf(o,"%.2f,%.2f ",x,y);}fprintf(o,"'/>");
 fprintf(o,"<rect x='740' y='112' width='345' height='82' rx='14' fill='#0f172a' stroke='#475569'/><line x1='765' y1='140' x2='820' y2='140' stroke='#22d3ee' stroke-width='5'/><text x='835' y='146' fill='white' font-family='Arial' font-size='15'>Strassen n^2.8074</text><line x1='765' y1='169' x2='820' y2='169' stroke='#f97316' stroke-width='5'/><text x='835' y='175' fill='white' font-family='Arial' font-size='15'>Classical n^3</text><text x='600' y='660' fill='#c7d2fe' font-family='Arial' font-size='16' text-anchor='middle'>Replacing 8 half-size products by 7 lowers the exponent from 3 to log2(7) ~= 2.8074.</text></svg>");fclose(o);puts("Created q4_strassen_vs_classical.svg");return 0;}
