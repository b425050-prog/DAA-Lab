#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define REPS 6
#define OPS 7
#define MAX_POINTS 256
#define W 1800
#define H 1260

static const char *rep_names[REPS] = {
    "Unsorted array", "Sorted array", "Singly linked unsorted",
    "Singly linked sorted", "Doubly linked unsorted", "Doubly linked sorted"
};
static const char *op_names[OPS] = {"Search","Insert","Delete","Maximum","Minimum","Predecessor","Successor"};
static const char *colors[OPS] = {"#38bdf8","#22c55e","#f97316","#e879f9","#facc15","#fb7185","#a78bfa"};
static const char *dashes[OPS] = {NULL,"14 8","5 7","18 6 4 6","2 6","10 5 2 5","20 7"};

typedef struct {
    int n;
    double y[REPS][OPS];
} Row;

static double map_log(double v, double vmin, double vmax, double a, double b) {
    double lv=log10(v), lmin=log10(vmin), lmax=log10(vmax);
    return a + (lv-lmin)/(lmax-lmin)*(b-a);
}

int main(void) {
    FILE *in=fopen("q1_experimental_data.dat","r");
    if(!in){perror("q1_experimental_data.dat"); fprintf(stderr,"Run q1_experimental_complexity first.\n"); return EXIT_FAILURE;}
    Row rows[MAX_POINTS]; int count=0; char line[8192];
    while(fgets(line,sizeof line,in) && count<MAX_POINTS){
        if(line[0]=='#'||line[0]=='\n') continue;
        char *p=line; char *end=NULL;
        long n=strtol(p,&end,10); if(end==p) continue; p=end;
        rows[count].n=(int)n;
        int ok=1;
        for(int r=0;r<REPS&&ok;++r) for(int o=0;o<OPS;++o){
            double v=strtod(p,&end); if(end==p){ok=0;break;} rows[count].y[r][o]=v>0?v:1; p=end;
        }
        if(ok) ++count;
    }
    fclose(in);
    if(count<2){fprintf(stderr,"Not enough data.\n");return EXIT_FAILURE;}

    FILE *f=fopen("q1_experimental_complexity.svg","w");
    if(!f){perror("q1_experimental_complexity.svg");return EXIT_FAILURE;}
    fprintf(f,"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\">\n",W,H,W,H);
    fputs("<defs><linearGradient id=\"bg\" x1=\"0\" y1=\"0\" x2=\"1\" y2=\"1\"><stop offset=\"0\" stop-color=\"#050b14\"/><stop offset=\"1\" stop-color=\"#111827\"/></linearGradient></defs>",f);
    fprintf(f,"<rect width=\"%d\" height=\"%d\" rx=\"30\" fill=\"url(#bg)\"/>\n",W,H);
    fputs("<text x=\"900\" y=\"56\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"34\" font-weight=\"700\" fill=\"#f8fafc\">Q1 · Experimental Dictionary Growth</text><text x=\"900\" y=\"92\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"18\" fill=\"#94a3b8\">96 measured sizes · dominant-step counts · log–log axes · all 42 operation/representation combinations</text>\n",f);

    const double mx0=70,my0=145,pw=530,ph=425,gx=35,gy=55;
    double nmin=rows[0].n,nmax=rows[count-1].n;
    for(int r=0;r<REPS;++r){
        int col=r%3,row=r/3;
        double x0=mx0+col*(pw+gx), y0=my0+row*(ph+gy);
        double px0=x0+70, px1=x0+pw-25, py0=y0+66, py1=y0+ph-58;
        double ymax=1.0;
        for(int i=0;i<count;++i) for(int o=0;o<OPS;++o) if(rows[i].y[r][o]>ymax)ymax=rows[i].y[r][o];
        if(ymax<2)ymax=2;
        fprintf(f,"<rect x=\"%.1f\" y=\"%.1f\" width=\"%.1f\" height=\"%.1f\" rx=\"20\" fill=\"#0b1220\" stroke=\"#263449\"/><text x=\"%.1f\" y=\"%.1f\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"22\" font-weight=\"700\" fill=\"#e2e8f0\">%s</text>\n",x0,y0,pw,ph,x0+pw/2,y0+35,rep_names[r]);
        int xt[]={64,256,1024,4096,16384,65536};
        for(int t=0;t<6;++t){double x=map_log(xt[t],nmin,nmax,px0,px1);fprintf(f,"<line x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" stroke=\"#172033\"/><text x=\"%.1f\" y=\"%.1f\" text-anchor=\"middle\" font-family=\"monospace\" font-size=\"11\" fill=\"#64748b\">%d</text>\n",x,py0,x,py1,x,py1+20,xt[t]);}
        double yt=1.0;
        while(yt<=ymax*1.001){double y=map_log(yt,1.0,ymax,py1,py0);fprintf(f,"<line x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" stroke=\"#172033\"/><text x=\"%.1f\" y=\"%.1f\" text-anchor=\"end\" dominant-baseline=\"middle\" font-family=\"monospace\" font-size=\"11\" fill=\"#64748b\">%.0f</text>\n",px0,y,px1,y,px0-8,y,yt);yt*=4.0;}
        for(int o=0;o<OPS;++o){
            fprintf(f,"<path d=\"");
            for(int i=0;i<count;++i){double x=map_log(rows[i].n,nmin,nmax,px0,px1),y=map_log(rows[i].y[r][o],1.0,ymax,py1,py0);fprintf(f,"%c%.1f %.1f ",i?'L':'M',x,y);} 
            fprintf(f,"\" fill=\"none\" stroke=\"%s\" stroke-width=\"2.4\" stroke-opacity=\".88\"",colors[o]);
            if(dashes[o])fprintf(f," stroke-dasharray=\"%s\"",dashes[o]);
            fputs("/>\n",f);
        }
    }

    fputs("<g font-family=\"Segoe UI,Arial\" font-size=\"15\">",f);
    double lx=215,ly=1080;
    for(int o=0;o<OPS;++o){double x=lx+(o%4)*360,y=ly+(o/4)*42;fprintf(f,"<line x1=\"%.0f\" y1=\"%.0f\" x2=\"%.0f\" y2=\"%.0f\" stroke=\"%s\" stroke-width=\"4\"",x,y,x+55,y,colors[o]);if(dashes[o])fprintf(f," stroke-dasharray=\"%s\"",dashes[o]);fprintf(f,"/><text x=\"%.0f\" y=\"%.0f\" fill=\"#cbd5e1\" dominant-baseline=\"middle\">%s</text>",x+68,y,op_names[o]);}
    fputs("</g>",f);
    fputs("<text x=\"900\" y=\"1188\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"17\" fill=\"#94a3b8\">Flat traces ⇒ O(1) · slowly rising trace ⇒ O(log n) · diagonal linear-growth traces ⇒ O(n)</text><text x=\"900\" y=\"1222\" text-anchor=\"middle\" font-family=\"Segoe UI,Arial\" font-size=\"14\" fill=\"#64748b\">Exact overlaps are expected when different operations execute the same dominant number of steps.</text></svg>\n",f);
    fclose(f);
    printf("Created q1_experimental_complexity.svg from %d measured rows.\n",count);
    return EXIT_SUCCESS;
}
