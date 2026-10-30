"""Build all three C17 solutions with strict warnings."""
import os,shutil,subprocess
from lab_config import ROOT,NAMES,executable
cc=os.environ.get('CC') or shutil.which('gcc') or shutil.which('clang')
if not cc: raise SystemExit('Install GCC/Clang or set CC to its executable path.')
(ROOT/'bin').mkdir(exist_ok=True)
for q,name in enumerate(NAMES,1):
    subprocess.run([cc,'-std=c17','-O2','-Wall','-Wextra','-Wpedantic','-Werror',
                    str(ROOT/f'Q-{q}'/f'q{q}_{name}.c'),'-o',str(executable(q))],check=True)
    print(f'Q{q}: strict C17 build passed')
