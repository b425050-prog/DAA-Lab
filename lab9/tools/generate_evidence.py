"""Run deterministic scaling cases; plot actual counters, never invented timings."""
import collections, functools, heapq, json, random
from lab_config import ROOT, EVENTS
import validate as v

def deviation_large(a):
    out=v.run(7,v.array_input(a)); domains=[sorted(v.choices(x)) for x in a]
    h=[(d[0],i,0) for i,d in enumerate(domains)]; heapq.heapify(h)
    high=max(x[0] for x in h); best=high-h[0][0]
    while True:
        low,i,j=heapq.heappop(h); best=min(best,high-low)
        if j+1==len(domains[i]): break
        x=domains[i][j+1]; high=max(high,x); heapq.heappush(h,(x,i,j+1))
    assert out['deviation']==best
    assert max(out['values'])-min(out['values'])==best
    assert all(x in d for x,d in zip(out['values'],domains))
    return out

def sticks_large(a):
    out=v.run(4,v.array_input(a)); h=a[:]; heapq.heapify(h); total=0
    while len(h)>1:
        x=heapq.heappop(h)+heapq.heappop(h); total+=x; heapq.heappush(h,x)
    assert out['cost']==total
    return out

def superstring_large(strings):
    out=v.run(10,str(len(strings))+'\n'+'\n'.join(strings)+'\n')
    n=len(strings); ov=[[v.overlap(a,b) for b in strings] for a in strings]
    @functools.lru_cache(None)
    def dp(mask,last):
        rest=mask^(1<<last)
        if not rest: return len(strings[last])
        return min(dp(rest,j)+len(strings[last])-ov[j][last]
                   for j in range(n) if rest>>j&1)
    assert out['optimal']==min(dp((1<<n)-1,j) for j in range(n))
    assert all(s in out['exact'] and s in out['greedy'] for s in strings)
    return out

def main():
    records=[]; rng=random.Random(900425050)
    for q in range(1,11):
        sizes=(list(range(1,9)) if q==1 else [4,8,16,32,48,64] if q==2
               else [4,8,16,32,64,96,128] if q==9
               else list(range(2,13)) if q==10
               else [16,32,64,128,256,512,1024,2048])
        rows=[]
        for n in sizes:
            if q==1:
                items=[(10+i%5,2+i%3,1+i%4) for i in range(n)]
                out=v.decay_case(items,2*n/3)
                inp=f'{n} {2*n/3}\n'+''.join(f'{a} {b} {c}\n' for a,b,c in items)
            elif q==2:
                symbols=[chr(i) for i in range(33,127) if chr(i) not in '\\"']
                pairs=[(symbols[i],1+(i*17)%101) for i in range(n)]
                out=v.huffman_case(pairs); inp=f'{n}\n'+''.join(f'{s} {f}\n' for s,f in pairs)
            elif q==3:
                a=[(10*(i+1),15) for i in range(n)]; D=10*(n+1)
                out=v.fuel_case(a,D,10); inp=f'{n} {D} 10\n'+''.join(f'{d} {f}\n' for d,f in a)
            elif q==4:
                a=[1+(i*37)%1000 for i in range(n)]; out=sticks_large(a); inp=v.array_input(a)
            elif q==5:
                a=[i%7 for i in range(n)]; out=v.candy_case(a); inp=v.array_input(a)
            elif q==6:
                s=('abcde'*((n+4)//5))[:n]; out=v.string_case(s,3); inp=s+'\n3\n'
            elif q==7:
                a=[1+(i*29)%1000 for i in range(n)]; out=deviation_large(a); inp=v.array_input(a)
            elif q==8:
                a=[(3*i,3*i+10) for i in range(n)]; out=v.rooms_case(a)
                inp=f'{n}\n'+''.join(f'{s} {e}\n' for s,e in a)
            elif q==9:
                a=[1+(i*11)%23 for i in range(n)]; out=v.alphabetic_case(a); inp=v.array_input(a)
            else:
                # Unique leading letters exclude containment and duplicate reduction.
                a=[chr(97+i)+''.join(rng.choice('xyz') for _ in range(7)) for i in range(n)]
                out=superstring_large(a); inp=f'{n}\n'+'\n'.join(a)+'\n'
            rows.append((n,out['work'],out.get('dp_work',0)))
            records.append({'q':q,'size':n,'input':inp,'trace':out,'oracle':'PASS'})
        folder=ROOT/f'Q-{q}'
        (folder/f'q{q}_data.dat').write_text('# size work exact_dp_transitions\n'+'\n'.join(' '.join(map(str,r)) for r in rows)+'\n')
    (ROOT/'scaling_evidence.json').write_text(json.dumps({'seed':900425050,'executions':v.executions,'events':EVENTS,'cases':records},indent=2)+'\n')
    print(f'{v.executions} independent scaling checks passed; 10 measured datasets written.')

if __name__=='__main__': main()
