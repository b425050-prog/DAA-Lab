import { useCallback, useEffect, useMemo, useRef, useState } from "react";
import {
  Activity,
  ArrowDownToLine,
  ArrowRight,
  BookOpen,
  Braces,
  Check,
  ChevronDown,
  ChevronRight,
  CircleHelp,
  Code2,
  ExternalLink,
  FlaskConical,
  GitBranch,
  Layers3,
  LoaderCircle,
  Menu,
  Network,
  PanelLeftClose,
  Pause,
  Play,
  RotateCcw,
  Search,
  ShieldCheck,
  SkipBack,
  SkipForward,
  Sparkles,
  Upload,
  X,
} from "lucide-react";
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import { Catalog, Graph, Json, Problem, Run, download } from "./types";
import GraphView, { graphSteps } from "./GraphView";
import { DataChart, Sequence } from "./Charts";
import { ResultPanel, ScalarCards, TraceVisual, Transcript } from "./Results";
import { normalizeInput, parseWorkspace } from "./input";
import InputEditor from "./InputEditor";
import Hanoi from "./Hanoi";

const STORE = "daa-lab-studio-v1";
const tabs = [
  ["visual", "Visualization", Network],
  ["results", "Results", Braces],
  ["evidence", "Experiments", Activity],
  ["notes", "Notes", BookOpen],
  ["source", "Source", Code2],
] as const;
type Saved = { selected?: string; drafts?: Record<string, string> };
function saved(): Saved {
  try {
    return parseWorkspace(JSON.parse(localStorage.getItem(STORE) ?? "null"));
  } catch {
    return {};
  }
}
function inputPreview(problem: Problem, text: string): Graph | null {
  if (problem.lab !== 10) return null;
  const tokens = text.trim().split(/\s+/).map(Number);
  if (tokens.some((v) => !Number.isInteger(v))) return null;
  const [n, m, k] = tokens;
  if (problem.question === 2) {
    if (n < 1 || m < 1 || n * m > 2500 || k < 0 || k > 2000) return null;
    return {
      kind: "grid",
      rows: n,
      cols: m,
      cells: Array.from(
        { length: Math.min(k, Math.floor((tokens.length - 3) / 2)) },
        (_, i) => tokens.slice(3 + 2 * i, 5 + 2 * i),
      ),
    };
  }
  if (n < 1 || n > 200 || m < 0 || m > 2000) return null;
  const q = problem.question,
    offset = q === 1 ? 2 : q === 3 ? 3 + n : 2 + n,
    stride = q === 3 ? 3 : 2;
  const edges = Array.from(
    {
      length: Math.min(
        m,
        Math.max(0, Math.floor((tokens.length - offset) / stride)),
      ),
    },
    (_, i) => tokens.slice(offset + stride * i, offset + stride * (i + 1)),
  ).filter(([a, b]) => a >= 1 && b >= 1 && a <= n && b <= n);
  return {
    kind: "graph",
    n,
    edges,
    weights:
      q === 3
        ? tokens.slice(3, 3 + n)
        : q === 4
          ? tokens.slice(2, 2 + n)
          : undefined,
  };
}

function Blank({ onRun }: { onRun: () => void }) {
  return (
    <div className="empty-state initial-state">
      <div className="empty-illustration">
        <span />
        <span />
        <span />
        <span />
        <Network size={34} />
      </div>
      <span className="eyebrow">YOUR NEXT EXPERIMENT</span>
      <h2>Give an algorithm something to solve.</h2>
      <p>
        Start with the example or enter your own data.
        <br />
        Your C program computes every result.
      </p>
      <button className="primary" onClick={onRun}>
        <Play size={15} fill="currentColor" /> Run this input{" "}
        <ArrowRight size={15} />
      </button>
      <span className="caption">Ctrl / ⌘ + Enter to run</span>
    </div>
  );
}

export default function App() {
  const [catalog, setCatalog] = useState<Catalog | null>(null),
    [loadError, setLoadError] = useState("");
  const [selected, setSelected] = useState(saved().selected ?? "L10Q1"),
    [drafts, setDrafts] = useState<Record<string, string>>(
      saved().drafts ?? {},
    );
  const [query, setQuery] = useState(""),
    [openLab, setOpenLab] = useState(10),
    [mobileNav, setMobileNav] = useState(false),
    [tab, setTab] = useState("visual");
  const [run, setRun] = useState<Run | null>(null),
    [busy, setBusy] = useState(false),
    [error, setError] = useState(""),
    [errorOutput, setErrorOutput] = useState("");
  const [health, setHealth] = useState<{
      status: string;
      message?: string;
    } | null>(null),
    [countOnly, setCountOnly] = useState(false);
  const [step, setStep] = useState(0),
    [playing, setPlaying] = useState(false),
    [speed, setSpeed] = useState(1),
    [source, setSource] = useState(""),
    [audit, setAudit] = useState<Json | null>(null),
    [showAudit, setShowAudit] = useState(false);
  const generation = useRef(0),
    workspaceInput = useRef<HTMLInputElement>(null),
    activeRequest = useRef<{ id: string; controller: AbortController } | null>(
      null,
    );
  const problem =
    catalog?.problems.find((p) => p.id === selected) ?? catalog?.problems[0];
  const text = problem ? (drafts[problem.id] ?? problem.sample) : "";
  const graph = useMemo(
    () => run?.graph ?? (problem ? inputPreview(problem, text) : null),
    [run, problem, text],
  );
  const hanoi =
    run && ["L1Q4", "L7Q3"].includes(run.problemId) && !run.countOnly;
  const moveCount = hanoi
    ? run.stdout
        .split("\n")
        .filter((line) =>
          /(?:disc|disk)\s+\d+.*?\b[ABCD]\b.*?\b[ABCD]\b/i.test(line),
        ).length
    : 0;
  const total = run ? (graph ? graphSteps(graph) : moveCount) : 0;
  const url = catalog ? `https://github.com/${catalog.repository}` : "";
  const fetchCatalog = () => {
    setLoadError("");
    fetch("/api/catalog")
      .then((r) => {
        if (!r.ok) throw Error();
        return r.json();
      })
      .then((c: Catalog) => {
        setCatalog(c);
        const p = c.problems.find((p) => p.id === selected);
        if (p) setOpenLab(p.lab);
      })
      .catch(() =>
        setLoadError(
          "Could not connect to Lab Studio. Start the backend with python run.py, then retry.",
        ),
      );
  };
  useEffect(() => {
    fetchCatalog();
    const check = () =>
      fetch("/api/health")
        .then((r) => r.json())
        .then(setHealth)
        .catch(() =>
          setHealth({
            status: "offline",
            message:
              "The backend is not reachable. Keep the launcher terminal open, or restart python run.py.",
          }),
        );
    void check();
    window.addEventListener("focus", check);
    return () => window.removeEventListener("focus", check);
  }, []);
  useEffect(() => {
    try {
      localStorage.setItem(
        STORE,
        JSON.stringify({ version: 1, selected, drafts }),
      );
    } catch {
      /* Storage may be disabled. */
    }
  }, [selected, drafts]);
  useEffect(() => {
    if (!playing || !total) return;
    const timer = setInterval(
      () =>
        setStep((previous) => {
          if (previous >= total) {
            setPlaying(false);
            return previous;
          }
          return previous + 1;
        }),
      650 / speed,
    );
    return () => clearInterval(timer);
  }, [playing, speed, total]);
  useEffect(() => {
    if (!problem || tab !== "source") return;
    let live = true;
    setSource("");
    fetch("/api/source/" + problem.id)
      .then((r) => r.json())
      .then((data) => {
        if (live) setSource(data.text ?? data.error);
      })
      .catch(() => {
        if (live) setSource("Could not load source.");
      });
    return () => {
      live = false;
    };
  }, [problem, tab]);
  const execute = useCallback(async () => {
    if (!problem || activeRequest.current) return;
    const version = ++generation.current;
    const request = {
      id: crypto.randomUUID(),
      controller: new AbortController(),
    };
    activeRequest.current = request;
    setBusy(true);
    setError("");
    setErrorOutput("");
    setPlaying(false);
    try {
      const response = await fetch("/api/run", {
        method: "POST",
        signal: request.controller.signal,
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          requestId: request.id,
          problemId: problem.id,
          input: text,
          countOnly,
        }),
      });
      let result;
      try {
        result = await response.json();
      } catch {
        throw Error(
          "The backend returned an invalid response. Start the app with python run.py; opening index.html directly cannot execute C.",
        );
      }
      if (version !== generation.current) return;
      if (!response.ok)
        throw Object.assign(new Error(result.error ?? "Execution failed."), {
          output: result.output ?? "",
        });
      setRun(result);
      setStep(
        graphSteps(result.graph) ||
          result.stdout
            .split("\n")
            .filter((line: string) =>
              /(?:disc|disk)\s+\d+.*?\b[ABCD]\b.*?\b[ABCD]\b/i.test(line),
            ).length,
      );
      setTab("visual");
      setHealth({ status: "ready" });
    } catch (cause) {
      if (
        version === generation.current &&
        !request.controller.signal.aborted
      ) {
        setError(
          cause instanceof TypeError
            ? "Cannot reach the C backend. Run python run.py and keep its terminal open, then retry."
            : cause instanceof Error
              ? cause.message
              : "The backend could not be reached.",
        );
        setErrorOutput((cause as { output?: string }).output ?? "");
        setRun(null);
      }
    } finally {
      if (activeRequest.current === request) activeRequest.current = null;
      if (version === generation.current) setBusy(false);
    }
  }, [problem, text, busy, countOnly]);
  useEffect(() => {
    const handler = (event: KeyboardEvent) => {
      if ((event.ctrlKey || event.metaKey) && event.key === "Enter") {
        event.preventDefault();
        void execute();
      }
    };
    window.addEventListener("keydown", handler);
    return () => window.removeEventListener("keydown", handler);
  }, [execute]);
  const cancel = () => {
    const request = activeRequest.current;
    if (request) {
      activeRequest.current = null;
      request.controller.abort();
      void fetch("/api/cancel", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ requestId: request.id }),
      }).catch(() => {});
    }
    generation.current++;
    setBusy(false);
    setPlaying(false);
  };
  const resetResult = () => {
    cancel();
    setRun(null);
    setError("");
    setErrorOutput("");
    setStep(0);
  };
  const edit = (value: string) => {
    if (!problem) return;
    resetResult();
    setDrafts((previous) => ({
      ...previous,
      [problem.id]: normalizeInput(value),
    }));
  };
  const select = (p: Problem) => {
    resetResult();
    setSelected(p.id);
    setOpenLab(p.lab);
    setTab("visual");
    setCountOnly(false);
    setMobileNav(false);
    setQuery("");
  };
  const exportWorkspace = () =>
    download(
      "daa-workspace.json",
      JSON.stringify({ version: 1, selected, drafts }, null, 2),
      "application/json",
    );
  async function importWorkspace(file: File) {
    try {
      if (file.size > 4_200_000)
        throw new Error("Workspace backups must be at most 4 MB.");
      const value = JSON.parse(await file.text());
      const clean = parseWorkspace(
        value,
        new Set(catalog?.problems.map((p) => p.id)),
      );
      resetResult();
      setDrafts(clean.drafts);
      const p = catalog?.problems.find((p) => p.id === clean.selected);
      if (p) {
        setSelected(p.id);
        setOpenLab(p.lab);
      }
      setTab("visual");
      setCountOnly(false);
    } catch (cause) {
      setError(
        cause instanceof Error
          ? cause.message
          : "Could not import this workspace.",
      );
    }
  }
  async function openAudit() {
    setShowAudit(true);
    try {
      const response = await fetch("/api/audit");
      setAudit(await response.json());
    } catch {
      setAudit({ error: "Could not load audit report." });
    }
  }
  if (!catalog || !problem)
    return (
      <div className="loading-screen">
        <FlaskConical size={38} />
        <h1>DAA Lab Studio</h1>
        {loadError ? (
          <>
            <p>{loadError}</p>
            <button className="primary" onClick={fetchCatalog}>
              Retry connection
            </button>
          </>
        ) : (
          <>
            <LoaderCircle className="spin" />
            <p>Opening your workbench…</p>
          </>
        )}
      </div>
    );
  return (
    <div className="app-shell">
      {mobileNav && (
        <div className="nav-scrim" onClick={() => setMobileNav(false)} />
      )}
      <aside className={"sidebar " + (mobileNav ? "nav-open" : "")}>
        <a className="brand" href="#" onClick={(e) => e.preventDefault()}>
          <div className="brand-mark">
            <FlaskConical size={23} />
          </div>
          <span>
            DAA<span>LAB STUDIO</span>
          </span>
          <button
            className="mobile-close"
            aria-label="Close navigation"
            onClick={() => setMobileNav(false)}
          >
            <X size={18} />
          </button>
        </a>
        <div className="workspace-label">
          <span className="status-dot" /> PERSONAL WORKSPACE <span>01</span>
        </div>
        <label className="search">
          <Search size={15} />
          <input
            placeholder="Find an algorithm…"
            aria-label="Search algorithms"
            value={query}
            onChange={(e) => setQuery(e.target.value)}
          />
          {query && (
            <button aria-label="Clear search" onClick={() => setQuery("")}>
              <X size={13} />
            </button>
          )}
        </label>
        <div className="sidebar-caption">
          THE LAB COLLECTION <span>64</span>
        </div>
        <nav aria-label="Lab collection">
          {catalog.labs.map((lab) => {
            const matches = catalog.problems.filter(
              (p) =>
                p.lab === lab.id &&
                (!query ||
                  `${p.title} ${p.id} ${p.theme}`
                    .toLowerCase()
                    .includes(query.toLowerCase())),
            );
            if (!matches.length) return null;
            const open = !!query || openLab === lab.id;
            return (
              <div
                key={lab.id}
                className={"lab-group " + (open ? "expanded" : "")}
              >
                <button
                  className="lab-toggle"
                  onClick={() => setOpenLab(openLab === lab.id ? 0 : lab.id)}
                  aria-expanded={open}
                >
                  <span className="lab-index">
                    {String(lab.id).padStart(2, "0")}
                  </span>
                  <span>{lab.title}</span>
                  {open ? (
                    <ChevronDown size={14} />
                  ) : (
                    <ChevronRight size={14} />
                  )}
                </button>
                {open && (
                  <div className="question-list">
                    {matches.map((p) => (
                      <button
                        key={p.id}
                        className={p.id === problem.id ? "active" : ""}
                        onClick={() => select(p)}
                      >
                        <span className="question-dot" />
                        <span>{p.title}</span>
                        <small>{String(p.question).padStart(2, "0")}</small>
                      </button>
                    ))}
                  </div>
                )}
              </div>
            );
          })}
          {query &&
            !catalog.problems.some((p) =>
              `${p.title} ${p.id} ${p.theme}`
                .toLowerCase()
                .includes(query.toLowerCase()),
            ) && <p className="no-matches">No algorithms match “{query}”.</p>}
        </nav>
        <div className="sidebar-footer">
          <button onClick={openAudit}>
            <ShieldCheck size={16} />
            <span>Code review & checks</span>
            <ArrowRight size={14} />
          </button>
          <a href={url} target="_blank" rel="noreferrer">
            <GitBranch size={16} />
            <span>Open repository</span>
            <ExternalLink size={13} />
          </a>
          <div className="sidebar-footnote">
            10 laboratories. One place to explore.
          </div>
        </div>
      </aside>
      <div className="main-shell">
        <header className="topbar">
          <div>
            <button
              className="mobile-menu"
              aria-label="Open navigation"
              onClick={() => setMobileNav(true)}
            >
              <Menu size={20} />
            </button>
            <span>Workspace</span>
            <ChevronRight size={13} />
            <span>Lab {String(problem.lab).padStart(2, "0")}</span>
            <ChevronRight size={13} />
            <strong>
              Question {String(problem.question).padStart(2, "0")}
            </strong>
          </div>
          <div className="workspace-actions">
            <input
              className="sr-only"
              ref={workspaceInput}
              type="file"
              accept=".json"
              onChange={(e) => {
                const file = e.target.files?.[0];
                if (file) void importWorkspace(file);
                e.target.value = "";
              }}
            />
            <button
              onClick={() => workspaceInput.current?.click()}
              title="Import workspace"
              aria-label="Import workspace"
              className="save-workspace"
            >
              <Upload size={14} />
            </button>
            <button onClick={exportWorkspace} className="save-workspace">
              <ArrowDownToLine size={14} /> Export workspace
            </button>
          </div>
        </header>
        <main>
          <div className="page-heading">
            <div>
              <span className="eyebrow">
                <span className="tiny-rule" /> ALGORITHM WORKBENCH
              </span>
              <h1>
                {problem.title}
                <span className="title-dot">.</span>
              </h1>
              <p>
                {problem.theme} <span> / </span> Lab{" "}
                {String(problem.lab).padStart(2, "0")} · Question{" "}
                {String(problem.question).padStart(2, "0")}
              </p>
            </div>
            <div className="heading-badge">
              <span
                className={
                  "status-dot " + (health?.status === "ready" ? "" : "offline")
                }
              />
              {health?.status === "ready"
                ? "C engine ready"
                : "Engine setup needed"}
              <small>LOCAL EXECUTION</small>
            </div>
          </div>
          {health?.message && <div className="notice">{health.message}</div>}
          {error && (
            <div className="error-banner" role="alert">
              <div>
                <strong>Couldn’t complete this run</strong>
                <p>{error}</p>
              </div>
              <button aria-label="Dismiss error" onClick={() => setError("")}>
                <X size={17} />
              </button>
            </div>
          )}
          <div className="workbench">
            <InputEditor
              key={problem.id}
              problem={problem}
              text={text}
              onChange={edit}
              busy={busy}
              onRun={() => void execute()}
              onCancel={cancel}
              countOnly={countOnly}
              setCountOnly={(value) => {
                resetResult();
                setCountOnly(value);
              }}
            />
            <div className="output-column">
              <section className="output-card">
                <div className="output-header">
                  <div>
                    <span className="card-number">02</span>
                    <h2>Explore the result</h2>
                  </div>
                  <span className={"run-status " + (run ? "complete" : "")}>
                    {busy ? (
                      <>
                        <LoaderCircle className="spin" size={12} /> COMPUTING
                      </>
                    ) : run ? (
                      <>
                        <Check size={12} /> COMPLETED
                      </>
                    ) : (
                      "READY WHEN YOU ARE"
                    )}
                  </span>
                </div>
                <div className="tabs" role="tablist" aria-label="Output views">
                  {tabs.map(([id, label, Icon], index) => (
                    <button
                      key={id}
                      role="tab"
                      tabIndex={tab === id ? 0 : -1}
                      aria-selected={tab === id}
                      className={tab === id ? "selected" : ""}
                      onClick={() => setTab(id)}
                      onKeyDown={(event) => {
                        if (
                          event.key === "ArrowRight" ||
                          event.key === "ArrowLeft"
                        ) {
                          event.preventDefault();
                          const next =
                            (index +
                              (event.key === "ArrowRight"
                                ? 1
                                : tabs.length - 1)) %
                            tabs.length;
                          setTab(tabs[next][0]);
                          (
                            event.currentTarget.parentElement?.children[
                              next
                            ] as HTMLButtonElement
                          )?.focus();
                        }
                      }}
                    >
                      <Icon size={15} />
                      {label}
                      {id === "evidence" && problem.evidence.length > 0 && (
                        <small>{problem.evidence.length}</small>
                      )}
                    </button>
                  ))}
                </div>
                {run?.warnings?.map((warning, i) => (
                  <div className="trace-warning" key={i} role="status">
                    {warning}
                  </div>
                ))}
                <div className="output-body" role="tabpanel">
                  {tab === "visual" && (
                    <>
                      {graph ? (
                        <GraphView
                          key={problem.id}
                          graph={graph}
                          step={run ? step : graphSteps(graph)}
                          preview={!run}
                        />
                      ) : run ? (
                        hanoi && moveCount > 0 ? (
                          <Hanoi run={run} step={step} />
                        ) : run.datasets.length ? (
                          <>
                            {run.datasets.map((data) => (
                              <DataChart
                                key={data.name}
                                dataset={data}
                                caption="Generated by this C run with your current input."
                              />
                            ))}
                          </>
                        ) : (
                          <TraceVisual run={run} />
                        )
                      ) : (
                        <Blank onRun={() => void execute()} />
                      )}
                    </>
                  )}
                  {tab === "results" &&
                    (run ? (
                      <ResultPanel key={run.input + run.problemId} run={run} />
                    ) : (
                      <div className="empty-state">
                        <Braces size={28} />
                        <h3>Your results will appear here.</h3>
                        <p>
                          Run the algorithm to inspect tables, console output,
                          and JSON.
                        </p>
                        {errorOutput && <Transcript text={errorOutput} />}
                      </div>
                    ))}
                  {tab === "evidence" && (
                    <div className="evidence-view">
                      <div className="evidence-note">
                        <Activity size={18} />
                        <div>
                          <strong>Repository experiments</strong>
                          <p>
                            Recorded measurements from the original lab. These
                            charts are separate from your current run.
                          </p>
                        </div>
                      </div>
                      {problem.evidence.length ? (
                        problem.evidence.map((data) => (
                          <DataChart
                            key={problem.id + data.name}
                            dataset={data}
                            caption="Recorded repository data. Click a legend entry to show or hide a series."
                          />
                        ))
                      ) : (
                        <div className="empty-state">
                          <h3>No recorded dataset for this question.</h3>
                          <p>Run your input to inspect its computed results.</p>
                        </div>
                      )}
                    </div>
                  )}
                  {tab === "notes" && (
                    <article className="markdown">
                      <div className="evidence-note">
                        <BookOpen size={18} />
                        <div>
                          <strong>Original lab notes</strong>
                          <p>
                            Some interface inputs have been adapted. Check Input
                            → ? for the current format and Code review for
                            project corrections.
                          </p>
                        </div>
                      </div>
                      <ReactMarkdown remarkPlugins={[remarkGfm]}>
                        {problem.notes}
                      </ReactMarkdown>
                    </article>
                  )}
                  {tab === "source" && (
                    <div className="source-view">
                      <div className="source-toolbar">
                        <span className="mono">
                          {problem.sourceKind === "override"
                            ? "engine/overrides/"
                            : "repository/"}
                          {problem.source}
                        </span>
                        <a
                          href={`${url}/blob/${catalog.commit}/${problem.source}`}
                          target="_blank"
                          rel="noreferrer"
                        >
                          Original <ExternalLink size={12} />
                        </a>
                      </div>
                      <p className="source-note">
                        The compiler uses a disposable copy. Original repository
                        files stay read-only; corrected overrides live inside
                        lab-studio.
                      </p>
                      <pre>
                        <code>{source || "Loading source…"}</code>
                      </pre>
                    </div>
                  )}
                </div>
                {tab === "visual" &&
                  run &&
                  graph?.maxima?.length !== undefined &&
                  graph.maxima.length > 0 && (
                    <div className="graph-step-output">
                      <span>
                        {problem.id === "L10Q3"
                          ? "Maximum eligible component"
                          : "Maximum active component"}
                      </span>
                      <strong>
                        {problem.id === "L10Q4"
                          ? step
                            ? String(graph.maxima[step - 1] ?? "NONE")
                            : "Initial graph"
                          : String(graph.maxima[step] ?? "NONE")}
                      </strong>
                      <small>C result at step {step}</small>
                    </div>
                  )}
                {tab === "visual" && run && total > 0 && (
                  <div className="playback">
                    <button
                      title="First step"
                      aria-label="First step"
                      onClick={() => {
                        setStep(0);
                        setPlaying(false);
                      }}
                    >
                      <SkipBack size={17} />
                    </button>
                    <button
                      className="play-toggle"
                      title={playing ? "Pause playback" : "Play playback"}
                      aria-label={playing ? "Pause playback" : "Play playback"}
                      onClick={() => {
                        if (step >= total) setStep(0);
                        setPlaying(!playing);
                      }}
                    >
                      {playing ? (
                        <Pause size={16} fill="currentColor" />
                      ) : (
                        <Play size={16} fill="currentColor" />
                      )}
                    </button>
                    <button
                      title="Next step"
                      aria-label="Next step"
                      onClick={() => {
                        setStep(Math.min(total, step + 1));
                        setPlaying(false);
                      }}
                    >
                      <SkipForward size={17} />
                    </button>
                    <input
                      type="range"
                      min="0"
                      max={total}
                      value={step}
                      onChange={(e) => {
                        setStep(Number(e.target.value));
                        setPlaying(false);
                      }}
                      aria-label="Playback step"
                    />
                    <span className="mono">
                      {step} <span>/ {total}</span>
                    </span>
                    <select
                      aria-label="Playback speed"
                      value={speed}
                      onChange={(e) => setSpeed(Number(e.target.value))}
                    >
                      <option value=".5">0.5×</option>
                      <option value="1">1×</option>
                      <option value="2">2×</option>
                      <option value="4">4×</option>
                    </select>
                  </div>
                )}
                {tab === "visual" && graph && (
                  <div className="graph-legend">
                    <span>
                      <i className="legend-accepted" />{" "}
                      {problem.id === "L10Q1" ? "Bipartition A" : "Component"}
                    </span>
                    {problem.id === "L10Q1" && (
                      <>
                        <span>
                          <i className="legend-other" /> Bipartition B
                        </span>
                        <span>
                          <i className="legend-rejected" /> Rejected attempt
                        </span>
                      </>
                    )}
                    <span className="legend-note">
                      {run
                        ? "Computed from this run"
                        : "Input preview · run to compute"}
                    </span>
                  </div>
                )}
              </section>
              <section className="summary-card">
                <div className="summary-heading">
                  <div>
                    <span className="card-number">03</span>
                    <h2>Run summary</h2>
                  </div>
                  {run && (
                    <button
                      onClick={() =>
                        download(
                          `${problem.id}-result.json`,
                          JSON.stringify(run, null, 2),
                          "application/json",
                        )
                      }
                    >
                      <ArrowDownToLine size={14} /> Export result
                    </button>
                  )}
                </div>
                {run ? (
                  <>
                    <ScalarCards run={run} />
                    <div className="run-foot">
                      <span>
                        <Check size={13} /> C program exited successfully
                      </span>
                      <span>
                        {run.elapsedMs.toLocaleString()} ms{" "}
                        <span>process wall time</span>
                      </span>
                    </div>
                    {run.files.length > 0 && (
                      <div className="generated-files">
                        {run.files.map((file) => (
                          <button
                            key={file.name}
                            onClick={() => download(file.name, file.text)}
                          >
                            <ArrowDownToLine size={14} /> {file.name}
                          </button>
                        ))}
                      </div>
                    )}
                  </>
                ) : (
                  <div className="summary-placeholder">
                    <span className="placeholder-line" />
                    <p>
                      Your answer, operation counts, and execution details will
                      appear after a run.
                    </p>
                  </div>
                )}
              </section>
              {errorOutput && tab !== "results" && (
                <Transcript text={errorOutput} />
              )}
            </div>
          </div>
          <footer className="page-footer">
            <span>
              <Layers3 size={13} /> 64 problems · 10 labs · C17
            </span>
            <span>
              Inputs saved on this device <Check size={12} />
            </span>
          </footer>
        </main>
      </div>
      {showAudit && (
        <div className="modal-backdrop" onClick={() => setShowAudit(false)}>
          <section
            className="audit-modal"
            role="dialog"
            aria-modal="true"
            aria-label="Code review and checks"
            onClick={(e) => e.stopPropagation()}
          >
            <div className="card-heading">
              <div>
                <ShieldCheck size={19} />
                <h2>Code review & checks</h2>
              </div>
              <button
                onClick={() => setShowAudit(false)}
                aria-label="Close code review"
              >
                <X size={19} />
              </button>
            </div>
            <p>
              Corrections apply to the project’s own source copies. Existing
              repository files are preserved.
            </p>
            {audit ? (
              <Transcript text={JSON.stringify(audit, null, 2)} />
            ) : (
              <LoaderCircle className="spin" />
            )}
          </section>
        </div>
      )}
    </div>
  );
}
