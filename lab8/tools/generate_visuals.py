"""Render algorithm stories from validated C traces, with a consistent local design."""
import json
import math
import os
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
from lab_config import ROOT, TITLES, SHORT, SAMPLES
W,H=960,600
BG='#071a20'; PANEL='#10292e'; INK='#eaf6ef'; MUTED='#9bb5b6'
MINT='#81e7bb'; PEACH='#f6b69b'; LAV='#b5b2f2'; LINE='#234047'
def font(size,bold=False,mono=False):
    candidates = ([Path('C:/Windows/Fonts')/('consolab.ttf' if bold else 'consola.ttf')] if mono else
                  [Path('C:/Windows/Fonts')/('segoeuib.ttf' if bold else 'segoeui.ttf')])
    candidates += [Path('/usr/share/fonts/truetype/dejavu')/('DejaVuSansMono.ttf' if mono else 'DejaVuSans-Bold.ttf' if bold else 'DejaVuSans.ttf')]
    for path in candidates:
        if path.exists(): return ImageFont.truetype(str(path),size)
    return ImageFont.load_default(size=size)
F={size:font(size) for size in [12,14,16,18,20,22,24,28,32,40,52,64]}
def text(d,xy,s,size=18,fill=INK,bold=False,mono=False,anchor=None):
    d.text(xy,str(s),font=font(size,bold,mono),fill=fill,anchor=anchor)
def wrapped(d,xy,s,width=208,size=17,fill=MUTED):
    x,y=xy; line=''
    for word in s.split():
        trial=(line+' '+word).strip()
        if d.textlength(trial,font=font(size))>width and line:
            text(d,(x,y),line,size,fill); y+=size+8; line=word
        else: line=trial
    text(d,(x,y),line,size,fill); return y+size+8
def roundbox(d,box,fill=PANEL,outline=LINE,r=16): d.rounded_rectangle(box,r,fill=fill,outline=outline,width=1)
def glow(im,xy,color,r=15):
    d=ImageDraw.Draw(im)
    for radius in range(r,2,-3):
        # Fixed soft rings survive GIF palettes without muddy gradients.
        d.ellipse((xy[0]-radius,xy[1]-radius,xy[0]+radius,xy[1]+radius),outline=LINE,width=2)
    d.ellipse((xy[0]-4,xy[1]-4,xy[0]+4,xy[1]+4),fill=color)
def base(q,t,phase,work):
    im=Image.new('RGB',(W,H),BG); d=ImageDraw.Draw(im)
    for y in range(22,H,24):
        for x in range(22,W,24): d.ellipse((x,y,x+1,y+1),fill='#183139')
    d.arc((760,-220,1130,150),0,360,fill=LINE,width=1)
    text(d,(34,23),f'ALGORITHM ATELIER   /   LAB 08   /   Q{q:02}',14,MINT,True,True)
    text(d,(34,55),TITLES[q-1],32,INK,True)
    text(d,(36,105),SHORT[q-1],14,MUTED,False,True)
    roundbox(d,(32,154,664,514)); roundbox(d,(684,154,928,514))
    text(d,(704,176),'STATE NOTES',13,MINT,True,True)
    d.line((704,203,908,203),fill=LINE)
    roundbox(d,(32,535,928,574),fill='#0b2329',r=10)
    text(d,(48,546),phase,15,INK)
    text(d,(910,546),f'{work} operations',15,MINT,True,True,anchor='ra')
    text(d,(34,582),'C17 IMPLEMENTATION  /  CHECKED TRACE  /  SATYAM DHAL - B425050',11,MUTED,False,True)
    d.line((34,130,928,130),fill=LINE,width=1)
    d.line((34,130,34+int(894*t),130),fill=MINT,width=2)
    return im,d
def note(d,label,value,detail):
    text(d,(704,225),label,14,MUTED,True)
    text(d,(704,252),value,32,MINT,True,True)
    wrapped(d,(704,311),detail)
def tile(d,x,y,value,label='',active=False,width=62):
    roundbox(d,(x,y,x+width,y+62),fill='#174237' if active else '#132e34',outline=MINT if active else LINE,r=10)
    text(d,(x+width/2,y+22),value,22,MINT if active else INK,True,True,anchor='mm')
    if label: text(d,(x+width/2,y+47),label,12,MUTED,False,True,anchor='mm')
def arrow(d,a,b,color=MINT,width=2):
    d.line((a,b),fill=color,width=width); ang=math.atan2(b[1]-a[1],b[0]-a[0])
    d.polygon([b,(b[0]-9*math.cos(ang-.45),b[1]-9*math.sin(ang-.45)),(b[0]-9*math.cos(ang+.45),b[1]-9*math.sin(ang+.45))],fill=color)
def save(frames,path,hold=1400):
    path.parent.mkdir(parents=True,exist_ok=True)
    frames[0].save(path,save_all=True,append_images=frames[1:],duration=[500]+[100]*(len(frames)-2)+[hold],loop=0,optimize=False,disposal=2)
    frames[-1].save(path.with_suffix('.png'))
def trace(q): return json.loads((ROOT/f'Q-{q}'/f'q{q}_sample_trace.json').read_text())
def grid(d,values,active,path=None,x=84,y=210,cell=46,labels=None):
    rows=len(values); cols=len(values[0]); path=set(path or [])
    for i,row in enumerate(values):
        for j,value in enumerate(row):
            idx=i*cols+j; filled=idx<=active or i==0 or j==0
            col='#194536' if (i,j) in path else '#203d40' if filled else '#0a2027'
            box=(x+j*cell,y+i*cell,x+(j+1)*cell-4,y+(i+1)*cell-4)
            roundbox(d,box,fill=col,outline=MINT if idx==active or (i,j) in path else LINE,r=6)
            if filled: text(d,(box[0]+(cell-4)/2,box[1]+(cell-4)/2),value,17,INK,True,True,anchor='mm')
    if labels:
        a,b=labels
        for i,c in enumerate(' '+a): text(d,(x-19,y+i*cell+cell/2-3),c or ' ',16,PEACH,True,True,anchor='mm')
        for j,c in enumerate(' '+b): text(d,(x+j*cell+cell/2-2,y-18),c or ' ',16,PEACH,True,True,anchor='mm')
def array_story(q,out):
    a=list(map(int,SAMPLES[q-1].split()[1:])); n=len(a); vals=out['dp']; frames=[]
    for f in range(42):
        t=f/41; upto=min(n-1,int(t*(n+2))); finished=f>=34
        im,d=base(q,t,'Trace the winning predecessor chain' if finished else f'Solve subsequences ending at index {upto}',out['work'] if finished else upto*(upto+1)//2)
        text(d,(54,177),'INPUT ORDER IS PRESERVED',14,MUTED,True,True)
        gap=72 if n==8 else 82
        for i,x in enumerate(a): tile(d,52+i*gap,218,x,f'i={i}',i==upto,width=gap-9)
        text(d,(54,303),'BEST ENDING HERE',14,MUTED,True,True)
        maxv=max(vals)
        for i,v in enumerate(vals):
            x=52+i*gap
            if i<=upto:
                h=90*v/maxv; roundbox(d,(x,432-h,x+gap-9,432),fill='#3a6857' if i!=upto else MINT,r=7)
                text(d,(x+(gap-9)/2,445),v,18,MINT,True,True,anchor='mm')
        if finished:
            indices=[]; best=max(range(n),key=lambda i:vals[i]); k=best
            while k>=0: indices.append(k); k=out['parent'][k]
            indices.reverse()
            for i,j in zip(indices,indices[1:]): arrow(d,(52+i*gap+(gap-9)/2,204),(52+j*gap+(gap-9)/2,204),PEACH)
        note(d,'MAXIMUM SUM' if q==5 else 'MAXIMUM LENGTH',out['result'],
             'Each state chooses a smaller earlier value. Strict comparison rejects equal neighbours. The orange path reconstructs one optimal subsequence.')
        text(d,(54,484),' -> '.join(map(str,out['subsequence'])) if finished else 'd[i] = best legal predecessor + current contribution',15,PEACH if finished else MUTED)
        frames.append(im)
    return frames
def string_story(q,out):
    a,b=SAMPLES[q-1].splitlines(); values=out['dp']; m=len(a); n=len(b); path=[]; i=m;j=n
    if q==3:
        while i and j:
            path.append((i,j))
            if a[i-1]==b[j-1]: i-=1;j-=1
            elif values[i-1][j]>=values[i][j-1]: i-=1
            else: j-=1
    else:
        while i or j:
            path.append((i,j))
            if i and j and values[i][j]==values[i-1][j-1]+(a[i-1]!=b[j-1]): i-=1;j-=1
            elif i and values[i][j]==values[i-1][j]+1: i-=1
            else: j-=1
    path.append((i,j)); frames=[]; total=(m+1)*(n+1)
    for f in range(48):
        t=f/47; fill=min(1,t/.7); active=int(fill*(total-1)); pathcount=int(max(0,(t-.7)/.3)*len(path))+1 if t>=.7 else 0
        shown=path[:pathcount]; im,d=base(q,t,'Recover the answer along the green path' if t>=.7 else 'Fill the prefix table, left to right',min(m*n,int(fill*m*n)))
        text(d,(54,176),f'X = {a}     Y = {b}',16,MUTED,True,True)
        grid(d,values,active,shown,x=110,y=222,cell=33,labels=(a,b))
        note(d,'EDIT DISTANCE' if q==6 else 'LCS LENGTH',out['result'],
             'The green traceback turns an optimal value into an actual answer. Diagonal steps match or substitute; horizontal and vertical steps explain the remaining choices.' if q==6 else
             'A match extends the diagonal prefix. Otherwise keep the better neighbouring prefix. Traceback recovers a common subsequence.')
        result='M S M M M M I' if q==6 else out['subsequence']
        if q==6: result=' '.join(out['ops'])
        text(d,(54,486),result if t>=.7 else 'One cell stores one solved prefix pair.',18,PEACH if t>=.7 else MUTED,True,True)
        frames.append(im)
    return frames
def coin_story(q,out):
    frames=[]; coins=[1,3,4] if q==1 else [1,2,5]; v=6 if q==1 else 10
    stages=[]; row=[0]*(v+1); row[0]=1
    if q==2:
        for coin in coins:
            for x in range(coin,v+1): row[x]+=row[x-coin]
            stages.append(row.copy())
        assert row==out['dp']
    for f in range(42):
        t=f/41; im,d=base(q,t,'Reconstruct 3 + 3 = 6' if q==1 and t>.8 else 'Coin outer loop counts combinations once' if q==2 else 'Build minimum costs for increasing amounts',int(t*out['work']))
        if q==1:
            upto=min(v,int(t*(v+2))); text(d,(54,180),'AMOUNT STATES',14,MUTED,True,True)
            for x in range(v+1): tile(d,52+x*84,230,out['dp'][x] if x<=upto else '?',f'V={x}',x==upto,width=73)
            text(d,(54,332),'AVAILABLE COINS',14,MUTED,True,True)
            for j,c in enumerate(coins):
                x=95+j*125; d.ellipse((x-26,370,x+26,422),fill='#75513b',outline=PEACH,width=2); text(d,(x,395),c,24,PEACH,True,True,anchor='mm')
            if t>.8:
                arrow(d,(579,325),(328,325),PEACH); arrow(d,(328,325),(76,325),PEACH)
            note(d,'MINIMUM COINS',out['result'],'Greedy picks 4 + 1 + 1. Dynamic programming finds 3 + 3. Store a chosen coin so the optimum can be reconstructed.')
            text(d,(54,479),'d[x] = 1 + min d[x - coin]',19,MINT,True,True)
        else:
            cells=3*(v+1); active=int(t*(cells-1)); stage=min(2,active//(v+1)); amount=active%(v+1)
            text(d,(54,180),'WAYS AFTER EACH DENOMINATION',14,MUTED,True,True)
            for x in range(v+1): text(d,(134+x*44,219),x,15,MUTED,True,True,anchor='mm')
            for i,c in enumerate(coins):
                text(d,(60,258+i*59),f'+ {c}',20,PEACH,True,True)
                for x in range(v+1):
                    filled=i<stage or i==stage and x<=amount
                    roundbox(d,(113+x*44,239+i*59,153+x*44,289+i*59),fill='#194536' if i==stage and x==amount else '#132e34',r=6)
                    text(d,(133+x*44,264+i*59),stages[i][x] if filled else '.',18,INK if filled else MUTED,True,True,anchor='mm')
            note(d,'COMBINATIONS',out['result'],'The coin loop is outside the amount loop. 1 + 2 and 2 + 1 belong to the same combination; they are not counted twice.')
            text(d,(54,465),'ways[0] = 1: the empty combination',17,MINT,True,True)
        frames.append(im)
    return frames
def rod_story(out):
    prices=[1,5,8,9,10,17,17,20]; frames=[]
    for f in range(42):
        t=f/41; upto=min(8,int(t*10)); im,d=base(7,t,'Follow first_cut[] until no rod remains' if t>.8 else f'Try each first cut for rod length {upto}',upto*(upto+1)//2)
        text(d,(54,180),'PIECE LENGTH / SELLING PRICE',14,MUTED,True,True)
        for i,p in enumerate(prices): tile(d,52+i*73,213,p,f'L={i+1}',i+1==upto,width=63)
        text(d,(54,305),'BEST REVENUE FOR EACH ROD LENGTH',14,MUTED,True,True)
        for i,p in enumerate(out['dp']):
            x=53+i*66; roundbox(d,(x,343,x+57,399),fill='#194536' if i==upto else '#132e34',r=7)
            text(d,(x+28,363),p if i<=upto else '?',20,MINT,True,True,anchor='mm'); text(d,(x+28,386),i,12,MUTED,False,True,anchor='mm')
        if t>.8:
            x=54; colors=[PEACH,MINT]
            for i,piece in enumerate(out['pieces']):
                width=piece*70; roundbox(d,(x,438,x+width-4,475),fill=colors[i%2],r=6)
                text(d,(x+width/2,456),f'{piece} inches',17,BG,True,True,anchor='mm'); x+=width
        note(d,'MAXIMUM REVENUE',out['result'],'Do not assume the longest piece is best. Choosing a 2-inch first cut leaves an optimal 6-inch remainder. Total length is exactly eight.')
        frames.append(im)
    return frames
def obst_story(out):
    frames=[]; roots=out['roots']; positions={}; links=[]
    def walk(i,j,depth):
        x=352+(i+j)*28; y=237+depth*54
        label=f'd{i}' if i==j else f'k{roots[i][j]+1}'; positions[label]=(x,y)
        if i!=j:
            k=roots[i][j]
            for lo,hi in [(i,k),(k+1,j)]:
                child=f'd{lo}' if lo==hi else f'k{roots[lo][hi]+1}'; links.append((label,child)); walk(lo,hi,depth+1)
    walk(0,5,0)
    for f in range(45):
        t=f/44; im,d=base(8,t,'Reconstruct keys and unsuccessful-search leaves' if t>.65 else 'Solve intervals from shortest to longest',int(t*out['work']))
        text(d,(54,178),'INTERVAL ROOTS',14,MUTED,True,True)
        for i in range(6):
            for j in range(i,6):
                x=58+j*42; y=216+i*43; value='d' if i==j else roots[i][j]+1
                roundbox(d,(x,y,x+36,y+35),fill='#194536' if j-i<=int(t*8) else '#0a2027',r=6)
                if j-i<=int(t*8): text(d,(x+18,y+17),value,16,MINT,True,True,anchor='mm')
        if t>.45:
            for a,b in links: d.line((positions[a],positions[b]),fill=LINE,width=2)
            count=int(min(1,(t-.45)/.5)*len(positions))+1
            for label,xy in list(positions.items())[:count]:
                color=PEACH if label[0]=='d' else MINT; d.ellipse((xy[0]-17,xy[1]-17,xy[0]+17,xy[1]+17),fill=PANEL,outline=color,width=2); text(d,xy,label,15,color,True,True,anchor='mm')
        note(d,'EXPECTED COST',out['result'],'Probabilities include successful keys p and failed-search gaps q. Dummy leaves contribute their depth too. This convention gives the canonical cost 2.75.')
        text(d,(54,484),'k1..k5 = 10, 20, 30, 40, 50',16,MUTED,False,True)
        frames.append(im)
    return frames
def collatz_story(out):
    values=out['trajectory']; peak=max(values); logs=[math.log2(x) for x in values]; frames=[]
    points=[(64+i/(len(values)-1)*570,448-x/math.log2(peak)*220) for i,x in enumerate(logs)]
    for f in range(52):
        t=f/51; upto=max(1,int(t*(len(values)-1)))
        im,d=base(9,t,f'n = 27 | step {upto} | current value {values[upto]}',upto)
        text(d,(54,177),'A RISE BEFORE THE DESCENT',14,MUTED,True,True)
        for level in range(0,15,3):
            y=448-level/math.log2(peak)*220; d.line((64,y,634,y),fill=LINE); text(d,(55,y),f'2^{level}',11,MUTED,False,True,anchor='ra')
        d.line(points[:upto+1],fill=MINT,width=3); glow(im,points[upto],PEACH)
        text(d,(64,461),'0',13,MUTED); text(d,(634,461),f'{len(values)-1} steps',13,MUTED,anchor='ra')
        note(d,'TRAJECTORY PEAK',out['peak'],f'27 reaches 1 in {out["steps"]} transitions. This trajectory is an observation. Overflow and step limits are explicit outcomes; no convergence proof is claimed.')
        text(d,(54,490),'value axis: log2   /   no steps skipped in the C trace',13,MUTED,False,True)
        frames.append(im)
    return frames
def banner():
    frames=[]
    for f in range(48):
        t=f/48; im=Image.new('RGB',(1200,450),BG); d=ImageDraw.Draw(im)
        for x in range(20,1200,26):
            for y in range(20,450,26): d.ellipse((x,y,x+1,y+1),fill='#183139')
        text(d,(48,35),'DAA LABORATORY / 08',17,MINT,True,True)
        text(d,(48,92),'Every state',64,INK,True); text(d,(48,167),'has a story.',64,INK,True)
        text(d,(50,268),'OPTIMIZE. RECONSTRUCT. EXPLORE.',20,MUTED,False,True)
        for x,label in [(48,'9 C SOLUTIONS'),(248,'9 VISUAL STORIES'),(500,'CHECKED EVIDENCE')]:
            roundbox(d,(x,321,x+180 if x==48 else x+232,363),fill=PANEL,r=20); text(d,(x+16,332),label,14,MINT,True,True)
        center=(934,214)
        for r in [76,120,162]: d.ellipse((center[0]-r,center[1]-r,center[0]+r,center[1]+r),outline=LINE,width=1)
        coords=[]
        for i in range(9):
            ang=2*math.pi*i/9-math.pi/2; xy=(center[0]+145*math.cos(ang),center[1]+145*math.sin(ang));coords.append(xy)
            d.line((xy,center),fill=LINE,width=1)
        active=int(t*9)
        for i,xy in enumerate(coords):
            d.ellipse((xy[0]-23,xy[1]-23,xy[0]+23,xy[1]+23),fill='#194536' if i==active else PANEL,outline=MINT if i==active else LINE,width=2)
            text(d,xy,f'{i+1:02}',18,MINT if i==active else MUTED,True,True,anchor='mm')
        d.ellipse((876,156,992,272),fill=PANEL,outline=PEACH,width=2); text(d,(934,212),'DP',40,INK,True,True,anchor='mm')
        text(d,(48,408),'SATYAM DHAL / B425050 / 29 SEPTEMBER 2026',14,MUTED,False,True)
        frames.append(im)
    save(frames,ROOT/'assets'/'lab8_banner.gif',100)
def course_journey():
    themes=['Foundations','Structural trade-offs','Divide & prove','Sort & sweep',
            'Select & partition','Transform & optimize','Puzzle & search','State & story']
    counts=[6,3,6,6,4,8,7,9]; frames=[]
    for f in range(48):
        active=min(7,f//6); im=Image.new('RGB',(1200,400),BG); d=ImageDraw.Draw(im)
        text(d,(48,30),'ONE CONTINUOUS ALGORITHM NOTEBOOK',14,MINT,True,True)
        text(d,(48,62),'From first principles to optimal choices.',34,INK,True)
        for i,theme in enumerate(themes):
            x=48+(i%4)*279; y=124+(i//4)*116
            roundbox(d,(x,y,x+260,y+99),fill='#194536' if i==active else PANEL,outline=MINT if i==active else LINE,r=15)
            text(d,(x+17,y+13),f'LAB {i+1:02} / {counts[i]} QUESTIONS',13,MINT,True,True)
            text(d,(x+17,y+43),theme,19,INK,True)
            d.line((x+17,y+80,x+243,y+80),fill=LINE)
            if i<=active: d.line((x+17,y+80,x+243,y+80),fill=MINT,width=2)
        text(d,(48,365),'08 LABS / 49 QUESTIONS / C17 + PROOFS + CHECKED VISUAL EVIDENCE',14,MUTED,False,True)
        frames.append(im)
    save(frames,ROOT.parent/'assets'/'course_journey_lab8.gif',200)
def main():
    for q in range(1,10):
        out=trace(q)
        if q in (1,2): frames=coin_story(q,out)
        elif q in (3,6): frames=string_story(q,out)
        elif q in (4,5): frames=array_story(q,out)
        elif q==7: frames=rod_story(out)
        elif q==8: frames=obst_story(out)
        else: frames=collatz_story(out)
        save(frames,ROOT/f'Q-{q}'/f'q{q}_walkthrough.gif'); print(f'Rendered Q{q}: {len(frames)} frames from validated C output.')
    banner()
    course_journey()
    gallery=[]
    for q in range(1,10):
        im=Image.open(ROOT/f'Q-{q}'/f'q{q}_walkthrough.png').convert('RGB')
        gallery.extend([im]*4)
    save(gallery,ROOT/'assets'/'nine_stories.gif',500)
    contact=Image.new('RGB',(1440,900),BG)
    for q in range(1,10):
        im=Image.open(ROOT/f'Q-{q}'/f'q{q}_walkthrough.png'); im.thumbnail((480,300))
        contact.paste(im,(((q-1)%3)*480,((q-1)//3)*300))
    contact.save(ROOT/'assets'/'lab8_gallery.png')
    frames=[]
    for f in range(32):
        im=Image.new('RGB',(1200,42),BG);d=ImageDraw.Draw(im)
        d.line((12,20,1188,20),fill=LINE,width=1)
        for i in range(9):
            x=40+i*140; d.ellipse((x-4,16,x+4,24),fill=MINT if i==int(f/32*9) else LINE)
        frames.append(im)
    save(frames,ROOT/'assets'/'animated_divider.gif',100)
if __name__=='__main__': main()
