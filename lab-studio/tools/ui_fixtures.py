"""Generate disposable UI integration inputs from real C executions."""
from pathlib import Path
import json
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from backend.runner import ROOT,CATALOG,run_problem

def main():
    results={}
    for problem in CATALOG['problems']:
        results[problem['id']]=run_problem(problem['id'],problem['sample'])
    target=ROOT/'.cache/ui-fixtures.json';target.parent.mkdir(exist_ok=True)
    target.write_text(json.dumps({'catalog':CATALOG,'runs':results}),encoding='utf-8')
    print('Generated 64 real C results for UI integration checks.',flush=True)
if __name__=='__main__':main()
