import { describe, it, expect } from "vitest";
import { readFileSync } from "node:fs";
import { resolve } from "node:path";
import { csv, isMatrix, legacyBlocks, displayValue } from "../data";
import {
  parseWorkspace,
  normalizeInput,
  syncCounts,
  resizeMatrices,
  updateMatrixCell,
  fieldLabels,
} from "../input";
import type { Catalog, Run } from "../types";
const fixtures = JSON.parse(
  readFileSync(resolve(process.cwd(), "../.cache/ui-fixtures.json"), "utf8"),
) as { catalog: Catalog; runs: Record<string, Run> };
const p = (id: string) => fixtures.catalog.problems.find((p) => p.id === id)!;
describe("Input contracts and recovery", () => {
  it("rejects corrupted local state without trusting its shape", () => {
    for (const data of [
      null,
      [],
      { drafts: null },
      { drafts: { L1Q1: 5 } },
      { version: 2, drafts: {} },
    ])
      expect(() => parseWorkspace(data)).toThrow();
  });
  it("restores old and new workspace backups and normalizes imported files", () => {
    expect(
      parseWorkspace({
        selected: "L8Q3",
        drafts: { L8Q3: "\ufeffABC\r\nAC\r\n" },
      }).drafts.L8Q3,
    ).toBe("ABC\nAC\n");
    expect(normalizeInput("A\rB")).toBe("A\nB");
  });
  it("syncs array, graph and weighted graph counts", () => {
    expect(syncCounts(p("L1Q3"), "8\n3 1 2\n")).toBe("3\n3 1 2\n");
    expect(syncCounts(p("L10Q1"), "3 8\n1 2\n2 3\n")).toBe("3 2\n1 2\n2 3\n");
    expect(syncCounts(p("L10Q3"), "3 8 2\n1 2 3\n1 2 7\n")).toBe(
      "3 1 2\n1 2 3\n1 2 7\n",
    );
  });
  it("resizes both matrices while preserving their upper-left values", () => {
    const next = resizeMatrices("2\n1 2\n3 4\n5 6\n7 8\n", 3);
    expect(next).toBe("3\n1 2 0\n3 4 0\n0 0 0\n5 6 0\n7 8 0\n0 0 0\n");
    expect(fieldLabels(p("L3Q4"), next)[4]).toBe("Matrix B · row 1");
  });
});
describe("Exact and structured outputs", () => {
  it("exports escaped CSV and blank cells for chart gaps", () => {
    expect(
      csv({
        name: "t",
        labels: ["Index", "A, B"],
        rows: [
          [0, NaN],
          [1, 3],
        ],
      }),
    ).toBe('Index,"A, B"\n0,\n1,3');
  });
  it("keeps null, large integers, binary codes and nested objects distinct", () => {
    expect(displayValue(null)).toBe("NONE");
    expect(displayValue("9007199254740993")).toBe("9007199254740993");
    expect(displayValue("001")).toBe("001");
    expect(
      isMatrix([
        [1, 2],
        [3, 4],
      ]),
    ).toBe(true);
    expect(isMatrix([[{ x: 1 }]])).toBe(false);
  });
  it("groups actual matrix stdout into matrices rather than unrelated line charts", () => {
    const blocks = legacyBlocks(fixtures.runs.L3Q4);
    expect(blocks[0].rows).toEqual([
      [19, 22],
      [43, 50],
    ]);
    expect(blocks[0].matrix).toBe(true);
  });
});

it("clearing a matrix cell never shifts neighbouring values", () => {
  expect(updateMatrixCell("2\n1 2\n3 4\n5 6\n7 8\n", 0, 0, "")).toBe(
    "2\n0 2\n3 4\n5 6\n7 8\n",
  );
});
it("keeps special matrices linked when editing and resizing", () => {
  const next = resizeMatrices("2\n1 2\n2 1\n3 4\n4 3\n", 4, true);
  const changed = updateMatrixCell(next, 0, 5, "9", true)
    .trim()
    .split(/\s+/)
    .slice(1)
    .map(Number);
  expect([changed[0], changed[5], changed[10], changed[15]]).toEqual([
    9, 9, 9, 9,
  ]);
  expect(changed.slice(16, 20)).toEqual([3, 4, 0, 0]);
});

it("renders a one-cell matrix without treating scalar metrics as matrices", () => {
  const blocks = legacyBlocks({
    ...fixtures.runs.L3Q4,
    stdout: "A x B using Strassen:\n6\nScalar multiplications: 1\n",
  });
  expect(blocks.map((b) => b.rows)).toEqual([[[6]]]);
});
