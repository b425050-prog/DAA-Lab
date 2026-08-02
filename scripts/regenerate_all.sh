#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"
cd "$repo_root"

if ! command -v gcc >/dev/null 2>&1; then
  echo "ERROR: GCC was not found." >&2
  exit 1
fi

if ! command -v gnuplot >/dev/null 2>&1; then
  echo "ERROR: GNUPlot was not found. Install it and add it to PATH." >&2
  exit 1
fi

make
export DAA_NO_OPEN=1

./lab1/Q-1/q1_growth_order.exe
./lab1/Q-2/q2_coin_toss.exe
./lab1/Q-3/q3_bubble_sort.exe
./lab1/Q-4/q4_towers_of_hanoi.exe

echo "Data files and SVG graphs for Q-1 to Q-4 were regenerated."
echo "Q-5 and Q-6 are interactive solutions and do not require graphs."
