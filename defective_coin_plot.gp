set terminal svg size 1000,620 enhanced font 'Arial,13'
set output 'defective_coin_complexity.svg'
set title 'Defective Coin: Number of Balance Weighings'
set xlabel 'Number of coins (n)'
set ylabel 'Scale operations'
set logscale x 2
set grid
set key left top
plot 'defective_coin_data.dat' using 1:2 with linespoints lw 2 pt 7 title 'Defective coin at end', \
     '' using 1:3 with linespoints lw 2 pt 5 title 'No defective coin', \
     '' using 1:4 with lines lw 2 dt 2 title 'log_2(n) + 1 bound'
