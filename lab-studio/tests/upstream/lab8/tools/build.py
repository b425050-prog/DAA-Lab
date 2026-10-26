"""Portable strict C17 build. Set CC and EXTRA_CFLAGS to override the compiler."""
import os
import shlex
import subprocess
from lab_config import ROOT, NAMES
SUFFIX = '.exe' if os.name == 'nt' else ''
def executable(q, plot=False):
    name = f'q{q}_plot_evidence' if plot else f'q{q}_{NAMES[q-1]}'
    return ROOT / 'bin' / (name + SUFFIX)
def build():
    (ROOT / 'bin').mkdir(exist_ok=True)
    cc = shlex.split(os.environ.get('CC', 'gcc'))
    flags = ['-std=c17', '-O2', '-Wall', '-Wextra', '-Wpedantic', '-Werror']
    flags += shlex.split(os.environ.get('EXTRA_CFLAGS', ''))
    count = 0
    for q in range(1, 10):
        for plot in (False, True):
            dest = executable(q, plot)
            source = ROOT / f'Q-{q}' / (dest.stem + '.c')
            subprocess.run(cc + flags + [str(source), '-lm', '-o', str(dest)], check=True)
            count += 1
    print(f'PASS: {count}/18 C17 sources; warnings treated as errors.')
if __name__ == '__main__': build()
