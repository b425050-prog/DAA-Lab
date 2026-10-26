"""Run from anywhere: python path/to/lab-studio/run.py [--no-browser] [--dev]."""
from pathlib import Path
import argparse
import os
import shutil
import subprocess
import threading
import webbrowser
from backend.server import serve

ROOT = Path(__file__).resolve().parent

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--no-browser', action='store_true')
    parser.add_argument('--dev', action='store_true', help='Run Vite with live reload alongside the API')
    parser.add_argument('--port', type=int, default=8765)
    args = parser.parse_args()
    if args.dev and args.port != 8765:
        parser.error('Development mode uses port 8765 for the Vite API proxy.')
    npm = shutil.which('npm.cmd' if os.name == 'nt' else 'npm')
    frontend = ROOT / 'frontend'
    if args.dev or not (frontend / 'dist' / 'index.html').exists():
        if not npm:
            parser.error('Install Node.js 20.19+ or 22.12+ to build the interface.')
        if not (frontend / 'node_modules').exists():
            subprocess.run([npm, 'ci', '--no-audit', '--no-fund'], cwd=frontend, check=True)
        if not args.dev:
            subprocess.run([npm, 'run', 'build'], cwd=frontend, check=True)
    vite = None
    url = f'http://127.0.0.1:{args.port}'
    try:
        if args.dev:
            vite = subprocess.Popen([npm, 'run', 'dev', '--', '--strictPort'], cwd=frontend)
            url = 'http://127.0.0.1:5173'
        if not args.no_browser:
            threading.Timer(1.5, lambda: webbrowser.open(url)).start()
        serve(args.port)
    finally:
        if vite:
            vite.terminate()
            try:
                vite.wait(timeout=5)
            except subprocess.TimeoutExpired:
                vite.kill()

if __name__ == '__main__':
    main()
