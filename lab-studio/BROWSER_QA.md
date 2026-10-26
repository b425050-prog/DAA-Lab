# Browser checks

The production build was exercised through the local HTTP server in a browser at desktop (1440 × 1000) and mobile (390 × 844) viewports.

Verified flows:

- Lab 10 bipartiteness: run the example, show seven accepted arrivals and one rejected odd-cycle attempt, inspect the decision table and final colours, reset and advance playback.
- Lab 10 component weights: inspect signed maxima and `NONE`, and verify step 0 displays 8. Component-weight results do not invent bipartiteness accept/reject decisions.
- Lab 8 LCS: execute the actual C program, render the 8 × 7 DP table, show length 4 and subsequence `BCBA`.
- Recorded evidence: display the repository dataset, toggle a logarithmic axis, and keep it labelled separately from the current run.
- Mobile navigation: open the menu, search for Hanoi, select the three-peg problem, run it, and advance the disk playback.
- Result export: download the complete JSON result successfully.
- Production TypeScript and Vite build: passed.

Automated arithmetic, HTTP, source compilation, examples, and independent oracle results are recorded separately in `AUDIT.json`. Not every visual state of all 64 problems was manually inspected. Programs without intermediate C events display final computed outputs rather than fabricated animation.
