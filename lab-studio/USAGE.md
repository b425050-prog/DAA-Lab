# DAA Lab Studio

A personal browser interface for all 64 problems in the DAA-Lab repository. The algorithms execute as compiled C17 programs on your machine. Python connects them to a React interface.

Everything added for this project lives in `lab-studio/`. The repository's existing files and README are unchanged. The interface uses the original C sources as read-only resources and 15 corrected/adapted overrides. Builds and runs use disposable copies; nothing is written into the original lab directories.

## Start

You need Python 3.10+, GCC or Clang on PATH, and Node.js 20.19+ or 22.12+. No Python packages are required.

From a terminal:

```sh
cd lab-studio
python run.py
```

On Windows, double-click `start.cmd` to open Chrome. **Keep the launcher terminal open while using the app**; closing it stops the backend. If Chrome is not installed, the launcher uses your default browser. The first launch installs the frontend's locked dependencies and builds the interface. Later launches rebuild when the interface source changes, and reuse matching installed dependencies. It opens `http://127.0.0.1:8765` in your browser. Compilation happens automatically on the first run of each problem and is cached under `.cache/`. GCC/Clang is found on PATH or in common Windows MSYS2/LLVM locations. Stop the server with Ctrl+C.

To prepare explicitly:

```sh
cd lab-studio/frontend
npm ci
npm run build
cd ..
python run.py --no-browser
```

Python may be named `python3` on Linux/macOS. Set the `CC` environment variable to a compiler executable path if needed. On Windows with MSYS2 UCRT64 installed, add `C:\msys64\ucrt64\bin` to PATH in the terminal running this project. Use the compiler installation appropriate to your machine.

## Use the workbench

1. Choose a lab and question, or search by algorithm name.
2. Edit the example in **Fields** or **Raw input**. Counts must agree with the data. The **?** button explains the contract. Import/download plain-text input with the buttons above the editor.
3. Click **Run algorithm**, or press Ctrl+Enter / Command+Enter.
4. Inspect **Visualization**, **Results**, **Experiments**, **Notes**, or **Source**.

Graph problems show vertices/edges or grids, a step slider, playback speed, zoom, and SVG download. Vertex dragging changes the layout; it does not change algorithm input. Bipartiteness has an edge-entry control that updates the edge count. Rejected edges appear as dashed attempted edges and never influence the C program's maintained bipartite graph. Vertex colours use the final valid bipartition throughout playback. Other graph colours illustrate connected components of the displayed state; numerical answers always come from C.

Use **Sync counts with data** after changing array values or edge rows. Matrix problems offer a cell grid and an order selector that preserves the upper-left values of both matrices when resizing. Special-pattern matrix cells are linked so edits preserve the required recursive structure. **Cancel run** stops the active execution.

DP programs that return matrices show heatmap tables. Other programs show returned sequences, numerical charts, structured tables, or their actual text transcript. Three-peg and four-peg Hanoi have disk playback. Programs without recorded intermediate states show the final output. The interface does not invent algorithm traces. If an optional JSON trace exceeds the interactive output budget, the successful calculation and its console answer remain available, with a visible explanation.

**Experiments** contains the repository's recorded `.dat` measurements; these are explicitly separated from the current run. Click legend items to toggle series, enable logarithmic axes where appropriate, or download CSV data. Large recorded datasets are sampled evenly to 400 points with the original row count displayed. Log axes omit nonpositive points. Runtime is process wall time, including process launch and output polling; use algorithm work counters for meaningful complexity comparisons.

**Results** offers tables, the complete console transcript, and full JSON export. Exact JSON integers above JavaScript's safe-integer limit are represented by decimal strings. Large tables and graph drawings have display limits; exports retain full results. File sorting exposes downloads for the actual generated input and sorted output; file paths are supplied internally.

Input drafts are saved in browser local storage on this device. Export workspace produces a JSON backup; use the adjacent import button to restore it. No account or database is needed. The original notes are display copies from the recorded source commit; adapted inputs and project corrections are documented here and in `CORRECTIONS.md`.

## Inputs adapted for the interface

| Problem | Input |
| --- | --- |
| Lab 1 Q1 | minimum n, maximum n, integer step; `2 <= min <= max <= 100` |
| Lab 1 Q2 | toss count `1..100000`, biased heads probability `0..1`, positive seed |
| Lab 1 Q3 | array length, then the array; compares both original bubble-sort routines on identical data |
| Lab 1 Q4 | disk count `0..12`; emits moves and exact growth data |
| Lab 1 Q6 | random element count, maximum generated value, deterministic seed |
| Lab 2 Q1 | maximum measured input size `128..65536`; runs the original representation experiment |
| Lab 5 Q3/Q4 | element count, then seed; input/output files stay inside a disposable run folder |

All other problems retain their documented stdin order, including the nested Lab 7 layout. Blank lines are meaningful for the modern string programs. Modern programs validate their own input bounds. For legacy programs, the workbench also checks token counts and numeric types before calling scanf.

Interactive limits: most legacy sizes are at most 2,000; selection-sort invariant playback supports 150; matrices support order 32; integer recursive matrix entries are limited to ±1,000,000; FFT vectors support 2,000; merged arrays support 10,000 total values; knapsack supports 1,000,000 DP cells; legacy LCS supports two strings of at most 1,000 characters; matrix-chain dimension count supports 200. Other legacy numbers are bounded to ±1,000,000,000. These envelopes prevent unsafe allocations or arithmetic in older CLI examples. A run stops after 12 seconds or 2 MB of output. Modern puzzle programs offer **Count only** where supported.

## Development and verification

```sh
python run.py --dev --no-browser
# API: 127.0.0.1:8765; Vite: 127.0.0.1:5173
```

```sh
python -m unittest discover -s tests -p "test_*.py" -v
python tools/audit.py --full
cd frontend
npm test
```

The audit checks all repository C translation units with the project overrides applied, executes all 64 examples, runs focused arithmetic/HTTP regressions, executes the independent repository oracle suites for Labs 7–10, and runs the earlier C experimental validators. All evidence-writing validators run in temporary copies. The saved `AUDIT.json` contains exact outcomes and scope; a passing audit is evidence for these cases, not a proof for every possible input.

```text
lab-studio/
  frontend/          React + TypeScript + Vite, Chart.js, SVG visuals
  backend/           local HTTP bridge and C compiler/runner
  engine/overrides/  only 15 corrected/adapted C sources and headers
  engine/provenance.json  original file hashes and source commit
  catalog.json       compact 64-problem metadata and input examples
  backend/catalog.py reads original notes/datasets in place
  tests/             HTTP, arithmetic, launch and execution regressions
  frontend/src/tests/ component/integration checks with real C outputs
  tools/audit.py     repeatable verification
  run.py             launch/build helper
```

Do not open `frontend/index.html` directly in Chrome: native C execution requires the local server. You can also explicitly choose a browser:

```sh
python run.py --browser chrome
python run.py --browser system
python run.py --check
```

If another instance is already running, the launcher reuses a matching instance. When an older backend occupies the port, stop its terminal with Ctrl+C and restart, or choose `--port 8766`.

The local server binds to loopback and serves a single-origin interface. It is intended for personal use on your machine. A static website host alone cannot execute the native C programs.
