"""Independent optimality, witness, malformed-input and scaling checks."""
import collections, functools, heapq, itertools, json, math, random, subprocess
from pathlib import Path
from lab_config import ROOT, NAMES, SAMPLES, executable
R=random.Random(42505009)
executions=0

def run(q,text,success=True):
    global executions
    p=subprocess.run([str(executable(q)),'--json'],input=text,text=True,
                     capture_output=True,timeout=30)
    executions+=1
    if not success:
        assert p.returncode and p.stderr,(q,text,p.stdout)
        return
    assert p.returncode==0,(q,text,p.stderr)
    return json.loads(p.stdout)

def array_input(a): return str(len(a))+'\n'+' '.join(map(str,a))+'\n'
def prefix_free(codes):
    assert len(set(codes))==len(codes)
    assert all(not b.startswith(a) for i,a in enumerate(codes)
               for j,b in enumerate(codes) if i!=j)

def decay_case(items,W):
    text=f'{len(items)} {W}\n'+''.join(f'{v} {w} {rate}\n' for v,w,rate in items)
    out=run(1,text); s=out['schedule']; used=0; value=0
    assert [row['rate'] for row in s]==sorted((x[2] for x in items),reverse=True)
    for row in s:
        v,w,rate=items[row['id']-1]; x=row['amount']
        assert -1e-7<=x<=w+1e-7 and abs(row['start']-used)<1e-7
        value+=v/w*x-rate*x*(used+x/2); used+=x
    assert used<=W+1e-7 and abs(value-out['value'])<1e-6*max(1,abs(value))
    # Concavity gives F(opt) <= F(x) + max_y gradient(x).(y-x).
    # The independent linear oracle is ordinary fractional knapsack.
    gradients=[]
    for i,row in enumerate(s):
        v,w,rate=items[row['id']-1]
        g=v/w-rate*(row['start']+row['amount'])
        g-=sum(r['rate']*r['amount'] for r in s[i+1:])
        gradients.append((g,w,row['amount']))
    remaining=W; upper=0; current=sum(g*x for g,_,x in gradients)
    for g,w,_ in sorted(gradients,reverse=True):
        if g<=0: break
        x=min(w,remaining); upper+=g*x; remaining-=x
    gap=upper-current
    assert gap<=1e-5*max(1,abs(value)),('Q1 optimality certificate',items,W,gap)
    return out

def huffman_case(pairs):
    out=run(2,str(len(pairs))+'\n'+''.join(f'{s} {f}\n' for s,f in pairs))
    h=[f for _,f in pairs]; heapq.heapify(h); cost=0
    while len(h)>1:
        x=heapq.heappop(h)+heapq.heappop(h); cost+=x; heapq.heappush(h,x)
    if len(pairs)==1: cost=pairs[0][1]
    codes=out['codes']; assert out['cost']==cost
    assert [(c['length'],c['symbol']) for c in codes]==sorted((c['length'],c['symbol']) for c in codes)
    prefix_free([c['code'] for c in codes])
    assert sum(c['frequency']*len(c['code']) for c in codes)==cost
    number=0; previous=codes[0]['length']
    for i,c in enumerate(codes):
        if i: number=(number+1)<<(c['length']-previous)
        assert c['code']==format(number,f'0{c["length"]}b'); previous=c['length']
    return out

def fuel_case(stations,D,F):
    out=run(3,f'{len(stations)} {D} {F}\n'+''.join(f'{d} {f}\n' for d,f in stations))
    dp=[-1]*(len(stations)+1); dp[0]=F
    for d,f in sorted(stations):
        for k in range(len(stations)-1,-1,-1):
            if dp[k]>=d: dp[k+1]=max(dp[k+1],dp[k]+f)
    answer=next((k for k,r in enumerate(dp) if r>=D),-1)
    assert out['stops']==answer
    if answer>=0:
        reach=F; position=-1
        assert len(out['stations'])==len(set(out['stations']))==answer
        for i in out['stations']:
            d,f=stations[i-1]; assert position<=d<=reach; reach+=f; position=d
        assert reach>=D
    return out

@functools.lru_cache(None)
def merge_opt(a):
    if len(a)<2: return 0
    return min(a[i]+a[j]+merge_opt(tuple(sorted(a[i]+a[j] if k==i else a[k]
                   for k in range(len(a)) if k!=j)))
               for i in range(len(a)) for j in range(i+1,len(a)))

def sticks_case(a):
    out=run(4,array_input(a)); assert out['cost']==merge_opt(tuple(sorted(a)))
    bag=collections.Counter(a); cost=0
    for x,y,z in out['merges']:
        assert z==x+y and bag[x]>0; bag[x]-=1
        assert bag[y]>0; bag[y]-=1; bag[z]+=1; cost+=z
    assert cost==out['cost'] and sum(bag.values())==min(1,len(a))
    return out

def candy_case(a):
    out=run(5,array_input(a)); expected=[1]*len(a)
    changed=True
    while changed:
        changed=False
        for i in range(len(a)):
            for j in [i-1,i+1]:
                if 0<=j<len(a) and a[i]>a[j] and expected[i]<=expected[j]:
                    expected[i]=expected[j]+1; changed=True
    assert out['candies']==expected and out['total']==sum(expected)
    assert out['work']==2*max(0,len(a)-1)
    return out

def string_case(s,k):
    out=run(6,s+'\n'+str(k)+'\n'); counts=collections.Counter(s)
    maximum=max(counts.values(),default=0)
    tied=sum(v==maximum for v in counts.values())
    possible=not s or (maximum-1)*max(k,1)+tied<=len(s)
    assert out['possible']==possible,(s,k,out)
    if possible:
        assert collections.Counter(out['string'])==counts
        last={}
        for i,c in enumerate(out['string']):
            assert c not in last or i-last[c]>=k; last[c]=i
    else: assert out['string']==''
    return out

def choices(x):
    x=2*x if x%2 else x; result=[x]
    while x%2==0: x//=2; result.append(x)
    return result

def deviation_case(a):
    out=run(7,array_input(a)); domains=[choices(x) for x in a]
    optimal=min(max(t)-min(t) for t in itertools.product(*domains))
    assert out['deviation']==optimal
    assert all(x in domain for x,domain in zip(out['values'],domains))
    assert max(out['values'])-min(out['values'])==optimal
    return out

def rooms_case(meetings):
    text=str(len(meetings))+'\n'+''.join(f'{s} {e}\n' for s,e in meetings)
    out=run(8,text); events=[]
    for s,e in meetings:
        if s!=e: events.extend([(s,1),(e,-1)])
    current=best=0
    for _,delta in sorted(events): current+=delta; best=max(best,current)
    assert out['rooms']==best
    for i,(s,e) in enumerate(meetings):
        room=out['assignment'][i]
        if s==e: assert room==0; continue
        assert 1<=room<=best
        for j in range(i):
            if out['assignment'][j]==room:
                a,b=meetings[j]; assert e<=a or b<=s
    return out

def alphabetic_opt(weights):
    prefix=[0]
    for w in weights: prefix.append(prefix[-1]+w)
    @functools.lru_cache(None)
    def dp(i,j):
        if i==j: return 0
        return prefix[j+1]-prefix[i]+min(dp(i,k)+dp(k+1,j) for k in range(i,j))
    return dp(0,len(weights)-1)

def alphabetic_case(a):
    out=run(9,array_input(a)); assert out['cost']==alphabetic_opt(a),(a,out)
    codes=[c['code'] for c in out['leaves']]; prefix_free(codes)
    assert codes==sorted(codes) and len(codes)==len(a)
    assert out['cost']==sum(w*len(code) for w,code in zip(a,codes))
    assert out['cost']==sum(m[2] for m in out['merges'])
    return out

def overlap(a,b):
    return next((k for k in range(min(len(a),len(b)),0,-1) if a.endswith(b[:k])),0)

def superstring_case(strings):
    out=run(10,str(len(strings))+'\n'+'\n'.join(strings)+'\n')
    reduced=list(dict.fromkeys(strings))
    reduced=[s for s in reduced if not any(s!=t and s in t for t in reduced)]
    best=math.inf
    for permutation in itertools.permutations(reduced):
        text=permutation[0]
        for word in permutation[1:]: text+=word[overlap(text,word):]
        best=min(best,len(text))
    assert out['optimal']==best==len(out['exact'])
    assert all(s in out['greedy'] and s in out['exact'] for s in strings)
    assert len(out['greedy'])>=best
    greedy=reduced[:]
    while len(greedy)>1:
        pairs=[(overlap(a,b),i,j) for i,a in enumerate(greedy)
               for j,b in enumerate(greedy) if i!=j]
        k,i,j=min(pairs,key=lambda x:(-x[0],x[1],x[2]))
        greedy[i]+=greedy[j][k:]; del greedy[j]
    assert greedy[0]==out['greedy']
    return out

def main():
    for _ in range(36):
        n=R.randint(1,6); decay_case([(R.randint(1,20),R.randint(1,5),R.randint(1,5))
                                    for _ in range(n)],R.randint(0,12))
    for _ in range(60):
        huffman_case([(chr(65+i),R.randint(1,50)) for i in range(R.randint(1,12))])
        fuel_case([(R.randint(0,60),R.randint(0,30)) for _ in range(R.randint(0,10))],60,R.randint(0,60))
        sticks_case([R.randint(1,12) for _ in range(R.randint(0,6))])
        candy_case([R.randint(-2,5) for _ in range(R.randint(0,20))])
        string_case(''.join(R.choice('abcde') for _ in range(R.randint(0,24))),R.randint(0,6))
        deviation_case([R.randint(1,20) for _ in range(R.randint(1,5))])
        rooms_case([(s,s+R.randint(0,15)) for s in [R.randint(0,30) for _ in range(R.randint(0,15))]])
        superstring_case([''.join(R.choice('abc') for _ in range(R.randint(1,5)))
                          for _ in range(R.randint(1,6))])
    for _ in range(250): alphabetic_case([R.randint(1,4) for _ in range(R.randint(1,20))])
    for n in range(1,9): alphabetic_case([1]*n)
    for items,W in [([(10,1,1),(9,1,2)],2),([(0,1,1)],0),([(5,2,1),(5,2,1)],2)]:
        decay_case(items,W)
    bad={1:['0 1\n','1 1\n3 0 2\n','1 1\n3 1 nan\n'],
         2:['2\nA 1\nA 2\n','1\nA 0\n'],3:['1 5 1\n6 1\n','-1 1 1\n'],
         4:['1\n0\n'],5:['1\n1 extra\n'],6:['ABC\n3\n','abc\n-1\n'],
         7:['0\n','1\n0\n'],8:['1\n5 4\n'],9:['0\n','1\n-1\n'],
         10:['0\n','1\nA\n']}
    for q,texts in bad.items():
        for text in texts: run(q,text,False)
    for q,sample in enumerate(SAMPLES,1):
        out=run(q,sample)
        folder=ROOT/f'Q-{q}'
        (folder/f'q{q}_sample_trace.json').write_text(json.dumps(out,indent=2)+'\n')
        result=subprocess.run([str(executable(q))],input=sample,text=True,capture_output=True,check=True)
        (folder/f'q{q}_sample_output.txt').write_text(result.stdout)
    report={'seed':42505009,'strict_sources':10,'oracle_checked_executions':executions,
            'result':'PASS','q1_model':'continuous unit-rate integrated consumption',
            'q1_validation':'concavity-based global optimality gap certificate',
            'q9_validation':'independent interval DP, ordered prefix code and weighted cost'}
    (ROOT/'verification.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))

if __name__=='__main__': main()
