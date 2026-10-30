"""Original animated diagrams and measured plots; no external image assets."""
import json,math
from pathlib import Path
from PIL import Image,ImageDraw,ImageFont
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from lab_config import ROOT
W,H=1100,620
BG='#091723';PANEL='#102c3a';INK='#edf8ff';MUTED='#96b3c2'
CYAN='#6bdcfa';GOLD='#ffce88';MINT='#8febb5';PINK='#eea6cb';PURPLE='#b5a6ff'
ASSETS=ROOT/'assets';ASSETS.mkdir(exist_ok=True)
SHORT=['Two parities. One shortest route.','Every hub sends a wave.','Two searches meet in the middle.']
def font(size,bold=False):
    for p in [f'C:/Windows/Fonts/{"segoeuib" if bold else "segoeui"}.ttf',f'/usr/share/fonts/truetype/dejavu/DejaVuSans{"-Bold" if bold else ""}.ttf']:
        if Path(p).exists():return ImageFont.truetype(p,size)
    return ImageFont.load_default()
F={(n,b):font(n,b) for n in [14,16,18,20,22,24,28,32,36,40,44,48,56,64] for b in [False,True]}
def text(d,xy,s,size=20,fill=INK,bold=False,anchor=None):d.text(xy,str(s),font=F[size,bold],fill=fill,anchor=anchor)
def box(d,r,fill=PANEL,outline='#264959'):d.rounded_rectangle(r,radius=18,fill=fill,outline=outline,width=1)
def arrow(d,a,b,colour,width=3):
    dx=b[0]-a[0];dy=b[1]-a[1];length=math.hypot(dx,dy);ux=dx/length;uy=dy/length
    end=(b[0]-30*ux,b[1]-30*uy);start=(a[0]+30*ux,a[1]+30*uy)
    d.line((*start,*end),fill=colour,width=width)
    d.polygon([end,(end[0]-12*ux+6*uy,end[1]-12*uy-6*ux),(end[0]-12*ux-6*uy,end[1]-12*uy+6*ux)],fill=colour)
def node(d,x,y,label,fill,active=False,r=28):
    if active:d.ellipse((x-r-7,y-r-7,x+r+7,y+r+7),outline=fill,width=2)
    d.ellipse((x-r,y-r,x+r,y+r),fill=fill)
    text(d,(x,y-2),label,24,BG,True,'mm')
def frame(q,t):
    im=Image.new('RGB',(W,H),BG);d=ImageDraw.Draw(im)
    for x in range(25,W,34):
        for y in range(25,H,34):d.point((x,y),fill='#25404d')
    text(d,(42,26),'DAA / LAB 11 / BREADTH-FIRST SEARCH',16,CYAN,True)
    text(d,(1058,26),f'Q{q:02}  /  FRONTIERS IN MOTION',16,MUTED,anchor='ra')
    text(d,(42,66),SHORT[q-1],44,INK,True);box(d,(42,145,1058,491))
    out=json.loads((ROOT/f'Q-{q}/q{q}_sample_trace.json').read_text())
    if q==1:
        # Show the actual sample's two alternative routes to vertex two.
        text(d,(72,166),'SOURCE 1   /   TARGET 2   /   ODD PENALTY C = 5',18,MUTED)
        a,b,c=(140,385),(400,216),(660,385)
        arrow(d,a,c,PINK);text(d,(400,429),'1 edge of weight 1: odd',18,PINK,anchor='mt')
        # Upper two-edge detour crosses parity twice.
        a,b,c=(140,385),(400,216),(660,385)
        progress=min(2,int(t*3))
        arrow(d,a,b,MINT if progress>=1 else '#436474');arrow(d,b,c,MINT if progress>=2 else '#436474')
        text(d,(240,293),'1',20,MINT,True);text(d,(540,293),'1',20,MINT,True)
        node(d,*a,1,CYAN,True);node(d,*b,3,MINT,progress==1);node(d,*c,2,GOLD,progress==2)
        box(d,(744,201,1027,451),BG)
        text(d,(766,222),'ODD STATE',16,PINK,True);text(d,(766,252),'1 + 5 = 6',32,INK,True)
        text(d,(766,318),'EVEN STATE',16,MINT,True);text(d,(766,348),'2 + 0 = 2',32,MINT,True)
        text(d,(766,410),'Choose the even route',18,GOLD)
        text(d,(43,517),'Vertex 2 costs 2, even though its lightest raw path costs 1.',28,MINT,True)
        text(d,(43,566),'State = (vertex, parity). Apply C once, when selecting the final answer.',18,MUTED)
    elif q==2:
        step=min(3,int(t*4));coords={i:(130+i*135,285) for i in range(7)}
        coords[7]=(535,418);coords[8]=(940,418)
        for u,v in [(i,i+1) for i in range(6)]+[(3,7)]:d.line((*coords[u],*coords[v]),fill='#476574',width=3)
        for i,(x,y) in coords.items():
            row=out['rows'][i];owner=int(row[1]);dist=int(row[2]);visible=0<=dist<=step
            fill=(MINT if owner==1 else CYAN) if visible else '#4b6270'
            node(d,x,y,i+1,fill,visible and dist==step)
            text(d,(x,y+39),f'h{owner} / d{dist}' if visible else 'unassigned',16,fill if visible else MUTED,anchor='mt')
        text(d,(72,170),f'RADIUS R = 3   /   CURRENT WAVE d = {step}',18,MUTED)
        text(d,(72,205),'Hub 1 is vertex 7. Hub 2 is vertex 1.',20,GOLD,True)
        text(d,(72,409),'Vertex 4 is equally close to both hubs.',18,MUTED)
        text(d,(43,517),'The tie at vertex 4 belongs to hub 1.',32,MINT,True)
        text(d,(43,566),'Ordered seeds + FIFO layers preserve hub-index priority. Vertices 8 and 9 stay unassigned.',18,MUTED)
    else:
        points={'hit':(140,315),'hot':(330,315),'dot':(540,233),'lot':(540,405),'dog':(755,233),'log':(755,405),'cog':(950,315)}
        edges=[('hit','hot'),('hot','dot'),('hot','lot'),('dot','dog'),('lot','log'),('dog','cog'),('log','cog')]
        stage=min(4,int(t*5));forward=[{'hit'},{'hit','hot'},{'hit','hot','dot','lot'},{'hit','hot','dot','lot'},{'hit','hot','dot','lot','dog','log'}][stage]
        backward={'cog'} if stage<3 else {'cog','dog','log'}
        for u,v in edges:
            colour=GOLD if stage==4 and (u,v) in [('hit','hot'),('hot','dot'),('dot','dog'),('dog','cog')] else '#476574'
            d.line((*points[u],*points[v]),fill=colour,width=4 if colour==GOLD else 2)
        for word,(x,y) in points.items():
            fill=GOLD if word in forward&backward else CYAN if word in forward else PINK if word in backward else '#4b6270'
            box(d,(x-49,y-25,x+49,y+25),fill,fill);text(d,(x,y-2),word,24,BG,True,'mm')
        text(d,(72,169),'FORWARD FROM hit',18,CYAN,True);text(d,(1028,169),'BACKWARD FROM cog',18,PINK,True,'ra')
        text(d,(43,517),'hit → hot → dot → dog → cog   /   4 mutations',32,GOLD,True)
        text(d,(43,566),'Generate one-letter neighbours through exact encoded-word lookup. Expand complete layers.',18,MUTED)
    return im
def gif(frames,path,duration=110):
    frames[0].save(path,save_all=True,append_images=frames[1:],duration=duration,loop=0,optimize=True,disposal=2)
def chart(q):
    data=[list(map(int,l.split())) for l in (ROOT/f'Q-{q}/q{q}_data.dat').read_text().splitlines() if l and not l.startswith('#')]
    short=['Parity states and 0-1 BFS','Ordered hubs and radius-limited BFS','Bidirectional search in binary word cubes'][q-1]
    captions=['E = 3V - 4. Count: initialisations + arc scans + deque actions + answer rows.',
              'Two-hub ladder; R = V. Count: initialisations + seeds + arc scans + queues + answer rows.',
              'N = 2^L, L = 1..10. Count: letters + initialisations + candidates + hash probes + queues.']
    for dark in [False,True]:
        bg=BG if dark else 'white';fg=INK if dark else '#233d50';colour=CYAN if dark else '#157c9c'
        with plt.rc_context({'font.family':'DejaVu Sans','font.size':11,'svg.fonttype':'none'}):
            fig,ax=plt.subplots(figsize=(8.2,4.15));fig.subplots_adjust(left=.12,right=.97,top=.83,bottom=.25)
            fig.set_facecolor(bg);ax.set_facecolor(bg)
            ax.plot([r[0] for r in data],[r[1] for r in data],'o-',color=colour,lw=2.5,ms=5)
            ax.set_xlabel('Vertices V' if q<3 else 'Dictionary words N (word length L grows)',color=fg)
            ax.set_ylabel('Counted operations',color=fg);ax.set_title(short,color=fg,fontweight='bold',pad=15)
            ax.tick_params(colors=fg);ax.ticklabel_format(style='plain',axis='both',useOffset=False)
            ax.grid(alpha=.16,color=fg);ax.set_axisbelow(True)
            for s in ax.spines.values():s.set_color('#496574' if dark else '#b7c9d5')
            fig.text(.5,.075,captions[q-1],ha='center',fontsize=10,color=MUTED if dark else '#5a7280')
            fig.text(.5,.025,'Linear axes. Counter totals describe this family; complexity is derived in the README.',ha='center',fontsize=10,color=MUTED if dark else '#5a7280')
            fig.savefig(ROOT/f'Q-{q}/q{q}_graph.{"svg" if dark else "png"}',dpi=160,bbox_inches='tight',facecolor=bg);plt.close(fig)
def main():
    final=[]
    for q in [1,2,3]:
        frames=[frame(q,k/39) for k in range(40)];gif(frames,ROOT/f'Q-{q}/q{q}_animation.gif')
        frames[-1].save(ROOT/f'Q-{q}/q{q}_overview.png',optimize=True);final.append(frames[-1]);chart(q)
    gif(final,ASSETS/'three_stories.gif',2100)
    hero=[]
    for k in range(40):
        im=Image.new('RGB',(1200,420),BG);d=ImageDraw.Draw(im)
        text(d,(48,38),'WEEK 11 / BREADTH-FIRST SEARCH',18,CYAN,True)
        text(d,(45,100),'Let the frontier unfold.',56,INK,True)
        text(d,(49,190),'Parity states. Shared waves. Meeting paths.',28,GOLD)
        text(d,(50,280),'3 C17 solutions / independently checked / fully reproducible',20,MUTED)
        text(d,(50,334),'Satyam Dhal / B425050 / IIIT Bhubaneswar',18,MUTED)
        center=(1010,210)
        for i in range(4):
            radius=35+i*28;d.ellipse((center[0]-radius,center[1]-radius,center[0]+radius,center[1]+radius),outline='#2c5265',width=2)
            angle=k/40*math.tau+i; x=center[0]+radius*math.cos(angle);y=center[1]+radius*math.sin(angle)
            d.ellipse((x-7,y-7,x+7,y+7),fill=[CYAN,GOLD,MINT,PINK][i])
        text(d,center,'11',48,CYAN,True,'mm');hero.append(im)
    gif(hero,ASSETS/'lab11_banner.gif',95);hero[0].save(ASSETS/'lab11_banner.png',optimize=True)
    rootassets=ROOT.parent/'assets';rootassets.mkdir(exist_ok=True);journey=[]
    for k in range(55):
        im=Image.new('RGB',(1200,360),BG);d=ImageDraw.Draw(im)
        text(d,(45,28),'THE ALGORITHM NOTEBOOK / B425050',18,CYAN,True)
        text(d,(43,77),'Eleven labs. Sixty-seven questions.',40,INK,True)
        d.line((80,220,1120,220),fill='#37566a',width=3)
        for i in range(11):
            x=80+i*104;active=i<=k//5
            node(d,x,220,f'{i+1:02}',[CYAN,MINT,GOLD,PINK,PURPLE][i%5] if active else '#3b5361',i==k//5,24)
        text(d,(45,300),'Explore the repository: implementation → evidence → proof → visual story.',20,MUTED)
        journey.append(im)
    gif(journey,rootassets/'course_journey_lab11.gif',100);journey[-1].save(rootassets/'course_journey_lab11.png',optimize=True)
    print('Created six animated GIFs, three measured SVG/PNG graphs and matching stills.')
if __name__=='__main__':main()
