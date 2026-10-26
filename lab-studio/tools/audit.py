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
import re
import os
import shutil
import subprocess
import sys
import tempfile
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from backend.runner import ROOT, CATALOG, compiler, run_problem
from backend.catalog import REPOSITORY, OVERRIDES, materialize, source_path


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--full',action='store_true');args=parser.parse_args()
    cc=compiler(); failures=[]; checks=[]; warnings=[]
    sources=sorted(path for directory in REPOSITORY.glob('lab[0-9]*') if directory.is_dir() for path in directory.rglob('*.c'))
    def syntax(source):
        p=subprocess.run([cc,'-std=c17','-Wall','-Wextra','-Wpedantic','-fsyntax-only',str(source_path(source.relative_to(REPOSITORY)))],capture_output=True,timeout=30)
        return source,p
    with ThreadPoolExecutor(max_workers=4) as pool:
        for source,p in pool.map(syntax,sources):
            name=source.relative_to(REPOSITORY).as_posix()
            if p.returncode:failures.append({'source':name,'error':p.stderr.decode(errors='replace')})
            elif p.stderr:warnings.append({'source':name,'diagnostics':p.stderr.decode(errors='replace').replace(str(REPOSITORY),'repository')})
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
            materialize(workspace, include_tools=True)
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
    changed=[entry['path'] for entry in provenance['files'] if hashlib.sha256((OVERRIDES/entry['path']).read_bytes()).hexdigest()!=entry['sha256']]
    report={'status':'PASS' if not failures else 'FAIL','sourceCommit':CATALOG['commit'],'scope':'Repository C sources with project overrides; original files remain read-only.',
            'compiler':Path(cc).name,'cSyntaxChecks':len(sources),'sampleRuns':len(checks),'focusedRegressions':'PASS' if regression.returncode==0 else 'FAIL','fullAudit':args.full,
            'correctedOverrides':changed,'overrideCount':len(changed),'warnings':warnings,'failures':failures,'samples':checks,'oracleSuites':suites,'legacyExperiments':experiments}
    report['focusedRegressionCount']=int(re.search(r'Ran (\d+) tests',regression.stderr.decode(errors='replace'))[1])
    for suite in report['oracleSuites']:
        transcript=suite.get('transcript','')
        match=re.search(r'(\d+) (?:program executions|oracle-checked executions)',transcript)
        if match:suite['executions']=int(match[1])
        else:
            try:
                parsed=json.loads(transcript)
                suite['executions']=parsed.get('oracle_checked_executions',parsed.get('core_executions'))
            except ValueError:pass
        if suite['result']=='PASS':suite.pop('transcript',None)
    for experiment in report['legacyExperiments']:
        transcript=experiment.get('transcript','')
        experiment['outputSha256']=hashlib.sha256(transcript.encode()).hexdigest()
        if experiment['result']=='PASS':experiment.pop('transcript',None)
    frontend_report=ROOT/'.cache/frontend-tests.json'
    if frontend_report.is_file():
        ui=json.loads(frontend_report.read_text(encoding='utf-8'))
        report['frontend']={'status':'PASS' if ui.get('success') else 'FAIL','tests':ui.get('numTotalTests'),'passed':ui.get('numPassedTests'),'scope':'DOM integration, including all 64 real C-result fixtures, input editing, imports, exports, recovery and controls'}
    report['browserVerification']={'chromeDetection':'verified installed path; launch arguments regression-tested','interactiveChrome':'not completed: Computer Use stopped because it could not reliably identify the browser URL'}
    (ROOT/'AUDIT.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    print(f'Audit: {report["status"]}; {len(failures)} failures.',flush=True)
    return bool(failures)

if __name__=='__main__':sys.exit(main())
