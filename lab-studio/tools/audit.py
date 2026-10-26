"""Repeat the audit without writing outside lab-studio or temporary folders.

python tools/audit.py          # compiler checks, all 64 examples, regressions
python tools/audit.py --full   # also repository validators and C experiments
"""
from pathlib import Path
import argparse
from concurrent.futures import ThreadPoolExecutor
import datetime
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from backend.runner import ROOT, SOURCES, CATALOG, compiler, run_problem


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--full',action='store_true');args=parser.parse_args()
    cc=compiler(); failures=[]; checks=[]; warnings=[]
    sources=sorted(SOURCES.rglob('*.c'))
    def syntax(source):
        p=subprocess.run([cc,'-std=c17','-Wall','-Wextra','-Wpedantic','-fsyntax-only',str(source)],capture_output=True,timeout=30)
        return source,p
    with ThreadPoolExecutor(max_workers=4) as pool:
        for source,p in pool.map(syntax,sources):
            name=source.relative_to(SOURCES).as_posix()
            if p.returncode:failures.append({'source':name,'error':p.stderr.decode(errors='replace')})
            elif p.stderr:warnings.append({'source':name,'diagnostics':p.stderr.decode(errors='replace').replace(str(SOURCES),'engine/sources')})
    print(f'Syntax checks: {len(sources)-len(failures)}/{len(sources)}',flush=True)
    for item in CATALOG['problems']:
        try:
            out=run_problem(item['id'],item['sample'])
            checks.append({'id':item['id'],'result':'PASS','outputSha256':hashlib.sha256(out['stdout'].encode()).hexdigest()})
        except Exception as e:failures.append({'id':item['id'],'error':str(e),'output':getattr(e,'output','')})
    print(f'Examples: {len(checks)}/64',flush=True)
    env={**os.environ,'PYTHONUTF8':'1','DAA_NO_OPEN':'1'}
    regression=subprocess.run([sys.executable,'-X','utf8','-m','unittest','discover','-s',str(ROOT/'tests'),'-p','test_*.py','-v'],cwd=ROOT,env=env,capture_output=True,timeout=120)
    if regression.returncode:failures.append({'suite':'regressions','error':regression.stderr.decode(errors='replace')})
    print(regression.stderr.decode(errors='replace'),flush=True)
    suites=[];experiments=[]
    if args.full:
        # Upstream scripts write evidence. Run copies in a disposable tree.
        with tempfile.TemporaryDirectory(prefix='daa-full-audit-') as temp:
            workspace=Path(temp)
            shutil.copytree(SOURCES,workspace,dirs_exist_ok=True)
            shutil.copytree(ROOT/'tests/upstream',workspace,dirs_exist_ok=True)
            for lab in (7,8,9,10):
                directory=workspace/('lab7/lab7' if lab==7 else f'lab{lab}')/'tools'
                if lab in (9,10):
                    p=subprocess.run([sys.executable,'-X','utf8','build.py'],cwd=directory,env=env,capture_output=True,timeout=120)
                    if p.returncode:
                        failures.append({'suite':f'lab{lab}-build','error':p.stderr.decode(errors='replace')});continue
                script='test_all.py' if lab==7 else 'validate.py'
                p=subprocess.run([sys.executable,'-X','utf8',script],cwd=directory,env=env,capture_output=True,timeout=300)
                transcript=(p.stdout+p.stderr).decode(errors='replace')
                suites.append({'lab':lab,'result':'PASS' if p.returncode==0 else 'FAIL','transcript':transcript})
                if p.returncode:failures.append({'suite':f'lab{lab}-oracles','error':transcript})
                print(f'Lab {lab} independent oracles: {"PASS" if p.returncode==0 else "FAIL"}',flush=True)
            for source in sorted(workspace.glob('lab[2-6]/Q-*/*experimental*.c')):
                # Lab 2 Q1 has a project-adapted stdin parameter and was exercised above.
                if source.parent.parent.name=='lab2' and source.parent.name=='Q-1':continue
                exe=workspace/('experiment.exe' if os.name=='nt' else 'experiment')
                p=subprocess.run([cc,'-std=c17','-O2',str(source),'-lm','-o',str(exe)],capture_output=True,timeout=60)
                if p.returncode:
                    failures.append({'experiment':source.relative_to(workspace).as_posix(),'error':p.stderr.decode(errors='replace')});continue
                work=workspace/'experiment-output';work.mkdir(exist_ok=True)
                p=subprocess.run([str(exe)],cwd=work,env=env,capture_output=True,timeout=120)
                experiments.append({'source':source.relative_to(workspace).as_posix(),'result':'PASS' if p.returncode==0 else 'FAIL','transcript':(p.stdout+p.stderr).decode(errors='replace')})
                if p.returncode:failures.append({'experiment':source.name,'error':experiments[-1]['transcript']})
                print(f'Experiment {source.parent.parent.name}/{source.parent.name}: {experiments[-1]["result"]}',flush=True)
    provenance=json.loads((ROOT/'engine/provenance.json').read_text())
    changed=[entry['path'] for entry in provenance['files'] if hashlib.sha256((SOURCES/entry['path']).read_bytes()).hexdigest()!=entry['sha256']]
    report={'status':'PASS' if not failures else 'FAIL','sourceCommit':CATALOG['commit'],'scope':'All project-owned C copies; original repository files are not modified.',
            'compiler':Path(cc).name,'cSyntaxChecks':len(sources),'sampleRuns':len(checks),'focusedRegressions':'PASS' if regression.returncode==0 else 'FAIL','fullAudit':args.full,
            'modifiedProjectCopies':changed,'warnings':warnings,'failures':failures,'samples':checks,'oracleSuites':suites,'legacyExperiments':experiments}
    (ROOT/'AUDIT.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    print(f'Audit: {report["status"]}; {len(failures)} failures.',flush=True)
    return bool(failures)

if __name__=='__main__':sys.exit(main())
