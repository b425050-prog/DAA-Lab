"""Independent BFS/flood-fill and exhaustive-swap checks. No DSU oracle."""
import collections, itertools, json, random, re, subprocess
from lab_config import ROOT,NAMES,SAMPLES,executable
R=random.Random(42505010); executions=0

def run(q,text,valid=True):
    global executions
    p=subprocess.run([str(executable(q))],input=text,text=True,capture_output=True,timeout=30)
    executions+=1
    if not valid:
        assert p.returncode and p.stderr,(q,text,p.stdout); return
    assert p.returncode==0,(q,text,p.stderr)
    out={'stdout':p.stdout,'work':int(re.search(r'Work: (\d+)',p.stdout)[1])}
    if q==1:
        out['accepted']=['REJECTED' not in line for line in p.stdout.splitlines() if line.startswith('Edge ')]
        out['colours']=list(map(int,re.search(r'Final colours:([^\n]*)',p.stdout)[1].split()))
    elif q==2: out['counts']=list(map(int,re.findall(r'islands=(\d+)',p.stdout)))
    elif q in [3,4]:
        out['maxima']=[None if s=='NONE' else int(s) for s in re.findall(r'maximum=(NONE|-?\d+)',p.stdout)]
    else: out['permutation']=list(map(int,re.search(r'Smallest permutation:([^\n]*)',p.stdout)[1].split()))
    return out

def components(n,edges,active=None):
    if active is None: active=set(range(n))
    graph=[[] for _ in range(n)]
    for u,v in edges:
        if u in active and v in active: graph[u].append(v); graph[v].append(u)
    seen=set(); groups=[]
    for start in sorted(active):
        if start in seen: continue
        seen.add(start); queue=[start]
        for u in queue:
            for v in graph[u]:
                if v not in seen: seen.add(v); queue.append(v)
        groups.append(queue)
    return groups

def bipartite(n,edges):
    graph=[[] for _ in range(n)]; colour={}
    for u,v in edges: graph[u].append(v); graph[v].append(u)
    for start in range(n):
        if start in colour: continue
        colour[start]=0; queue=[start]
        for u in queue:
            for v in graph[u]:
                if v not in colour: colour[v]=colour[u]^1;queue.append(v)
                elif colour[v]==colour[u]: return False
    return True

def edge_input(edges): return ''.join(f'{u+1} {v+1}\n' for u,v in edges)
def q1_case(n,edges):
    text=f'{n} {len(edges)}\n'+edge_input(edges); out=run(1,text); kept=[]; expected=[]
    for edge in edges:
        ok=bipartite(n,kept+[edge]);expected.append(ok)
        if ok: kept.append(edge)
    assert out['accepted']==expected
    assert len(out['colours'])==n and all(c in [0,1] for c in out['colours'])
    assert all(out['colours'][u]!=out['colours'][v] for u,v in kept)
    return text,out

def q2_case(rows,cols,ops):
    text=f'{rows} {cols} {len(ops)}\n'+''.join(f'{r} {c}\n' for r,c in ops)
    out=run(2,text);land=set();expected=[]
    for cell in ops:
        land.add(cell);seen=set(); count=0
        for start in sorted(land):
            if start in seen: continue
            count+=1;seen.add(start);queue=[start]
            for r,c in queue:
                for nxt in [(r-1,c),(r+1,c),(r,c-1),(r,c+1)]:
                    if nxt in land and nxt not in seen:seen.add(nxt);queue.append(nxt)
        expected.append(count)
    assert out['counts']==expected,(text,out,expected)
    return text,out

def q3_case(weights,edges,limit):
    n=len(weights);text=f'{n} {len(edges)} {limit}\n'+' '.join(map(str,weights))+'\n'
    text+=''.join(f'{u+1} {v+1} {w}\n' for u,v,w in edges)
    out=run(3,text);expected=[];kept=[]
    for step in range(len(edges)+1):
        sums=[sum(weights[i] for i in group) for group in components(n,kept) if len(group)<=limit]
        expected.append(max(sums,default=None))
        if step<len(edges): kept.append(edges[step][:2])
    assert out['maxima']==expected,(text,out,expected)
    return text,out

def q4_case(weights,edges,order):
    n=len(weights);text=f'{n} {len(edges)}\n'+' '.join(map(str,weights))+'\n'
    text+=edge_input(edges)+' '.join(str(i+1) for i in order)+'\n'
    out=run(4,text);active=set(range(n));expected=[]
    for u in order:
        active.remove(u)
        sums=[sum(weights[i] for i in group) for group in components(n,edges,active)]
        expected.append(max(sums,default=0))
    assert out['maxima']==expected,(text,out,expected)
    return text,out

def q5_case(a,edges,exhaustive=True):
    n=len(a);text=f'{n} {len(edges)}\n'+' '.join(map(str,a))+'\n'+edge_input(edges)
    out=run(5,text);expected=a[:]
    for group in components(n,edges):
        for i,value in zip(sorted(group),sorted(a[i] for i in group)):expected[i]=value
    assert out['permutation']==expected
    if exhaustive:
        seen={tuple(a)};queue=[tuple(a)]
        for state in queue:
            for u,v in edges:
                b=list(state);b[u],b[v]=b[v],b[u];b=tuple(b)
                if b not in seen:seen.add(b);queue.append(b)
        assert tuple(expected)==min(seen)
    return text,out

def main():
    for _ in range(100):
        n=R.randint(1,9);edges=[(R.randrange(n),R.randrange(n)) for _ in range(R.randint(0,20))]
        q1_case(n,edges)
        rows,cols=R.randint(1,5),R.randint(1,5)
        q2_case(rows,cols,[(R.randrange(rows),R.randrange(cols)) for _ in range(R.randint(0,25))])
        weights=[R.randint(-12,12) for _ in range(n)]
        q3_case(weights,[(u,v,R.randint(-10,10)) for u,v in edges],R.randint(0,n))
        order=list(range(n));R.shuffle(order);q4_case(weights,edges,order)
        n=R.randint(1,7);a=list(range(1,n+1));R.shuffle(a)
        q5_case(a,[(R.randrange(n),R.randrange(n)) for _ in range(R.randint(0,7))])
    # Regression: stale maximum falls after negative-weight merges and deletions.
    q3_case([8,-10,7,3,-2],[(0,1,4),(2,3,1),(0,2,9),(3,4,-1)],2)
    q3_case([-5,-8],[(0,1,0)],0)
    q4_case([10,-20,5],[(0,1),(1,2)],[2,1,0])
    q4_case([-7],[],[0]);q2_case(1,1,[(0,0)]*5)
    q1_case(3,[(0,1),(1,2),(2,0),(2,0),(0,0),(0,1)])
    bad={1:['0 0\n','2 1\n1 3\n','2 0\nextra\n'],
         2:['1000000 2 0\n','2 2 1\n2 0\n','1 1 -1\n'],
         3:['2 0 3\n1 2\n','1 1 1\n1\n1 1\n','1 0 1\n9223372036854775808\n'],
         4:['2 0\n1 2\n1 1\n','1 0\n1\n2\n','1 1\n1\n1 2\n1\n'],
         5:['3 0\n1 1 3\n','2 1\n2 1\n1 3\n','1 0\n0\n']}
    for q,texts in bad.items():
        for text in texts:run(q,text,False)
    for q,text in enumerate(SAMPLES,1):
        out=run(q,text);folder=ROOT/f'Q-{q}'
        (folder/f'q{q}_sample_input.txt').write_text(text)
        (folder/f'q{q}_sample_output.txt').write_text(out['stdout'])
        (folder/f'q{q}_sample_trace.json').write_text(json.dumps(out,indent=2)+'\n')
    report={'seed':42505010,'result':'PASS','strict_sources':5,
            'core_executions':executions,'oracles':'BFS, flood fill, deletion replay, exhaustive legal swaps',
            'negative_weights':True,'q3_edge_order':'arrival order; edge weights are labels'}
    (ROOT/'verification.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))

if __name__=='__main__':main()
