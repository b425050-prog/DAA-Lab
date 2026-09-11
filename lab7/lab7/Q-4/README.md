# Q4 · Security Switches

[← Lab 07 dashboard](../README.md) · [Problem sheet](../Problem-Sheet-Lab-07.pdf) · [Input conventions](../INPUTS.md)

## Files

| File | Purpose |
|---|---|
| [q4_security_switches.c](q4_security_switches.c) | C17 solution: inverse gray-code path |
| [q4_sample_input.txt](q4_sample_input.txt) | Ready-to-run input |
| [q4_sample_output.txt](q4_sample_output.txt) | Captured program output |
| [../tools/test_all.py](../tools/test_all.py) | Independent validators for all seven questions |

## Build and run

From this question folder:

```bash
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q4_security_switches.c -o q4
./q4 < q4_sample_input.txt
```

In Windows PowerShell:

```powershell
gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror q4_security_switches.c -o q4.exe
Get-Content q4_sample_input.txt | .\q4.exe
```

Input: **`n` switches**. Keep the lab's `common/` folder beside the `Q-*` folders.

Append `--count` to suppress the move trace: `./q4 --count < q4_sample_input.txt`.

## Sample input

```text
4
```

<details>
<summary>Sample output</summary>

```text
Minimum moves: 10
Initial: 1111
Move 1: switch 2 -> 1101
Move 2: switch 1 -> 1100
Move 3: switch 4 -> 0100
Move 4: switch 1 -> 0101
Move 5: switch 2 -> 0111
Move 6: switch 1 -> 0110
Move 7: switch 3 -> 0010
Move 8: switch 1 -> 0011
Move 9: switch 2 -> 0001
Move 10: switch 1 -> 0000
Verified: all switches are off.
```

</details>

## Algorithm and analysis
**Answer.** With `n` switches initially all on, the minimum is

```math
S(n)=\left\lfloor\frac{2^{n+1}}{3}\right\rfloor.
```

The first values for `n=0,1,2,3,4,5` are `0,1,2,5,10,21`.

### State graph and algorithm

Number switches from the right, starting at 1. Write each configuration as a bitmask. Switch 1 is always legal. Switch `j>1` is legal exactly when its lower `j-1` bits equal the mask with only bit `j-2` set.

The legal configurations lie along the binary reflected Gray-code path:

```text
g(r) = r XOR (r >> 1), for r = 0,1,...,2^n-1.
```

To invert a Gray code `g`, compute `r=g XOR (g>>1) XOR (g>>2) ...`. The all-on bitmask therefore has rank with alternating bits `1010...` from its most significant bit, equal to `floor(2^(n+1)/3)`. The all-off mask has rank 0.

Starting at the all-on rank, repeatedly decrease the rank by one. The single bit changed between `g(r)` and `g(r-1)` is at zero-based position `ctz(r)`, the number of trailing zero bits in `r`. The implementation computes this portably with a short shift loop, checks the original switch rule, toggles the bit, and prints the new state.

For three switches:

```text
111 -> 110 -> 010 -> 011 -> 001 -> 000
```

### Why this is minimum

Any state has at most two legal moves: toggle switch 1, or toggle the switch immediately left of the rightmost on-switch, if that switch exists. The reflected Gray ordering connects all `2^n` configurations with exactly these legal edges. Equivalently, split configurations by their leftmost bit: each half is an `(n-1)`-switch path, and the leftmost switch connects the halves only when the suffix is `100...0`. These two paths join at endpoints into one path.

Thus the legal state graph has no shortcut edges. The unique path from the starting rank to zero has length exactly the rank. Decreasing it at every step is optimal.

**Complexity.** Count only uses `O(n)` bit operations and `O(1)` machine words. Across all decrements, trailing-zero scan work sums to `O(S(n))`; printing `n` bits per state makes the full trace `O(n*S(n))` time. Auxiliary storage is `O(1)` machine words under the supported `n<=63` bound. The output has `Theta(n*S(n))` characters.
