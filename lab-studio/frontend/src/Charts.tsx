import { useEffect, useMemo, useRef, useState } from "react";
import {
  Chart as ChartJS,
  LinearScale,
  LogarithmicScale,
  PointElement,
  LineElement,
  Tooltip,
  Legend,
  Filler,
} from "chart.js";
import { Line } from "react-chartjs-2";
import { Download, Maximize2, X } from "lucide-react";
import { Dataset, download } from "./types";
import { csv } from "./data";
ChartJS.register(
  LinearScale,
  LogarithmicScale,
  PointElement,
  LineElement,
  Tooltip,
  Legend,
  Filler,
);
const colours = [
  "#366d59",
  "#e08751",
  "#6c74a6",
  "#af653d",
  "#4d9aa3",
  "#a38940",
  "#af6785",
  "#6d8c43",
  "#5a87b1",
  "#b55b58",
  "#8a70ad",
  "#777777",
];
export function DataChart({
  dataset,
  caption,
}: {
  dataset: Dataset;
  caption?: string;
}) {
  const [logY, setLogY] = useState(false),
    [logX, setLogX] = useState(false),
    [expanded, setExpanded] = useState(false);
  const container = useRef<HTMLDivElement>(null);
  const [visible, setVisible] = useState<Set<number>>(
    () =>
      new Set(
        dataset.labels
          .slice(1, 7)
          .flatMap((name, i) => (/validat/i.test(name) ? [] : [i + 1])),
      ),
  );
  useEffect(() => {
    if (!expanded) return;
    const close = (event: KeyboardEvent) => {
      if (event.key === "Escape") setExpanded(false);
    };
    document.addEventListener("keydown", close);
    return () => document.removeEventListener("keydown", close);
  }, [expanded]);
  const data = useMemo(
    () => ({
      datasets: dataset.labels.slice(1).map((name, i) => ({
        label: name.replaceAll("_", " "),
        data: dataset.rows.map((row) => ({
          x: row[0],
          y:
            (!logY || row[i + 1] > 0) && (!logX || row[0] > 0)
              ? row[i + 1]
              : null,
        })),
        borderColor: colours[i % colours.length],
        backgroundColor: colours[i % colours.length],
        borderWidth: 2,
        pointRadius: dataset.rows.length > 80 ? 0 : 2,
        pointHoverRadius: 5,
        tension: 0.12,
        hidden: !visible.has(i + 1),
      })),
    }),
    [dataset, logY, logX, visible],
  );
  return (
    <div
      ref={container}
      className={"data-chart " + (expanded ? "chart-expanded" : "")}
      role={expanded ? "dialog" : undefined}
      aria-label={expanded ? dataset.name : undefined}
    >
      <div className="chart-actions">
        <span className="mono">{dataset.name}</span>
        <div>
          <button
            aria-pressed={logX}
            className={logX ? "selected" : ""}
            onClick={() => setLogX(!logX)}
          >
            Log X
          </button>
          <button
            aria-pressed={logY}
            className={logY ? "selected" : ""}
            onClick={() => setLogY(!logY)}
          >
            Log Y
          </button>
          <button
            title="Download chart data as CSV"
            aria-label="Download chart data as CSV"
            onClick={() =>
              download(
                dataset.name.replace(/\.dat$/, "") + ".csv",
                csv(dataset),
                "text/csv",
              )
            }
          >
            <Download size={15} />
          </button>
          <button
            title={expanded ? "Close expanded chart" : "Expand chart"}
            aria-label={expanded ? "Close expanded chart" : "Expand chart"}
            onClick={() => setExpanded(!expanded)}
          >
            {expanded ? <X size={15} /> : <Maximize2 size={15} />}
          </button>
        </div>
      </div>
      <div className="chart-canvas">
        <Line
          data={data}
          options={{
            responsive: true,
            maintainAspectRatio: false,
            animation: false,
            interaction: { mode: "nearest", intersect: false },
            plugins: {
              legend: { display: false },
              tooltip: { backgroundColor: "#15241f", padding: 12 },
            },
            scales: {
              x: {
                type: logX ? "logarithmic" : "linear",
                title: {
                  display: true,
                  text: dataset.labels[0].replaceAll("_", " "),
                },
                grid: { color: "#e9ebe5" },
                ticks: { maxTicksLimit: 8 },
              },
              y: {
                type: logY ? "logarithmic" : "linear",
                grid: { color: "#e9ebe5" },
                ticks: { maxTicksLimit: 7 },
              },
            },
          }}
        />
      </div>
      <div className="chart-legend">
        {dataset.labels.slice(1).map((name, i) => (
          <button
            key={i}
            aria-pressed={visible.has(i + 1)}
            className={!visible.has(i + 1) ? "muted" : ""}
            onClick={() =>
              setVisible((previous) => {
                const next = new Set(previous);
                next.has(i + 1) ? next.delete(i + 1) : next.add(i + 1);
                return next;
              })
            }
          >
            <i style={{ background: colours[i % colours.length] }} />
            {name.replaceAll("_", " ")}
          </button>
        ))}
      </div>
      <p className="caption">
        {caption}{" "}
        {dataset.totalRows && dataset.totalRows > dataset.rows.length
          ? `${dataset.rows.length} evenly sampled rows shown from ${dataset.totalRows.toLocaleString()}.`
          : ""}{" "}
        {(logY || logX) &&
          "Nonpositive values on a logarithmic axis are omitted."}
      </p>
    </div>
  );
}

export function Sequence({
  values,
  title = "Result values",
}: {
  values: (number | string | null)[];
  title?: string;
}) {
  if (!values.length) return null;
  if (values.some((v) => typeof v === "string"))
    return (
      <div className="sequence-exact">
        <h3>{title}</h3>
        {values.slice(0, 500).map((v, i) => (
          <span key={i}>{v === null ? "NONE" : String(v)}</span>
        ))}
        {values.length > 500 && (
          <p className="caption">
            First 500 values shown; export JSON for all values.
          </p>
        )}
      </div>
    );
  return (
    <DataChart
      dataset={{
        name: title,
        labels: ["Index", "Value"],
        rows: values.map((v, i) => [i, v === null ? NaN : Number(v)]),
      }}
      caption="Values returned by the C program. Missing values appear as gaps."
    />
  );
}
