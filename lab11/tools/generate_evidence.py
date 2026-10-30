"""Reproducible scaling families checked independently of the C algorithms."""
import itertools,json
import validate as v
from lab_config import ROOT,EVENTS
def main():
    records=[]
    for q in [1,2,3]:
        rows=[]
        sizes=[32,64,128,256,512,1024,2048] if q<3 else [2,4,8,16,32,64,128,256,512,1024]
        for n in sizes:
            if q==1:
                # Connected, mixed-weight directed family with several possible parity states.
                edges=[(i,i+1,i%2) for i in range(n-1)]
                edges += [(i,i+2,1) for i in range(n-2)]+[(i+1,i,0) for i in range(n-1)]
                text,out=v.parity_case(n,edges,0,5)
                parameters={'vertices':n,'edges':len(edges),'penalty':5}
            elif q==2:
                # A ladder, not merely a path; hub index order reverses vertex order.
                half=n//2;edges=[(i,i+1) for i in range(half-1)]
                edges += [(half+i,half+i+1) for i in range(half-1)]+[(i,half+i) for i in range(half)]
                text,out=v.voronoi_case(n,edges,[n-1,0],n)
                parameters={'vertices':n,'edges':len(edges),'hubs':2,'radius':n}
            else:
                # Connected binary cubes. Every dimension changes; distance is exactly L.
                length=n.bit_length()-1
                words=[''.join(w) for w in itertools.product('ab',repeat=length)]
                text,out=v.word_case(words,'a'*length,'b'*length,length)
                parameters={'dictionary_words':n,'length':length,'expected_mutations':length}
            rows.append((n,out['work']))
            records.append({'q':q,'size':n,'parameters':parameters,'input':text,'result':out,'oracle':'PASS'})
        (ROOT/f'Q-{q}'/f'q{q}_data.dat').write_text('# size counted_operations\n'+'\n'.join(f'{n} {w}' for n,w in rows)+'\n')
    (ROOT/'scaling_evidence.json').write_text(json.dumps({'executions':v.executions,'events':EVENTS,'cases':records},indent=2)+'\n')
    print(f'{v.executions} independently checked scaling executions passed.')
if __name__=='__main__':main()
