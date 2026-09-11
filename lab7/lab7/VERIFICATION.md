# Verification report

## Local run

- Date: September 11, 2026.
- Platform: Windows, GCC 16.1.0 (MSYS2 UCRT64).
- Compile flags: `-std=c17 -O2 -Wall -Wextra -Wpedantic -Werror`.
- Result: **all 7 programs compiled; all 341 program executions passed**.
- The reorganized `Q-*` paths were retested, and `build_windows.bat` successfully compiled all seven programs into their question folders' `output/` directories.
- The count includes valid and intentionally invalid inputs, plus independent checks performed inside Python.

Run again from the `lab7/` folder:

```bash
python3 tools/test_all.py
```

On Windows use `py -3 tools/test_all.py` or `python tools/test_all.py`. The test runner compiles before testing and uses only Python's standard library. Randomized cases use a fixed seed for reproducibility.

## What was checked

| Problem | Independent evidence |
|---|---|
| Coins | Replay 1â€“25 row constructions; compare final coordinate sets; exhaust candidate translations for 1â€“12 rows; check a billion-row count |
| Eggs | Compare 1â€“5 eggs and even floor counts 0â€“40 to classical minimax DP, including the reported first drop; verify 2/100 and large boundaries; replay adaptive strategy for every threshold 0â€“100 |
| Hanoi | Replay every move for 0â€“12 and 64 disks; compare 0â€“5 disks with graph BFS; require exactly 33 moves for 8 disks |
| Switches | Verify toggle legality and printed states for 0â€“9 switches; compare with BFS distance; test 63-bit count and trace cap |
| Target | Independently propagate every surviving position for 2â€“50 and 2000 spots; require no survivor after the final shot |
| Scientists | Direct overlap oracle on fixed and seeded random lifetimes; tied maxima, touching intervals, empty input, zero-length lives, negative years, and signed 64-bit endpoints |
| Matrix chain | Exhaust all parenthesizations of small chains; parse and evaluate the emitted expression; preserve matrix order; test one matrix, ties, overflow, and a representable answer with an overflowing alternative |
| Input handling | Missing and malformed input, negative sizes, excessively long tokens, extra data, reversed lifetimes, zero dimensions, and documented range limits |

Tests using exhaustive search are intentionally small; they complement the proofs in the individual question READMEs, rather than establishing all-size correctness on their own. Large-output traces are bounded by the input contracts.

## GitHub CI

The included workflow is configured to compile and run the same tests with GCC and Clang on Ubuntu, adding AddressSanitizer and UndefinedBehaviorSanitizer. It triggers on pushes, pull requests, or manual dispatch. No repository secrets are needed.

**Remote CI has not been run by this local package creation.** The reported passing result above is the local GCC run. GitHub will show the actual workflow status after the repository is pushed.
