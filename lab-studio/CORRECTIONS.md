# Corrections in this project only

Source baseline: `91b6e69bd366391a07a162d4cac98b72992cc7cb` from `b425050-prog/DAA-Lab`. Original paths below are relative to `engine/sources/` in this folder. The original repository files are preserved.

## Arithmetic and convergence

- **Lab 6 Q6, `q6_knapsack.h`:** adding a positive profit to a DP subtotal could overflow signed 64-bit arithmetic. The project copy checks before addition and fails explicitly if the optimum cannot be represented. Allocation also checks the table's byte count. Regressions cover overflow, exact `LLONG_MAX`, and negative profits.
- **Lab 6 Q8, `q8_matrix_chain.h`:** saturated costs used `ULLONG_MAX` as both overflow and a possible value. Overflow could leave an invalid split, leading to invalid reconstruction or recursion. The project copy tracks reachability separately, skips overflowing candidates, accepts an exact `ULLONG_MAX` answer, and refuses reconstruction when no representable result exists. Regressions cover a fully overflowing chain, a valid alternative despite an overflowing split, and the exact boundary.
- **Lab 6 Q2, `q2_algorithms.h`:** an unchanged Rayleigh quotient was treated as convergence, and reaching the iteration limit also returned success. For a 90-degree rotation, the quotient stays zero although no real eigenpair exists. The project copy requires a small eigenvector residual as well as a stable quotient, and reports failure at the iteration limit. The CLI labels a successful result as an eigenvalue estimate: uniform-start power iteration cannot guarantee dominance for every matrix.

## Input handling

- **Lab 3 Q4, Q5, Q6:** matrix, array, and trace reads were unchecked in some loops. The project copies reject failed reads instead of using missing/indeterminate data. Recursive matrix inputs have explicit small bounds for exact arithmetic. The backend additionally validates legacy token counts and types.
- **File-based sorting:** the interface accepts count and seed only. It supplies fixed relative filenames inside a temporary run folder, preventing user input from overwriting repository files.

## Interface adapters, not algorithm bug claims

Lab 1's four fixed demonstrations accept parameters in the project; the bubble sort and Hanoi implementations are retained. Coin experiments accept deterministic seeds. Lab 1 Q6 adds a seed input. Lab 2 Q1 accepts the maximum experiment size. The growth-order explanation groups functions of equal asymptotic order and distinguishes asymptotic classes from finite numerical values. Existing GNUPlot launch helpers are unused by the parameterized demonstrations; charts are rendered in the interface.

The party-attendance example uses distinct endpoints, matching the original program's precondition. Modern graph solutions already handle rejected edges, duplicate additions, signed weights, and reverse-deletion timing; independent oracle suites verify those cases. No defect is claimed merely because an algorithm has documented assumptions or limits.

The source hash list in `engine/provenance.json` identifies the original copies. `AUDIT.json` lists which project copies differ and records the checks actually run.
