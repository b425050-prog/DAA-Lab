#!/usr/bin/env python3
"""Build all seven C programs with GCC or Clang; no third-party packages."""
import os
from pathlib import Path
import shlex
import subprocess

ROOT = Path(__file__).resolve().parents[1]
NAMES = [
    "q1_coin_triangle", "q2_egg_dropping_dp", "q3_reves_puzzle",
    "q4_security_switches", "q5_moving_target", "q6_best_time_alive",
    "q7_matrix_chain_dp",
]
SUFFIX = ".exe" if os.name == "nt" else ""


def executable(q):
    directory = ROOT / f"Q-{q}" / "output" if os.name == "nt" else ROOT / "bin" / f"Q-{q}"
    return directory / (NAMES[q - 1] + SUFFIX)


def build():
    compiler = shlex.split(os.environ.get("CC", "gcc"))
    flags = ["-std=c17", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-Werror"]
    flags += shlex.split(os.environ.get("EXTRA_CFLAGS", ""))
    for q, name in enumerate(NAMES, 1):
        dest = executable(q)
        dest.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(compiler + flags + [str(ROOT / f"Q-{q}" / (name + ".c")),
                       "-o", str(dest)], check=True)
    print("Built all 7 C17 programs with warnings treated as errors.")


if __name__ == "__main__":
    build()
