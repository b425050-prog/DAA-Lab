# Lab 09 input contracts

[Dashboard](README.md)

All numeric inputs are whitespace separated, except Q6 whose first line is the string. Programs accept either no argument for readable output or `--json` for a result trace. Limits prevent overflow and keep the exact comparators practical.

## Q01 Fractional Knapsack with Deterioration Rate

`n W`, followed by n rows `value weight decay_rate`. n = 1..8; W and value = 0..10000; weight and rate = 0.001..10000. Real numbers are allowed.

[Sample](Q-1/q1_sample_input.txt)

## Q02 Huffman Coding

`n`, followed by n rows `symbol frequency`. Symbols are distinct printable non-space ASCII except double quote and backslash; frequencies = 1..10^9. There are 92 supported symbols. Omit zero-frequency symbols.

[Sample](Q-2/q2_sample_input.txt)

## Q03 Minimum Initial Fuel (Reverse Greedy)

`n D F`, followed by n rows `distance fuel`. n = 0..100000; D, F and fuel = 0..10^9; station distance = 0..D. Stations may be unsorted. One fuel unit covers one distance unit; tank capacity is unlimited.

[Sample](Q-3/q3_sample_input.txt)

## Q04 Minimum Cost to Connect Sticks

`n`, followed by n positive lengths. n = 0..100000; length = 1..10^9. An empty or singleton collection costs zero.

[Sample](Q-4/q4_sample_input.txt)

## Q05 Candy Distribution Problem (Bi-directional Slope Greedy)

`n`, followed by n ratings. n = 0..100000; ratings = -10^9..10^9. Equal ratings impose no extra constraint.

[Sample](Q-5/q5_sample_input.txt)

## Q06 Reorganise String with K-Distance Apart

First line: lowercase ASCII string, possibly empty, up to 100000 characters. Second line: K = 0..100000. Positions are zero based; equality at exactly K is allowed.

[Sample](Q-6/q6_sample_input.txt)

## Q07 Minimise Deviation in Array (Two-Way Greedy with Max-Heap)

`n`, followed by n positive values. n = 1..100000; values = 1..10^9. Arithmetic and totals use signed 64-bit integers.

[Sample](Q-7/q7_sample_input.txt)

## Q08 Minimum Number of Meeting Rooms

`n`, followed by n rows `start end`. n = 0..100000; 0 <= start <= end <= 10^9. Zero-length intervals use room ID 0 and consume no room.

[Sample](Q-8/q8_sample_input.txt)

## Q09 Hu-Tucker Greedy Simulation

`n`, followed by n ordered positive weights. n = 1..256; weight = 1..10^9. Leaf order is fixed and weights belong to leaves, rather than internal search keys.

[Sample](Q-9/q9_sample_input.txt)

## Q10 Greedy Superstring Conjecture

`n`, followed by n nonempty lowercase ASCII words, each 1..64 characters. n = 1..12. Duplicate and contained words are removed before either algorithm.

[Sample](Q-10/q10_sample_input.txt)