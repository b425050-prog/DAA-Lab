import type { Dataset, Json, Run } from "./types";
export const displayValue = (value: Json | undefined): string =>
  value === null
    ? "NONE"
    : value === undefined
      ? "—"
      : typeof value === "object"
        ? JSON.stringify(value)
        : String(value);
export const isScalar = (value: Json): boolean =>
  value === null || typeof value !== "object";
export const isMatrix = (value: Json[]): value is Json[][] =>
  value.length > 0 &&
  value.every((row) => Array.isArray(row) && row.every(isScalar));
export function csv(dataset: Dataset): string {
  const quote = (v: string) =>
    /[",\n]/.test(v) ? '"' + v.replaceAll('"', '""') + '"' : v;
  return [
    dataset.labels.map(quote).join(","),
    ...dataset.rows.map((row) =>
      row.map((v) => (Number.isFinite(v) ? String(v) : "")).join(","),
    ),
  ].join("\n");
}
export function legacyBlocks(
  run: Run,
): { name: string; rows: number[][]; matrix: boolean }[] {
  const lines = run.stdout.split("\n"),
    result: { name: string; rows: number[][]; matrix: boolean }[] = [];
  for (let i = 0; i < lines.length; i++) {
    const pieces = lines[i].split(":"),
      numeric = (
        pieces.length > 1 ? pieces.slice(1).join(":") : lines[i]
      ).trim();
    if (!numeric || !/^[-+\d.eE\s]+$/.test(numeric)) continue;
    const values = numeric.split(/\s+/).map(Number),
      matrix = ["L3Q4", "L3Q5", "L6Q2"].includes(run.problemId);
    if (
      !values.every(Number.isFinite) ||
      (values.length < 2 && (!matrix || pieces.length > 1))
    )
      continue;
    const previous = result.at(-1);
    if (
      matrix &&
      previous &&
      previous.rows[0].length === values.length &&
      i > 0 &&
      /^\s*[-+\d.eE\s]+$/.test(lines[i - 1])
    )
      previous.rows.push(values);
    else
      result.push({
        name:
          pieces.length > 1
            ? pieces[0].trim()
            : lines[i - 1]?.replace(/:$/, "").trim() || "Result values",
        rows: [values],
        matrix,
      });
  }
  return result;
}
