from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
TITLES = [
    'Fractional Knapsack with Deterioration Rate', 'Huffman Coding',
    'Minimum Initial Fuel (Reverse Greedy)', 'Minimum Cost to Connect Sticks',
    'Candy Distribution Problem (Bi-directional Slope Greedy)',
    'Reorganise String with K-Distance Apart',
    'Minimise Deviation in Array (Two-Way Greedy with Max-Heap)',
    'Minimum Number of Meeting Rooms', 'Hu-Tucker Greedy Simulation',
    'Greedy Superstring Conjecture',
]
SHORT = ['Decay and scheduling', 'Canonical Huffman codes', 'Retroactive refuelling',
         'Minimum merge cost', 'Two directional slopes', 'Heap and cooldown',
         'Shrink the maximum', 'Reuse the earliest room', 'Alphabetic tree',
         'Greedy versus exact']
NAMES = ['deteriorating_knapsack', 'huffman_coding', 'minimum_refuelling_stops',
         'connect_sticks', 'candy_distribution', 'reorganise_string',
         'minimise_deviation', 'meeting_rooms', 'hu_tucker_simulation',
         'greedy_superstring']
SAMPLES = [
    '3 4\n12 3 1\n8 2 2\n9 3 0.5\n',
    '6\nA 45\nB 13\nC 12\nD 16\nE 9\nF 5\n',
    '4 100 10\n10 60\n20 30\n30 30\n60 40\n',
    '5\n4 3 2 6 7\n', '5\n1 3 4 5 2\n', 'aaadbbcc\n3\n',
    '5\n4 1 5 20 3\n', '6\n0 30\n5 10\n15 20\n20 30\n30 40\n8 12\n',
    '8\n1 2 23 4 3 3 5 19\n', '3\nabb\nbba\nbbc\n',
]
BOUNDS = [
    'O(3^n n^3) time; O(n^2) workspace; n <= 8',
    'O(n log n + B) time; O(n + B) space',
    'O(n log n) time; O(n) space', 'O(n log n) time; O(n) space',
    'Theta(n) time; O(n) space', 'O(m log sigma) time; O(m + sigma) space',
    'O(n log M log n) time; O(n) space', 'O(n log n) time; O(n) space',
    'O(n^3) time; O(n) space; n <= 256',
    'Greedy O(n^3 L^2); exact O(2^n n^2 + n^2 l^2); n <= 12',
]
EVENTS = ['enumerated faces + sort comparisons', 'heap + code-sort comparisons',
          'heap + station-sort comparisons', 'heap comparisons',
          'adjacent rating checks', 'heap comparisons', 'heap comparisons',
          'heap + meeting-sort comparisons', 'compatible-pair checks',
          'overlap candidate checks']

def executable(q):
    import os
    return ROOT/'bin'/('q'+str(q)+'_'+NAMES[q-1]+('.exe' if os.name=='nt' else ''))
