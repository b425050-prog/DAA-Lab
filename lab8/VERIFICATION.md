# Lab 08 verification

Local run: 4 October 2026. Compiler: GCC, strict C17 (`-Wall -Wextra -Wpedantic -Werror`).

- 18/18 C sources built successfully.
- 993 program executions passed: independent oracles, reconstructed witness checks, malformed inputs, numeric boundaries, and deterministic scaling experiments.
- Q1: breadth-first shortest paths; Q2: coin-multiplicity enumeration.
- Q3: subsequence enumeration and a rolling-row length oracle.
- Q4: exhaustive subsequences and patience sorting; Q5: exhaustive subsequences and a Fenwick maximum oracle.
- Q6: rolling-row distance plus complete forward edit-script replay.
- Q7: compositions and memoized revenue, with exact piece-length/revenue replay (including negative prices).
- Q8: exhaustive BST shapes on small cases, recursive interval oracle, independent depth-weighted tree cost, and the canonical 2.75 instance.
- Q9: Python arbitrary-precision trajectories enforce the C uint64_t boundary and reproduce each interval row, overflow, and step-limit status.
- Operation counters checked against exact counts where a closed form exists.
- Every chart point was accepted only after its corresponding oracle passed.

Animation state values and reconstructed answers come from the validated C sample traces. Q2's intermediate coin-stage counts are also checked against the C result during generation. Collatz plots report observations on the specified finite interval, with no claimed bound for arbitrary starts.

Run `python tools/validate.py` to rebuild and repeat this report. Run `python tools/generate_visuals.py` to refresh the visual assets (requires Pillow).
