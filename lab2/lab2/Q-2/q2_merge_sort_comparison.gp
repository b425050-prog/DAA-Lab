set encoding utf8
set terminal svg size 1700,900 enhanced font 'Arial,15' background rgb '#07111F'
set output 'q2_merge_sort_comparison.svg'

set datafile commentschars '#'
set border 31 front lc rgb '#334155' lw 1.2
set grid xtics ytics lc rgb '#1E293B' lw 1
set tics textcolor rgb '#CBD5E1'
set xlabel 'Input size, n' textcolor rgb '#E2E8F0'
set key textcolor rgb '#E2E8F0' opaque box samplen 2 spacing 1.15
set style line 1 lc rgb '#22D3EE' lw 3.2 pt 7 ps 0.8
set style line 2 lc rgb '#A78BFA' lw 3.2 pt 5 ps 0.8
set style line 3 lc rgb '#FBBF24' lw 2.2 dt 2

set multiplot layout 1,2

set title 'Measured primitive work' tc rgb '#F8FAFC' font 'Arial,18'
set ylabel 'Comparisons + merge writes' textcolor rgb '#E2E8F0'
set logscale x 2
set logscale y 2
set xrange [256:65536]
plot 'q2_merge_sort.dat' using 1:2 with linespoints ls 1 title '2-way merge sort', \
     '' using 1:3 with linespoints ls 2 title '3-way merge sort', \
     '' using 1:($4*2.0) with lines ls 3 title 'Scaled n log₂ n reference'

unset logscale y
set title 'Normalized by n log₂ n' tc rgb '#F8FAFC' font 'Arial,18'
set ylabel 'Work / (n log₂ n)' textcolor rgb '#E2E8F0'
set yrange [0:*]
plot 'q2_merge_sort.dat' using 1:($2/$4) with linespoints ls 1 title '2-way normalized', \
     '' using 1:($3/$4) with linespoints ls 2 title '3-way normalized'

unset multiplot
