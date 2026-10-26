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

    def test_every_problem_and_source_over_http(self):
        from backend.runner import CATALOG
        for item in CATALOG['problems']:
            with self.subTest(problem=item['id']):
                status,result=self.request('/api/run',{'problemId':item['id'],'input':item['sample']},headers={'Content-Type':'application/json','User-Agent':'Mozilla/5.0 Chrome/130.0.0.0'})
                self.assertEqual(status,200,result)
                self.assertEqual(result['problemId'],item['id'])
                self.assertIsInstance(result['stdout'],str)
                status,source=self.request('/api/source/'+item['id'])
                self.assertEqual(status,200)
                self.assertIn('main(',source['text'].replace('main (','main('))

    def test_production_page_and_assets_are_served(self):
        import re
        with urlopen(self.base,timeout=10) as response:
            self.assertEqual(response.status,200);html=response.read().decode()
        resources=re.findall(r'(?:src|href)="(/assets/[^\"]+)"',html)
        self.assertTrue(resources)
        for resource in resources:
            with urlopen(self.base+resource,timeout=10) as response:
                self.assertEqual(response.status,200)
                self.assertGreater(len(response.read()),0)

    def test_cancel_route_and_invalid_request_ids(self):
        status,result=self.request('/api/cancel',{'requestId':'not-running-123'})
        self.assertEqual(status,200);self.assertFalse(result['cancelled'])
        self.assertEqual(self.request('/api/run',{'problemId':'L6Q5','input':'5','requestId':[]})[0],422)

if __name__=='__main__':unittest.main()
