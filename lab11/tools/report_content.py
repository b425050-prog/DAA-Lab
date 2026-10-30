PROBLEMS=[
 'Find each minimum source-to-vertex cost in a directed 0/1 graph, adding C once when the number of weight-1 edges is odd.',
 'Assign each vertex within radius R to its nearest hub, breaking equal-distance ties by hub index; report its distance or -1.',
 'Find the fewest one-letter dictionary mutations from A to B and print a shortest sequence, with N up to 100000 and L up to 10.']
IDEAS=[
 'Replace each vertex by even and odd parity states. A weight-1 edge flips parity; a weight-0 edge preserves it. Run 0-1 BFS, then choose min(d_even, d_odd+C).',
 'Seed one FIFO queue with distinct hubs in increasing hub-index order. First discovery fixes distance and owner. Stop expanding vertices at distance R.',
 'Encode words exactly in base 26 and find neighbours through an open-addressed table. Expand a complete layer of the smaller BFS frontier; reconstruct through the best meeting.']
PROOFS=[
 'Each original route maps to a route in the two-layer state graph with the same raw weight and the correct ending parity, and conversely. The 0/1 deque processes minimum-distance entries first; stale entries are skipped and each state is settled once. The only remaining decision is which final parity minimises the effective cost. C belongs in the final comparison, not on every weight-1 edge.',
 'FIFO BFS visits vertices in nondecreasing distance. Within each distance layer, processing order is nondecreasing owner index: it holds for the ordered seed layer, and each layer appends the next layer in that same owner order. Therefore the first shortest discovery has the smallest eligible hub index. Pruning at R cannot affect any vertex at distance at most R.',
 'Base-26 encoding is injective for a fixed length, and replacing one digit generates exactly the 25L possible one-letter changes. Both searches have complete distance layers. Before their first meeting, their visited balls of depths a and b are disjoint, implying the shortest distance exceeds a+b. Expanding one complete next layer finds the minimum crossing; a shorter route would have intersected the earlier balls. Parent chains on both sides therefore recover a shortest valid sequence.']
ANALYSES=[
 'The expanded graph has 2V states and 2E implicit arcs. Each settled state scans its arcs once; each successful relaxation creates one deque entry. Total time is O(V+E), including initialisation and output, with O(V+E) space. At most 2E+1 entries are ever pushed, so a circular deque of capacity 2E+2 is sufficient.',
 'Building adjacency lists takes O(V+E). Each covered vertex is queued once, each expanded adjacency entry is scanned once, and all V output rows are written. Time is O(V+E+K), which is O(V+E) for distinct hubs; space is O(V+E). Unassigned vertices print owner=-1 and distance=-1.',
 'Building encodings takes O(NL). At most N+1 words are visited from each side, with 25L constant-time digit replacements per expanded word. With expected constant-time hash lookup, time is O(NL+26NL), or O(NL) for the fixed alphabet; space is O(N+L). Adversarial hash collisions can give O(N^2 L) worst-case time. Bidirectional search reduces explored vertices on many inputs but retains the same worst-case search order.']
RESULTS=[
 'For vertex 2, an odd route of raw weight 1 costs 6, while the even route 1 -> 3 -> 2 costs 2. Vertex 6 is unreachable.',
 'Vertex 4 is three edges from both hubs. It belongs to hub 1 at vertex 7. Vertex 8 exceeds radius 3, while vertex 9 is disconnected; both remain unassigned.',
 'The sample requires four mutations: hit -> hot -> dot -> dog -> cog. A may start outside the dictionary; B must be present, including the A=B case.']
BOUNDS=['O(V+E) time and space','O(V+E) time and space','O(NL) expected time; O(N+L) space']
SOURCES={}
