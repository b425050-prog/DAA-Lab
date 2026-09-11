#!/usr/bin/env python3
"""Draw a self-contained animated mathematical diagram for the GitHub README.

Optional dependency: Pillow. Fonts fall back across Windows/macOS/Linux.
No network calls. No generated timestamps or machine-specific output paths.
"""
from pathlib import Path
import math
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
ASSETS = ROOT / "assets"
W, H = 1280, 460
BG, PANEL = "#09111f", "#101e31"
INK, MUTED, TEAL, GOLD = "#f1f5fc", "#94a9c4", "#60e4c0", "#ffd580"


def font(size, bold=False):
    candidates = (["C:/Windows/Fonts/segoeuib.ttf",
                   "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"] if bold else
                  ["C:/Windows/Fonts/segoeui.ttf",
                   "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"])
    candidates += ["/System/Library/Fonts/Supplemental/Arial.ttf"]
    for path in candidates:
        if Path(path).exists():
            return ImageFont.truetype(path, size)
    return ImageFont.load_default(size=size)


F = {s: font(s) for s in (15, 17, 19, 22, 24)}
F[64], F[30] = font(64, True), font(30, True)


def xy(p):
    i, j = p
    return 1003 + (i - j) * 34, 128 + math.sqrt(3) * (i + j) * 34


START = {(i, j) for i in range(4) for j in range(4 - i)}
END = {(2 - i, 2 - j) for i, j in START}
MOVES = [((0, 3), (-1, 2)), ((3, 0), (2, -1)), ((0, 0), (2, 2))]


def frame(index):
    im = Image.new("RGB", (W, H), BG)
    d = ImageDraw.Draw(im)
    for x in range(18, W, 28):
        for y in range(18, H, 28):
            d.ellipse((x, y, x + 1, y + 1), fill="#1a2a3c")
    d.rounded_rectangle((34, 30, 274, 67), radius=18, fill="#153430")
    d.text((52, 36), "ALGORITHM FIELD NOTES", font=F[15], fill=TEAL)
    d.text((44, 90), "DAA / LAB 07", font=F[64], fill=INK)
    d.text((48, 183), "Seven puzzles. One language.", font=F[30], fill=INK)
    d.text((49, 239), "Geometry  /  Search  /  Dynamic programming", font=F[22], fill=MUTED)
    d.line((49, 292, 682, 292), fill="#2a3a51", width=1)
    for x, value, label in [(49, "07", "C SOLUTIONS"), (267, "341", "LOCAL TEST RUNS"),
                            (499, "C17", "PORTABLE CODE")]:
        d.text((x, 315), value, font=F[30], fill=TEAL)
        d.text((x, 363), label, font=F[15], fill=MUTED)
    d.text((49, 413), "PROOFS INCLUDED.  MOVES REPLAYED.  EDGE CASES CHECKED.", font=F[15], fill=MUTED)
    d.rounded_rectangle((760, 28, 1244, 432), radius=24, fill=PANEL, outline="#2b405a", width=2)
    d.text((787, 49), "01 / INVERT THE TRIANGLE", font=F[17], fill=TEAL)
    d.text((787, 79), "10 coins. 3 relocations. Minimum proven.", font=F[17], fill=MUTED)

    # One coin moves at a time. Start/end holds make the loop easy to follow.
    t = max(0.0, min(3.0, (index - 14) / 20))
    completed = min(3, int(t))
    active = t - completed
    positions = {p: xy(p) for p in START}
    for source, dest in MOVES[:completed]:
        positions[source] = xy(dest)
    active_source = None
    if completed < 3 and active > 0:
        source, dest = MOVES[completed]
        active_source = source
        a, b = xy(source), xy(dest)
        u = active * active * (3 - 2 * active)
        bend = math.sin(math.pi * u) * (44 if completed < 2 else -57)
        positions[source] = (a[0] + (b[0] - a[0]) * u + (bend if completed == 2 else 0),
                             a[1] + (b[1] - a[1]) * u - (bend if completed < 2 else 0))
    for p in sorted(END):
        x, y = xy(p)
        d.ellipse((x - 21, y - 21, x + 21, y + 21), outline="#365774", width=1)
    moving_sources = {a for a, _ in MOVES}
    for original, (x, y) in sorted(positions.items(), key=lambda v: v[0] == active_source):
        col = GOLD if original in moving_sources else TEAL
        d.ellipse((x - 23, y - 20, x + 23, y + 26), fill="#070f1b")
        d.ellipse((x - 22, y - 22, x + 22, y + 22), fill=col)
        d.ellipse((x - 15, y - 15, x + 15, y + 15), outline="#142438", width=2)
        d.ellipse((x - 7, y - 11, x - 2, y - 6), fill="#fff6d8")
    status = "TRIANGLE INVERTED" if completed == 3 else (
             f"MOVING COIN {completed + 1} OF 3" if active > 0 else "KEEP 7 COINS. MOVE 3.")
    d.text((787, 391), status, font=F[15], fill=GOLD if completed < 3 else TEAL)
    for i in range(3):
        col = TEAL if i < completed else "#334761"
        d.rounded_rectangle((1137 + i * 26, 397, 1153 + i * 26, 403), radius=3, fill=col)
    return im


def main():
    ASSETS.mkdir(exist_ok=True)
    frames = [frame(i) for i in range(96)]
    frames[0].save(ASSETS / "lab7_banner.png")
    frames[0].save(ASSETS / "lab7_banner.gif", save_all=True, append_images=frames[1:],
                   duration=80, loop=0, optimize=True, disposal=2)
    labels = [("C17", 88), ("7 SOLUTIONS", 155), ("341 LOCAL TEST RUNS", 235), ("MIT LICENSE", 142)]
    svg = ['<svg xmlns="http://www.w3.org/2000/svg" width="620" height="34" viewBox="0 0 620 34" role="img" aria-label="C17; 7 solutions; 341 local test runs; MIT license">']
    x = 0
    for label, width in labels:
        svg.append(f'<rect x="{x}" y="1" width="{width-8}" height="30" rx="6" fill="#153430"/>')
        svg.append(f'<text x="{x+(width-8)/2}" y="21" text-anchor="middle" fill="#60e4c0" font-family="Arial,sans-serif" font-size="12" font-weight="bold">{label}</text>')
        x += width
    svg.append('</svg>')
    (ASSETS / "lab7_badges.svg").write_text("\n".join(svg) + "\n", encoding="utf-8")
    print(f"Wrote {len(frames)} animation frames, a still banner, and local badges.")


if __name__ == "__main__":
    main()
