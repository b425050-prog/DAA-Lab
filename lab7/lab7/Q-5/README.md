# Q5 · Hitting a Moving Target

[← Lab 07 dashboard](../README.md) · [Problem sheet](../Problem-Sheet-Lab-07.pdf) · [Input conventions](../INPUTS.md)

## Files

| File | Purpose |
|---|---|
| [q5_moving_target.c](q5_moving_target.c) | C17 solution: two sweeps and exhaustive belief sets |
| [q5_sample_input.txt](q5_sample_input.txt) | Ready-to-run input |
| [q5_sample_output.txt](q5_sample_output.txt) | Captured program output |
| [../tools/test_all.py](../tools/test_all.py) | Independent validators for all seven questions |

## Build and run

From this question folder:

```bash
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q5_moving_target.c -o q5
./q5 < q5_sample_input.txt
```

In Windows PowerShell:

```powershell
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q5_moving_target.c -o q5.exe
Get-Content q5_sample_input.txt | .\q5.exe
```

Input: **`n` hiding spots**. Keep the lab's `common/` folder beside the `Q-*` folders.

## Sample input

```text
5
```

<details>
<summary>Sample output</summary>

```text
Guaranteed shots: 6
Shot 1: spot 2; surviving positions: 4
Shot 2: spot 3; surviving positions: 3
Shot 3: spot 4; surviving positions: 3
Shot 4: spot 4; surviving positions: 1
Shot 5: spot 3; surviving positions: 1
Shot 6: spot 2; surviving positions: 0
Verified: no target path can survive the complete schedule.
```

</details>

## Algorithm and analysis
**Answer.** A guaranteed strategy exists for every `n>1`.

For two spots, shoot **1, 1**. A target missed on the first shot is at 2 and must move to 1.

For `n>=3`, shoot:

```text
2, 3, ..., n-1, n-1, n-2, ..., 2
```

There are `2(n-2)` shots. For five spots the sequence is `2,3,4,4,3,2`. The claim needed by the handout is guaranteed capture; the implementation does not rely on a shortest-schedule search.

### Why the two sweeps work

A mandatory step flips the parity of the target's position after every shot. During the first sweep the shot position also flips parity, so the two remain either equal in parity throughout that sweep or opposite throughout it.

Consider a target with the same parity as the first shot. If it is not hit at spot 2, it must lie to the right. To cross behind the shooter while avoiding a hit, its displacement relative to the shooter would have to pass through zero: each step changes this even displacement by 0 or -2. Thus it cannot cross without being hit. At the last spot `n-1`, there is no larger spot of the same parity within the line. Every target in this parity class has therefore been caught.

Any survivor has the opposite parity from `n-1` at the end of the first sweep. It must move, flipping its parity; the shooter repeats `n-1`. They now have the same parity. The reverse sweep applies the same argument from right to left and catches all remaining paths.

This proof allows the target to choose its direction adversarially after every missed shot. It does not assume randomness or a predetermined trajectory. The exact movement rule matters: allowing the target to stay still invalidates this parity argument. For the related general graph problem, see [Britnell and Wildon](https://arxiv.org/abs/1204.5490).

### Program validation

Start with every spot marked possible. After a shot at `s`, remove `s`. Before the next shot, replace the remaining set by the union of its adjacent spots. This is precisely the set of states reachable by all histories that have survived the shots. The final set is empty, proving the printed sequence covers every history.

**Complexity.** Constructing/printing the schedule alone is `O(n)` time and `O(1)` auxiliary space. The supplied program also scans the belief set after each of `O(n)` shots, taking **`O(n^2)` time and `O(n)` space**. This is deterministic validation over all possible paths, without enumerating exponentially many individual histories.
