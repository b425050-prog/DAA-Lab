from pathlib import Path
import subprocess
from build import ROOT, executable

for q in range(1, 8):
    folder = ROOT / f"Q-{q}"
    data = (folder / f"q{q}_sample_input.txt").read_text()
    result = subprocess.run([str(executable(q))], input=data, text=True,
                            capture_output=True, check=True, timeout=20)
    (folder / f"q{q}_sample_output.txt").write_text(result.stdout, encoding="utf-8")
print("Captured all seven sample outputs.")
