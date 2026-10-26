import { Run } from "./types";
export default function Hanoi({ run, step }: { run: Run; step: number }) {
  const moves = run.stdout.split("\n").flatMap((line) => {
    const m = line.match(
      /(?:disc|disk)\s+(\d+).*?\b([ABCD])\b.*?\b([ABCD])\b/i,
    );
    return m ? [{ disk: Number(m[1]), from: m[2], to: m[3] }] : [];
  });
  const n = Number(run.input.trim().split(/\s+/)[0]),
    pegs = run.problemId === "L7Q3" ? ["A", "B", "C", "D"] : ["A", "B", "C"];
  const state: Record<string, number[]> = {
    A: Array.from({ length: n }, (_, i) => n - i),
    B: [],
    C: [],
    D: [],
  };
  for (const move of moves.slice(0, step)) {
    state[move.from].pop();
    state[move.to].push(move.disk);
  }
  return (
    <div className="hanoi-stage">
      <span className="eyebrow">
        {pegs.length}-PEG HANOI · {n} DISKS
      </span>
      <svg
        viewBox="0 0 620 360"
        role="img"
        aria-label={`Hanoi state after ${step} moves`}
      >
        <rect width="620" height="360" fill="#f9faf6" />
        {pegs.map((peg, i) => {
          const x = ((i + 0.5) * 620) / pegs.length;
          return (
            <g key={peg}>
              <line
                x1={x}
                y1="85"
                x2={x}
                y2="290"
                stroke="#b2beae"
                strokeWidth="5"
                strokeLinecap="round"
              />
              <rect
                x={x - 60}
                y="285"
                width="120"
                height="6"
                rx="3"
                fill="#8e9e88"
              />
              {state[peg].map((disk, j) => (
                <rect
                  key={disk}
                  x={x - 12 - (disk * 50) / Math.max(n, 1)}
                  y={280 - (j + 1) * Math.min(17, 180 / Math.max(n, 1))}
                  width={24 + (disk * 100) / Math.max(n, 1)}
                  height={Math.max(2, Math.min(15, 170 / Math.max(n, 1)))}
                  rx="4"
                  fill={`hsl(${85 + disk * 13} 33% ${50 + disk * 2}%)`}
                  stroke="#4c6b4b"
                />
              ))}
              <text
                x={x}
                y="320"
                textAnchor="middle"
                fontSize="15"
                fill="#58634e"
              >
                {peg}
              </text>
            </g>
          );
        })}
      </svg>
      <p className="caption">
        {step
          ? moves[step - 1]
            ? `Move disk ${moves[step - 1].disk}: ${moves[step - 1].from} → ${moves[step - 1].to}`
            : "Final state"
          : "Initial state"}
      </p>
    </div>
  );
}
