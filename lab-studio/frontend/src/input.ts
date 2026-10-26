import type { Problem } from "./types";
export const normalizeInput = (text: string) =>
  text.replace(/^\uFEFF/, "").replace(/\r\n?/g, "\n");
export function inputLines(text: string) {
  return normalizeInput(text).replace(/\n$/, "").split("\n");
}
const arrays = new Set([
  "L1Q3",
  "L1Q5",
  "L2Q2",
  "L3Q1",
  "L3Q2",
  "L3Q3",
  "L3Q6",
  "L5Q1",
  "L5Q2",
  "L6Q1",
  "L6Q4",
  "L8Q4",
  "L8Q5",
  "L8Q7",
  "L9Q4",
  "L9Q5",
  "L9Q7",
  "L9Q9",
]);
export function syncCounts(problem: Problem, text: string): string {
  const lines = inputLines(text),
    first = lines[0].trim().split(/\s+/);
  const headerWidths: Record<string, number> = {
    L2Q3: 2,
    L4Q3: 2,
    L6Q3: 2,
    L6Q6: 2,
    L8Q1: 2,
    L8Q2: 2,
    L9Q1: 2,
    L9Q3: 3,
    L10Q1: 2,
    L10Q2: 3,
    L10Q3: 3,
    L10Q4: 2,
    L10Q5: 2,
  };
  if (first.length !== (headerWidths[problem.id] ?? 1)) return text;
  const count = (line: string) =>
    line.trim() ? line.trim().split(/\s+/).length : 0;
  if (arrays.has(problem.id) && lines.length > 1)
    first[0] = String(count(lines[1]));
  else if (
    [
      "L4Q1",
      "L4Q4",
      "L4Q5",
      "L4Q6",
      "L7Q6",
      "L9Q1",
      "L9Q2",
      "L9Q3",
      "L9Q8",
      "L9Q10",
    ].includes(problem.id)
  )
    first[0] = String(lines.slice(1).filter((s) => s.trim()).length);
  else if (
    ["L8Q1", "L8Q2", "L6Q6", "L4Q3", "L4Q2"].includes(problem.id) &&
    lines.length > 1
  )
    first[0] = String(count(lines[1]));
  else if (problem.id === "L6Q8" && lines.length > 1)
    first[0] = String(count(lines[1]));
  else if (problem.id === "L6Q3" && lines.length > 2) {
    first[0] = String(count(lines[1]));
    first[1] = String(count(lines[2]));
  } else if (problem.id === "L10Q1")
    first[1] = String(lines.slice(1).filter((s) => s.trim()).length);
  else if (problem.id === "L10Q2")
    first[2] = String(lines.slice(1).filter((s) => s.trim()).length);
  else if (["L10Q3", "L10Q5"].includes(problem.id) && lines.length > 1) {
    first[0] = String(count(lines[1]));
    first[1] = String(lines.slice(2).filter((s) => s.trim()).length);
  } else if (problem.id === "L10Q4" && lines.length > 2) {
    first[0] = String(count(lines[1]));
    first[1] = String(lines.slice(2, -1).filter((s) => s.trim()).length);
  } else if (problem.id === "L2Q3" && lines.length >= 3) {
    first[0] = String(lines.length - 2);
    first[1] = String(count(lines[1]));
  }
  lines[0] = first.join(" ");
  return lines.join("\n") + "\n";
}
export function fieldLabels(problem: Problem, text: string): string[] {
  const lines = inputLines(text),
    n = Number(lines[0].trim().split(/\s+/)[0]);
  return lines.map((_, i) => {
    if (
      ["L3Q4", "L3Q5", "L6Q2"].includes(problem.id) &&
      i > 0 &&
      Number.isInteger(n) &&
      n > 0
    )
      return `Matrix ${i <= n ? "A" : "B"} · row ${i <= n ? i : i - n}`;
    if (problem.id === "L2Q3" && i > 0)
      return i === lines.length - 1
        ? "Method: 1 = sequential; 2 = balanced"
        : `Sorted array ${i}`;
    if (problem.id === "L10Q1" && i > 0) return `Edge ${i} · from to`;
    if (problem.id === "L10Q2" && i > 0)
      return `Addition ${i} · row column (zero-based)`;
    if (problem.id === "L10Q3" && i > 1)
      return `Edge ${i - 1} · from to weight`;
    if (problem.id === "L10Q4" && i > 1)
      return i === lines.length - 1
        ? "Deletion order (every vertex once)"
        : `Edge ${i - 1} · from to`;
    if (problem.id === "L10Q5" && i > 1)
      return `Allowed swap ${i - 1} · position position`;
    if (problem.id === "L7Q6" && i > 0)
      return `Scientist ${i} · name birth death`;
    if (problem.id === "L9Q1" && i > 0)
      return `Item ${i} · value weight decay rate`;
    if (problem.id === "L9Q2" && i > 0)
      return `Symbol ${i} · character frequency`;
    if (problem.id === "L9Q3" && i > 0) return `Station ${i} · distance fuel`;
    if (problem.id === "L9Q8" && i > 0) return `Meeting ${i} · start end`;
    if (problem.id === "L9Q10" && i > 0) return `Word ${i}`;
    return problem.fieldLabels[i] ?? `Data row ${i + 1}`;
  });
}
export function resizeMatrices(
  text: string,
  order: number,
  patterned = false,
): string {
  if (!Number.isInteger(order) || order < 1 || order > 32) return text;
  const tokens = text.trim().split(/\s+/),
    old = Number(tokens.shift());
  if (patterned) {
    return (
      [
        String(order),
        ...[0, 1].flatMap((matrix) => {
          const seed = Array.from({ length: order }, (_, i) =>
            i < old ? (tokens[matrix * old * old + i] ?? "0") : "0",
          );
          return Array.from({ length: order }, (_, r) =>
            Array.from({ length: order }, (_, c) => seed[r ^ c] ?? "0").join(
              " ",
            ),
          );
        }),
      ].join("\n") + "\n"
    );
  }
  return (
    [
      String(order),
      ...[0, 1].flatMap((matrix) =>
        Array.from({ length: order }, (_, r) =>
          Array.from({ length: order }, (_, c) =>
            r < old && c < old
              ? (tokens[matrix * old * old + r * old + c] ?? "0")
              : "0",
          ).join(" "),
        ),
      ),
    ].join("\n") + "\n"
  );
}

export function updateMatrixCell(
  text: string,
  matrix: number,
  index: number,
  value: string,
  patterned = false,
): string {
  const tokens = text.trim().split(/\s+/),
    order = Number(tokens.shift());
  if (
    !Number.isInteger(order) ||
    order < 1 ||
    order > 32 ||
    /\s/.test(value.trim())
  )
    return text;
  const values = Array.from(
      { length: 2 * order * order },
      (_, i) => tokens[i] ?? "0",
    ),
    cell = value.trim() || "0";
  if (patterned) {
    const key = Math.floor(index / order) ^ (index % order);
    for (let r = 0; r < order; r++)
      for (let c = 0; c < order; c++)
        if ((r ^ c) === key)
          values[matrix * order * order + r * order + c] = cell;
  } else values[matrix * order * order + index] = cell;
  return (
    [
      String(order),
      ...[0, 1].flatMap((m) =>
        Array.from({ length: order }, (_, r) =>
          values
            .slice(
              m * order * order + r * order,
              m * order * order + (r + 1) * order,
            )
            .join(" "),
        ),
      ),
    ].join("\n") + "\n"
  );
}

export type Workspace = {
  version: 1;
  selected: string;
  drafts: Record<string, string>;
};
export function parseWorkspace(
  value: unknown,
  allowedIds?: Set<string>,
): Workspace {
  if (!value || typeof value !== "object" || Array.isArray(value))
    throw Error("Choose a Lab Studio workspace JSON backup.");
  const data = value as Record<string, unknown>;
  if (
    (data.version !== undefined && data.version !== 1) ||
    !data.drafts ||
    typeof data.drafts !== "object" ||
    Array.isArray(data.drafts)
  )
    throw Error("Choose a Lab Studio workspace JSON backup.");
  const drafts: Record<string, string> = {};
  for (const [id, text] of Object.entries(data.drafts)) {
    if (
      !/^L(?:10|[1-9])Q(?:10|[1-9])$/.test(id) ||
      (allowedIds && !allowedIds.has(id))
    )
      continue;
    if (
      typeof text !== "string" ||
      new TextEncoder().encode(text).length > 64000
    )
      throw Error("A saved input is invalid or exceeds 64 KB.");
    drafts[id] = normalizeInput(text);
  }
  return {
    version: 1,
    selected:
      typeof data.selected === "string" &&
      /^L(?:10|[1-9])Q(?:10|[1-9])$/.test(data.selected)
        ? data.selected
        : "L10Q1",
    drafts,
  };
}
