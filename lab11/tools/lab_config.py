from pathlib import Path
import os
ROOT = Path(__file__).resolve().parents[1]
TITLES = ['Shortest Path on Parity-Constrained Graphs via 0-1 BFS',
          'Multi-Source BFS for Radius-Constrained Voronoi Cell Partitioning',
          'Bidirectional BFS for Shortest Transformation Sequence in a Word Graph']
NAMES = ['parity_shortest_paths','radius_voronoi','bidirectional_word_bfs']
SAMPLES = ['6 6 1 5\n1 2 1\n1 3 1\n3 2 1\n2 4 0\n3 5 0\n5 4 1\n',
           '9 7 2 3\n7 1\n1 2\n2 3\n3 4\n4 5\n5 6\n6 7\n4 8\n',
           '6 3\nhit cog\nhot\ndot\ndog\nlot\nlog\ncog\n']
EVENTS = ['vertex/state initialisations + arc scans + deque actions + answer rows',
          'vertex initialisations + hub seeds + arc scans + queue actions + answer rows',
          'encoded letters + distance initialisations + mutation candidates + hash probes + queue actions']
def executable(q): return ROOT/'bin'/f'q{q}_{NAMES[q-1]}{".exe" if os.name=="nt" else ""}'
