"""Integration regressions for the reported execution/launch failures."""
from pathlib import Path
import subprocess
import sys
import tempfile
import threading
import unittest
from unittest.mock import patch
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from backend.runner import BY_ID,RunError,build,compiler,compiler_identity,execute_process,run_problem
import run as launcher

class Reliability(unittest.TestCase):
    def test_bom_and_windows_line_endings(self):
        result=run_problem('L8Q3','\ufeffABC\r\nAC\r\n')
        self.assertEqual(result['trace']['result'],2)
        self.assertEqual(result['input'],'ABC\nAC\n')

    def test_file_sort_export_can_be_used_as_input_again(self):
        result=run_problem('L5Q3','5\n425050\n')
        self.assertEqual(result['input'],'5\n425050\n')
        self.assertEqual(run_problem('L5Q3',result['input'])['files'],result['files'])

    def test_large_dp_keeps_successful_answer_when_trace_is_too_large(self):
        text='A'*800+'\n'+'A'*800+'\n'
        result=run_problem('L8Q3',text)
        self.assertIn('800',result['stdout'])
        self.assertTrue(result['trace'] is not None or result['warnings'])

    def test_large_graph_keeps_structured_results(self):
        result=run_problem('L10Q1','201 1\n1 201\n')
        self.assertEqual(result['graph']['kind'],'large')
        self.assertEqual(result['graph']['accepted'],[True])
        self.assertEqual(len(result['graph']['colours']),201)

    def test_cancellation_terminates_the_process(self):
        event=threading.Event()
        with tempfile.TemporaryDirectory() as temp:
            timer=threading.Timer(.1,event.set);timer.start()
            try:
                with self.assertRaisesRegex(RunError,'cancelled'):
                    execute_process(Path(sys.executable),'',Path(temp),['-c','import time;time.sleep(5)'],event)
            finally:timer.cancel()

    def test_failed_build_does_not_poison_the_cache(self):
        cc=compiler();compiler_identity(cc)
        with tempfile.TemporaryDirectory() as temp,patch('backend.runner.CACHE',Path(temp)):
            with patch('backend.runner.subprocess.run',return_value=subprocess.CompletedProcess([],1,b'',b'compiler failure')):
                with self.assertRaises(RunError):build(BY_ID['L1Q5'])
            self.assertFalse(list(Path(temp).rglob('*.exe')))
            executable=build(BY_ID['L1Q5'])
            self.assertTrue(executable.is_file());self.assertGreater(executable.stat().st_size,0)

    def test_frontend_fingerprint_changes_with_source(self):
        original=launcher.ROOT
        with tempfile.TemporaryDirectory() as temp,patch.object(launcher,'ROOT',Path(temp)):
            root=Path(temp)/'frontend';(root/'src').mkdir(parents=True)
            for name in ('package.json','package-lock.json','vite.config.ts','tsconfig.json','index.html'):(root/name).write_text('initial')
            a=launcher.frontend_fingerprint();(root/'src/App.tsx').write_text('updated')
            self.assertNotEqual(a,launcher.frontend_fingerprint())

    def test_chrome_launch_arguments(self):
        with patch.object(launcher,'chrome_path',return_value='C:/Program Files/Google/Chrome/Application/chrome.exe'),patch.object(launcher.subprocess,'Popen') as launch:
            launcher.open_browser('http://127.0.0.1:8765','chrome')
            self.assertEqual(launch.call_args.args[0],['C:/Program Files/Google/Chrome/Application/chrome.exe','http://127.0.0.1:8765'])

    def test_launcher_rebuilds_updates_without_reinstalling_matching_dependencies(self):
        import json
        with tempfile.TemporaryDirectory() as temp,patch.object(launcher,'ROOT',Path(temp)),patch.object(launcher.shutil,'which',return_value='npm'),patch.object(launcher.subprocess,'run') as command:
            root=Path(temp)/'frontend';(root/'src').mkdir(parents=True);(root/'dist').mkdir();(root/'node_modules').mkdir()
            for name in ('package.json','vite.config.ts','tsconfig.json','index.html'):(root/name).write_text('initial')
            package={'packages':{'':{'version':'1'},'node_modules/example':{'version':'1.0'}}}
            (root/'package-lock.json').write_text(json.dumps(package));(root/'node_modules/.package-lock.json').write_text(json.dumps(package));(root/'dist/index.html').write_text('old build')
            launcher.ensure_frontend()
            self.assertEqual(command.call_args.args[0],['npm','run','build'])
            self.assertEqual(command.call_count,1)
            command.reset_mock();launcher.ensure_frontend();command.assert_not_called()
            (root/'src/App.tsx').write_text('new build needed');launcher.ensure_frontend();command.assert_called_once()

if __name__=='__main__':unittest.main()
