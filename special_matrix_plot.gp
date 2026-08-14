set terminal svg size 1000,620 enhanced font 'Arial,13'
set output 'special_matrix_complexity.svg'
set title 'Special-Pattern Matrix Multiplication: O(n^2) Validation'
set xlabel 'Matrix order (n)'
set ylabel 'Operation-growth measure'
set logscale xy 2
set grid
set key left top
plot 'special_matrix_data.dat' using 1:5 with linespoints lw 2 pt 7 title 'D&C arithmetic operations', \
     '' using 1:6 with lines lw 2 dt 2 title 'n^2 reference', \
     '' using 1:7 with lines lw 2 dt 3 title 'n^3 classical reference'
