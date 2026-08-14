#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CC="${CC:-gcc}"
CFLAGS="-std=c17 -O2 -Wall -Wextra -pedantic"

if ! command -v gnuplot >/dev/null 2>&1; then
    echo "Error: gnuplot is required to regenerate SVG plots." >&2
    exit 1
fi

run_plot() {
    local dir="$1"
    local generator="$2"
    local plot="$3"
    local libs="${4:-}"

    echo "[+] $dir"
    (
        cd "$ROOT/$dir"
        $CC $CFLAGS "$generator.c" $libs -o "$generator"
        "./$generator"
        gnuplot "$plot"
        rm -f "$generator" *.dat
    )
}

run_plot "Q-1" "binary_ternary_generate_data" "binary_ternary_plot.gp"
run_plot "Q-2" "defective_coin_generate_data" "defective_coin_plot.gp" "-lm"
run_plot "Q-3" "max_min_generate_data" "max_min_plot.gp"
run_plot "Q-4" "strassen_generate_data" "strassen_plot.gp" "-lm"
run_plot "Q-5" "special_matrix_generate_data" "special_matrix_plot.gp"
run_plot "Q-6" "selection_sort_generate_data" "selection_sort_plot.gp"

echo "All Lab 03 SVG plots regenerated successfully."
