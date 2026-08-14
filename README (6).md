<div align="center">

# Design and Analysis of Algorithms - Lab 03

### Divide & Conquer • Search • Matrix Algorithms • Loop Invariants

![Language](https://img.shields.io/badge/Language-C17-00599C?style=for-the-badge&logo=c)
![Plots](https://img.shields.io/badge/Plots-GNUPlot-2F4F4F?style=for-the-badge)
![Course](https://img.shields.io/badge/DAA-Lab%2003-6f42c1?style=for-the-badge)

**BTech (CS-B and CE), 3rd Semester • August 11, 2026**  
**Instructor: Dr. Ajaya Kumar Dash**

</div>

---

## Lab workflow

> **Implement → verify → measure → visualize → explain**

Each problem folder contains a clean interactive C implementation, an independent data generator where validation is useful, a GNUPlot script, a committed SVG graph, and a detailed explanation.

## Problems

| # | Problem | Main idea | Key result |
|---:|---|---|---|
| 1 | [Binary vs Ternary Search](Q-1/) | Search-interval reduction | Binary uses fewer key probes |
| 2 | [Search the Defective Coin](Q-2/) | Balance-scale D&C | `O(log2 n + c)` weighings |
| 3 | [Max and Min using D&C](Q-3/) | Tournament-style combine | at most `3n/2` comparisons |
| 4 | [Strassen Matrix Multiplication](Q-4/) | Seven recursive products | `Theta(n^2.8074)` |
| 5 | [Special-Pattern Matrix Product](Q-5/) | Two-product block identity | `Theta(n^2)` |
| 6 | [Loop Invariants in Sorting](Q-6/) | Selection sort proof | best = worst = `Theta(n^2)` |

## Repository map

```text
lab3/
├── README.md
├── 2026_Week3_DAA_Lab_03.pdf
├── .gitignore
├── Makefile
├── scripts/
│   └── regenerate_all.sh
├── Q-1/
├── Q-2/
├── Q-3/
├── Q-4/
├── Q-5/
└── Q-6/
```

Every `Q-*` directory contains:

```text
README.md
<algorithm>.c
<algorithm>_generate_data.c
<plot>.gp
<final_graph>.svg
```

Generated `.dat` files are intentionally ignored. They can always be recreated from the supplied generator programs.

## Quick start

From `lab3/`:

```bash
make all
```

This compiles all twelve C sources into `bin/`.

To regenerate every graph (requires GNUPlot):

```bash
./scripts/regenerate_all.sh
```

## Problem sheet

The original assignment is included as [`2026_Week3_DAA_Lab_03.pdf`](2026_Week3_DAA_Lab_03.pdf).

---

<div align="center">

### Clean code • reproducible experiments • explicit complexity validation

[Back to repository root](../README.md)

</div>
