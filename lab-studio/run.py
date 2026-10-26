"""Build and launch the local C-backed workbench, with Chrome support."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys
import webbrowser
from urllib.request import urlopen
from urllib.error import URLError

ROOT = Path(__file__).resolve().parent


def frontend_fingerprint():
    digest = hashlib.sha256()
    files = list((ROOT/'frontend/src').rglob('*'))
    files += [ROOT/'frontend'/name for name in ('package.json','package-lock.json','vite.config.ts','tsconfig.json','index.html')]
    for path in sorted(p for p in files if p.is_file()):
        digest.update(str(path.relative_to(ROOT)).encode());digest.update(path.read_bytes())
    return digest.hexdigest()


def ensure_frontend(dev=False):
    frontend=ROOT/'frontend'; cache=ROOT/'.cache';cache.mkdir(exist_ok=True)
    stamp=cache/'frontend-build.json'
    fingerprint=frontend_fingerprint()
    try: previous=json.loads(stamp.read_text())
    except (OSError,ValueError):previous={}
    if not dev and previous.get('fingerprint')==fingerprint and (frontend/'dist/index.html').is_file():return
    npm=shutil.which('npm.cmd' if os.name=='nt' else 'npm')
    if not npm:raise RuntimeError('Node.js is needed to build this update. Install Node.js 20.19+ or 22.12+ and restart this launcher.')
    lock_hash=hashlib.sha256((frontend/'package-lock.json').read_bytes()).hexdigest()
    dependency_stamp=cache/'dependency-lock.txt'
    dependencies_ready = False
    try:
        locked=json.loads((frontend/'package-lock.json').read_text())['packages']
        installed=json.loads((frontend/'node_modules/.package-lock.json').read_text())['packages']
        dependencies_ready=all(name=='' or (package.get('optional') and name not in installed) or installed.get(name,{}).get('version')==package.get('version') for name,package in locked.items())
    except (OSError,ValueError,KeyError):
        pass
    if not dependencies_ready:
        print('Installing the locked interface dependencies…',flush=True)
        try:
            subprocess.run([npm,'ci','--no-audit','--no-fund'],cwd=frontend,check=True)
        except subprocess.CalledProcessError as error:
            raise RuntimeError('Dependency installation failed. Close any running Vite/test terminals using this folder and retry; Windows can lock their executable files.') from error
    dependency_stamp.write_text(lock_hash)
    if not dev:
        print('Building the current interface…',flush=True)
        subprocess.run([npm,'run','build'],cwd=frontend,check=True)
        stamp.write_text(json.dumps({'fingerprint':fingerprint}))


def chrome_path():
    candidates=[shutil.which(name) for name in ('google-chrome','google-chrome-stable','chrome','chromium','chromium-browser')]
    if os.name=='nt':
        candidates += [str(Path(os.environ.get(key,''))/'Google/Chrome/Application/chrome.exe') for key in ('PROGRAMFILES','PROGRAMFILES(X86)','LOCALAPPDATA') if os.environ.get(key)]
    if sys.platform=='darwin':candidates.append('/Applications/Google Chrome.app/Contents/MacOS/Google Chrome')
    return next((path for path in candidates if path and Path(path).is_file()),None)


def open_browser(url,choice='chrome'):
    if choice=='none':return
    chrome=chrome_path() if choice=='chrome' else None
    try:
        if chrome:subprocess.Popen([chrome,url],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
        else:
            if choice=='chrome':print('Chrome was not found; opening the default browser.',flush=True)
            if not webbrowser.open(url):print('Open this URL in your browser: '+url,flush=True)
    except OSError:print('Open this URL in Chrome: '+url,flush=True)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--no-browser',action='store_true')
    parser.add_argument('--browser',choices=('chrome','system','none'),default='chrome')
    parser.add_argument('--dev',action='store_true')
    parser.add_argument('--port',type=int,default=8765)
    parser.add_argument('--check',action='store_true',help='Print compiler/browser availability and exit')
    args=parser.parse_args()
    if not 1<=args.port<=65535:parser.error('Port must be between 1 and 65535.')
    if args.dev and args.port!=8765:parser.error('Development mode uses API port 8765.')
    from backend.runner import compiler
    from backend.server import REVISION,serve
    if args.check:
        try: cc=compiler()
        except Exception as error:cc=str(error)
        print(json.dumps({'compiler':cc,'chrome':chrome_path(),'python':sys.version.split()[0]},indent=2));return 0
    ensure_frontend(args.dev)
    url=f'http://127.0.0.1:{args.port}'
    browser='none' if args.no_browser else args.browser
    try:
        with urlopen(url+'/api/health',timeout=1) as response:active=json.load(response)
    except (OSError,ValueError):active=None
    if active is not None:
        if isinstance(active,dict) and active.get('app')=='daa-lab-studio' and active.get('revision')==REVISION and not args.dev:
            print('Lab Studio is already running at '+url);open_browser(url,browser);return 0
        raise RuntimeError(f'Port {args.port} is occupied by an older instance or another app. Stop the old terminal with Ctrl+C, or choose --port 8766.')
    vite=None
    try:
        if args.dev:
            npm=shutil.which('npm.cmd' if os.name=='nt' else 'npm')
            vite=subprocess.Popen([npm,'run','dev','--','--strictPort'],cwd=ROOT/'frontend')
            url='http://127.0.0.1:5173'
        serve(args.port,on_ready=lambda:open_browser(url,browser))
    finally:
        if vite:
            vite.terminate()
            try:vite.wait(timeout=5)
            except subprocess.TimeoutExpired:vite.kill()
    return 0

if __name__=='__main__':
    try:sys.exit(main())
    except KeyboardInterrupt:pass
    except (RuntimeError,OSError,subprocess.SubprocessError) as error:
        print('Lab Studio could not start: '+str(error),file=sys.stderr)
        print('See lab-studio/USAGE.md for setup, or run python run.py --check.',file=sys.stderr)
        sys.exit(1)
