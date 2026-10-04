"""Independent oracles, witness replay, negative input tests, and measured evidence."""
import bisect
from collections import deque
from functools import lru_cache
import itertools
import json
import math
import random
import subprocess
from lab_config import ROOT, NAMES, SAMPLES, EVENTS
from build import build, executable
MAX = 2**64-1
RUNS = 0
def run(q, data, ok=True):
    global RUNS
    RUNS += 1
    p = subprocess.run([str(executable(q)), '--json'], input=data, text=True, capture_output=True, timeout=20)
    if not ok:
        assert p.returncode == 2 and p.stderr, (q, data, p)
        return
    assert p.returncode == 0, (q, data[:100], p.stderr)
    return json.loads(p.stdout)
def subsequence(s, a):
    it = iter(a)
    return all(any(x == y for y in it) for x in s)
def subset_oracle(a, summed=False):
    best = 0
    for mask in range(1 << len(a)):
        s = [x for i,x in enumerate(a) if mask >> i & 1]
        if all(x < y for x,y in zip(s,s[1:])):
            best = max(best, sum(s) if summed else len(s))
    return best
def msis_fenwick(a):
    values = sorted(set(a)); tree = [0]*(len(values)+1); best=0
    for x in a:
        rank = bisect.bisect_left(values,x)+1; k=rank-1; prev=0
        while k: prev=max(prev,tree[k]); k-=k&-k
        value=prev+x; k=rank; best=max(best,value)
        while k<len(tree): tree[k]=max(tree[k],value); k+=k&-k
    return best
def string_oracle(a,b,edit=False):
    row = list(range(len(b)+1)) if edit else [0]*(len(b)+1)
    for i,x in enumerate(a,1):
        nxt = [i if edit else 0]
        for j,y in enumerate(b,1):
            nxt.append(min(row[j]+1,nxt[-1]+1,row[j-1]+(x!=y)) if edit else
                       (row[j-1]+1 if x==y else max(row[j],nxt[-1])))
        row=nxt
    return row[-1]
def collatz(n, cap):
    values=[n]
    while values[-1]!=1 and len(values)-1<cap:
        x=values[-1]; nxt=3*x+1 if x%2 else x//2
        if nxt>MAX: break
        values.append(nxt)
    x=values[-1]
    status='reached_1' if x==1 else 'step_limit' if len(values)-1==cap else 'overflow'
    return values,status
def check(q,data, exhaustive=True):
    out=run(q,data); tokens=data.split()
    if q in (1,2):
        c,v=map(int,tokens[:2]); coins=list(map(int,tokens[2:])); assert c==len(coins)
        if q==1:
            distance={0:0}; queue=deque([0])
            while queue:
                x=queue.popleft()
                for coin in coins:
                    y=x+coin
                    if y<=v and y not in distance: distance[y]=distance[x]+1; queue.append(y)
            assert out['result']==distance.get(v,-1)
            if out['result']>=0: assert sum(out['coins'])==v and len(out['coins'])==out['result'] and set(out['coins'])<=set(coins)
            else: assert not out['coins']
            assert out['work']==c*v
        else:
            @lru_cache(None)
            def ways(i,left):
                if i==c: return int(left==0)
                return sum(ways(i+1,left-k*coins[i]) for k in range(left//coins[i]+1))
            assert out['result']==ways(0,v)
            assert out['work']==sum(max(0,v-x+1) for x in coins)
    elif q in (3,6):
        a,b=data.splitlines()[:2]; assert out['result']==string_oracle(a,b,q==6)
        assert out['work']==len(a)*len(b)
        if q==3:
            s=out['subsequence']; assert len(s)==out['result'] and subsequence(s,a) and subsequence(s,b)
            if exhaustive and len(a)<=10:
                assert out['result']==max((len(s) for k in range(len(a)+1) for s in itertools.combinations(a,k) if subsequence(s,b)),default=0)
        else:
            source=[]; target=[]; edits=0
            for op,x,y in zip(out['ops'],out['from'],out['to']):
                assert op in 'MSDI'
                if op!='I': source.append(x)
                if op!='D': target.append(y)
                if op=='M': assert x==y
                else: edits+=1
            assert ''.join(source)==a and ''.join(target)==b and edits==out['result']
            assert len(out['ops'])==len(out['from'])==len(out['to'])
    elif q in (4,5):
        n=int(tokens[0]); a=list(map(int,tokens[1:])); assert n==len(a)
        if exhaustive and n<=12: expected=subset_oracle(a,q==5)
        elif q==4:
            tails=[]
            for x in a:
                k=bisect.bisect_left(tails,x)
                if k==len(tails): tails.append(x)
                else: tails[k]=x
            expected=len(tails)
        else: expected=msis_fenwick(a)
        s=out['subsequence']; assert subsequence(s,a) and all(x<y for x,y in zip(s,s[1:]))
        assert (sum(s) if q==5 else len(s))==out['result']==expected
        assert out['work']==n*(n-1)//2
    elif q==7:
        n=int(tokens[0]); prices=[0]+list(map(int,tokens[1:])); assert len(prices)==n+1
        @lru_cache(None)
        def revenue(left):
            return max((prices[k]+revenue(left-k) for k in range(1,left+1)),default=0)
        expected=revenue(n)
        if exhaustive and n<=12:
            def compositions(left):
                if left==0: yield 0
                for k in range(1,left+1):
                    for rest in compositions(left-k): yield prices[k]+rest
            assert expected==max(compositions(n))
        pieces=out['pieces']; assert sum(pieces)==n and all(1<=x<=n for x in pieces)
        assert sum(prices[k] for k in pieces)==out['result']==expected
        assert out['work']==n*(n+1)//2
    elif q==8:
        n=int(tokens[0]); p=list(map(float,tokens[1+n:1+2*n])); d=list(map(float,tokens[1+2*n:])); roots=out['roots']
        @lru_cache(None)
        def cost(i,j):
            if i==j: return d[i]
            weight=sum(p[i:j])+sum(d[i:j+1])
            return min(cost(i,k)+cost(k+1,j)+weight for k in range(i,j))
        assert math.isclose(out['result'],cost(0,n),rel_tol=1e-9,abs_tol=1e-9)
        def replay(i,j,depth):
            if i==j: return d[i]*(depth+1)
            k=roots[i][j]; assert i<=k<j
            return p[k]*(depth+1)+replay(i,k,depth+1)+replay(k+1,j,depth+1)
        assert math.isclose(out['result'],replay(0,n,0),rel_tol=1e-9,abs_tol=1e-9)
        if exhaustive and n<=6:
            # Enumerate tree shapes, then charge every key and dummy by its depth.
            def all_costs(i,j,depth):
                if i==j: return [d[i]*(depth+1)]
                return [p[k]*(depth+1)+l+r for k in range(i,j)
                        for l in all_costs(i,k,depth+1) for r in all_costs(k+1,j,depth+1)]
            assert math.isclose(out['result'],min(all_costs(0,n,0)),abs_tol=1e-9)
        assert out['work']==n*(n+1)*(n+2)//6
    elif q==9:
        start,a,b,cap=map(int,tokens); seq,status=collatz(start,cap)
        assert seq==out['trajectory'] and status==out['status'] and max(seq)==out['peak'] and len(seq)-1==out['steps']
        work=0; counters={'reached_1':0,'overflow':0,'step_limit':0}; longest=0; champion=a
        for row,n in zip(out['interval'],range(a,b+1)):
            seq,status=collatz(n,cap); steps=len(seq)-1
            assert row==dict(start=n,steps=steps,peak=max(seq),status=status)
            work+=steps; counters[status]+=1
            if status=='reached_1' and steps>longest: longest=steps; champion=n
        assert len(out['interval'])==b-a+1 and out['work']==work
        assert (out['completed'],out['overflow'],out['limited'])==tuple(counters[x] for x in ('reached_1','overflow','step_limit'))
        assert out['longest']==longest and out['champion']==champion
    return out
def tests():
    rng=random.Random(42505008)
    for q,data in enumerate(SAMPLES,1): check(q,data)
    for coins in ([],[2],[1,3,4],[2,5],[7,11]):
        for v in range(16):
            data=f'{len(coins)} {v}\n'+ ' '.join(map(str,coins))+'\n'
            check(1,data); check(2,data)
    for _ in range(80):
        coins=rng.sample(range(1,12),rng.randrange(1,6)); v=rng.randrange(35)
        data=f'{len(coins)} {v}\n'+ ' '.join(map(str,coins))+'\n'; check(1,data); check(2,data)
        a=''.join(rng.choices('ABC',k=rng.randrange(9))); b=''.join(rng.choices('ABC',k=rng.randrange(9)))
        check(3,a+'\n'+b+'\n'); check(6,a+'\n'+b+'\n')
        a=[rng.randrange(-10,11) for _ in range(rng.randrange(12))]; check(4,f'{len(a)}\n'+ ' '.join(map(str,a))+'\n')
        a=[rng.randrange(1,31) for _ in range(rng.randrange(12))]; check(5,f'{len(a)}\n'+ ' '.join(map(str,a))+'\n')
        p=[rng.randrange(-10,31) for _ in range(rng.randrange(11))]; check(7,f'{len(p)}\n'+ ' '.join(map(str,p))+'\n')
    for _ in range(45):
        n=rng.randrange(7); weights=[rng.randrange(10) for _ in range(2*n+1)]
        if sum(weights)==0: weights[0]=1
        probs=[x/sum(weights) for x in weights]
        data=f'{n}\n'+ ' '.join(str(i*10) for i in range(n))+'\n'+ ' '.join(map(str,probs[:n]))+'\n'+ ' '.join(map(str,probs[n:]))+'\n'
        check(8,data)
    for n in [1,2,3,27,97,871,6171,MAX,MAX-1,(MAX-1)//3+2]+[rng.randrange(1,1000000) for _ in range(60)]:
        check(9,f'{n} 1 25 {rng.choice([1,10,10000])}\n')
    for q in (3,6):
        for a,b in [('', ''), ('a-b','ab'), ('a b','ab'), ('"\\','\\"')]: check(q,a+'\n'+b+'\n')
    check(4,'4\n-9223372036854775808 0 0 9223372036854775807\n')
    check(5,f'1\n{MAX}\n'); check(7,'2\n-5 -100\n'); check(8,'0\n1\n')
    invalid={1:['2 6\n0 3\n','2 6\n3 3\n','1 -1\n2\n'],
             2:['2 4\n1 1\n','100 1000\n'+' '.join(map(str,range(1,101)))+'\n'],
             3:['ABC\n','x\n\x01\n'],4:['2\n1\n','-1\n'],
             5:['1\n0\n',f'2\n1 {MAX}\n'],6:['one\n'],
             7:['1\n9223372036854775808\n'],8:['2\n2 1\n.2 .2\n.2 .2 .2\n','1\n1\n0.3\n0.1 0.1\n','0\nnan\n'],
             9:['0 1 10 100\n','1 10 1 100\n','1 1 10 0\n',f'{MAX+1} 1 1 1\n','-1 1 1 1\n']}
    for q,cases in invalid.items():
        for data in cases: run(q,data,False)
    for q,data in enumerate(SAMPLES,1): run(q,data+'EXTRA\n',False)
def evidence():
    rng=random.Random(8)
    for q in range(1,10):
        directory=ROOT/f'Q-{q}'; sample=SAMPLES[q-1]
        (directory/f'q{q}_sample_input.txt').write_text(sample,encoding='utf-8')
        out=check(q,sample)
        (directory/f'q{q}_sample_trace.json').write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
        human=subprocess.run([str(executable(q))],input=sample,text=True,capture_output=True,check=True)
        (directory/f'q{q}_sample_output.txt').write_text(human.stdout,encoding='utf-8')
        rows=[]
        scales=[8,12,16,24,32,48,64,96,128,192,256]
        if q==8: scales=[4,6,8,12,16,24,32,48,64,80]
        if q==9: scales=[16,24,32,48,64,96,128,192,256,512,1024]
        for n in scales:
            if q in (1,2): data=f'3 {n}\n1 3 4\n'; ref=3*n
            elif q in (3,6):
                a=''.join(rng.choices('ACGT',k=n)); b=''.join(rng.choices('ACGT',k=n)); data=a+'\n'+b+'\n'; ref=n*n
            elif q in (4,5): data=f'{n}\n'+' '.join(str(rng.randrange(1,10000)) for _ in range(n))+'\n'; ref=n*(n-1)//2
            elif q==7: data=f'{n}\n'+' '.join(str(rng.randrange(1,1000)) for _ in range(n))+'\n'; ref=n*(n+1)//2
            elif q==8:
                probs=[1/(2*n+1)]*(2*n+1); data=f'{n}\n'+' '.join(map(str,range(n)))+'\n'+' '.join(map(str,probs[:n]))+'\n'+' '.join(map(str,probs[n:]))+'\n'; ref=n*(n+1)*(n+2)//6
            else: data=f'27 1 {n} 10000\n'; ref=None
            result=check(q,data,False); ref=result['work'] if ref is None else ref
            rows.append((n,result['work'],ref,1))
        dat='# scale measured reference validated\n'+'\n'.join(' '.join(map(str,row)) for row in rows)+'\n'
        (directory/f'q{q}_experimental_data.dat').write_text(dat,encoding='utf-8')
        (directory/f'q{q}_experiment_output.txt').write_text(f'Measured event: {EVENTS[q-1]}\nAll {len(rows)} rows oracle-checked before writing.\n'+dat,encoding='utf-8')
        subprocess.run([str(executable(q,True))],cwd=directory,check=True)
if __name__=='__main__':
    build(); tests(); evidence()
    report=f'''# Lab 08 verification

Local run: 4 October 2026. Compiler: GCC, strict C17 (`-Wall -Wextra -Wpedantic -Werror`).

- 18/18 C sources built successfully.
- {RUNS} program executions passed: independent oracles, reconstructed witness checks, malformed inputs, numeric boundaries, and deterministic scaling experiments.
- Q1: breadth-first shortest paths; Q2: coin-multiplicity enumeration.
- Q3: subsequence enumeration and a rolling-row length oracle.
- Q4: exhaustive subsequences and patience sorting; Q5: exhaustive subsequences and a Fenwick maximum oracle.
- Q6: rolling-row distance plus complete forward edit-script replay.
- Q7: compositions and memoized revenue, with exact piece-length/revenue replay (including negative prices).
- Q8: exhaustive BST shapes on small cases, recursive interval oracle, independent depth-weighted tree cost, and the canonical 2.75 instance.
- Q9: Python arbitrary-precision trajectories enforce the C uint64_t boundary and reproduce each interval row, overflow, and step-limit status.
- Operation counters checked against exact counts where a closed form exists.
- Every chart point was accepted only after its corresponding oracle passed.

Animation state values and reconstructed answers come from the validated C sample traces. Q2's intermediate coin-stage counts are also checked against the C result during generation. Collatz plots report observations on the specified finite interval, with no claimed bound for arbitrary starts.

Run `python tools/validate.py` to rebuild and repeat this report. Run `python tools/generate_visuals.py` to refresh the visual assets (requires Pillow).
'''
    (ROOT/'VERIFICATION.md').write_text(report,encoding='utf-8'); print(f'PASS: {RUNS} oracle-checked executions.')
