"""Exercise the actual local HTTP contract, including failure responses."""
import json
from pathlib import Path
import sys
import threading
from urllib.error import HTTPError
from urllib.request import Request, urlopen
import unittest
from http.server import ThreadingHTTPServer
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from backend.server import Handler

class HTTPContract(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.server=ThreadingHTTPServer(('127.0.0.1',0),Handler)
        cls.thread=threading.Thread(target=cls.server.serve_forever,daemon=True);cls.thread.start()
        cls.base=f'http://127.0.0.1:{cls.server.server_port}'

    @classmethod
    def tearDownClass(cls):
        cls.server.shutdown();cls.server.server_close();cls.thread.join()

    def request(self,path,data=None,headers=None):
        req=Request(self.base+path,data=json.dumps(data).encode() if data is not None else None,headers=headers or {})
        try:
            with urlopen(req,timeout=30) as response:return response.status,json.load(response)
        except HTTPError as error:return error.code,json.load(error)

    def test_catalog_and_run(self):
        status,catalog=self.request('/api/catalog');self.assertEqual(status,200);self.assertEqual(len(catalog['problems']),64)
        status,out=self.request('/api/run',{'problemId':'L8Q3','input':'ABC\nAC\n'})
        self.assertEqual(status,200);self.assertEqual(out['trace']['result'],2);self.assertEqual(out['trace']['subsequence'],'AC')

    def test_failure_contract(self):
        status,out=self.request('/api/run',{'problemId':'L10Q1','input':'3 1\n1 4\n'})
        self.assertEqual(status,422);self.assertIn('error',out)
        self.assertEqual(self.request('/api/run',{'problemId':'unknown','input':''})[0],404)
        self.assertEqual(self.request('/api/source/../../README.md')[0],404)
        self.assertEqual(self.request('/api/not-a-route')[0],404)

    def test_local_origin(self):
        self.assertEqual(self.request('/api/run',{'problemId':'L8Q3','input':'A\nA\n'},headers={'Origin':'https://example.com'})[0],403)
        self.assertEqual(self.request('/api/catalog',headers={'Host':'example.com'})[0],403)

if __name__=='__main__':unittest.main()
