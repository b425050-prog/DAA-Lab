"""Generate the committed Lab 06 GIF visual system with Pillow.

The visuals are deterministic and intentionally self-contained: no browser,
font download, or external design service is required to rebuild them.
"""

from pathlib import Path
from math import sin, pi
from PIL import Image, ImageDraw, ImageFont, ImageFilter

ROOT = Path(__file__).resolve().parents[2]
LAB = ROOT / "lab6"
ASSETS = LAB / "assets"
ROOT_ASSETS = ROOT / "assets"
ASSETS.mkdir(parents=True, exist_ok=True)

FONT_PATHS = [
    Path("C:/Windows/Fonts/seguisb.ttf"),
    Path("C:/Windows/Fonts/segoeui.ttf"),
    Path("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"),
]


def font(size: int, bold: bool = False):
    candidates = ([Path("C:/Windows/Fonts/seguisb.ttf")] if bold else []) + FONT_PATHS
    for path in candidates:
        if path.exists():
            return ImageFont.truetype(str(path), size)
    return ImageFont.load_default()


F_TITLE = font(38, True)
F_SUB = font(20)
F_BODY = font(22, True)
F_SMALL = font(16)
F_MONO = font(20)

THEMES = [
    ((34, 211, 238), (167, 139, 250)),
    ((45, 212, 191), (96, 165, 250)),
    ((251, 191, 36), (244, 114, 182)),
    ((248, 113, 113), (251, 146, 60)),
    ((52, 211, 153), (34, 211, 238)),
    ((167, 139, 250), (244, 114, 182)),
    ((96, 165, 250), (52, 211, 153)),
    ((251, 146, 60), (250, 204, 21)),
]


def background(size, accent, phase=0.0):
    w, h = size
    image = Image.new("RGB", size)
    pixels = image.load()
    for y in range(h):
        for x in range(w):
            t = (x / w + y / h) / 2
            glow = max(0.0, 1.0 - ((x - w * (0.78 + 0.06 * sin(phase))) ** 2 +
                                   (y - h * 0.15) ** 2) ** 0.5 / (w * 0.65))
            pixels[x, y] = (
                int(5 + 11 * t + accent[0] * glow * 0.09),
                int(12 + 10 * t + accent[1] * glow * 0.08),
                int(28 + 21 * t + accent[2] * glow * 0.08),
            )
    return image


def centered(draw, xy, text, used_font, fill, anchor="mm"):
    draw.text(xy, text, font=used_font, fill=fill, anchor=anchor)


def pill(draw, box, text, colour):
    draw.rounded_rectangle(box, radius=18, fill=(7, 18, 38), outline=colour, width=2)
    centered(draw, ((box[0] + box[2]) // 2, (box[1] + box[3]) // 2),
             text, F_SMALL, colour)


def chrome(image, q, title, subtitle, accent_a, accent_b, progress):
    draw = ImageDraw.Draw(image)
    draw.rounded_rectangle((30, 28, image.width - 30, image.height - 28), 28,
                           fill=(4, 10, 24), outline=(51, 65, 85), width=2)
    draw.rectangle((30, 28, image.width - 30, 130), fill=(8, 18, 42))
    draw.text((62, 48), f"LAB 06  /  Q{q:02d}", font=F_SMALL, fill=accent_a)
    draw.text((62, 72), title, font=F_TITLE, fill=(244, 248, 255))
    draw.text((64, 115), subtitle, font=F_SMALL, fill=(166, 181, 204))
    pill(draw, (image.width - 260, 55, image.width - 62, 98),
         f"FRAME {progress + 1:02d}", accent_b)
    return draw


def bars(draw, phase, a, b):
    values = [5, 1, 8, 3, 7, 2, 6, 4]
    if phase >= 2:
        values = sorted(values)
    base, left = 388, 155
    for i, value in enumerate(values):
        x = left + i * 78
        colour = a if (i + phase) % 3 else b
        draw.rounded_rectangle((x, base - value * 24, x + 52, base), 10,
                               fill=colour, outline=(230, 245, 255), width=1)
        centered(draw, (x + 26, base + 23), str(value), F_SMALL, (203, 213, 225))


def matrix(draw, phase, a, b):
    left, top, cell = 260, 180, 58
    for r in range(4):
        for c in range(4):
            hot = (r + c + phase) % 4 == 0
            colour = b if hot else a
            draw.rounded_rectangle((left+c*cell, top+r*cell,
                                    left+c*cell+48, top+r*cell+48), 8,
                                   fill=colour if hot else tuple(v//3 for v in colour),
                                   outline=colour, width=2)
    draw.text((540, 225), "A x B", font=F_TITLE, fill=(240, 246, 255))
    draw.text((540, 280), "row · column", font=F_BODY, fill=b)


def fft(draw, phase, a, b):
    points = []
    for x in range(130, 780, 8):
        y = 270 + 56 * sin((x - 130) / 40 + phase * 0.8) + 22 * sin((x - 130) / 17)
        points.append((x, int(y)))
    draw.line(points, fill=a, width=5)
    for i in range(8):
        x = 160 + i * 82
        height = 30 + ((i * 17 + phase * 19) % 100)
        draw.rounded_rectangle((x, 410-height, x+32, 410), 7, fill=b)
    centered(draw, (455, 165), "time domain  →  frequency domain  →  coefficients",
             F_BODY, (228, 235, 246))


def reversals(draw, phase, a, b):
    values = [1, 4, 3, 2, 5] if phase < 2 else [1, 2, 3, 4, 5]
    for i, value in enumerate(values):
        x = 170 + i * 120
        active = phase < 2 and 1 <= i <= 3
        draw.rounded_rectangle((x, 230, x+86, 326), 18,
                               fill=b if active else (18, 37, 66),
                               outline=a, width=3)
        centered(draw, (x+43, 278), str(value), F_TITLE, (248, 250, 252))
    if phase < 2:
        draw.arc((270, 165, 610, 390), 195, 345, fill=b, width=6)
        centered(draw, (440, 390), "reverse [2..4]", F_BODY, b)
    else:
        centered(draw, (455, 390), "sorted using reversals only", F_BODY, a)


def fibonacci(draw, phase, a, b):
    values = [0, 1, 1, 2, 3, 5, 8]
    visible = min(len(values), phase + 3)
    for i, value in enumerate(values):
        x = 100 + i * 108
        fill = a if i < visible else (20, 32, 55)
        draw.rounded_rectangle((x, 245, x+78, 318), 14, fill=fill,
                               outline=b, width=2)
        centered(draw, (x+39, 281), str(value), F_BODY, (248, 250, 252))
        if 1 < i < visible:
            centered(draw, (x+39, 350), "↑", F_BODY, b)
    centered(draw, (455, 190), "F(i) = F(i-1) + F(i-2)", F_TITLE, (240, 246, 255))


def grid(draw, phase, a, b, lcs=False):
    rows, cols = 6, 10
    left, top, cell = 185, 170, 52
    filled = min(rows * cols, (phase + 1) * 12)
    for r in range(rows):
        for c in range(cols):
            index = r * cols + c
            colour = a if index < filled else (18, 31, 52)
            if lcs and index < filled and (r * 3 + c) % 7 == 0:
                colour = b
            draw.rounded_rectangle((left+c*cell, top+r*42,
                                    left+c*cell+44, top+r*42+34), 6,
                                   fill=colour, outline=(63, 81, 108), width=1)
    centered(draw, (455, 442),
             "match ↖   best ↑ / ←" if lcs else "exclude ↑   or   include ↖ + profit",
             F_BODY, b)


def chain(draw, phase, a, b):
    left, top, cell = 280, 165, 62
    for length in range(1, 7):
        for i in range(7 - length):
            j = i + length - 1
            active = length <= phase + 2
            x, y = left + j * cell, top + i * 42
            draw.rounded_rectangle((x, y, x+50, y+32), 6,
                                   fill=a if active else (19, 31, 54),
                                   outline=b, width=1)
    centered(draw, (455, 442), "short chains  →  longer chains  →  optimal split",
             F_BODY, b)


QUESTION_DATA = [
    ("Array Operations", "scan, order, deduplicate, reverse, partition", bars, "Theta(n log n) suite"),
    ("Matrix Operations", "quadratic surfaces and cubic kernels", matrix, "Theta(n³) dominant"),
    ("FFT Convolution", "pad, transform, multiply, invert", fft, "O(n log n)"),
    ("Reversal Sort", "divide, rotate blocks, merge", reversals, "O(n log² n) cost"),
    ("Fibonacci DP", "two remembered states replace recursion", fibonacci, "Theta(n) time"),
    ("0/1 Knapsack", "item × capacity decision grid", lambda d,p,a,b: grid(d,p,a,b,False), "Theta(nW)"),
    ("Longest Common Subsequence", "matches become a recoverable path", lambda d,p,a,b: grid(d,p,a,b,True), "Theta(mn)"),
    ("Matrix-Chain DP", "intervals remember their cheapest split", chain, "Theta(n³)"),
]


def question_gifs():
    for index, (title, subtitle, painter, bound) in enumerate(QUESTION_DATA, 1):
        a, b = THEMES[index - 1]
        frames = []
        for phase in range(6):
            image = background((920, 500), a, phase * 0.55)
            draw = chrome(image, index, title, subtitle, a, b, phase)
            painter(draw, phase, a, b)
            pill(draw, (650, 425, 850, 468), bound, a)
            frames.append(image)
        name = [
            "q1_array_workbench.gif", "q2_matrix_kernels.gif",
            "q3_fft_butterflies.gif", "q4_reversal_merge.gif",
            "q5_fibonacci_states.gif", "q6_knapsack_grid.gif",
            "q7_lcs_backtrack.gif", "q8_matrix_chain_splits.gif",
        ][index - 1]
        frames[0].save(LAB / f"Q-{index}" / name, save_all=True,
                       append_images=frames[1:], duration=650, loop=0,
                       optimize=True)


def banner():
    frames = []
    for phase in range(12):
        image = background((1200, 360), THEMES[0][0], phase * 0.55)
        draw = ImageDraw.Draw(image)
        draw.rounded_rectangle((24, 22, 1176, 338), 36, fill=(5, 12, 29),
                               outline=THEMES[phase % 8][0], width=3)
        centered(draw, (600, 92), "DAA LABORATORY · LAB 06", F_TITLE,
                 (243, 248, 255))
        centered(draw, (600, 145), "STRUCTURE  ·  TRANSFORM  ·  OPTIMIZE",
                 F_BODY, THEMES[phase % 8][0])
        centered(draw, (600, 200),
                 "arrays  matrices  convolution  reversals  dynamic programming",
                 F_SUB, (180, 194, 216))
        for i in range(8):
            x = 204 + i * 112
            colour = THEMES[i][0]
            radius = 13 + (8 if i == phase % 8 else 0)
            draw.ellipse((x-radius, 270-radius, x+radius, 270+radius), fill=colour)
            centered(draw, (x, 310), f"Q{i+1}", F_SMALL, colour)
        frames.append(image)
    frames[0].save(ASSETS / "lab6_banner.gif", save_all=True,
                   append_images=frames[1:], duration=360, loop=0, optimize=True)


def divider():
    frames = []
    for phase in range(16):
        image = Image.new("RGB", (1200, 70), (5, 12, 28))
        draw = ImageDraw.Draw(image)
        draw.line((40, 35, 1160, 35), fill=(49, 67, 91), width=2)
        for i in range(8):
            x = 110 + i * 140
            glow = 1.0 if i == phase % 8 else 0.35
            c = tuple(int(v * glow) for v in THEMES[i][0])
            draw.ellipse((x-9, 26, x+9, 44), fill=c)
        frames.append(image)
    frames[0].save(ASSETS / "animated_divider.gif", save_all=True,
                   append_images=frames[1:], duration=140, loop=0, optimize=True)


def pipeline():
    labels = ["DESIGN", "IMPLEMENT", "COUNT", "CHECK", "PLOT", "EXPLAIN"]
    frames = []
    for phase in range(12):
        image = background((1200, 250), THEMES[5][0], phase * 0.3)
        draw = ImageDraw.Draw(image)
        centered(draw, (600, 45), "REPRODUCIBLE EVIDENCE PIPELINE", F_BODY,
                 (239, 246, 255))
        for i, label in enumerate(labels):
            x = 95 + i * 190
            active = i <= phase % 7
            colour = THEMES[i % 8][0]
            draw.rounded_rectangle((x, 90, x+150, 160), 18,
                                   fill=colour if active else (17, 31, 53),
                                   outline=colour, width=2)
            centered(draw, (x+75, 125), label, F_SMALL,
                     (7, 15, 31) if active else (203, 213, 225))
            if i < len(labels)-1:
                draw.line((x+153, 125, x+187, 125), fill=(100, 116, 139), width=3)
                draw.polygon([(x+187,125),(x+177,119),(x+177,131)], fill=(100,116,139))
        centered(draw, (600, 210), "same input · independent oracle · committed evidence",
                 F_SMALL, (170, 186, 208))
        frames.append(image)
    frames[0].save(ASSETS / "pipeline.gif", save_all=True,
                   append_images=frames[1:], duration=330, loop=0, optimize=True)


def gallery():
    frames = []
    for phase in range(16):
        image = background((1200, 620), THEMES[phase % 8][0], phase * 0.25)
        draw = ImageDraw.Draw(image)
        centered(draw, (600, 55), "EIGHT ALGORITHMIC STORIES", F_TITLE,
                 (243, 248, 255))
        for i, (title, _, _, bound) in enumerate(QUESTION_DATA):
            row, col = divmod(i, 4)
            x, y = 60 + col * 285, 115 + row * 220
            active = i == (phase // 2) % 8
            a, b = THEMES[i]
            draw.rounded_rectangle((x, y, x+250, y+178), 22,
                                   fill=(14, 28, 50) if not active else tuple(v//2 for v in a),
                                   outline=a, width=4 if active else 2)
            centered(draw, (x+125, y+38), f"Q{i+1:02d}", F_BODY, a)
            centered(draw, (x+125, y+82), title, F_SMALL, (239, 245, 255))
            centered(draw, (x+125, y+128), bound, F_SMALL, b)
        centered(draw, (600, 570), "theory beside implementation beside evidence",
                 F_BODY, THEMES[phase % 8][1])
        frames.append(image)
    frames[0].save(ASSETS / "lab6_eight_stories.gif", save_all=True,
                   append_images=frames[1:], duration=360, loop=0, optimize=True)


def course_journey():
    labels = ["FOUNDATIONS", "STRUCTURE", "DIVIDE", "SWEEP", "SELECT", "OPTIMIZE"]
    frames = []
    for phase in range(12):
        image = background((1200, 300), THEMES[phase % 8][0], phase * 0.3)
        draw = ImageDraw.Draw(image)
        centered(draw, (600, 48), "COURSE JOURNEY · NOW THROUGH LAB 06", F_BODY,
                 (241, 246, 255))
        draw.line((110, 150, 1090, 150), fill=(65, 81, 106), width=5)
        for i, label in enumerate(labels):
            x = 120 + i * 194
            active = i <= phase % 7
            colour = THEMES[(i+2) % 8][0]
            radius = 28 if i == 5 else 20
            draw.ellipse((x-radius,150-radius,x+radius,150+radius),
                         fill=colour if active else (25, 40, 63), outline=colour, width=3)
            centered(draw, (x, 205), f"LAB {i+1:02d}", F_SMALL, colour)
            centered(draw, (x, 235), label, font(13, True), (191, 203, 220))
        centered(draw, (600, 275), "33 solved questions · C17 · validators · SVG · GIF",
                 F_SMALL, (170, 186, 207))
        frames.append(image)
    frames[0].save(ROOT_ASSETS / "course_journey_lab6.gif", save_all=True,
                   append_images=frames[1:], duration=380, loop=0, optimize=True)


if __name__ == "__main__":
    question_gifs()
    banner()
    divider()
    pipeline()
    gallery()
    course_journey()
    print("Generated 13 Lab 06 and repository GIF assets.")
