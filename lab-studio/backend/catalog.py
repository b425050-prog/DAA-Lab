"""Read repository resources in place; keep only project metadata and fixes here."""
import json
import math
from pathlib import Path
import posixpath
import re

ROOT = Path(__file__).resolve().parents[1]
REPOSITORY = ROOT.parent
OVERRIDES = ROOT / 'engine' / 'overrides'

FORMATS = {
 'L1Q1':'Minimum n, maximum n, and integer step. Use 2 ≤ minimum ≤ maximum ≤ 100.',
 'L1Q2':'Toss count (1–100000), heads probability for the biased coin (0–1), then a positive integer seed.',
 'L1Q3':'Array length n, then n integers. Both bubble sorts receive this same array.',
 'L1Q4':'One disk count, 0–12. Playback uses pegs A, B, C.',
 'L1Q5':'Array length n, then n values: a run of zeros followed by a run of ones.',
 'L1Q6':'Element count n, maximum random value, then a nonnegative integer seed.',
 'L2Q1':'Maximum experiment size, 128–65536. The experiment compares all six representations.',
 'L2Q2':'Array length n, n integers, then method: 1 for two-way or 2 for three-way merge sort.',
 'L2Q3':'Array count k and length n, then k sorted arrays of n integers each, then method: 1 sequential or 2 balanced.',
 'L3Q1':'Array length n, n integers in nondecreasing order, then the search target.',
 'L3Q2':'Coin count n (at least 2), then n positive weights. All coins are normal, or exactly one is lighter.',
 'L3Q3':'Array length n, then n integers.',
 'L3Q4':'Matrix order n, then n rows of matrix A and n rows of matrix B. Entries are integers in ±1000000.',
 'L3Q5':'Power-of-two order n, then the two recursively patterned matrices A and B. Entries are integers in ±1000000.',
 'L3Q6':'Array length n, n integers, then trace: 1 to show invariant checks or 0 to hide them.',
 'L4Q1':'Item count n, then n rows: number colour. Numbers must already be sorted. Colours: RED, BLUE, YELLOW (or r, b, y).',
 'L4Q2':'Elements per set n, n integers of the first set, n integers of the second set, then the target sum.',
 'L4Q3':'Element count n and tuple size k, then n integers, then the target sum. Elements must be distinct.',
 'L4Q4':'Person count n, then n entry exit pairs. Entry < exit; every endpoint must be distinct.',
 'L4Q5':'Interval count n, then n left right pairs with left ≤ right.',
 'L4Q6':'Interval count n, then n left right pairs. Intervals are closed.',
 'L5Q1':'Array length n, then n integers. An even-length median averages the two middle ranks.',
 'L5Q2':'Array length n, n integers, then rank k (1 = smallest).',
 'L5Q3':'Generated element count n, then a nonnegative seed. File names are supplied by the app.',
 'L5Q4':'Generated element count n, then a nonnegative seed. File names are supplied by the app.',
 'L6Q1':'Array length n, n integers, then a pivot position in 1..n.',
 'L6Q2':'Square matrix order n, then n rows of A and n rows of B. Finite real values are supported.',
 'L6Q3':'Vector lengths m and n (n ≥ m), then m values of A and n values of B.',
 'L6Q4':'Permutation length n, then a permutation of 1..n.',
 'L6Q5':'Fibonacci index n in 0..93 (exact unsigned 64-bit result).',
 'L6Q6':'Item count n and capacity W, then n positive weights, then n profits.',
 'L6Q7':'Two nonempty strings on separate lines, without spaces. Each is at most 1000 characters.',
 'L6Q8':'Dimension count N (at least 2), then N positive dimensions describing N−1 matrices.',
}


def parse_dat(text):
    rows, headers = [], []
    for line in text.splitlines():
        if line.strip().startswith('#'):
            headers.append(line.strip().lstrip('# ').split())
            continue
        try:
            values = list(map(float, line.split()))
        except ValueError:
            continue
        if len(values) > 1 and all(math.isfinite(v) for v in values):
            rows.append(values)
    if not rows:
        return None
    width = len(rows[0])
    rows = [row for row in rows if len(row) == width]
    labels = next((h for h in reversed(headers) if len(h) == width), ['Input size']+[f'Series {i}' for i in range(1,width)])
    count = len(rows)
    if count > 400:
        rows = [rows[round(i*(count-1)/399)] for i in range(400)]
    return {'labels': labels, 'rows': rows, 'totalRows': count}


def source_path(relative):
    override = OVERRIDES / relative
    return override if override.is_file() else REPOSITORY / relative


def materialize(destination, lab=None, include_tools=False):
    """Create a disposable source tree, applying only the corrected overrides."""
    import shutil
    for directory in sorted(REPOSITORY.glob('lab[0-9]*')):
        if not directory.is_dir() or (lab is not None and directory.name != f'lab{lab}'):
            continue
        for path in directory.rglob('*'):
            if not path.is_file() or path.suffix not in ({'.c','.h','.py'} if include_tools else {'.c','.h'}):
                continue
            relative = path.relative_to(REPOSITORY)
            target = destination / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source_path(relative), target)


def load_catalog():
    catalog = json.loads((ROOT / 'catalog.json').read_text(encoding='utf-8'))
    if not (REPOSITORY / 'lab1').is_dir():
        raise RuntimeError('Keep lab-studio inside the DAA-Lab repository; its original labs provide the read-only resources.')
    for item in catalog['problems']:
        item['sourceKind'] = 'override' if (OVERRIDES/item['source']).is_file() else 'repository'
        directory = REPOSITORY / item['resourceDir']
        document = (directory / 'README.md').read_text(encoding='utf-8-sig')
        document = re.sub(r'</?(?:p|div|span|table|tbody|tr|td|th|img|a|br|h[1-6]|details|summary|strong|em)\b[^>]*>', '', document)
        document = re.sub(r'!\[[^\]]*\]\([^)]*\)', '', document)
        def link(match):
            label, target = match[1], match[2]
            if target.startswith(('http:', 'https:', '#')):
                return match[0]
            path = posixpath.normpath(item['resourceDir']+'/'+target)
            return f'[{label}](https://github.com/{catalog["repository"]}/blob/{catalog["commit"]}/{path})'
        item['notes'] = re.sub(r'\[([^\]]*)\]\(([^)]+)\)', link, document)
        item['evidence'] = []
        for path in sorted(directory.rglob('*.dat')):
            data = parse_dat(path.read_text(encoding='utf-8-sig'))
            if data:
                item['evidence'].append({'name': path.name, **data})
        guide = FORMATS.get(item['id'])
        if not guide:
            path = directory.parent / 'INPUTS.md'
            original = path.read_text(encoding='utf-8-sig') if path.exists() else ''
            if item['lab'] == 7:
                line = next((line for line in original.splitlines() if line.startswith(f'| Q{item["question"]} |')), '')
                guide = '| Program | Input | Range | Output |\n|---|---|---|---|\n' + line
            else:
                sections = re.split(r'(?m)^## ', original)
                guide = next((s for s in sections[1:] if re.match(r'Q0?'+str(item['question'])+r'\b',s)), '')
                guide = re.sub(r'\[Sample\]\([^)]*\)', '', guide)
        item['inputGuide'] = guide or item['note']
    return catalog
