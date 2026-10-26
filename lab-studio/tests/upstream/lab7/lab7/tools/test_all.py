#!/usr/bin/env python3
"""Independent oracles and legal-move replays for all lab problems."""
from collections import deque
from functools import lru_cache
from pathlib import Path
import random
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from build import build, executable  # noqa: E402

RUNS = 0
RNG = random.Random(7)


def run(q, data, *args, valid=True):
    global RUNS
    RUNS += 1
    result = subprocess.run([str(executable(q)), *args],
                            input=data, text=True, capture_output=True, timeout=20)
    if valid:
        assert result.returncode == 0, (q, data, result.stderr)
        assert not result.stderr, result.stderr
    else:
        assert result.returncode != 0 and "Error:" in result.stderr, (q, data, result)
    return result.stdout


def number(text, label):
    return int(re.search(re.escape(label) + r": (\d+)", text).group(1))


def coins():
    for n in range(1, 26):
        out = run(1, str(n))
        count = number(out, "Minimum moves")
        a, b = map(int, re.search(r"Target offsets: (\d+) (\d+)", out).groups())
        start = {(i, j) for i in range(n) for j in range(n - i)}
        target = {(a - i, b - j) for i in range(n) for j in range(n - i)}
        state = start.copy()
        moves = re.findall(r"Move \d+: \((-?\d+),(-?\d+)\) -> \((-?\d+),(-?\d+)\)", out)
        assert len(moves) == count
        for values in moves:
            x, y, u, v = map(int, values)
            assert (x, y) in state and (u, v) not in state
            state.remove((x, y))
            state.add((u, v))
        assert state == target
        if n <= 12:
            # All translations that can share a lattice point with the original.
            overlap = max(sum(i <= u and j <= v and i + j >= u + v - n + 1
                              for i, j in start)
                          for u in range(2 * n - 1) for v in range(2 * n - 1))
            assert count == len(start) - overlap
    assert number(run(1, "1000000000", "--count"), "Minimum moves") == 166666666833333333


def eggs():
    # Classical floor-based minimax DP, independent of the implementation's DP.
    table = [[0] * 41 for _ in range(6)]
    for f in range(41):
        table[1][f] = f
    for e in range(2, 6):
        for f in range(1, 41):
            table[e][f] = 1 + min(max(table[e - 1][x - 1], table[e][f - x])
                                  for x in range(1, f + 1))
    for e in range(1, 6):
        for f in range(0, 41, 2):
            out = run(2, f"{e} {f}")
            assert number(out, "Minimum worst-case drops") == table[e][f]
            if f:
                x = int(re.search(r"first drop: floor (\d+)", out).group(1))
                if e == 1:
                    assert x == 1
                else:
                    assert 1 + max(table[e - 1][x - 1], table[e][f - x]) == table[e][f]
    assert number(run(2, "2 100"), "Minimum worst-case drops") == 14
    assert number(run(2, "1 1000000000000"), "Minimum worst-case drops") == 10**12
    assert number(run(2, "64 1000000000000"), "Minimum worst-case drops") == 40
    # Every possible threshold in the original 100-floor scenario.
    @lru_cache(None)
    def coverage(m, e):
        return 0 if not m or not e else coverage(m - 1, e - 1) + 1 + coverage(m - 1, e)
    for threshold in range(101):
        lo, hi, e, m, drops = 0, 100, 2, 14, 0
        while lo < hi:
            assert e > 0 and m > 0
            x = min(hi, lo + coverage(m - 1, e - 1) + 1)
            if x > threshold:
                hi, e = x - 1, e - 1
            else:
                lo = x
            m, drops = m - 1, drops + 1
        assert lo == threshold and drops <= 14


def hanoi_distance(n):
    initial, goal = (0,) * n, (3,) * n
    queue, seen = deque([(initial, 0)]), {initial}
    while queue:
        state, distance = queue.popleft()
        if state == goal:
            return distance
        top = [next((i for i in range(n) if state[i] == p), n) for p in range(4)]
        for p in range(4):
            if top[p] == n:
                continue
            for q in range(4):
                if p != q and top[p] < top[q]:
                    next_state = list(state)
                    next_state[top[p]] = q
                    next_state = tuple(next_state)
                    if next_state not in seen:
                        seen.add(next_state)
                        queue.append((next_state, distance + 1))


def hanoi():
    for n in list(range(13)) + [64]:
        out = run(3, str(n))
        stacks = [list(range(n, 0, -1)), [], [], []]
        moves = re.findall(r"Move \d+: disk (\d+) ([A-D]) -> ([A-D])", out)
        assert len(moves) == number(out, "Minimum moves")
        for disk, source, dest in moves:
            disk, source, dest = int(disk), ord(source) - 65, ord(dest) - 65
            assert stacks[source] and stacks[source][-1] == disk
            assert not stacks[dest] or stacks[dest][-1] > disk
            stacks[source].pop()
            stacks[dest].append(disk)
        assert stacks == [[], [], [], list(range(n, 0, -1))]
        if n <= 5:
            assert len(moves) == hanoi_distance(n)
        if n == 8:
            assert len(moves) == 33


def switches():
    for n in range(10):
        out = run(4, str(n))
        initial = state = (1 << n) - 1
        moves = re.findall(r"Move \d+: switch (\d+) -> ([01]+)", out)
        for switch, printed in moves:
            bit = int(switch) - 1
            assert 0 <= bit < n
            assert bit == 0 or state & ((1 << bit) - 1) == 1 << (bit - 1)
            state ^= 1 << bit
            assert state == int(printed, 2)
        assert state == 0
        queue, distances = deque([initial]), {initial: 0}
        while queue:
            current = queue.popleft()
            for bit in range(n):
                if bit == 0 or current & ((1 << bit) - 1) == 1 << (bit - 1):
                    nxt = current ^ (1 << bit)
                    if nxt not in distances:
                        distances[nxt] = distances[current] + 1
                        queue.append(nxt)
        assert len(moves) == distances[0] == number(out, "Minimum moves")
    assert number(run(4, "63", "--count"), "Minimum moves") == (1 << 64) // 3
    run(4, "21", valid=False)


def target():
    for n in list(range(2, 51)) + [2000]:
        out = run(5, str(n))
        schedule = re.findall(r"Shot \d+: spot (\d+); surviving positions: (\d+)", out)
        assert len(schedule) == (2 if n == 2 else 2 * n - 4)
        possible = set(range(1, n + 1))
        for index, (spot, count) in enumerate(schedule):
            spot = int(spot)
            assert 1 <= spot <= n
            possible.discard(spot)
            assert len(possible) == int(count)
            if index + 1 < len(schedule):
                possible = {q for p in possible for q in (p - 1, p + 1) if 1 <= q <= n}
        assert not possible


def scientists():
    cases = [[], [(1, 1)], [(1, 3), (3, 5)], [(1, 4), (2, 3), (6, 9), (7, 8)],
             [(-(1 << 63), (1 << 63) - 1)], [(1900, 1910)] * 4]
    for _ in range(40):
        cases.append([tuple(sorted((RNG.randrange(-10, 11), RNG.randrange(-10, 11))))
                      for _ in range(RNG.randrange(1, 20))])
    for intervals in cases:
        data = str(len(intervals)) + "\n" + "".join(
            f"Scientist_{i} {b} {d}\n" for i, (b, d) in enumerate(intervals))
        out = run(6, data)
        years = sorted({y for b, d in intervals if b < d for y in (b, d)})
        segments = [(a, b, sum(lo <= a < hi for lo, hi in intervals))
                    for a, b in zip(years, years[1:])]
        peak = max((c for _, _, c in segments), default=0)
        expected = []
        for a, b, c in segments:
            if c == peak and peak:
                if expected and expected[-1][1] == a:
                    expected[-1] = (expected[-1][0], b)
                else:
                    expected.append((a, b))
        actual = [tuple(map(int, m)) for m in re.findall(r"Best interval: \[(-?\d+), (-?\d+)\)", out)]
        assert number(out, "Maximum scientists alive") == peak and actual == expected


def matrix_chain():
    cases = [[30, 35, 15, 5, 10, 20, 25], [10, 20], [10, 10, 10, 10],
             [10**9, 10**9, 10**9, 1]]
    cases += [[RNG.randrange(1, 50) for _ in range(RNG.randrange(2, 9))] for _ in range(40)]
    for p in cases:
        n = len(p) - 1
        out = run(7, f"{n}\n" + " ".join(map(str, p)))
        @lru_cache(None)
        def all_costs(i, j):
            if i == j:
                return (0,)
            return tuple(a + b + p[i] * p[k + 1] * p[j + 1]
                         for k in range(i, j) for a in all_costs(i, k) for b in all_costs(k + 1, j))
        expected = min(all_costs(0, n - 1))
        assert number(out, "Minimum scalar multiplications") == expected
        expression = out.split("Optimal ordering: ")[1].strip()
        tokens = iter(re.findall(r"A\d+|[()x]", expression))
        leaves = []
        def parse():
            t = next(tokens)
            if t.startswith("A"):
                i = int(t[1:]) - 1
                leaves.append(i)
                return p[i], p[i + 1], 0
            assert t == "("
            r1, c1, v1 = parse()
            assert next(tokens) == "x"
            r2, c2, v2 = parse()
            assert next(tokens) == ")" and c1 == r2
            return r1, c2, v1 + v2 + r1 * c1 * c2
        assert parse() == (p[0], p[-1], expected)
        assert list(tokens) == [] and leaves == list(range(n))
    run(7, "2\n1000000000 1000000000 1000000000", valid=False)


def invalid_input():
    for q in range(1, 8):
        for data in ("", "not-a-number", "-1", "9" * 150):
            run(q, data, valid=False)
    for q, data in [(1, "0"), (1, "1001"), (1, "4 junk"), (2, "0 100"),
                    (2, "65 100"), (2, "2 -1"), (3, "65"), (4, "64"), (5, "1"),
                    (6, "1 Ada 2000 1900"), (7, "2 10 0 20"), (7, "0")]:
        run(q, data, valid=False)


if __name__ == "__main__":
    build()
    for test in (coins, eggs, hanoi, switches, target, scientists, matrix_chain, invalid_input):
        test()
        print(f"PASS {test.__name__}")
    print(f"PASS: all 7 solutions; {RUNS} program executions plus independent oracle checks.")
