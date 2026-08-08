set encoding utf8
set terminal svg size 1700,900 enhanced font 'Arial,15' background rgb '#07111F'
set output 'q3_merge_k_sorted_arrays.svg'

set datafile commentschars '#'
set border 31 front lc rgb '#334155' lw 1.2
set grid xtics ytics lc rgb '#1E293B' lw 1
set tics textcolor rgb '#CBD5E1'
set key textcolor rgb '#E2E8F0' opaque box samplen 2 spacing 1.15
set style line 1 lc rgb '#FB7185' lw 3.2 pt 7 ps 0.8
set style line 2 lc rgb '#22D3EE' lw 3.2 pt 5 ps 0.8

set multiplot layout 1,2

set title 'Vary k while n = 256' tc rgb '#F8FAFC' font 'Arial,18'
set xlabel 'Number of sorted arrays, k' textcolor rgb '#E2E8F0'
set ylabel 'Element writes' textcolor rgb '#E2E8F0'
set logscale x 2
set logscale y 2
set xrange [2:128]
plot 'q3_merge_k_arrays.dat' using 1:2 with linespoints ls 1 title 'Sequential — Θ(nk²)', \
     '' using 1:3 with linespoints ls 2 title 'Balanced — Θ(nk log k)'

set title 'Vary n while k = 16' tc rgb '#F8FAFC' font 'Arial,18'
set xlabel 'Elements per array, n' textcolor rgb '#E2E8F0'
set xrange [32:2048]
plot 'q3_merge_k_arrays.dat' using 4:5 with linespoints ls 1 title 'Sequential', \
     '' using 4:6 with linespoints ls 2 title 'Balanced'

unset multiplot
