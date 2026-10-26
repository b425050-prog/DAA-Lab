import { useEffect, useRef, useState } from "react";
import { Minus, Plus, RotateCcw, Download } from "lucide-react";
import { Graph, download } from "./types";
type Point = { x: number; y: number };
export function graphSteps(g: Graph | null): number {
  if (!g) return 0;
  if (g.kind === "large") return 0;
  return g.kind === "grid"
    ? (g.cells?.length ?? 0)
    : (g.deletions?.length ?? g.edges?.length ?? 0);
}
export default function GraphView({
  graph,
  step,
  preview = false,
}: {
  graph: Graph;
  step: number;
  preview?: boolean;
}) {
  const [zoom, setZoom] = useState(1),
    [positions, setPositions] = useState<Record<number, Point>>({}),
    [dragging, setDragging] = useState<number | null>(null);
  const svg = useRef<SVGSVGElement>(null);
  useEffect(() => {
    setPositions({});
    setZoom(1);
  }, [graph]);
  function save() {
    if (svg.current)
      download(
        "daa-graph.svg",
        new XMLSerializer().serializeToString(svg.current),
        "image/svg+xml",
      );
  }
  if (graph.kind === "large")
    return (
      <div className="empty-state">
        <h3>{graph.n?.toLocaleString()} vertices processed</h3>
        <p>
          The full output is available in Results. Graph drawings are limited to
          200 vertices for readability.
        </p>
      </div>
    );
  if (graph.kind === "grid") {
    const rows = graph.rows!,
      cols = graph.cols!,
      cells = graph.cells!.slice(0, step);
    if (rows * cols > 2500)
      return (
        <div className="empty-state">
          <h3>
            {rows} × {cols} grid
          </h3>
          <p>
            The result is available in the table. Grid drawing supports up to
            2,500 cells.
          </p>
        </div>
      );
    const active = new Set(cells.map(([r, c]) => `${r},${c}`));
    const w = Math.min(44, 530 / cols, 290 / rows),
      ox = (620 - cols * w) / 2,
      oy = (400 - rows * w) / 2,
      gap = w > 8 ? 2 : 0;
    return (
      <div className="graph-stage">
        <div className="graph-kicker">
          FOUR-NEIGHBOUR CONNECTIVITY{" "}
          <span>
            {preview
              ? "INPUT PREVIEW"
              : `${graph.counts?.[step - 1] ?? 0} ISLANDS`}
          </span>
        </div>
        <svg
          ref={svg}
          viewBox="0 0 620 400"
          role="img"
          aria-label={`Grid after ${step} land additions`}
        >
          <rect width="620" height="400" fill="#f9faf6" />
          {Array.from({ length: rows * cols }, (_, i) => {
            const r = Math.floor(i / cols),
              c = i % cols,
              on = active.has(`${r},${c}`),
              last = cells.at(-1);
            return (
              <g key={i}>
                <rect
                  x={ox + c * w + gap}
                  y={oy + r * w + gap}
                  width={w - 2 * gap}
                  height={w - 2 * gap}
                  rx={w > 8 ? 4 : 0}
                  fill={on ? "#c9e7ac" : "#e9eeeb"}
                  stroke={
                    on && last?.[0] === r && last?.[1] === c
                      ? "#385d42"
                      : "#d7dfd9"
                  }
                  strokeWidth={w > 8 ? 1.5 : 0.1}
                />
                {w > 20 && (
                  <text
                    x={ox + (c + 0.5) * w}
                    y={oy + (r + 0.55) * w}
                    textAnchor="middle"
                    fontSize="9"
                    fill="#516456"
                  >
                    {r},{c}
                  </text>
                )}
              </g>
            );
          })}
        </svg>
        <div className="graph-tools">
          <button onClick={save} aria-label="Download grid as SVG">
            <Download size={16} />
          </button>
        </div>
      </div>
    );
  }
  const n = graph.n!,
    edges = graph.edges ?? [],
    deletions = graph.deletions?.slice(0, step) ?? [],
    inactive = new Set(deletions);
  const shownEdges = graph.deletions ? edges : edges.slice(0, step);
  const xy = (node: number): Point =>
    positions[node] ?? {
      x:
        310 +
        Math.cos(-Math.PI / 2 + (2 * Math.PI * (node - 1)) / n) *
          Math.min(155, 90 + n * 13),
      y:
        185 +
        Math.sin(-Math.PI / 2 + (2 * Math.PI * (node - 1)) / n) *
          Math.min(110, 70 + n * 8),
    };
  // Colour each displayed connected component for non-bipartite modes. This is a
  // drawing aid; answers and accept/reject decisions always come from C.
  const groups = new Map<number, number>();
  let colour = 0;
  for (let v = 1; v <= n; v++)
    if (!inactive.has(v) && !groups.has(v)) {
      const queue = [v];
      groups.set(v, colour++);
      for (let at = 0; at < queue.length; at++)
        for (const [i, [a, b]] of shownEdges.entries()) {
          if (graph.accepted?.[i] === false) continue;
          const z = a === queue[at] ? b : b === queue[at] ? a : 0;
          if (z && !inactive.has(z) && !groups.has(z)) {
            groups.set(z, groups.get(v)!);
            queue.push(z);
          }
        }
    }
  const palette = [
    "#c9e7ac",
    "#c6dce5",
    "#efd0b8",
    "#d8c9e9",
    "#e5ddab",
    "#bde2d8",
  ];
  const last = shownEdges.at(-1),
    edgeRejected = graph.accepted?.[shownEdges.length - 1] === false;
  function move(event: React.PointerEvent<SVGSVGElement>) {
    if (dragging === null || !svg.current) return;
    const point = svg.current.createSVGPoint();
    point.x = event.clientX;
    point.y = event.clientY;
    const screen = svg.current.getScreenCTM();
    if (!screen) return;
    const local = point.matrixTransform(screen.inverse());
    setPositions((previous) => ({
      ...previous,
      [dragging]: {
        x: 310 + (local.x - 310) / zoom,
        y: 200 + (local.y - 200) / zoom,
      },
    }));
  }
  return (
    <div className="graph-stage">
      <div className="graph-kicker">
        {preview ? "INPUT PREVIEW" : "EXECUTION VIEW"}
        <span>
          {n} VERTICES · {edges.length} EDGES
        </span>
      </div>
      <svg
        ref={svg}
        viewBox="0 0 620 400"
        onPointerMove={move}
        onPointerUp={() => setDragging(null)}
        onPointerCancel={() => setDragging(null)}
        role="img"
        aria-label={`Graph at step ${step} of ${graphSteps(graph)}. Drag vertices to rearrange.`}
      >
        <defs>
          <pattern
            id="dots"
            width="22"
            height="22"
            patternUnits="userSpaceOnUse"
          >
            <circle cx="1" cy="1" r="0.7" fill="#d9dfd6" />
          </pattern>
        </defs>
        <rect width="620" height="400" fill="#f9faf6" />
        <rect width="620" height="400" fill="url(#dots)" />
        <g transform={`translate(310 200) scale(${zoom}) translate(-310 -200)`}>
          {shownEdges.map(([a, b, weight], i) => {
            if (
              a < 1 ||
              b < 1 ||
              a > n ||
              b > n ||
              inactive.has(a) ||
              inactive.has(b)
            )
              return null;
            const p = xy(a),
              q = xy(b),
              rejected = graph.accepted?.[i] === false,
              latest = i === shownEdges.length - 1 && !graph.deletions;
            return (
              <g key={i} opacity={rejected && !latest ? 0.35 : 1}>
                {a === b ? (
                  <path
                    d={`M ${p.x - 10} ${p.y - 12} C ${p.x - 65} ${p.y - 90} ${p.x + 65} ${p.y - 90} ${p.x + 10} ${p.y - 12}`}
                    fill="none"
                    stroke={rejected ? "#b85d4d" : "#648274"}
                    strokeWidth="2"
                    strokeDasharray={rejected ? "6 5" : undefined}
                  />
                ) : (
                  <line
                    x1={p.x}
                    y1={p.y}
                    x2={q.x}
                    y2={q.y}
                    stroke={
                      rejected ? "#b85d4d" : latest ? "#3d7057" : "#a5b6aa"
                    }
                    strokeWidth={latest ? 3 : 1.8}
                    strokeDasharray={rejected ? "6 5" : undefined}
                  />
                )}
                {weight !== undefined && (
                  <g>
                    <rect
                      x={(p.x + q.x) / 2 - 15}
                      y={(p.y + q.y) / 2 - 10}
                      width="30"
                      height="20"
                      rx="5"
                      fill="#fafbf8"
                    />
                    <text
                      x={(p.x + q.x) / 2}
                      y={(p.y + q.y) / 2 + 4}
                      textAnchor="middle"
                      fontSize="11"
                      fill="#5d6b60"
                    >
                      {weight}
                    </text>
                  </g>
                )}
              </g>
            );
          })}
          {Array.from({ length: n }, (_, i) => i + 1).map((v) => {
            const p = xy(v),
              dead = inactive.has(v),
              bip = graph.colours?.length === n,
              index = bip ? graph.colours![v - 1] : (groups.get(v) ?? 0);
            return (
              <g
                key={v}
                className="graph-node"
                opacity={dead ? 0.28 : 1}
                onPointerDown={(event) => {
                  event.currentTarget.setPointerCapture(event.pointerId);
                  setDragging(v);
                }}
                transform={`translate(${p.x} ${p.y})`}
              >
                <circle
                  r={n > 50 ? 10 : 24}
                  fill={dead ? "#e0e4dd" : palette[index % palette.length]}
                  stroke={dead ? "#a5afa6" : "#567258"}
                  strokeWidth="1.5"
                />
                <text
                  textAnchor="middle"
                  dominantBaseline="central"
                  fontFamily="system-ui"
                  fontWeight="600"
                  fontSize={n > 50 ? 8 : 15}
                  fill="#243b2a"
                >
                  {v}
                </text>
                {graph.weights && n < 25 && (
                  <text textAnchor="middle" y="42" fontSize="11" fill="#64705e">
                    w = {graph.weights[v - 1]}
                  </text>
                )}
                {dead && n < 50 && (
                  <line x1="-14" y1="14" x2="14" y2="-14" stroke="#798779" />
                )}
              </g>
            );
          })}
        </g>
      </svg>
      <div className="graph-annotation">
        {preview
          ? "Enter your edges, then run to see C’s decisions."
          : graph.deletions
            ? step
              ? `Deleted vertex ${deletions.at(-1)}`
              : "Initial graph"
            : last
              ? `Edge ${shownEdges.length}: ${last[0]} → ${last[1]}${graph.accepted?.length ? (edgeRejected ? " · rejected, odd cycle" : " · accepted") : ""}`
              : "Initial state · all vertices isolated"}
      </div>
      <div className="graph-tools">
        <button
          onClick={() => setZoom(Math.max(0.5, zoom - 0.2))}
          aria-label="Zoom out"
        >
          <Minus size={16} />
        </button>
        <span>{Math.round(zoom * 100)}%</span>
        <button
          onClick={() => setZoom(Math.min(2, zoom + 0.2))}
          aria-label="Zoom in"
        >
          <Plus size={16} />
        </button>
        <button
          onClick={() => {
            setPositions({});
            setZoom(1);
          }}
          aria-label="Reset graph layout"
        >
          <RotateCcw size={16} />
        </button>
        <button onClick={save} aria-label="Download graph as SVG">
          <Download size={16} />
        </button>
      </div>
      <div className="graph-hint">Drag vertices to arrange the graph</div>
    </div>
  );
}
