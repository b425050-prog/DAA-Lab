set encoding utf8
set terminal svg size 1800,1200 enhanced font 'Arial,14' background rgb '#07111F'
set output 'q1_dictionary_operations.svg'

set datafile commentschars '#'
set border 31 front lc rgb '#334155' lw 1.2
set grid xtics ytics lc rgb '#1E293B' lw 1
set tics textcolor rgb '#CBD5E1'
set xlabel 'Dictionary size, n' textcolor rgb '#E2E8F0'
set ylabel 'Representative work units' textcolor rgb '#E2E8F0'
set key textcolor rgb '#E2E8F0' opaque box samplen 2 spacing 1.05
set logscale x 2
set logscale y 2
set xrange [16:65536]
set yrange [0.8:100000]
set format x '%.0f'
set format y '%.0f'
set xtics (16, 64, 256, 1024, 4096, 16384, 65536)
set style line 1 lc rgb '#22D3EE' lw 3 pt 7 ps 0.75
set style line 2 lc rgb '#A78BFA' lw 3 pt 5 ps 0.75
set style line 3 lc rgb '#FB7185' lw 3 pt 9 ps 0.75

set multiplot layout 3,2 rowsfirst

set title 'Unsorted Array' tc rgb '#F8FAFC' font 'Arial,17'
plot 'q1_dictionary_operations.dat' using 1:4 with linespoints ls 3 title 'Search · Max · Min · Pred · Succ — O(n)', \
     '' using 1:2 with linespoints ls 1 title 'Insert · Delete — O(1)'

set title 'Sorted Array' tc rgb '#F8FAFC' font 'Arial,17'
plot 'q1_dictionary_operations.dat' using 1:4 with linespoints ls 3 title 'Insert · Delete — O(n)', \
     '' using 1:3 with linespoints ls 2 title 'Search — O(log n)', \
     '' using 1:2 with linespoints ls 1 title 'Max · Min · Pred · Succ — O(1)'

set title 'Singly Linked Unsorted List' tc rgb '#F8FAFC' font 'Arial,17'
plot 'q1_dictionary_operations.dat' using 1:4 with linespoints ls 3 title 'Search · Delete · Max · Min · Pred · Succ — O(n)', \
     '' using 1:2 with linespoints ls 1 title 'Insert — O(1)'

set title 'Singly Linked Sorted List' tc rgb '#F8FAFC' font 'Arial,17'
plot 'q1_dictionary_operations.dat' using 1:4 with linespoints ls 3 title 'Search · Insert · Delete · Pred — O(n)', \
     '' using 1:2 with linespoints ls 1 title 'Max · Min · Succ — O(1)'

set title 'Doubly Linked Unsorted List' tc rgb '#F8FAFC' font 'Arial,17'
plot 'q1_dictionary_operations.dat' using 1:4 with linespoints ls 3 title 'Search · Max · Min · Pred · Succ — O(n)', \
     '' using 1:2 with linespoints ls 1 title 'Insert · Delete — O(1)'

set title 'Doubly Linked Sorted List' tc rgb '#F8FAFC' font 'Arial,17'
plot 'q1_dictionary_operations.dat' using 1:4 with linespoints ls 3 title 'Search · Insert — O(n)', \
     '' using 1:2 with linespoints ls 1 title 'Delete · Max · Min · Pred · Succ — O(1)'

unset multiplot
