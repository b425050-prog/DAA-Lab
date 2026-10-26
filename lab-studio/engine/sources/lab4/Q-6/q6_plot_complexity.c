/* DAA Lab 04 Q6 - SVG plot for maximum closed-interval overlap. */

#include <math.h>
#include <stdio.h>
#include "../common/svg_plot.h"

#define MAX_ROWS 32
typedef struct { double n,endpoints,sort_cmp,sweep,total,reference,ratio,peak; int valid; } Row;
static double lg(double x){return log(x)/log(2.0);}
int main(void){
 FILE*in=fopen("q6_experimental_data.dat","r");if(!in){fputs("Run q6_experimental_validation first.\n",stderr);return 1;}
 Row a[MAX_ROWS];int count=0;char line[280];while(count<MAX_ROWS&&fgets(line,sizeof line,in)){
  if(line[0]=='#')continue;
  if(sscanf(line,"%lf %lf %lf %lf %lf %lf %lf %lf %d",&a[count].n,&a[count].endpoints,
    &a[count].sort_cmp,&a[count].sweep,&a[count].total,&a[count].reference,
    &a[count].ratio,&a[count].peak,&a[count].valid)==9){if(!a[count].valid)return 2;++count;}}
 fclose(in);if(count<2)return 1;double xmin=lg(a[0].n),xmax=lg(a[count-1].n);
 double ymin=lg(a[0].reference)-.5,ymax=lg(a[count-1].total)+.6;
 FILE*out=fopen("q6_max_overlap_complexity.svg","w");if(!out)return 1;
 svg_begin(out,"Maximum Closed-Interval Overlap",
  "Sort 2n endpoints; group equal coordinates with START -> measure -> END semantics",
  "#a78bfa","#22d3ee","O(n log n)");svg_grid(out,count-1,5);
 fprintf(out,"<polyline fill='none' stroke='#94a3b8' stroke-width='3' stroke-dasharray='10 8' points='");
 for(int i=0;i<count;++i)fprintf(out,"%.2f,%.2f ",svg_map(lg(a[i].n),xmin,xmax,SVG_LEFT,SVG_RIGHT),
  svg_map(lg(a[i].reference),ymin,ymax,SVG_BOTTOM,SVG_TOP));
 fprintf(out,"'/>\n<polyline fill='none' stroke='url(#lineA)' stroke-width='6' stroke-linejoin='round' filter='url(#glow)' points='");
 for(int i=0;i<count;++i)fprintf(out,"%.2f,%.2f ",svg_map(lg(a[i].n),xmin,xmax,SVG_LEFT,SVG_RIGHT),
  svg_map(lg(a[i].total),ymin,ymax,SVG_BOTTOM,SVG_TOP));fprintf(out,"'/>\n");
 for(int i=0;i<count;++i){double x=svg_map(lg(a[i].n),xmin,xmax,SVG_LEFT,SVG_RIGHT);
  double y=svg_map(lg(a[i].total),ymin,ymax,SVG_BOTTOM,SVG_TOP);svg_point(out,x,y,"#a78bfa",5);
  char s[24];snprintf(s,sizeof s,"%.0f",a[i].n);svg_label(out,x,651,s,12,"#94a3b8","middle");}
 for(int i=0;i<=4;++i){double p=ymin+(ymax-ymin)*i/4.0;char s[24];snprintf(s,sizeof s,"2^%.1f",p);
  svg_label(out,98,svg_map(p,ymin,ymax,SVG_BOTTOM,SVG_TOP)+5,s,12,"#94a3b8","end");}
 svg_legend_item(out,160,194,"#a78bfa","Measured endpoint sweep");
 svg_legend_item(out,455,194,"#94a3b8","Reference 2n log2(2n)");
 svg_axis_titles(out,"Number of intervals n (log2-spaced)","Operations (log2 scale)");
 svg_footer(out,"Inclusivity is not a footnote: intervals starting and ending at p must both count at p.");
 fclose(out);puts("Created q6_max_overlap_complexity.svg");return 0;}
