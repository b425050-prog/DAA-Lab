set terminal svg size 1000,620 enhanced font 'Arial,13'
set output 'strassen_growth.svg'
set title 'Strassen vs Classical Matrix Multiplication'
set xlabel 'Matrix order (n)'
set ylabel 'Scalar multiplications'
set logscale xy 2
set grid
set key left top
plot 'strassen_data.dat' using 1:2 with linespoints lw 2 pt 7 title 'Strassen: 7^{log_2 n}', \
     '' using 1:3 with linespoints lw 2 pt 5 title 'Classical: n^3', \
     '' using 1:4 with lines lw 2 dt 2 title 'n^{log_2 7}'
