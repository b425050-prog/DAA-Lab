"""Compile project-owned C sources and execute them in disposable working folders."""
from __future__ import annotations
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import threading
import time

ROOT = Path(__file__).resolve().parents[1]
SOURCES = ROOT / 'engine' / 'sources'
CACHE = ROOT / '.cache'
CATALOG = json.loads((ROOT / 'catalog.json').read_text(encoding='utf-8'))
BY_ID = {item['id']: item for item in CATALOG['problems']}
BUILD_LOCK = threading.Lock()
RUN_SLOTS = threading.BoundedSemaphore(2)
TIMEOUT = 12
OUTPUT_LIMIT = 2 * 1024 * 1024


class RunError(Exception):
    def __init__(self, message, status=422, output=''):
        self.message, self.status, self.output = message, status, output
        super().__init__(message)


def compiler():
    configured = os.environ.get('CC')
    found = shutil.which(configured or 'gcc') or (None if configured else shutil.which('clang'))
    if not found:
        raise RunError('A C compiler is needed. Install GCC or Clang, add it to PATH, then restart Lab Studio.', 503)
    return found


def build(item):
    source = SOURCES / item['source']
    digest = hashlib.sha256(source.read_bytes())
    # Headers are small; hashing all of them avoids stale builds after a shared fix.
    for header in sorted(SOURCES.rglob('*.h')):
        digest.update(header.read_bytes())
    cc = compiler()
    digest.update(cc.encode())
    target = CACHE / 'bin' / (item['id'] + '-' + digest.hexdigest()[:16] + ('.exe' if os.name == 'nt' else ''))
    with BUILD_LOCK:
        if not target.exists():
            target.parent.mkdir(parents=True, exist_ok=True)
            result = subprocess.run([cc, '-std=c17', '-O2', '-Wall', '-Wextra', '-Wpedantic',
                                     str(source), '-lm', '-o', str(target)], capture_output=True, timeout=60)
            if result.returncode:
                raise RunError('The C program could not be compiled.', 500, result.stderr.decode(errors='replace'))
    return target


def validate_input(item, text):
    if not isinstance(text, str) or len(text.encode('utf-8')) > 64_000:
        raise RunError('Input must be text of at most 64 KB.')
    if any(ord(c) > 126 or (ord(c) < 32 and c not in '\r\n\t') for c in text):
        raise RunError('Use printable ASCII text, spaces, tabs, and newlines.')
    pid = item['id']
    # These programs accept file names. Keep every generated file inside the run folder.
    if pid in ('L5Q3', 'L5Q4'):
        tokens = text.split()
        if len(tokens) != 2:
            raise RunError('Enter just the element count and seed. Lab Studio supplies the file names.')
        text = f'{tokens[0]}\n{tokens[1]}\ninput.txt\noutput.txt\n'
    # The common modern readers check exact bounds themselves. Legacy scanf readers
    # need bounds before conversion, including a conservative arithmetic envelope.
    if item['lab'] <= 6 and pid != 'L6Q7':
        tokens = text.split()
        if not tokens:
            raise RunError('Enter inputs before running.')
        if len(tokens) > 12_000:
            raise RunError('This interactive workbench supports at most 12,000 input tokens.')
        for token in tokens:
            if re.fullmatch(r'[+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?', token):
                try:
                    value = float(token)
                except ValueError:
                    raise RunError('Invalid numeric input.')
                if not math.isfinite(value) or abs(value) > 1_000_000_000:
                    raise RunError('For legacy programs, numeric inputs must be finite and within ±1,000,000,000.')
        try:
            n = int(tokens[0])
        except ValueError:
            raise RunError('The first value must be an integer size.')
        limit = {'L1Q1': 100, 'L1Q2': 100_000, 'L1Q4': 12, 'L2Q1': 65_536,
                 'L3Q4': 32, 'L3Q5': 32, 'L6Q2': 32, 'L6Q8': 200}.get(pid, 2000)
        if n < 0 or n > limit:
            raise RunError(f'The first size must be between 0 and {limit:,} for interactive runs.')
        fixed = {'L1Q1':3,'L1Q2':3,'L1Q4':1,'L1Q6':3,'L2Q1':1,'L5Q3':4,'L5Q4':4,'L6Q5':1}
        expected = fixed.get(pid)
        if pid in ('L1Q3','L1Q5','L3Q2','L3Q3','L5Q1','L6Q4','L6Q8'):
            expected = n + 1
        if pid in ('L2Q2','L3Q1','L3Q6','L5Q2','L6Q1'):
            expected = n + 2
        if pid in ('L3Q4','L3Q5','L6Q2'):
            expected = 1 + 2*n*n
        if pid in ('L4Q1','L4Q4','L4Q5','L4Q6'):
            expected = 1 + 2*n
        if pid in ('L4Q2','L6Q6'):
            expected = 2 + 2*n
        if pid == 'L4Q3':
            expected = n + 3
        if pid in ('L2Q3','L6Q3'):
            try:
                other = int(tokens[1])
            except (ValueError, IndexError):
                raise RunError('The second size must be an integer.')
            expected = 3 + n*other if pid == 'L2Q3' else 2+n+other
        if expected is not None and len(tokens) != expected:
            raise RunError(f'The supplied sizes require {expected} values; received {len(tokens)}. Check the input format.')
        real_fields = pid in ('L1Q2','L6Q2','L6Q3')
        for i, token in enumerate(tokens):
            if (pid == 'L4Q1' and i>0 and i%2==0) or (pid in ('L5Q3','L5Q4') and i>=2):
                continue
            if real_fields and i>0 and not (pid=='L1Q2' and i==2):
                if not re.fullmatch(r'[+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?', token):
                    raise RunError('Expected numeric values.')
            elif not re.fullmatch(r'[+-]?\d+', token):
                raise RunError('Expected decimal integer values.')
        if pid in ('L3Q4','L3Q5') and any(abs(int(token))>1_000_000 for token in tokens[1:]):
            raise RunError('Integer matrix entries must be within ±1,000,000 to keep all recursive arithmetic exact in int64.')
        if pid=='L1Q2' and int(tokens[2])<=0:
            raise RunError('The coin simulation seed must be a positive integer.')
        if (pid=='L1Q6' and int(tokens[2])<0) or (pid in ('L5Q3','L5Q4') and int(tokens[1])<0):
            raise RunError('The deterministic seed must be nonnegative.')
        if pid == 'L2Q3' and (len(tokens) < 2 or not tokens[1].isdigit() or int(tokens[1]) > 2000 or n * int(tokens[1]) > 10_000):
            raise RunError('Use at most 2,000 elements per array and 10,000 total elements.')
        if pid == 'L6Q3' and (len(tokens) < 2 or not tokens[1].isdigit() or int(tokens[1]) > 2000):
            raise RunError('Vector lengths must be at most 2,000.')
        if pid == 'L6Q6':
            if len(tokens) < 2 or not tokens[1].isdigit() or (n + 1) * (int(tokens[1]) + 1) > 1_000_000:
                raise RunError('Use a knapsack table of at most 1,000,000 cells.')
        if pid == 'L3Q6' and n > 150:
            raise RunError('Selection-sort invariant playback supports at most 150 elements.')
    if pid == 'L6Q7' and (len(text.split()) != 2 or any(len(s) > 1000 for s in text.split())):
        raise RunError('Enter two nonempty strings of at most 1,000 characters each.')
    return text if text.endswith('\n') else text + '\n'


def execute_process(binary, text, directory, args=()):
    output, errors = directory / 'stdout.log', directory / 'stderr.log'
    stdin_path = directory / 'stdin.txt'
    stdin_path.write_text(text, encoding='ascii')
    env = {**os.environ, 'DAA_NO_OPEN': '1'}
    with stdin_path.open('rb') as inp, output.open('wb') as out, errors.open('wb') as err:
        process = subprocess.Popen([str(binary), *args], stdin=inp, stdout=out, stderr=err,
                                   cwd=directory, env=env)
        start = time.monotonic()
        try:
            while process.poll() is None:
                if time.monotonic() - start > TIMEOUT:
                    raise RunError(f'Run stopped after {TIMEOUT} seconds. Try a smaller input.')
                if output.stat().st_size + errors.stat().st_size > OUTPUT_LIMIT:
                    raise RunError('The output is too large for an interactive run. Try a smaller input.')
                time.sleep(0.025)
        finally:
            if process.poll() is None:
                process.kill()
            process.wait()
    if output.stat().st_size + errors.stat().st_size > OUTPUT_LIMIT:
        raise RunError('The output is too large for an interactive run. Try a smaller input.')
    stdout = output.read_text(encoding='utf-8', errors='replace')
    stderr = errors.read_text(encoding='utf-8', errors='replace')
    if process.returncode:
        raise RunError(stderr.strip() or 'The C program rejected this input. See its output below.', output=stdout)
    return stdout, stderr


def parse_dat(text):
    rows, labels = [], []
    for line in text.splitlines():
        parts = line.strip().split()
        if not parts:
            continue
        if parts[0].startswith('#'):
            candidate = line.lstrip('# ').split()
            if candidate:
                labels = candidate
            continue
        try:
            row = [float(x) for x in parts]
        except ValueError:
            continue
        if len(row) > 1 and all(math.isfinite(v) for v in row):
            rows.append(row)
    if not rows:
        return None
    width = len(rows[0])
    rows = [r for r in rows if len(r) == width]
    if len(labels) != width:
        labels = ['Input size'] + [f'Series {i}' for i in range(1, width)]
    return {'labels': labels, 'rows': rows, 'totalRows': len(rows)}


def graph_result(pid, text, stdout):
    if not pid.startswith('L10'):
        return None
    values = list(map(int, text.split()))
    n, m = values[:2]
    if pid == 'L10Q2':
        return {'kind': 'grid', 'rows': n, 'cols': m, 'cells': [values[i:i+2] for i in range(3, len(values), 2)],
                'counts': list(map(int, re.findall(r'islands=(\d+)', stdout)))}
    if n > 200:
        return {'kind': 'large', 'n': n}
    offset = 2 if pid == 'L10Q1' else 3 + n if pid == 'L10Q3' else 2 + n
    stride = 3 if pid == 'L10Q3' else 2
    edges = [values[i:i+stride] for i in range(offset, offset + stride*m, stride)]
    accepted = ['REJECTED' not in s for s in stdout.splitlines() if s.startswith('Edge ')]
    colours_match = re.search(r'Final colours:([^\n]*)', stdout)
    result = {'kind': 'graph', 'n': n, 'edges': edges, 'accepted': accepted,
              'colours': list(map(int, colours_match[1].split())) if colours_match else [],
              'maxima': [None if x == 'NONE' else int(x) for x in re.findall(r'maximum=(NONE|-?\d+)', stdout)]}
    if pid == 'L10Q3':
        # The original algorithm processes arrivals in input order; edge weights are metadata.
        result['weights'] = values[3:3+n]
    if pid == 'L10Q4':
        result['weights'] = values[2:2+n]
        result['deletions'] = values[offset+2*m:]
    if pid == 'L10Q5':
        result['initial'] = values[2:2+n]
        match = re.search(r'Smallest permutation:([^\n]*)', stdout)
        result['permutation'] = list(map(int, match[1].split())) if match else []
    return result


def run_problem(pid, text, count_only=False):
    if pid not in BY_ID:
        raise RunError('Unknown problem.', 404)
    if not RUN_SLOTS.acquire(blocking=False):
        raise RunError('Two runs are already active. Please try again shortly.', 429)
    try:
        item = BY_ID[pid]
        text = validate_input(item, text)
        binary = build(item)
        start = time.perf_counter()
        with tempfile.TemporaryDirectory(prefix='daa-studio-') as temp:
            directory = Path(temp)
            # Legacy executables change into argv[0]'s folder. Give each run its own copy.
            local_binary = directory / binary.name
            shutil.copy2(binary, local_binary)
            args = ['--count'] if count_only and pid in ('L7Q1', 'L7Q3', 'L7Q4') else []
            stdout, stderr = execute_process(local_binary, text, directory, args)
            elapsed = round((time.perf_counter() - start) * 1000, 2)
            trace = None
            if item['lab'] in (8, 9):
                raw, _ = execute_process(local_binary, text, directory, ['--json'])
                # Preserve 64-bit JSON integers as decimal strings rather than lose precision in JS.
                trace = json.loads(raw, parse_int=lambda x: int(x) if abs(int(x)) <= 2**53-1 else x)
            generated = []
            for path in sorted(directory.glob('*.dat')):
                if path.stat().st_size <= OUTPUT_LIMIT:
                    data = parse_dat(path.read_text(errors='replace'))
                    if data:
                        generated.append({'name': path.name, **data})
            files = []
            for name in ('input.txt', 'output.txt'):
                path = directory / name
                if path.exists() and path.stat().st_size <= OUTPUT_LIMIT:
                    files.append({'name': name, 'text': path.read_text(errors='replace')})
            # JSON invocation overwrites stdout.log, so retain the first human-readable result above.
            return {'problemId': pid, 'input': text, 'stdout': stdout, 'stderr': stderr,
                    'elapsedMs': elapsed, 'trace': trace, 'graph': graph_result(pid, text, stdout),
                    'datasets': generated, 'files': files, 'source': item['source'], 'engine': 'C17'}
    finally:
        RUN_SLOTS.release()
