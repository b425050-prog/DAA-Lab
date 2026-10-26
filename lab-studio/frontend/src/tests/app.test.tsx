import React from "react";
import { describe, it, expect, vi, beforeEach } from "vitest";
import { render, screen, fireEvent, waitFor } from "@testing-library/react";
import { readFileSync } from "node:fs";
import { resolve } from "node:path";
import App from "../App";
import { Sequence, DataChart } from "../Charts";
import GraphView from "../GraphView";
import type { Catalog, Run } from "../types";
vi.mock("react-chartjs-2", () => ({
  Line: ({ data }: { data: unknown }) => (
    <div role="img" aria-label="Data chart">
      {JSON.stringify(data)}
    </div>
  ),
}));
const fixtures = JSON.parse(
  readFileSync(resolve(process.cwd(), "../.cache/ui-fixtures.json"), "utf8"),
) as { catalog: Catalog; runs: Record<string, Run> };
beforeEach(() => {
  vi.stubGlobal(
    "fetch",
    vi.fn(async (url: string, options?: RequestInit) => {
      const value =
        url === "/api/catalog"
          ? fixtures.catalog
          : url === "/api/health"
            ? { status: "ready" }
            : url === "/api/run"
              ? fixtures.runs[JSON.parse(String(options?.body)).problemId]
              : url.startsWith("/api/source/")
                ? { text: "C source for " + url.split("/").at(-1) }
                : url === "/api/audit"
                  ? { status: "PASS" }
                  : { cancelled: true };
      return { ok: true, json: async () => value };
    }),
  );
});
describe("All 64 C-backed problem flows", () => {
  for (const problem of fixtures.catalog.problems)
    it(problem.id + " runs, renders and exposes its result", async () => {
      localStorage.setItem(
        "daa-lab-studio-v1",
        JSON.stringify({ selected: problem.id, drafts: {} }),
      );
      const view = render(<App />);
      await screen.findByRole("heading", { level: 1 });
      expect(view.container.querySelector("h1")?.textContent).toBe(
        problem.title + ".",
      );
      fireEvent.click(screen.getByRole("button", { name: /^Run algorithm/ }));
      await screen.findByText("COMPLETED", { exact: true });
      expect(view.container.querySelector(".error-banner")).toBeNull();
      fireEvent.click(screen.getByRole("tab", { name: "Results" }));
      fireEvent.click(screen.getByRole("button", { name: "Console" }));
      expect(view.container.querySelector("pre")?.textContent).toBe(
        fixtures.runs[problem.id].stdout +
          (fixtures.runs[problem.id].stderr
            ? "\n" + fixtures.runs[problem.id].stderr
            : ""),
      );
      fireEvent.click(screen.getByRole("tab", { name: "Source" }));
      await screen.findByText("C source for " + problem.id, { exact: true });
      fireEvent.click(screen.getByRole("tab", { name: "Notes" }));
      expect(
        view.container.querySelector(".markdown")?.textContent?.length,
      ).toBeGreaterThan(100);
      fireEvent.click(screen.getByRole("tab", { name: /^Experiments/ }));
      if (problem.evidence.length)
        expect(screen.getAllByRole("img", { name: "Data chart" }).length).toBe(
          problem.evidence.length,
        );
    });
});
describe("Recovery and presentation regressions", () => {
  it("recovers from a null saved workspace", async () => {
    localStorage.setItem("daa-lab-studio-v1", "null");
    render(<App />);
    expect(await screen.findByRole("heading", { level: 1 })).toBeTruthy();
  });
  it("shows an actionable offline error", async () => {
    render(<App />);
    await screen.findByRole("heading", { level: 1 });
    vi.mocked(fetch).mockRejectedValueOnce(new TypeError("Failed to fetch"));
    fireEvent.click(screen.getByRole("button", { name: /^Run algorithm/ }));
    expect(await screen.findByRole("alert")).toHaveProperty(
      "textContent",
      expect.stringContaining("Cannot reach the C backend"),
    );
  });
  it("preserves binary codes as text, never coerces them to numbers", () => {
    render(<Sequence values={["001", "010", "11"]} title="Codes" />);
    expect(screen.getByText("001", { exact: true })).toBeTruthy();
    expect(screen.queryByRole("img", { name: "Data chart" })).toBeNull();
  });
  it("never creates negative cell sizes for a thin grid", () => {
    const view = render(
      <GraphView
        graph={{
          kind: "grid",
          rows: 2500,
          cols: 1,
          cells: [[0, 0]],
          counts: [1],
        }}
        step={1}
      />,
    );
    for (const rect of view.container.querySelectorAll("rect")) {
      expect(Number(rect.getAttribute("width"))).toBeGreaterThanOrEqual(0);
      expect(Number(rect.getAttribute("height"))).toBeGreaterThanOrEqual(0);
    }
  });
});

describe("Editing, imports and controls", () => {
  async function open(id: string) {
    localStorage.setItem(
      "daa-lab-studio-v1",
      JSON.stringify({ selected: id, drafts: {} }),
    );
    const view = render(<App />);
    await screen.findByRole("heading", { level: 1 });
    return view;
  }
  it("synchronizes the count after editing an array", async () => {
    await open("L1Q3");
    fireEvent.change(screen.getByLabelText("Array values"), {
      target: { value: "9 1 4" },
    });
    fireEvent.click(
      screen.getByRole("button", { name: "Sync counts with data" }),
    );
    expect(
      (screen.getByLabelText("Array length") as HTMLInputElement).value,
    ).toBe("3");
  });
  it("resizes editable matrices without mixing A and B", async () => {
    await open("L3Q4");
    fireEvent.change(screen.getByRole("combobox", { name: "Matrix order" }), {
      target: { value: "3" },
    });
    expect(
      (screen.getByLabelText("Matrix A row 3 column 3") as HTMLInputElement)
        .value,
    ).toBe("0");
    expect(
      (screen.getByLabelText("Matrix B row 1 column 1") as HTMLInputElement)
        .value,
    ).toBe("5");
  });
  it("imports a workspace and restores its selected input", async () => {
    const view = await open("L10Q1");
    const file = new File(
      [
        JSON.stringify({
          version: 1,
          selected: "L6Q5",
          drafts: { L6Q5: "35\n" },
        }),
      ],
      "workspace.json",
      { type: "application/json" },
    );
    fireEvent.change(view.container.querySelector('input[accept=".json"]')!, {
      target: { files: [file] },
    });
    await screen.findByLabelText("Fibonacci index (0–93)");
    expect(
      (screen.getByLabelText("Fibonacci index (0–93)") as HTMLInputElement)
        .value,
    ).toBe("35");
  });
  it("imports BOM/CRLF text into the string fields", async () => {
    const view = await open("L8Q3");
    const file = new File(["\ufeffABC\r\nAC\r\n"], "input.txt", {
      type: "text/plain",
    });
    fireEvent.change(
      view.container.querySelector('input[accept=".txt,.in,.dat"]')!,
      { target: { files: [file] } },
    );
    await waitFor(() =>
      expect(
        (
          screen.getByLabelText(
            "First string (blank allowed)",
          ) as HTMLInputElement
        ).value,
      ).toBe("ABC"),
    );
    expect(
      (
        screen.getByLabelText(
          "Second string (blank allowed)",
        ) as HTMLInputElement
      ).value,
    ).toBe("AC");
  });
  it("exports a completed run as a downloadable JSON file", async () => {
    const clicked: string[] = [];
    vi.spyOn(HTMLAnchorElement.prototype, "click").mockImplementation(function (
      this: HTMLAnchorElement,
    ) {
      clicked.push(this.download);
    });
    await open("L6Q5");
    fireEvent.click(screen.getByRole("button", { name: /^Run algorithm/ }));
    await screen.findByText("COMPLETED", { exact: true });
    fireEvent.click(screen.getByRole("button", { name: "Export result" }));
    expect(clicked).toContain("L6Q5-result.json");
  });
  it("closes an expanded chart using Escape", () => {
    render(
      <DataChart
        dataset={{
          name: "Example",
          labels: ["n", "Work"],
          rows: [
            [1, 2],
            [2, 4],
          ],
        }}
      />,
    );
    fireEvent.click(screen.getByRole("button", { name: "Expand chart" }));
    expect(screen.getByRole("dialog")).toBeTruthy();
    fireEvent.keyDown(document, { key: "Escape" });
    expect(screen.queryByRole("dialog")).toBeNull();
  });
  it("cancels the active backend request", async () => {
    await open("L6Q5");
    const original = vi.mocked(fetch).getMockImplementation()!;
    vi.mocked(fetch).mockImplementation((...args) =>
      String(args[0]) === "/api/run"
        ? new Promise(() => {})
        : original(...args),
    );
    fireEvent.click(screen.getByRole("button", { name: /^Run algorithm/ }));
    fireEvent.click(await screen.findByRole("button", { name: /^Cancel run/ }));
    expect(
      vi.mocked(fetch).mock.calls.some((call) => call[0] === "/api/cancel"),
    ).toBe(true);
    expect(screen.getByRole("button", { name: /^Run algorithm/ })).toBeTruthy();
  });
  it("supports keyboard navigation between output tabs", async () => {
    await open("L6Q5");
    fireEvent.keyDown(screen.getByRole("tab", { name: "Visualization" }), {
      key: "ArrowRight",
    });
    expect(
      screen
        .getByRole("tab", { name: "Results" })
        .getAttribute("aria-selected"),
    ).toBe("true");
  });
});
