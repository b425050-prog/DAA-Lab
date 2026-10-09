"""Repository-native artwork from checked C traces and measured datasets."""
import json, math, os
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from lab_config import ROOT, SHORT, TITLES, EVENTS

W,H=1000,560
BG='#0b1023'; PANEL='#151c35'; INK='#edf2ff'; MUTED='#94a3c5'
GOLD='#f5cf83'; CYAN='#6ee7da'; PURPLE='#b3a0ff'; CORAL='#ff9891'
COLORS=[GOLD,CYAN,PURPLE,CORAL]
ASSETS=ROOT/'assets'; ASSETS.mkdir(exist_ok=True)
def font(size,bold=False,mono=False):
    candidates=([f'C:/Windows/Fonts/{"consola" if mono else "segoeuib" if bold else "segoeui"}.ttf']
                +[f'/usr/share/fonts/truetype/dejavu/DejaVuSans{"Mono" if mono else "-Bold" if bold else ""}.ttf'])
    for p in candidates:
        if Path(p).exists(): return ImageFont.truetype(p,size)
    return ImageFont.load_default()
F={s:font(s) for s in [14,16,18,20,22,24,28,32,40,48,64]}
FB={s:font(s,True) for s in [14,16,18,20,22,24,28,32,40,48,64]}
FM=font(27,mono=True)

def text(d,xy,s,size=20,color=INK,bold=False,anchor=None):
    d.text(xy,str(s),font=(FB if bold else F)[size],fill=color,anchor=anchor)
def box(d,rect,fill=PANEL,outline='#273251',radius=18):
    d.rounded_rectangle(rect,radius=radius,fill=fill,outline=outline,width=1)
def background(q,t):
    im=Image.new('RGB',(W,H),BG); d=ImageDraw.Draw(im)
    for x in range(28,W,36):
        for y in range(25,H,36): d.ellipse((x,y,x+1,y+1),fill='#1c2441')
    for r in [92,125,161]: d.arc((825-r,70-r,825+r,70+r),15+t*25,260+t*25,fill='#263253',width=1)
    text(d,(42,27),'DAA / LAB 09',16,GOLD,True)
    text(d,(958,27),f'Q{q:02d}  •  GREEDY ATELIER',16,MUTED,anchor='ra')
    text(d,(42,61),SHORT[q-1],40,INK,True)
    d.line((42,123,958,123),fill='#2b3555',width=1)
    box(d,(42,149,958,465))
    return im,d

def tree(d,codes,weights,t,alphabetic=False):
    nodes={''}; prefixes={}
    for i,c in enumerate(codes):
        for k in range(len(c)+1):
            p=c[:k]; nodes.add(p); prefixes.setdefault(p,[]).append(i)
    coords={p:(85+sum(prefixes[p])/len(prefixes[p])*830/max(1,len(codes)-1),181+len(p)*52) for p in nodes}
    depth=max(map(len,codes)); visible=min(depth,math.ceil(t*(depth+1)))
    for p in sorted(nodes,key=len):
        if p and len(p)<=visible:
            x,y=coords[p]; a,b=coords[p[:-1]]; d.line((a,b,x,y),fill=PURPLE,width=2)
            text(d,((a+x)/2+7,(b+y)/2),p[-1],14,MUTED)
    for p,(x,y) in coords.items():
        if len(p)>visible: continue
        leaf=p in codes
        color=GOLD if leaf else CYAN
        d.ellipse((x-13,y-13,x+13,y+13),fill=color)
        if leaf:
            i=codes.index(p); text(d,(x,y+21),f'{i+1 if alphabetic else chr(65+i)} : {weights[i]}',16,INK,anchor='mt')
            text(d,(x,y+44),p,14,GOLD,anchor='mt')
    if alphabetic: text(d,(70,429),'Leaves stay in their original order.',16,MUTED)

def frame(q,t,out):
    im,d=background(q,t); reveal=max(1,math.ceil(t*8)); metric=''; note=''
    if q==1:
        rows=out['schedule']; used=out['used']; x=78
        text(d,(70,172),'Consumption timeline  •  1 weight unit / time unit',20,MUTED)
        for row in rows:
            if row['amount']<=1e-8: continue
            width=800*row['amount']/used; end=x+width
            d.rounded_rectangle((x,237,min(end,x+width*t),308),12,fill=CYAN if row['id']==1 else GOLD)
            if t>.35:
                text(d,(x+20,250),f'ITEM {row["id"]}',24,BG,True)
                text(d,(x,328),f'{row["amount"]:.0f} units · {row["fraction"]:.0%} selected',20,MUTED)
            x=end
        text(d,(78,388),'Item 2 is skipped. Highest initial density alone is insufficient.',20,INK)
        metric='9.00 integrated value'; note='Decreasing decay-rate order + exact fractional quantity optimisation'
    elif q==2:
        cs=sorted(out['codes'],key=lambda x:x['symbol']); tree(d,[c['code'] for c in cs],[c['frequency'] for c in cs],t)
        metric=f'{out["cost"]} weighted bits'; note='Minimum weighted length, then canonicalise by length and symbol'
    elif q==3:
        d.line((88,307,904,307),fill='#38425e',width=4)
        reach=[10,70,110][min(2,int(t*3))]
        d.line((88,307,88+min(reach,100)*8.16,307),fill=CYAN,width=5)
        for i,(p,f) in enumerate([(10,60),(20,30),(30,30),(60,40)],1):
            x=88+p*8.16; picked=i in out['stations'] and t>(.2 if i==1 else .6)
            d.ellipse((x-12,295,x+12,319),fill=GOLD if picked else '#64728e')
            text(d,(x,266),f'+{f}',20,GOLD if picked else MUTED,anchor='ms')
            text(d,(x,332),str(p),18,MUTED,anchor='mt')
        text(d,(88,189),f'Reachable distance: {reach}',32,INK,True)
        text(d,(904,333),'D = 100',18,INK,anchor='rt')
        text(d,(88,398),'Retrospectively choose the largest fuel already reachable.',20,MUTED)
        metric='2 refuelling stops'; note='Travel order: station 1 → station 4; final reach = 110'
    elif q==4:
        step=min(len(out['merges'])-1,int(t*len(out['merges'])))
        a,b,c=out['merges'][step]
        for x,value,color in [(235,a,CYAN),(470,b,PURPLE),(745,c,GOLD)]:
            d.ellipse((x-56,220,x+56,332),fill=color); text(d,(x,272),value,40,BG,True,'mm')
        text(d,(350,272),'+',40,MUTED,anchor='mm'); text(d,(596,272),'→',40,MUTED,anchor='mm')
        text(d,(500,181),f'MERGE {step+1} / {len(out["merges"])}',18,MUTED,anchor='mt')
        text(d,(500,385),f'Accumulated cost: {sum(m[2] for m in out["merges"][:step+1])}',28,INK,True,'mt')
        metric='49 minimum total cost'; note='Always connect the two shortest sticks in the min-heap'
    elif q==5:
        text(d,(72,171),'RATINGS',16,MUTED); text(d,(72,215),'CANDIES',16,GOLD)
        for i,(r,c) in enumerate(zip([1,3,4,5,2],out['candies'])):
            x=190+i*145; text(d,(x,177),r,24,MUTED,anchor='mt')
            k=c if i<reveal else 1
            for j in range(k): d.rounded_rectangle((x-30,402-j*43,x+30,435-j*43),9,fill=COLORS[i%4])
            text(d,(x,224),k,28,INK,True,'mt')
        metric=f'{out["total"]} candies'; note='Maximum of left-slope and right-slope lower bounds'
    elif q==6:
        s=out['string']; text(d,(70,174),'Repeated letters need at least K = 3 positions of separation.',20,MUTED)
        for i,c in enumerate(s):
            x=78+i*105; color=COLORS[(ord(c)-97)%4]
            box(d,(x,245,x+85,332),color if i<reveal else '#202942')
            if i<reveal: text(d,(x+42,288),c,40,BG,True,'mm')
            text(d,(x+42,349),str(i),16,MUTED,anchor='mt')
        text(d,(70,408),'Take the most frequent available letter; release it after cooldown.',20,INK)
        metric='abcabcad  •  valid'; note='Frequency heap + FIFO cooldown queue'
    elif q==7:
        states=[[4,2,10,20,6],[4,2,10,10,6],[4,2,5,10,6],[4,2,5,5,6],[4,2,5,5,3]]
        values=states[min(4,int(t*5))]
        for i,a in enumerate(values):
            x=100+i*168; height=140*a/20
            box(d,(x,392-height,x+100,392),COLORS[i%4],radius=10)
            text(d,(x+50,399),a,24,INK,True,'mt')
        text(d,(70,175),'Normalise odd values, then repeatedly halve the current maximum.',20,MUTED)
        text(d,(70,214),f'Current interval [{min(values)}, {max(values)}]',24,INK,True)
        metric=f'{out["deviation"]} minimum deviation'; note='Verified witness [4, 2, 5, 5, 3] lies in the best interval [2, 5]'
    elif q==8:
        intervals=[(0,30),(5,10),(15,20),(20,30),(30,40),(8,12)]
        for room in range(1,4):
            y=190+room*62; text(d,(72,y+15),f'ROOM {room}',16,MUTED)
            d.line((200,y+26,915,y+26),fill='#303a56',width=1)
        for i,((s,e),room) in enumerate(zip(intervals,out['assignment'])):
            if i>=math.ceil(t*6): continue
            y=190+room*62; x=200+s*17.5
            box(d,(x,y,x+(e-s)*17.5-3,y+50),COLORS[room-1],radius=9)
            text(d,(x+8,y+13),f'M{i+1}',18,BG,True)
        for x in [0,10,20,30,40]: text(d,(200+x*17.5,426),str(x),16,MUTED,anchor='mt')
        metric='3 meeting rooms'; note='Half-open intervals: a room ending at t can be reused at t'
    elif q==9:
        tree(d,[a['code'] for a in out['leaves']],[1,2,23,4,3,3,5,19],t,True)
        metric=f'{out["cost"]} alphabetic cost'; note='Compatible merges → leaf depths → ordered prefix tree'
    else:
        text(d,(70,173),'INPUT   abb  /  bba  /  bbc',20,MUTED)
        for y,label,s,color in [(233,'GREEDY',out['greedy'],CORAL),(341,'EXACT',out['exact'],CYAN)]:
            text(d,(73,y+19),label,18,color,True)
            for i,c in enumerate(s):
                x=234+i*83; box(d,(x,y,x+69,y+63),color if i<reveal else '#29314b',radius=10)
                if i<reveal: text(d,(x+34,y+30),c,32,BG,True,'mm')
            text(d,(896,y+26),len(s),28,color,True,'mm')
        metric='7 versus 6 characters'; note='Greedy is a heuristic; exact subset DP certifies this instance'
    text(d,(44,489),metric,28,GOLD,True)
    text(d,(44,533),note,16,MUTED)
    return im

def save_gif(frames,path,duration=110):
    pal=frames[-1].quantize(colors=128)
    indexed=[im.quantize(palette=pal,dither=Image.Dither.NONE) for im in frames]
    indexed[0].save(path,save_all=True,append_images=indexed[1:],loop=0,
                    duration=[duration]*(len(indexed)-1)+[1200],disposal=2,optimize=True)

def charts(q):
    folder=ROOT/f'Q-{q}'; rows=[]
    for line in (folder/f'q{q}_data.dat').read_text().splitlines():
        if not line.startswith('#'): rows.append(list(map(int,line.split())))
    n=[r[0] for r in rows]; counts=[r[1] for r in rows]
    for dark in [False,True]:
        bg=BG if dark else 'white'; fg=INK if dark else '#26304a'
        with plt.rc_context({'font.family':'DejaVu Sans','font.size':11,'svg.fonttype':'none'}):
            fig,ax=plt.subplots(figsize=(7.7,3.8))
            fig.subplots_adjust(left=.1,right=.98,bottom=.24,top=.85)
            fig.set_facecolor(bg); ax.set_facecolor(bg)
            ax.plot(n,counts,'o-',color=GOLD if dark else '#7358b1',lw=2.4,ms=5,label='Measured work')
            if q==10:
                ax.plot(n,[r[2] for r in rows],'s--',color=CYAN if dark else '#138d86',label='Exact DP transitions')
                ax.set_yscale('log'); ax.legend(facecolor=bg,labelcolor=fg,edgecolor='#65718b')
            if max(n)>100: ax.set_xscale('log',base=2)
            ax.set_xlabel('Input size n',color=fg); ax.set_ylabel('Counted operations',color=fg)
            ax.set_title(SHORT[q-1]+' — measured scaling',color=fg,fontweight='bold',pad=12)
            ax.tick_params(colors=fg)
            for sp in ax.spines.values(): sp.set_color('#59627b' if dark else '#bcc4d3')
            ax.grid(alpha=.18,color=fg); ax.set_axisbelow(True)
            fig.text(.5,.035,EVENTS[q-1]+' • deterministic inputs; counters are not elapsed time',
                     ha='center',fontsize=8,color=MUTED if dark else '#626d80')
            fig.savefig(folder/f'q{q}_graph.{"svg" if dark else "png"}',dpi=150,bbox_inches='tight',facecolor=bg)
            plt.close(fig)

def main():
    final=[]
    for q in range(1,11):
        out=json.loads((ROOT/f'Q-{q}'/f'q{q}_sample_trace.json').read_text())
        frames=[frame(q,i/31,out) for i in range(32)]
        save_gif(frames,ROOT/f'Q-{q}'/f'q{q}_animation.gif')
        frames[-1].save(ROOT/f'Q-{q}'/f'q{q}_overview.png',optimize=True)
        final.append(frames[-1]); charts(q)
    save_gif(final,ASSETS/'ten_stories.gif',1700)
    hero=[]
    for k in range(36):
        im=Image.new('RGB',(1200,400),BG); d=ImageDraw.Draw(im)
        for r in [85,120,160,206]: d.ellipse((980-r,205-r,980+r,205+r),outline='#283553',width=2)
        for i in range(10):
            angle=(i/10+k/36)*math.tau; x=980+160*math.cos(angle); y=205+160*math.sin(angle)
            d.ellipse((x-13,y-13,x+13,y+13),fill=COLORS[i%4])
        text(d,(54,46),'WEEK 09 / DESIGN & ANALYSIS OF ALGORITHMS',18,GOLD,True)
        text(d,(52,101),'The greedy atelier.',64,INK,True)
        text(d,(55,197),'Choose. Prove. Challenge.',32,CYAN)
        text(d,(55,255),'10 C17 solutions  •  885 checked executions',22,MUTED)
        text(d,(55,301),'Heaps, exchange arguments, alphabetic trees & exact witnesses',20,MUTED)
        text(d,(980,205),'09',64,GOLD,True,'mm'); hero.append(im)
    save_gif(hero,ASSETS/'lab9_banner.gif',90); hero[0].save(ASSETS/'lab9_banner.png',optimize=True)
    repoassets=ROOT.parent/'assets'; repoassets.mkdir(exist_ok=True)
    journey=[]
    labels=['FOUNDATION','STRUCTURE','DIVIDE','SORT / SWEEP','SELECT','OPTIMISE','SEARCH','STATE / DP','GREEDY']
    for k in range(45):
        im=Image.new('RGB',(1200,360),BG); d=ImageDraw.Draw(im)
        text(d,(50,29),'DESIGN → IMPLEMENT → MEASURE → VERIFY → VISUALISE',18,GOLD,True)
        text(d,(50,68),'Nine labs. Fifty-nine questions.',40,INK,True)
        active=k//5; d.line((94,219,1110,219),fill='#35405b',width=3)
        for i,label in enumerate(labels):
            x=94+i*127; color=COLORS[i%4] if i<=active else '#344058'
            d.ellipse((x-27,192,x+27,246),fill=color)
            text(d,(x,217),f'{i+1:02}',22,BG if i<=active else MUTED,True,'mm')
            text(d,(x,269),label,14,MUTED,anchor='mt')
        text(d,(50,325),'Satyam Dhal / B425050 / IIIT Bhubaneswar',16,MUTED)
        text(d,(1150,325),'LAB 09 • 06 OCT 2026',16,GOLD,anchor='ra'); journey.append(im)
    save_gif(journey,repoassets/'course_journey_lab9.gif',100)
    svg='''<svg xmlns="http://www.w3.org/2000/svg" width="1200" height="340" viewBox="0 0 1200 340"><defs><linearGradient id="g"><stop stop-color="#0b1023"/><stop offset="1" stop-color="#242147"/></linearGradient></defs><rect width="1200" height="340" rx="24" fill="url(#g)"/><g fill="none" stroke="#4a4778" opacity=".6"><circle cx="1050" cy="170" r="90"/><circle cx="1050" cy="170" r="130"/><circle cx="1050" cy="170" r="170"/></g><g font-family="Segoe UI,Arial,sans-serif"><text x="50" y="60" fill="#f5cf83" font-size="18" letter-spacing="4">THE ALGORITHM NOTEBOOK</text><text x="48" y="133" fill="#edf2ff" font-size="53" font-weight="700">Design &amp; Analysis</text><text x="48" y="194" fill="#edf2ff" font-size="53" font-weight="700">of Algorithms Laboratory</text><text x="51" y="247" fill="#6ee7da" font-size="23">9 laboratories · 59 questions · C17 · proof + experiment</text><text x="51" y="298" fill="#94a3c5" font-size="18">SATYAM DHAL / B425050 / IIIT BHUBANESWAR</text></g></svg>'''
    (repoassets/'daa-banner.svg').write_text(svg,encoding='utf-8')
    print('Created 13 animated GIFs, 10 measured SVG charts and compact report graphics.')

if __name__=='__main__': main()
