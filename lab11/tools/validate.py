"""Independent Bellman-Ford, per-hub BFS and explicit word-graph BFS oracles."""
import collections,itertools,json,random,re,subprocess
from lab_config import ROOT,TITLES,SAMPLES,executable
R=random.Random(42505011); executions=0
def run(q,text,valid=True):
    global executions
    p=subprocess.run([str(executable(q))],input=text,text=True,capture_output=True,timeout=30)
    executions+=1
    if not valid:
        assert p.returncode and p.stderr,(q,text,p.stdout); return
    assert p.returncode==0,(q,text,p.stderr)
    result={'stdout':p.stdout,'work':int(re.search(r'Work: (\d+)',p.stdout)[1])}
    if q in [1,2]:
        lines=p.stdout.splitlines()[1:-1]
        result['rows']=[line.split() for line in lines]
    else:
        result['mutations']=int(re.search(r'Minimum mutations: (-?\d+)',p.stdout)[1])
        sequence=re.search(r'Sequence: (.*)',p.stdout)[1]
        result['path']=[] if sequence=='NONE' else sequence.split(' -> ')
    return result
def parity_case(n,edges,source,penalty):
    text=f'{n} {len(edges)} {source+1} {penalty}\n'+''.join(f'{u+1} {v+1} {w}\n' for u,v,w in edges)
    out=run(1,text); inf=10**30; distance=[inf]*(2*n);distance[2*source]=0
    # Bellman-Ford on an independently materialised two-layer graph.
    arcs=[(2*u+p,2*v+(p^w),w) for u,v,w in edges for p in range(2)]
    for _ in range(2*n-1):
        changed=False
        for u,v,w in arcs:
            if distance[u]+w<distance[v]:distance[v]=distance[u]+w;changed=True
        if not changed:break
    expected=[]
    for v in range(n):
        a,b=distance[2*v:2*v+2];best=min(a,b+penalty)
        expected.append([str(v+1),str(a) if a<inf else 'INF',str(b) if b<inf else 'INF',str(best) if best<inf else 'UNREACHABLE'])
    assert out['rows']==expected,(text,out,expected)
    return text,out
def voronoi_case(n,edges,hubs,radius):
    text=f'{n} {len(edges)} {len(hubs)} {radius}\n'+' '.join(str(h+1) for h in hubs)+'\n'
    text+=''.join(f'{u+1} {v+1}\n' for u,v in edges)
    out=run(2,text);graph=[[] for _ in range(n)]; all_dist=[]
    for u,v in edges:graph[u].append(v);graph[v].append(u)
    for hub in hubs:
        d=[10**9]*n;d[hub]=0;queue=[hub]
        for u in queue:
            for v in graph[u]:
                if d[v]>d[u]+1:d[v]=d[u]+1;queue.append(v)
        all_dist.append(d)
    expected=[]
    for v in range(n):
        d,i=min(((ds[v],i+1) for i,ds in enumerate(all_dist)),default=(10**9,-1))
        expected.append([str(v+1),str(i if d<=radius else -1),str(d if d<=radius else -1)])
    assert out['rows']==expected,(text,out,expected)
    return text,out
def word_case(words,a,b,known=None):
    text=f'{len(words)} {len(a)}\n{a} {b}\n'+'\n'.join(words)+'\n';out=run(3,text)
    if known is None:
        if b not in words:expected=-1
        else:
            vertices=list(dict.fromkeys([a]+words));d={a:0};queue=[a]
            for u in queue:
                for v in vertices:
                    if v not in d and sum(x!=y for x,y in zip(u,v))==1:d[v]=d[u]+1;queue.append(v)
            expected=d.get(b,-1)
    else:expected=known
    assert out['mutations']==expected,(text[:1000],out,expected)
    path=out['path']
    if expected<0:assert not path
    else:
        assert len(path)==expected+1 and path[0]==a and path[-1]==b
        assert all(w in words for w in path[1:])
        assert all(sum(x!=y for x,y in zip(u,v))==1 for u,v in zip(path,path[1:]))
    return text,out
def main():
    global executions
    regressions=0
    for penalty in [0,1,5,10**12]:
        parity_case(6,[(0,1,1),(0,2,1),(2,1,1),(1,3,0),(2,4,0),(4,3,1)],0,penalty);regressions+=1
    for n,edges,s,c in [(1,[],0,0),(1,[(0,0,1)],0,9),(3,[(0,1,0),(1,0,0),(1,2,1),(1,2,1)],0,4)]:
        parity_case(n,edges,s,c);regressions+=1
    for hubs,radius in [([5,0],2),([0,5],2),([5,0],0),([],20),([3,2],100)]:
        voronoi_case(8,[(0,1),(1,2),(2,3),(3,4),(4,5),(2,6)],hubs,radius);regressions+=1
    voronoi_case(9,[(i,i+1) for i in range(6)]+[(3,7)],[6,0],3);regressions+=1
    for words,a,b in [(['hot','dot','dog','lot','log','cog'],'hit','cog'),(['a'],'a','a'),(['b'],'a','a'),(['b'],'a','b'),(['aaa','bbb'],'aaa','bbb'),(['aaa','aba','abb','bbb'],'aaa','bbb')]:
        word_case(words,a,b);regressions+=1
    for _ in range(220):
        n=R.randint(1,13);edges=[(R.randrange(n),R.randrange(n),R.randrange(2)) for _ in range(R.randint(0,55))]
        parity_case(n,edges,R.randrange(n),R.choice([0,1,2,17,10**12]))
        edges=[(R.randrange(n),R.randrange(n)) for _ in range(R.randint(0,40))]
        hubs=R.sample(range(n),R.randrange(n+1));radius=R.randrange(n+1)
        voronoi_case(n,edges,hubs,radius)
    universe=[''.join(x) for x in itertools.product('abc',repeat=3)]
    for _ in range(300):
        words=R.sample(universe,R.randint(1,len(universe)));word_case(words,R.choice(universe),R.choice(universe))
    # Complete binary word cubes have Hamming distance as an exact closed-form oracle.
    for length in [1,2,5,10]:
        words=[''.join(x) for x in itertools.product('ab',repeat=length)]
        word_case(words,'a'*length,'b'*length,length)
    # Paper limit: 100,000 unique words, length ten. All lower encodings are present.
    def encode(i):
        s=[]
        for _ in range(10):s.append(chr(97+i%26));i//=26
        return ''.join(reversed(s))
    words=[encode(i) for i in range(100000)];a=words[0];b=words[-1]
    word_case(words,a,b,sum(x!=y for x,y in zip(a,b)))
    invalid={1:['0 0 1 0','2 1 1 -1\n1 2 0','2 1 1 2\n1 2 2','1 0 1 0 extra','2 1 1 0\n1 3 0'],
             2:['2 0 2 1\n1 1','1 0 0 -1','2 1 1 1\n1\n1 3','1 0 0 0 extra','2x 0 0 0'],
             3:['2 3\nhit cog\nhot\nhot','1 3\nhit cog\nCOG','1 3\nhit cog\ncogs','0 3\nhit cog','1 11\na a\na','1 1\na b\nb extra']}
    for q,cases in invalid.items():
        for text in cases:run(q,text,False)
    for q,sample in enumerate(SAMPLES,1):
        out=run(q,sample);folder=ROOT/f'Q-{q}'
        (folder/f'q{q}_sample_input.txt').write_text(sample)
        (folder/f'q{q}_sample_output.txt').write_text(out['stdout'])
        (folder/f'q{q}_sample_trace.json').write_text(json.dumps(out,indent=2)+'\n')
    summary={'seed':42505011,'executions':executions,'regressions':regressions,'random_graph_runs':440,'random_word_runs':300,'paper_limit_words':100000,'status':'PASS'}
    (ROOT/'verification.json').write_text(json.dumps(summary,indent=2)+'\n');print(json.dumps(summary))
if __name__=='__main__':main()
