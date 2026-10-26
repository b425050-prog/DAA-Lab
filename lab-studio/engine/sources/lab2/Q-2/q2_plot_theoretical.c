#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define W 1600
#define H 920

static double xmap(double n){return 120.0+(n/200000.0)*1390.0;}
static double ymap(double y,double ymax){return 760.0-(y/ymax)*570.0;}

static void curve(FILE*f,int kind,const char*color,const char*dash,double ymax){
    fputs("<path d=\"",f);
    for(int i=1;i<=500;++i){double n=400.0*i;double y=n*(log(n)/log(kind==2?2.0:3.0));fprintf(f,"%c%.2f %.2f ",i==1?'M':'L',xmap(n),ymap(y,ymax));}
    fprintf(f,"\" fill=\"none\" stroke=\"%s\" stroke-width=\"5\"",color);if(dash)fprintf(f," stroke-dasharray=\"%s\"",dash);fputs("/>\n",f);
}

int main(void){
    FILE*f=fopen("q2_theoretical_complexity.svg","w");if(!f){perror("q2_theoretical_complexity.svg");return EXIT_FAILURE;}
    double ymax=200000.0*(log(200000.0)/log(2.0));
    fprintf(f,"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\"><defs><linearGradient id=\"bg\" x1=\"0\" y1=\"0\" x2=\"1\" y2=\"1\"><stop offset=\"0\" stop-color=\"#07111f\"/><stop offset=\"1\" stop-color=\"#171126\"/></linearGradient></defs><rect width=\"100%%\" height=\"100%%\" rx=\"28\" fill=\"url(#bg)\"/>\n",W,H,W,H);
    fputs("<text x=\"800\" y=\"62\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"34\" font-weight=\"700\" fill=\"#f8fafc\">Q2 · Theoretical 2-Way vs 3-Way Merge Sort</text><text x=\"800\" y=\"100\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"18\" fill=\"#a5b4fc\">Depth-work models: n log₂ n and n log₃ n · both are Θ(n log n)</text>",f);
    fputs("<rect x=\"120\" y=\"155\" width=\"1390\" height=\"605\" rx=\"20\" fill=\"#0b1220\" stroke=\"#334155\"/>",f);
    for(int i=0;i<=5;++i){double n=40000.0*i,x=xmap(n);fprintf(f,"<line x1=\"%.1f\" y1=\"155\" x2=\"%.1f\" y2=\"760\" stroke=\"#1e293b\"/><text x=\"%.1f\" y=\"790\" text-anchor=\"middle\" font-family=\"monospace\" font-size=\"14\" fill=\"#94a3b8\">%.0fK</text>",x,x,x,n/1000.0);}
    for(int i=0;i<=5;++i){double frac=i/5.0,y=760.0-frac*570.0;fprintf(f,"<line x1=\"120\" y1=\"%.1f\" x2=\"1510\" y2=\"%.1f\" stroke=\"#1e293b\"/><text x=\"105\" y=\"%.1f\" text-anchor=\"end\" dominant-baseline=\"middle\" font-family=\"monospace\" font-size=\"13\" fill=\"#94a3b8\">%.1fM</text>",y,y,y,ymax*frac/1e6);}
    curve(f,2,"#38bdf8",NULL,ymax);curve(f,3,"#f472b6","12 8",ymax);
    fputs("<rect x=\"1025\" y=\"190\" width=\"425\" height=\"130\" rx=\"18\" fill=\"#111827\" fill-opacity=\".94\" stroke=\"#334155\"/><line x1=\"1060\" y1=\"230\" x2=\"1125\" y2=\"230\" stroke=\"#38bdf8\" stroke-width=\"5\"/><text x=\"1145\" y=\"237\" font-family=\"Segoe UI,Arial\" font-size=\"18\" fill=\"#e2e8f0\">2-way: n log₂ n</text><line x1=\"1060\" y1=\"278\" x2=\"1125\" y2=\"278\" stroke=\"#f472b6\" stroke-width=\"5\" stroke-dasharray=\"12 8\"/><text x=\"1145\" y=\"285\" font-family=\"Segoe UI,Arial\" font-size=\"18\" fill=\"#e2e8f0\">3-way: n log₃ n</text>",f);
    fputs("<text x=\"800\" y=\"842\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"18\" fill=\"#cbd5e1\">T₂(n)=2T₂(n/2)+Θ(n) · T₃(n)=3T₃(n/3)+Θ(n)</text><text x=\"800\" y=\"878\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"15\" fill=\"#64748b\">The logarithm base changes only a constant factor; asymptotically both curves are Θ(n log n).</text></svg>",f);
    fclose(f);puts("Created q2_theoretical_complexity.svg");return 0;
}
