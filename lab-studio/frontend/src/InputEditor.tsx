import { useRef, useState } from "react";
import {
  ArrowDownToLine,
  CircleHelp,
  Play,
  Sparkles,
  Upload,
  X,
} from "lucide-react";
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import { Problem, download } from "./types";
import {
  normalizeInput,
  inputLines,
  syncCounts,
  fieldLabels,
  resizeMatrices,
  updateMatrixCell,
} from "./input";
export default function InputEditor({
  problem,
  text,
  onChange,
  busy,
  onRun,
  onCancel,
  countOnly,
  setCountOnly,
}: {
  problem: Problem;
  text: string;
  onChange: (s: string) => void;
  busy: boolean;
  onRun: () => void;
  onCancel: () => void;
  countOnly: boolean;
  setCountOnly: (v: boolean) => void;
}) {
  const [raw, setRaw] = useState(false),
    [guide, setGuide] = useState(false),
    [edge, setEdge] = useState(["1", "2"]),
    [inputError, setInputError] = useState("");
  const upload = useRef<HTMLInputElement>(null);
  const lines = inputLines(text),
    labels = fieldLabels(problem, text),
    matrix = ["L3Q4", "L3Q5", "L6Q2"].includes(problem.id);
  const order = Number(lines[0]?.trim()),
    tokens = text.trim().split(/\s+/).slice(1);
  const setLine = (index: number, value: string) => {
    const next = [...lines];
    next[index] = value;
    onChange(next.join("\n") + "\n");
  };
  const addEdge = () => {
    const tokens = text.trim().split(/\s+/),
      n = Number(tokens[0]);
    const [a, b] = edge.map(Number);
    if (
      !Number.isInteger(n) ||
      !Number.isInteger(a) ||
      !Number.isInteger(b) ||
      a < 1 ||
      b < 1 ||
      a > n ||
      b > n
    ) {
      setInputError("Enter two integer vertex IDs within 1..V.");
      return;
    }
    setInputError("");
    const pairs = tokens.slice(2);
    pairs.push(String(a), String(b));
    onChange(
      `${n} ${Math.floor(pairs.length / 2)}\n` +
        Array.from({ length: Math.floor(pairs.length / 2) }, (_, i) =>
          pairs.slice(i * 2, i * 2 + 2).join(" "),
        ).join("\n") +
        "\n",
    );
  };
  return (
    <section className="input-card">
      <div className="card-heading">
        <div>
          <span className="card-number">01</span>
          <h2>Input</h2>
        </div>
        <button
          title="Input format"
          aria-label="Show input format"
          onClick={() => setGuide(!guide)}
        >
          <CircleHelp size={17} />
        </button>
      </div>
      <div className="input-actions">
        <button onClick={() => onChange(problem.sample)}>
          <Sparkles size={13} /> Load example
        </button>
        <button
          onClick={() => upload.current?.click()}
          title="Import input text"
          aria-label="Import input text"
        >
          <Upload size={14} />
        </button>
        <button
          onClick={() => download(`${problem.id}-input.txt`, text)}
          title="Download input text"
          aria-label="Download input text"
        >
          <ArrowDownToLine size={14} />
        </button>
      </div>
      <input
        className="sr-only"
        ref={upload}
        type="file"
        accept=".txt,.in,.dat"
        onChange={async (event) => {
          const element = event.currentTarget,
            file = element.files?.[0];
          element.value = "";
          if (file) {
            try {
              if (file.size > 64000)
                throw Error("Input files must be at most 64 KB.");
              onChange(normalizeInput(await file.text()));
              setInputError("");
            } catch (error) {
              setInputError(
                error instanceof Error
                  ? error.message
                  : "Could not read this file.",
              );
            }
          }
        }}
      />
      <div className="segmented">
        <button
          className={!raw ? "selected" : ""}
          onClick={() => setRaw(false)}
        >
          Fields
        </button>
        <button className={raw ? "selected" : ""} onClick={() => setRaw(true)}>
          Raw input
        </button>
      </div>
      {guide && (
        <div className="input-guide">
          <p>{problem.note}</p>
          {problem.inputGuide ? (
            <ReactMarkdown remarkPlugins={[remarkGfm]}>
              {problem.inputGuide.replace(/\[Sample\]\([^)]*\)/g, "")}
            </ReactMarkdown>
          ) : (
            <p>
              Each field follows the original program’s input order. The first
              value usually gives the number of elements; follow it with the
              stated values.
            </p>
          )}
        </div>
      )}
      {inputError && (
        <p className="inline-error" role="alert">
          {inputError}
        </p>
      )}
      {raw ? (
        <div className="raw-editor">
          <textarea
            aria-label="Raw program input"
            spellCheck={false}
            value={text}
            onChange={(e) => onChange(e.target.value)}
          />
        </div>
      ) : matrix && Number.isInteger(order) && order > 0 && order <= 32 ? (
        <div className="matrix-editor">
          <label>
            Matrix order
            <select
              aria-label="Matrix order"
              value={order}
              onChange={(e) =>
                onChange(
                  resizeMatrices(
                    text,
                    Number(e.target.value),
                    problem.id === "L3Q5",
                  ),
                )
              }
            >
              {(problem.id === "L3Q5"
                ? [1, 2, 4, 8, 16, 32]
                : Array.from({ length: 32 }, (_, i) => i + 1)
              ).map((n) => (
                <option key={n} value={n}>
                  {n} × {n}
                </option>
              ))}
            </select>
          </label>
          {problem.id === "L3Q5" && (
            <p className="caption">
              Linked cells update together to preserve the recursive matrix
              pattern.
            </p>
          )}
          {["A", "B"].map((name, m) => (
            <div key={name}>
              <h3>Matrix {name}</h3>
              <div className="matrix-input-scroll">
                <div
                  className="matrix-input-grid"
                  style={{ gridTemplateColumns: `repeat(${order},54px)` }}
                >
                  {Array.from({ length: order * order }, (_, i) => (
                    <input
                      key={i}
                      aria-label={`Matrix ${name} row ${Math.floor(i / order) + 1} column ${(i % order) + 1}`}
                      value={tokens[m * order * order + i] ?? ""}
                      onChange={(e) =>
                        onChange(
                          updateMatrixCell(
                            text,
                            m,
                            i,
                            e.target.value,
                            problem.id === "L3Q5",
                          ),
                        )
                      }
                      inputMode="decimal"
                    />
                  ))}
                </div>
              </div>
            </div>
          ))}
        </div>
      ) : (
        <div className="field-list">
          {lines.slice(0, 200).map((line, i) => (
            <label className="field" key={i}>
              <span>
                {labels[i]}
                <small>{String(i + 1).padStart(2, "0")}</small>
              </span>
              <input
                spellCheck={false}
                value={line}
                onChange={(e) => setLine(i, e.target.value)}
                aria-label={labels[i]}
              />
            </label>
          ))}
          {lines.length > 200 && (
            <p className="caption">
              Showing the first 200 lines. Use Raw input to edit all{" "}
              {lines.length} lines.
            </p>
          )}
          <div className="row-actions">
            <button
              onClick={() => onChange(lines.concat("").join("\n") + "\n")}
            >
              + Add row
            </button>
            {lines.length > 1 && (
              <button
                onClick={() => onChange(lines.slice(0, -1).join("\n") + "\n")}
              >
                Remove last
              </button>
            )}
          </div>
          <button
            className="sync-counts"
            onClick={() => onChange(syncCounts(problem, text))}
            disabled={syncCounts(problem, text) === normalizeInput(text)}
            title="Update counts to match the data rows and array values"
          >
            Sync counts with data
          </button>
        </div>
      )}
      {problem.id === "L10Q1" && (
        <div className="edge-editor">
          <span className="eyebrow">ADD AN EDGE</span>
          <div>
            <input
              type="number"
              min="1"
              value={edge[0]}
              onChange={(e) => setEdge([e.target.value, edge[1]])}
              aria-label="Edge from vertex"
            />
            <span>→</span>
            <input
              type="number"
              min="1"
              value={edge[1]}
              onChange={(e) => setEdge([edge[0], e.target.value])}
              aria-label="Edge to vertex"
            />
            <button onClick={addEdge} title="Add edge" aria-label="Add edge">
              +
            </button>
          </div>
          <p>
            Vertex IDs start at 1. Self-loops and repeated edges are supported.
          </p>
        </div>
      )}
      {problem.countMode && (
        <label className="count-option">
          <input
            type="checkbox"
            checked={countOnly}
            onChange={(e) => setCountOnly(e.target.checked)}
          />{" "}
          Count only (skip a long move transcript)
        </label>
      )}
      <div className="input-footer">
        <p>
          {problem.lab === 10 && problem.question === 2
            ? "Grid coordinates start at 0."
            : "Use ? for the exact input format."}
        </p>
        <button className="primary run-full" onClick={busy ? onCancel : onRun}>
          {busy ? <X size={16} /> : <Play size={15} fill="currentColor" />}
          {busy ? "Cancel run" : "Run algorithm"}
          <kbd>Ctrl ↵</kbd>
        </button>
      </div>
    </section>
  );
}
