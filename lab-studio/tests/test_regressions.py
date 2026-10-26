"""Focused regressions for arithmetic, input contracts, and result provenance."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from backend.runner import CATALOG, SOURCES, RunError, compiler, run_problem, validate_input, graph_result


class Regressions(unittest.TestCase):
    def test_arithmetic_headers(self):
        # Exercise the copied headers directly, including values above the UI's
        # conservative legacy numeric envelope.
        program = r'''
        #include <assert.h>
        #include "lab6/Q-6/q6_knapsack.h"
        #include "lab6/Q-8/q8_matrix_chain.h"
        int main(void) {
            size_t weights[]={1,1}; unsigned char chosen[2]; Q6Result k;
            long long overflow[]={LLONG_MAX,1};
            assert(!q6_knapsack(weights,overflow,2,2,chosen,&k));
            long long exact[]={LLONG_MAX,0};
            assert(q6_knapsack(weights,exact,2,2,chosen,&k) && k.maximum_profit==LLONG_MAX);
            long long negative[]={LLONG_MIN,-1};
            assert(q6_knapsack(weights,negative,2,2,chosen,&k) && k.maximum_profit==0);
            size_t split[9]={0}; Q8Result r;
            size_t huge[]={1000000000,1000000000,1000000000,1000000000};
            assert(!q8_matrix_chain(huge,4,split,&r));
            size_t normal[]={10,20,30,40};
            assert(q8_matrix_chain(normal,4,split,&r) && r.minimum_cost==18000);
            if(sizeof(size_t)==8) {
                size_t boundary[]={(size_t)ULLONG_MAX,1,1};
                assert(q8_matrix_chain(boundary,3,split,&r) && r.minimum_cost==ULLONG_MAX);
                size_t alternate[]={4294967296ULL,1,4294967296ULL,1};
                assert(q8_matrix_chain(alternate,4,split,&r) && r.minimum_cost==8589934592ULL);
            }
            return 0;
        }'''
        with tempfile.TemporaryDirectory(prefix='daa-regression-') as temp:
            p=Path(temp); (p/'test.c').write_text(program)
            exe=p/('test.exe' if sys.platform=='win32' else 'test')
            subprocess.run([compiler(),'-std=c17','-Wall','-Wextra','-Wpedantic','-Werror','-I',str(SOURCES),str(p/'test.c'),'-o',str(exe)],check=True,capture_output=True)
            subprocess.run([str(exe)],check=True,timeout=5)

    def test_missing_matrix_and_selection_reads(self):
        for pid,text in [('L3Q4','2\n1 2\n'),('L3Q5','2\n1 2\n'),('L3Q6','3\n1 2\n')]:
            with self.subTest(pid=pid), self.assertRaises(RunError):
                run_problem(pid,text)

    def test_file_sort_never_accepts_paths(self):
        item=next(p for p in CATALOG['problems'] if p['id']=='L5Q3')
        with self.assertRaises(RunError):
            validate_input(item,'10\n425050\n../../README.md\noutput.txt\n')
        self.assertEqual(validate_input(item,'10\n425050\n'),'10\n425050\ninput.txt\noutput.txt\n')

    def test_rejected_edges_and_duplicates(self):
        result=run_problem('L10Q1','3 5\n1 2\n2 3\n3 1\n1 1\n1 2\n')
        self.assertEqual(result['graph']['accepted'],[True,True,False,False,True])
        colours=result['graph']['colours']
        for edge,ok in zip(result['graph']['edges'],result['graph']['accepted']):
            if ok:self.assertNotEqual(colours[edge[0]-1],colours[edge[1]-1])

    def test_signed_maxima_and_deletion_moment(self):
        result=run_problem('L10Q4','2 1\n-5 -2\n1 2\n1 2\n')
        self.assertEqual(result['graph']['maxima'],[-2,0])
        self.assertEqual(result['graph']['deletions'],[1,2])

    def test_json_preserves_large_integers(self):
        result=run_problem('L8Q9','9007199254740993 1 1 1\n')
        self.assertIn('9007199254740993',json.dumps(result['trace']))

    def test_legacy_string_inputs_and_extra_values(self):
        self.assertIn('LCS length',run_problem('L6Q7','ABC\nAC\n')['stdout'])
        with self.assertRaises(RunError):run_problem('L3Q3','2\n1 2 3\n')

    def test_power_iteration_rejects_a_rotation(self):
        result=run_problem('L6Q2','2\n0 -1\n1 0\n1 0\n0 1\n')
        self.assertIn('No converged real eigenpair',result['stdout'])
        self.assertNotIn('Eigenvalue estimate (power iteration):',result['stdout'])

    def test_seed_and_matrix_numeric_envelopes(self):
        for pid,text in [('L1Q2','10\n.7\n-1\n'),('L1Q2','10\n.7\n3.4\n'),('L5Q4','10\n-1\n'),('L3Q4','1\n1000001\n1\n')]:
            with self.subTest(pid=pid),self.assertRaises(RunError):run_problem(pid,text)


if __name__=='__main__':unittest.main()
