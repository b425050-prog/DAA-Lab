set terminal svg size 1000,620 enhanced font 'Arial,13'
set output 'max_min_comparisons.svg'
set title 'D&C Maximum-Minimum: Comparison Count'
set xlabel 'Array size (n)'
set ylabel 'Comparisons'
set grid
set key left top
plot 'max_min_data.dat' using 1:2 with linespoints lw 2 pt 7 title 'Measured comparisons', \
     '' using 1:3 with lines lw 2 dt 2 title '3n/2 upper bound', \
     '' using 1:4 with lines lw 2 dt 3 title 'Exact count of implementation'
