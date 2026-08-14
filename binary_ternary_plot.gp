set terminal svg size 1000,620 enhanced font 'Arial,13'
set output 'binary_vs_ternary.svg'
set title 'Binary vs Ternary Search: Key Probes'
set xlabel 'Number of sorted elements (n)'
set ylabel 'Array probes'
set logscale x 2
set grid
set key left top
plot 'binary_ternary_data.dat' using 1:2 with linespoints lw 2 pt 7 title 'Binary search', \
     '' using 1:3 with linespoints lw 2 pt 5 title 'Ternary search'
