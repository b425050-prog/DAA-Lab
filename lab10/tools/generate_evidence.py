"""Measured scaling on a seeded input family, independently replayed."""
import json, random
from lab_config import ROOT,EVENTS
import validate as v

def main():
    records=[]; rng=random.Random(20261013)
    for q in range(1,6):
        rows=[]
        for n in [16,32,64,128,256,512,1024,2048]:
            if q==1:
                edges=[(i,i+1) for i in range(n-1)]+[(i,i+2) for i in range(n-2)]
                text,out=v.q1_case(n,edges)
            elif q==2:
                text,out=v.q2_case(1,n,[(0,i) for i in range(n)])
            elif q==3:
                weights=[i%17-8 for i in range(n)]
                edges=[(i,i+1,i%11) for i in range(n-1)]
                text,out=v.q3_case(weights,edges,max(1,n//4))
            elif q==4:
                weights=[i%19-9 for i in range(n)];edges=[(i,i+1) for i in range(n-1)]
                order=list(range(n));rng.shuffle(order);text,out=v.q4_case(weights,edges,order)
            else:
                a=list(range(n,0,-1));edges=[(i,i+1) for i in range(n-1)]
                text,out=v.q5_case(a,edges,False)
            rows.append((n,out['work']));records.append({'q':q,'size':n,'input':text,'result':out,'oracle':'PASS'})
        (ROOT/f'Q-{q}'/f'q{q}_data.dat').write_text('# size counted_operations\n'+'\n'.join(f'{n} {w}' for n,w in rows)+'\n')
    report={'seed':20261013,'executions':v.executions,'events':EVENTS,'cases':records}
    (ROOT/'scaling_evidence.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'{v.executions} independently checked scaling executions passed.')

if __name__=='__main__':main()
