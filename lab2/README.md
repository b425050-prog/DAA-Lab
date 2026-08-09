<p align="center">
  <img src="assets/lab2_banner.gif" alt="DAA Lab 02 animated banner" width="100%">
</p>

<p align="center">
  <img alt="Language" src="https://img.shields.io/badge/LANGUAGE-C17-00599C?style=for-the-badge&logo=c&logoColor=white">
  <img alt="Questions" src="https://img.shields.io/badge/QUESTIONS-3-7C3AED?style=for-the-badge">
  <img alt="Experiments" src="https://img.shields.io/badge/EXPERIMENTS-DETERMINISTIC-22C55E?style=for-the-badge">
  <img alt="Visuals" src="https://img.shields.io/badge/VISUALS-SVG%20%2B%20GIF-F97316?style=for-the-badge">
  <img alt="Status" src="https://img.shields.io/badge/STATUS-READY-06B6D4?style=for-the-badge">
</p>

<h1 align="center">DAA Laboratory · Lab 02</h1>
<p align="center"><strong>Dictionary operations · 2-way vs 3-way merge sort · merging k sorted arrays</strong></p>
<p align="center">Student: <strong>Satyam Dhal</strong> · Instructor: <strong>Dr. Ajaya Kumar Dash</strong> · 04 August 2026</p>

<p align="center"><img src="assets/animated_divider.gif" width="100%" alt="animated divider"></p>

<p align="center"><strong><a href="../README.md">← Main repository</a></strong> · <a href="Problem-Sheet-Lab-02.pdf">Original problem sheet</a></p>

## Laboratory dashboard

| Question | What the code actually does | Required result |
|---|---|---|
| **[Q1 · Dictionary Operations](Q-1/README.md)** | Implements instrumented worst-case operations over six representations, infers growth classes experimentally, then produces separate theoretical and experimental SVG plots | Experimental table agrees with `O(1)`, `O(log n)`, `O(n)` theory |
| **[Q2 · Merge Sort vs Modified Merge Sort](Q-2/README.md)** | Lets you enter an array and choose 2-way or 3-way merge sort; separate programs plot theoretical and measured growth | Both are `Θ(n log n)` |
| **[Q3 · Merging k Sorted Arrays](Q-3/README.md)** | Lets you enter k sorted arrays and choose sequential or balanced merging; measures both and provides two detailed animations | Sequential `Θ(nk²)` vs balanced `Θ(nk log k)` |

<p align="center">
  <img src="assets/pipeline.gif" width="100%" alt="animated reproducible pipeline">
</p>

## What is different in this rebuild

This version separates **algorithm**, **experiment**, and **visualization** instead of printing a pre-filled complexity table and calling that validation.

- Q1 measures dominant operation counts over **96 input sizes from 64 through 65,536**, fits the measurements against `1`, `log₂ n`, and `n`, and prints the experimentally inferred complexity of all **42 operation/representation combinations**.
- Q1 has one C program for the experiment, one C program for the theoretical plot, and one C program that reads the measured `.dat` file and plots the experimental data.
- Q2 has a clean interactive sorter plus two independent plot programs. Its experimental plot uses **80 deterministic random arrays from n = 2,000 through 160,000** and verifies every sorted result before accepting a data point.
- Q3 has no graph, exactly as requested. Instead, it includes two looping GIF animations that walk through the same `k = 8`, `n = 4` example merge-by-merge and keep comparison/write counters visible.

<p align="center"><img src="assets/animated_divider.gif" width="100%" alt="animated divider"></p>

## Repository map

```text
DAA-Lab/
└── lab2/
    ├── README.md
    ├── Problem-Sheet-Lab-02.pdf
    ├── assets/
    │   ├── lab2_banner.gif
    │   ├── animated_divider.gif
    │   └── pipeline.gif
    │
    ├── Q-1/
    │   ├── README.md
    │   ├── q1_experimental_complexity.c
    │   ├── q1_plot_theoretical.c
    │   ├── q1_plot_experimental.c
    │   ├── q1_experimental_data.dat
    │   ├── q1_theoretical_complexity.svg
    │   ├── q1_experimental_complexity.svg
    │   └── q1_sample_output.txt
    │
    ├── Q-2/
    │   ├── README.md
    │   ├── q2_merge_sort_interactive.c
    │   ├── q2_plot_theoretical.c
    │   ├── q2_plot_experimental.c
    │   ├── q2_experimental_data.dat
    │   ├── q2_theoretical_complexity.svg
    │   ├── q2_experimental_complexity.svg
    │   └── q2_sample_output.txt
    │
    └── Q-3/
        ├── README.md
        ├── q3_merge_k_arrays.c
        ├── q3_method1_sequential.gif
        ├── q3_method2_balanced.gif
        └── q3_sample_output.txt
```

## Run from the repository sequence

Every program is standalone C17. No GNUPlot, Python, JavaScript, helper library, or platform-specific build system is required to regenerate the plots. The C plotting programs write SVG directly.

From the repository root, for example:

```bash
cd lab2/Q-1
gcc -std=c17 -O2 -Wall -Wextra -pedantic q1_experimental_complexity.c -lm -o q1_experimental_complexity
./q1_experimental_complexity

gcc -std=c17 -O2 -Wall -Wextra -pedantic q1_plot_experimental.c -lm -o q1_plot_experimental
./q1_plot_experimental
```

The committed `.dat`, `.svg`, `.gif`, and sample-output files are already present so the repository is readable immediately after upload.

<p align="center"><img src="assets/animated_divider.gif" width="100%" alt="animated divider"></p>

## Final conclusions

| Topic | Conclusion |
|---|---|
| Dictionary representations | No representation dominates every operation; ordering and link direction trade query speed against update cost. |
| 2-way vs 3-way merge sort | Recursion depth changes, but both perform linear merging work per level and therefore remain `Θ(n log n)`. |
| Merging k arrays | Repeatedly merging into one growing accumulator costs `Θ(nk²)`; balancing the merge tree reduces it to `Θ(nk log k)`. |

<p align="center"><strong>Built so that every asymptotic claim has a reproducible experiment or a step-by-step validation beside it.</strong></p>


<p align="center"><strong><a href="../README.md">← Back to the DAA Laboratory repository</a></strong></p>
