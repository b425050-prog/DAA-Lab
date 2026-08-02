<p align="center">
  <img src="assets/daa-banner.svg" alt="Design and Analysis of Algorithms Laboratory" width="100%">
</p>

<h1 align="center">Design and Analysis of Algorithms Laboratory</h1>
<p align="center"><strong>A clean, reproducible C and GNUPlot laboratory repository</strong></p>

## Student and course details

| Field | Information |
|---|---|
| **Student** | Satyam |
| **Student ID** | B425050 |
| **Branch** | Computer Science and Engineering |
| **Institute** | International Institute of Information Technology, Bhubaneswar |
| **Course** | Design and Analysis of Algorithms Laboratory |
| **Instructor** | Dr. Ajaya Kumar Dash |
| **Semester** | 3rd Semester |

## Laboratory index

| Lab | Date | Topic | Status |
|---|---:|---|:---:|
| **[Lab 01](lab1/README.md)** | 28 July 2026 | Growth functions, probability simulation, sorting analysis, recursion, binary search and uniqueness | ✅ Complete |

Future laboratories can be added beside `lab1` and linked from this table, so this README remains the single navigation point for the entire course.

## Repository structure

```text
DAA-Lab-B425050/
├── README.md
├── .gitignore
├── .gitattributes
├── Makefile
├── scripts/
│   ├── build_windows.bat
│   ├── regenerate_all.sh
│   └── regenerate_all_windows.bat
├── assets/
│   └── daa-banner.svg
└── lab1/
    ├── README.md
    ├── Problem-Sheet-Lab-01.pdf
    ├── Q-1/
    ├── Q-2/
    ├── Q-3/
    ├── Q-4/
    ├── Q-5/
    └── Q-6/
```

Q-1 to Q-4 keep their source code, executables, data and SVG deliverables inside the question folders. The GNUPlot commands are embedded directly in each C program, so no separate plotting-script folder is required. A short temporary command file is created only while GNUPlot runs and is deleted immediately afterward. Q-5 and Q-6 contain only the requested C solutions, executables and explanations because those questions do not ask for graphs.

## Build and regenerate

### GNU/Linux

```bash
make
./scripts/regenerate_all.sh
```

### Windows with MinGW GCC and GNUPlot in `PATH`

```bat
scripts\build_windows.bat
scripts\regenerate_all_windows.bat
```

The included executables are statically compiled for x86-64 GNU/Linux. On Windows, run `scripts\build_windows.bat` once to replace them with native Windows executables. GNUPlot must be installed and available in `PATH` for automatic SVG generation.

## Repository conventions

- C programs use a fixed random seed where reproducibility matters, so the submitted `.dat` files can be regenerated.
- For Q-1 to Q-4, plotting commands are embedded in the C source. Running the executable regenerates the data, creates the SVG through GNUPlot and opens the graph automatically; no external `.plt` file is needed.
- Mathematical notation is rendered with readable symbols such as `n²`, `n³`, `log₂ n`, `Θ(n²)` and `2ⁿ − 1`; no LaTeX source is required to read the answers.
- Q-1 to Q-4 use enough data points, clear legends, grids and carefully chosen scales so the required curves remain distinguishable.
- Generated deliverables are deliberately version-controlled because they are part of the laboratory submission.

## Lab 01 at a glance

| Question | Main result |
|---|---|
| [Q-1](lab1/Q-1/README.md) | Functions ordered by increasing asymptotic growth |
| [Q-2](lab1/Q-2/README.md) | Fair-coin probability approaches 0.5; biased coin approaches its selected bias |
| [Q-3](lab1/Q-3/README.md) | Both versions are compared on Fisher–Yates random permutations |
| [Q-4](lab1/Q-4/README.md) | Towers of Hanoi requires `2ⁿ − 1` moves and has exponential growth |
| [Q-5](lab1/Q-5/README.md) | The first 1 is found by binary search |
| [Q-6](lab1/Q-6/README.md) | Pairwise uniqueness checking has quadratic worst-case growth |

---

<p align="center">B425050 · CSE · IIIT Bhubaneswar</p>
