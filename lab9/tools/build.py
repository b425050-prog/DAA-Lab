"""Strict, portable C17 build. CC may name GCC or Clang."""
from pathlib import Path
import os, shutil, subprocess
from lab_config import ROOT, NAMES

cc=os.environ.get('CC') or shutil.which('gcc') or shutil.which('clang')
if not cc: raise SystemExit('Install GCC/Clang or set CC to its executable path.')
(ROOT/'bin').mkdir(exist_ok=True)
for q,name in enumerate(NAMES,1):
    output=ROOT/'bin'/f'q{q}_{name}'
    if os.name=='nt': output=output.with_suffix('.exe')
    subprocess.run([cc,'-std=c17','-O2','-Wall','-Wextra','-Wpedantic','-Werror',
                    str(ROOT/f'Q-{q}'/f'q{q}_{name}.c'),'-lm','-o',str(output)],check=True)
    print(f'Q{q}: strict C17 build passed')
