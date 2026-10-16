from pathlib import Path
import os
ROOT=Path(__file__).resolve().parents[1]
TITLES=['Online Graph-Bipartiteness Testing','Dynamic Connected Components in a Grid',
        'Maximum Weight Component with Size Constraints','Reverse Time-Step DSU Application',
        'Lexicographically Smallest Connected Component Permutation']
SHORT=['Keep the graph bipartite','Watch the islands merge','The best eligible component',
       'Turn deletion into addition','Sort inside connected components']
NAMES=['online_bipartiteness','dynamic_grid_components','maximum_weight_component',
       'reverse_time_dsu','smallest_component_permutation']
SAMPLES=[
 '6 8\n1 2\n2 3\n3 1\n3 4\n4 1\n5 6\n2 5\n4 5\n',
 '3 4 8\n0 0\n0 1\n1 2\n2 1\n1 1\n1 1\n2 3\n2 2\n',
 '5 5 2\n8 -10 7 3 -2\n1 2 4\n3 4 1\n1 3 9\n4 5 -1\n2 5 2\n',
 '5 4\n5 -9 4 7 -3\n1 2\n2 3\n3 4\n4 5\n3 1 5 2 4\n',
 '7 3\n7 6 5 4 3 2 1\n1 3\n3 5\n2 4\n'
]
EVENTS=['DSU node visits (including final colour extraction)',
        'neighbour probes + DSU node visits',
        'DSU visits + heap comparisons + snapshot validity checks',
        'adjacency probes + DSU visits + heap comparisons + validity checks',
        'DSU node visits + sorting comparisons']
BOUNDS=['O(V + E alpha(V)) amortised time; O(V) space',
        'O(RC + k alpha(RC)) amortised time; O(RC) space',
        'O(V + E alpha(V) + V log V) amortised time; O(V) space',
        'O((V+E) alpha(V) + V log V) amortised time; O(V+E) space',
        'O((V+E) alpha(V) + V log V) time; O(V) space']
def executable(q): return ROOT/'bin'/f'q{q}_{NAMES[q-1]}{ ".exe" if os.name=="nt" else ""}'
