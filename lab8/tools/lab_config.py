from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
NAMES = ['minimum_coin_change', 'coin_change_ways', 'lcs_traceback',
         'longest_increasing_subsequence', 'maximum_sum_increasing_subsequence',
         'edit_distance_traceback', 'rod_cutting_reconstruction',
         'optimal_binary_search_tree', 'collatz_analyser']
TITLES = ['Minimum coin change', 'Count coin combinations', 'Longest common subsequence',
          'Longest increasing subsequence', 'Maximum sum increasing subsequence',
          'Edit distance & traceback', 'Rod cutting & reconstruction',
          'Optimal binary search trees', 'Collatz trajectories']
SHORT = ['FEWEST COINS', 'COUNT THE WAYS', 'FIND THE COMMON THREAD', 'BUILD AN ASCENT',
         'WEIGHT THE ASCENT', 'REWRITE THE STRING', 'CUT FOR VALUE', 'SEARCH WITH INTENT', 'FOLLOW THE UNKNOWN']
SAMPLES = ['3 6\n1 3 4\n', '3 10\n1 2 5\n', 'ABCBDAB\nBDCABA\n',
           '8\n10 9 2 5 3 7 101 18\n', '7\n1 101 2 3 100 4 5\n', 'kitten\nsitting\n',
           '8\n1 5 8 9 10 17 17 20\n', '5\n10 20 30 40 50\n0.15 0.10 0.05 0.10 0.20\n0.05 0.10 0.05 0.05 0.05 0.10\n',
           '27 1 100 10000\n']
BOUNDS = ['Theta(cV) time / Theta(V) space', 'O(cV) time / Theta(V) space',
          'Theta(mn) time / Theta(mn) space', 'Theta(n^2) time / Theta(n) space',
          'Theta(n^2) time / Theta(n) space', 'Theta(mn) time / Theta(mn) space',
          'Theta(n^2) time / Theta(n) space', 'Theta(n^3) time / Theta(n^2) space',
          'O(s + sum t(x)) time / O(s + r) space']
EVENTS = ['coin candidates', 'checked DP additions', 'prefix-pair states', 'predecessor pairs',
          'predecessor pairs', 'prefix-pair states', 'first-cut candidates', 'candidate roots', 'interval transitions']
