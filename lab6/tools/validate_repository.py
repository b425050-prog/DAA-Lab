"""Validate Lab 06 documentation links and committed visual evidence."""

from pathlib import Path
import re
import sys
import xml.etree.ElementTree as ET

from PIL import Image


ROOT = Path(__file__).resolve().parents[2]
LINK_PATTERN = re.compile(
    r"(?:!?\[[^\]]*\]\(([^)]+)\)|(?:src|href)=\"([^\"]+)\")"
)


def main() -> int:
    missing: list[tuple[str, str]] = []
    readmes = list(ROOT.rglob("README.md"))
    for readme in readmes:
        text = readme.read_text(encoding="utf-8")
        for match in LINK_PATTERN.finditer(text):
            raw = (match.group(1) or match.group(2)).strip()
            raw = raw.split("#", 1)[0].strip("<>")
            if not raw or "://" in raw or raw.startswith("#"):
                continue
            if not (readme.parent / raw).exists():
                missing.append((str(readme.relative_to(ROOT)), raw))

    svgs = list((ROOT / "lab6").rglob("*.svg"))
    for svg in svgs:
        ET.parse(svg)

    gifs = list((ROOT / "lab6").rglob("*.gif"))
    gifs.append(ROOT / "assets" / "course_journey_lab6.gif")
    for gif in gifs:
        with Image.open(gif) as image:
            if getattr(image, "n_frames", 1) < 6 or image.width < 800:
                raise ValueError(f"GIF does not meet frame/size contract: {gif}")

    datasets = list((ROOT / "lab6").rglob("*_experimental_data.dat"))
    for dataset in datasets:
        rows = [
            line.split()
            for line in dataset.read_text(encoding="utf-8").splitlines()
            if line and not line.startswith("#")
        ]
        if len(rows) < 5 or any(row[-1] != "1" for row in rows):
            raise ValueError(f"Dataset contains insufficient/invalid rows: {dataset}")

    if missing:
        for readme, target in missing:
            print(f"MISSING  {readme} -> {target}")
        return 2

    print(
        f"READMEs={len(readmes)} LINKS=PASS "
        f"SVG={len(svgs)} XML=PASS "
        f"GIF={len(gifs)} FRAMES=PASS "
        f"DATASETS={len(datasets)} VALID=PASS"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
