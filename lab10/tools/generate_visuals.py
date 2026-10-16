"""Draw original GitHub artwork from verified C outputs and BFS component states."""
import json,math
from pathlib import Path
from PIL import Image,ImageDraw,ImageFont
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from lab_config import ROOT,TITLES,SHORT,SAMPLES,EVENTS
from validate import components
W,H=1000,570
BG='#091620';PANEL='#102632';INK='#eef7fa';MUTED='#87a7b4'
MINT='#73e6c2';GOLD='#f3c981';BLUE='#80bfff';PINK='#f69bb1';COLORS=[MINT,BLUE,GOLD,PINK,'#b9a4f5']
ASSETS=ROOT/'assets';ASSETS.mkdir(exist_ok=True)
def font(size,bold=False):
    for path in [f'C:/Windows/Fonts/{"segoeuib" if bold else "segoeui"}.ttf',
                 f'/usr/share/fonts/truetype/dejavu/DejaVuSans{"-Bold" if bold else ""}.ttf']:
        if Path(path).exists():return ImageFont.truetype(path,size)
    return ImageFont.load_default()
F={(size,bold):font(size,bold) for size in [14,16,18,20,22,24,28,32,38,42,48,60,64] for bold in [False,True]}
def txt(d,xy,s,size=20,fill=INK,bold=False,anchor=None):d.text(xy,str(s),font=F[size,bold],fill=fill,anchor=anchor)
def box(d,rect,fill=PANEL,outline='#254452',radius=18):d.rounded_rectangle(rect,radius=radius,fill=fill,outline=outline,width=1)
def base(q,t):
    im=Image.new('RGB',(W,H),BG);d=ImageDraw.Draw(im)
    for x in range(22,W,32):
        for y in range(24,H,32):d.ellipse((x,y,x+1,y+1),fill='#203744')
    for i in range(3):
        r=65+i*24;d.arc((885-r,-25-r,885+r,-25+r),t*80,t*80+250,fill='#294b55',width=2)
    txt(d,(42,24),'DAA / LAB 10',16,MINT,True)
    txt(d,(958,24),f'Q{q:02d}  /  CONNECTIVITY IN MOTION',16,MUTED,anchor='ra')
    size=38 if len(SHORT[q-1])>36 else 42
    txt(d,(42,59),SHORT[q-1],size,INK,True)
    box(d,(42,137,958,462));return im,d
def vertex(d,x,y,label,colour,small=None):
    d.ellipse((x-25,y-25,x+25,y+25),fill=colour)
    txt(d,(x,y-1),label,24,BG,True,'mm')
    if small is not None:txt(d,(x,y+35),small,16,MUTED,anchor='mt')
def footer(d,metric,note):
    txt(d,(43,484),metric,28,MINT,True);txt(d,(43,533),note,16,MUTED)
def get_sample(q):return json.loads((ROOT/f'Q-{q}'/f'q{q}_sample_trace.json').read_text())

def draw(q,t,out):
    im,d=base(q,t)
    if q==1:
        edges=[(0,1),(1,2),(2,0),(2,3),(3,0),(4,5),(1,4),(3,4)]
        step=min(8,max(1,int(t*8)+1));coords={0:(170,224),2:(170,320),4:(170,416),1:(625,224),3:(625,320),5:(625,416)}
        for i,(u,v) in enumerate(edges[:step]):
            a,b=coords[u],coords[v];ok=out['accepted'][i]
            d.line((*a,*b),fill=COLORS[0] if ok else PINK,width=3 if i==step-1 else 2)
            if not ok:
                x,y=(a[0]+b[0])/2,(a[1]+b[1])/2
                txt(d,(x+16,y),'SKIP',16,PINK,True)
        for i,(x,y) in coords.items():vertex(d,x,y,i+1,MINT if out['colours'][i]==0 else BLUE)
        txt(d,(170,164),'COLOUR 0',16,MINT,True,'mt');txt(d,(625,164),'COLOUR 1',16,BLUE,True,'mt')
        box(d,(740,207,925,389),BG)
        u,v=edges[step-1];txt(d,(761,232),f'EDGE {step} / 8',18,MUTED)
        txt(d,(761,278),f'{u+1} ↔ {v+1}',32,INK,True)
        txt(d,(761,334),'ACCEPT' if out['accepted'][step-1] else 'ODD CYCLE',18,MINT if out['accepted'][step-1] else PINK,True)
        footer(d,'Final sample: 7 accepted · 1 rejected','Parity-to-root encodes relative colour; a rejected constraint is skipped.')
    elif q==2:
        ops=[(0,0),(0,1),(1,2),(2,1),(1,1),(1,1),(2,3),(2,2)]
        step=min(8,max(1,int(t*8)+1));land=set(ops[:step]);active={r*4+c for r,c in land}
        edges=[(r*4+c,nr*4+nc) for r,c in land for nr,nc in [(r+1,c),(r,c+1)] if (nr,nc) in land]
        groups=components(12,edges,active);colour={v:COLORS[i%5] for i,g in enumerate(groups) for v in g}
        for r in range(3):
            for c in range(4):
                x=92+c*83;y=180+r*83;fill=colour.get(r*4+c,'#1c3c4b')
                box(d,(x,y,x+68,y+68),fill,radius=13)
                txt(d,(x+34,y+32),r*4+c,18,BG if (r,c) in land else MUTED,True,'mm')
        txt(d,(530,183),f'ADD {step} / 8',18,MUTED)
        txt(d,(530,226),f'Cell {ops[step-1]}',32,INK,True)
        txt(d,(530,293),out['counts'][step-1],64,MINT,True)
        txt(d,(615,321),'islands',24,MUTED)
        txt(d,(530,397),'Duplicate: unchanged' if step==6 else 'Join distinct active roots only',20,GOLD)
        footer(d,'1 → 1 → 2 → 3 → 1 → 1 → 2 → 1','Zero-based coordinates; four-neighbour connectivity; repeated cells are safe.')
    elif q==3:
        weights=[8,-10,7,3,-2];all_edges=[(0,1),(2,3),(0,2),(3,4),(1,4)]
        step=min(5,int(t*6));edges=all_edges[:step];groups=components(5,edges)
        col={v:COLORS[i%5] for i,g in enumerate(groups) for v in g}
        for u,v in edges:
            x,xx=115+u*185,115+v*185
            if abs(u-v)==1:d.line((x,275,xx,275),fill='#587681',width=3)
            else:
                points=[(x+(xx-x)*a/40,275-65*4*(a/40)*(1-a/40)) for a in range(41)]
                d.line(points,fill='#587681',width=3)
        for i,w in enumerate(weights):vertex(d,115+i*185,275,i+1,col[i],f'w = {w}')
        txt(d,(72,165),f'AFTER {step} EDGE ARRIVALS   /   K = 2',18,MUTED)
        for i,g in enumerate(groups):
            x=72+i*172;eligible=len(g)<=2
            box(d,(x,356,x+154,429),BG,outline=MINT if eligible else '#41555d',radius=12)
            txt(d,(x+14,368),f'size {len(g)}',16,MUTED)
            txt(d,(x+14,394),f'sum {sum(weights[v] for v in g)}',20,MINT if eligible else '#657c87',True)
        value=out['maxima'][step]
        footer(d,f'Maximum eligible sum: {value if value is not None else "NONE"}','Signed sums can fall after a merge; discard stale heap snapshots before answering.')
    elif q==4:
        weights=[5,-9,4,7,-3];order=[2,0,4,1,3];edges=[(i,i+1) for i in range(4)]
        step=min(5,int(t*6));active=set(order[5-step:]);groups=components(5,edges,active)
        col={v:COLORS[i%5] for i,g in enumerate(groups) for v in g}
        txt(d,(72,166),'REVERSE TIME   /   RESTORE VERTICES 4 → 2 → 5 → 1 → 3',18,MUTED)
        for u,v in edges:
            d.line((115+u*185,280,115+v*185,280),fill=MINT if u in active and v in active else '#293f4a',width=3)
        for i,w in enumerate(weights):vertex(d,115+i*185,280,i+1,col.get(i,'#314854'),f'w = {w}')
        sums=[sum(weights[v] for v in g) for g in groups];maximum=max(sums,default=0)
        txt(d,(72,371),f'Active components: {len(groups)}',24,MUTED)
        txt(d,(540,371),f'Current maximum: {maximum}',24,MINT,True)
        footer(d,'Forward deletion answers: 4, 4, 7, 7, 0','Save the answer BEFORE each restore to avoid an off-by-one deletion result.')
    else:
        original=[7,6,5,4,3,2,1];groups=[[0,2,4],[1,3],[5],[6]]
        col={v:COLORS[i] for i,g in enumerate(groups) for v in g};step=min(7,int(t*8))
        txt(d,(70,161),'POSITION',16,MUTED)
        for i in range(7):
            x=210+i*100;txt(d,(x+34,166),i+1,18,MUTED,anchor='mt')
            box(d,(x,210,x+68,272),col[i],radius=12);txt(d,(x+34,239),original[i],28,BG,True,'mm')
            box(d,(x,336,x+68,398),col[i] if i<step else '#243e4b',radius=12)
            if i<step:txt(d,(x+34,365),out['permutation'][i],28,BG,True,'mm')
        txt(d,(70,231),'INPUT',18,MUTED,True);txt(d,(70,358),'SMALLEST',18,MINT,True)
        txt(d,(500,295),'sort values within each colour group',20,INK,anchor='mt')
        footer(d,'3 4 5 6 7 2 1','Connectivity allows every rearrangement inside a component; values never leave it.')
    return im

def gif(frames,path,duration=100):
    palette=frames[-1].quantize(colors=160)
    ims=[x.quantize(palette=palette,dither=Image.Dither.NONE) for x in frames]
    ims[0].save(path,save_all=True,append_images=ims[1:],duration=[duration]*(len(ims)-1)+[1300],loop=0,disposal=2,optimize=True)

def chart(q):
    folder=ROOT/f'Q-{q}';data=[list(map(int,l.split())) for l in (folder/f'q{q}_data.dat').read_text().splitlines() if not l.startswith('#')]
    for dark in [False,True]:
        bg=BG if dark else 'white';fg=INK if dark else '#25394a'
        with plt.rc_context({'font.family':'DejaVu Sans','font.size':11,'svg.fonttype':'none'}):
            fig,ax=plt.subplots(figsize=(7.8,3.8));fig.subplots_adjust(left=.11,right=.98,top=.84,bottom=.24)
            fig.set_facecolor(bg);ax.set_facecolor(bg)
            n,w=zip(*data);ax.plot(n,w,'o-',lw=2.5,ms=5,color=MINT if dark else '#177f75')
            ax.set_xscale('log',base=2);ax.set_xlabel('Input size n (vertices / cells)',color=fg)
            ax.set_ylabel('Counted operations',color=fg);ax.tick_params(colors=fg)
            ax.set_title(f'Q{q:02d} · measured work on growing inputs',fontweight='bold',color=fg,pad=12)
            ax.grid(alpha=.16,color=fg);ax.set_axisbelow(True)
            for sp in ax.spines.values():sp.set_color('#52727e' if dark else '#b9cbd1')
            # Two lines prevent the longer heap event description from colliding with axes.
            fig.text(.5,.055,'Deterministic C executions • '+EVENTS[q-1],ha='center',fontsize=7,color=MUTED if dark else '#607780')
            fig.text(.5,.015,'Operation counters are not elapsed time or a proof of worst-case complexity.',ha='center',fontsize=7,color=MUTED if dark else '#607780')
            fig.savefig(folder/f'q{q}_graph.{"svg" if dark else "png"}',dpi=150,bbox_inches='tight',facecolor=bg);plt.close(fig)

def main():
    final=[]
    for q in range(1,6):
        out=get_sample(q);frames=[draw(q,k/39,out) for k in range(40)]
        gif(frames,ROOT/f'Q-{q}'/f'q{q}_animation.gif');frames[-1].save(ROOT/f'Q-{q}'/f'q{q}_overview.png',optimize=True)
        final.append(frames[-1]);chart(q)
    gif(final,ASSETS/'five_stories.gif',1900)
    hero=[]
    nodes=[(900,90),(1050,155),(930,290),(1120,315),(800,225)]
    for k in range(40):
        im=Image.new('RGB',(1200,400),BG);d=ImageDraw.Draw(im)
        for i,(a,b) in enumerate([(0,1),(0,4),(4,2),(2,3),(1,3),(1,2)]):
            x,y=nodes[a];xx,yy=nodes[b];d.line((x,y,xx,yy),fill='#2c5561',width=2)
            p=(k/40+i*.13)%1;cx=x+(xx-x)*p;cy=y+(yy-y)*p;d.ellipse((cx-5,cy-5,cx+5,cy+5),fill=MINT)
        for i,(x,y) in enumerate(nodes):vertex(d,x,y,i+1,COLORS[i])
        txt(d,(50,39),'WEEK 10 / GRAPH REPRESENTATIONS & DISJOINT SETS',18,MINT,True)
        txt(d,(48,100),'Connected by design.',60,INK,True)
        txt(d,(51,190),'Find the root. Join the story.',32,GOLD)
        txt(d,(52,256),'5 C17 solutions · 566 independently checked executions',22,MUTED)
        txt(d,(52,310),'Parity · islands · signed maxima · reverse time · permutations',20,MUTED)
        hero.append(im)
    gif(hero,ASSETS/'lab10_banner.gif',95);hero[0].save(ASSETS/'lab10_banner.png',optimize=True)
    rootassets=ROOT.parent/'assets';rootassets.mkdir(exist_ok=True);journey=[]
    labels=['FOUND','STRUCT','DIVIDE','SWEEP','SELECT','OPTIMISE','SEARCH','STATE','GREEDY','CONNECT']
    for k in range(50):
        im=Image.new('RGB',(1200,350),BG);d=ImageDraw.Draw(im)
        txt(d,(48,32),'THE ALGORITHM NOTEBOOK / SATYAM DHAL / B425050',18,MINT,True)
        txt(d,(46,78),'Ten labs. Sixty-four questions.',42,INK,True)
        d.line((82,220,1117,220),fill='#3b5c68',width=3);active=k//5
        for i,label in enumerate(labels):
            x=82+i*115;d.ellipse((x-25,195,x+25,245),fill=COLORS[i%5] if i<=active else '#28414d')
            txt(d,(x,219),f'{i+1:02d}',20,BG if i<=active else MUTED,True,'mm')
            txt(d,(x,268),label,14,MUTED,anchor='mt')
        txt(d,(48,323),'Design → implement → measure → verify → visualise',18,GOLD)
        txt(d,(1150,323),'LAB 10 / 13 OCTOBER 2026',16,MUTED,anchor='ra');journey.append(im)
    gif(journey,rootassets/'course_journey_lab10.gif',100)
    (rootassets/'daa-banner.svg').write_text('''<svg xmlns="http://www.w3.org/2000/svg" width="1200" height="340" viewBox="0 0 1200 340"><defs><linearGradient id="bg"><stop stop-color="#091620"/><stop offset="1" stop-color="#173b45"/></linearGradient></defs><rect width="1200" height="340" rx="24" fill="url(#bg)"/><g stroke="#45757c" fill="none"><path d="M960 70L1100 160L980 280L890 190Z M890 190L1100 160" stroke-width="2"/></g><g fill="#73e6c2"><circle cx="960" cy="70" r="12"/><circle cx="1100" cy="160" r="12"/><circle cx="980" cy="280" r="12"/><circle cx="890" cy="190" r="12"/></g><g font-family="Segoe UI,Arial,sans-serif"><text x="48" y="56" fill="#73e6c2" font-size="18" letter-spacing="3">THE ALGORITHM NOTEBOOK</text><text x="45" y="132" fill="#eef7fa" font-size="51" font-weight="700">Design &amp; Analysis</text><text x="45" y="192" fill="#eef7fa" font-size="51" font-weight="700">of Algorithms Laboratory</text><text x="48" y="249" fill="#f3c981" font-size="23">10 laboratories · 64 questions · C17 · proof + experiment</text><text x="48" y="300" fill="#87a7b4" font-size="18">SATYAM DHAL / B425050 / IIIT BHUBANESWAR</text></g></svg>''',encoding='utf-8')
    print('Created 8 original animated GIFs, 5 measured SVG/PNG graphs, and course artwork.')

if __name__=='__main__':main()
