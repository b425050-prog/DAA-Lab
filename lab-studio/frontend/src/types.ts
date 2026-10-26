export type Json =
  null | boolean | number | string | Json[] | { [key: string]: Json };
export type Dataset = {
  name: string;
  labels: string[];
  rows: number[][];
  totalRows?: number;
};
export type Problem = {
  sourceKind?: "override" | "repository";
  id: string;
  lab: number;
  question: number;
  title: string;
  theme: string;
  source: string;
  sample: string;
  fieldLabels: string[];
  inputGuide: string;
  note: string;
  notes: string;
  evidence: Dataset[];
  countMode: boolean;
};
export type Catalog = {
  repository: string;
  commit: string;
  labs: { id: number; title: string; count: number }[];
  problems: Problem[];
};
export type Graph = {
  kind: string;
  n?: number;
  edges?: number[][];
  accepted?: boolean[];
  colours?: number[];
  maxima?: (number | null)[];
  weights?: number[];
  deletions?: number[];
  initial?: number[];
  permutation?: number[];
  rows?: number;
  cols?: number;
  cells?: number[][];
  counts?: number[];
};
export type Run = {
  problemId: string;
  input: string;
  countOnly?: boolean;
  warnings?: string[];
  stdout: string;
  stderr: string;
  elapsedMs: number;
  trace: Json;
  graph: Graph | null;
  datasets: Dataset[];
  files: { name: string; text: string }[];
  source: string;
  engine: string;
};
export function download(name: string, text: string, mime = "text/plain") {
  const url = URL.createObjectURL(new Blob([text], { type: mime }));
  const link = document.createElement("a");
  link.href = url;
  link.download = name;
  link.click();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}
