"""Same-origin local HTTP API and static frontend, using Python's standard library."""
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import json
import mimetypes
import os
from urllib.parse import urlsplit
from .runner import ROOT, CATALOG, BY_ID, RunError, compiler, run_problem, cancel_run
from .catalog import source_path
import hashlib
import re

REVISION = hashlib.sha256(b''.join(p.read_bytes() for p in sorted((ROOT/'backend').glob('*.py')))).hexdigest()[:16]


class Handler(BaseHTTPRequestHandler):
    server_version = 'LabStudio/1.0'

    def reply(self, status, content, content_type='application/json; charset=utf-8'):
        if isinstance(content, (dict, list)):
            content = json.dumps(content, ensure_ascii=True, allow_nan=False).encode()
        elif isinstance(content, str):
            content = content.encode('utf-8')
        self.send_response(status)
        self.send_header('Content-Type', content_type)
        self.send_header('Content-Length', str(len(content)))
        self.send_header('X-Content-Type-Options', 'nosniff')
        self.send_header('Cache-Control', 'no-store' if content_type.startswith('application/json') else 'no-cache')
        self.end_headers()
        try:
            self.wfile.write(content)
        except (BrokenPipeError, ConnectionResetError, ConnectionAbortedError):
            pass  # A cancelled browser request has already disconnected.

    def local_request(self):
        host = self.headers.get('Host', '').split(':')[0]
        if host not in ('127.0.0.1', 'localhost'):
            self.reply(403, {'error': 'Lab Studio accepts local requests only.'})
            return False
        origin = self.headers.get('Origin')
        if origin:
            try:
                parsed = urlsplit(origin)
                allowed = parsed.scheme == 'http' and parsed.hostname in ('127.0.0.1','localhost') and parsed.port in (self.server.server_port,5173,4173)
            except ValueError:
                allowed = False
            if not allowed:
                self.reply(403, {'error': 'Request origin is not allowed.'})
                return False
        return True

    def do_GET(self):
        if not self.local_request():
            return
        path = urlsplit(self.path).path
        if path == '/api/health':
            try:
                cc = compiler()
                self.reply(200, {'app':'daa-lab-studio','revision':REVISION,'status': 'ready', 'compiler': Path(cc).name, 'problems': len(BY_ID)})
            except RunError as error:
                self.reply(200, {'app':'daa-lab-studio','revision':REVISION,'status': 'compiler-missing', 'message': error.message, 'problems': len(BY_ID)})
        elif path == '/api/catalog':
            self.reply(200, CATALOG)
        elif path.startswith('/api/source/'):
            item = BY_ID.get(path.removeprefix('/api/source/'))
            if not item:
                self.reply(404, {'error': 'Unknown problem.'})
            elif not source_path(item['source']).is_file():
                self.reply(404, {'error':'The source file is missing from this checkout.'})
            else:
                self.reply(200, {'path': item['source'], 'text': source_path(item['source']).read_text(encoding='utf-8-sig')})
        elif path == '/api/audit':
            audit = ROOT / 'AUDIT.json'
            self.reply(200, json.loads(audit.read_text(encoding='utf-8')) if audit.exists() else {'status': 'Not yet run'})
        elif path.startswith('/api/'):
            self.reply(404, {'error': 'Unknown API route.'})
        else:
            public = (ROOT / 'frontend' / 'dist').resolve()
            file = (public / path.lstrip('/')).resolve()
            if not file.is_relative_to(public):
                self.reply(404, 'Not found', 'text/plain')
                return
            if not file.is_file():
                file = public / 'index.html' if '.' not in Path(path).name else file
            if file.is_file():
                mime = mimetypes.guess_type(file)[0] or 'application/octet-stream'
                if file.suffix == '.js':
                    mime = 'text/javascript'
                self.reply(200, file.read_bytes(), mime)
            else:
                self.reply(404, 'Build the frontend: cd frontend && npm ci && npm run build', 'text/plain')

    def do_POST(self):
        if not self.local_request():
            return
        route = urlsplit(self.path).path
        if route not in ('/api/run','/api/cancel'):
            self.reply(404, {'error': 'Unknown API route.'})
            return
        try:
            length = int(self.headers.get('Content-Length', '0'))
            if not 0 < length <= 100_000:
                raise RunError('Request body must be between 1 byte and 100 KB.', 413)
            body = json.loads(self.rfile.read(length))
            if not isinstance(body, dict):
                raise RunError('Provide a JSON object.')
            request_id = body.get('requestId')
            if request_id is not None and (not isinstance(request_id,str) or not re.fullmatch(r'[A-Za-z0-9_-]{8,80}',request_id)):
                raise RunError('Invalid request ID.')
            if route == '/api/cancel':
                self.reply(200, {'cancelled':cancel_run(request_id) if request_id else False})
                return
            if not isinstance(body.get('problemId'), str):
                raise RunError('Provide a problemId and input text.')
            self.reply(200, run_problem(body['problemId'], body.get('input', ''), body.get('countOnly') is True, request_id))
        except RunError as error:
            self.reply(error.status, {'error': error.message, 'output': error.output})
        except (ValueError, UnicodeError, TypeError):
            self.reply(400, {'error': 'Invalid JSON request.'})
        except Exception:
            import traceback
            traceback.print_exc()
            self.reply(500, {'error': 'An unexpected backend error occurred. Check the terminal for details.'})

    def log_message(self, format, *args):
        if os.environ.get('DAA_QUIET') != '1':
            super().log_message(format, *args)


def serve(port=8765, on_ready=None):
    server = ThreadingHTTPServer(('127.0.0.1', port), Handler)
    server.daemon_threads = True
    print(f'Lab Studio: http://127.0.0.1:{port}', flush=True)
    if on_ready:
        on_ready()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()
