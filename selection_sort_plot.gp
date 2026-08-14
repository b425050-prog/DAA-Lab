set terminal svg size 1000,620 enhanced font 'Arial,13'
set output 'selection_sort_complexity.svg'
set title 'Selection Sort: Best and Worst Cases Both Theta(n^2)'
set xlabel 'Array size (n)'
set ylabel 'Comparisons'
set grid
set key left top
plot 'selection_sort_data.dat' using 1:2 with lines lw 3 title 'Already sorted', \
     '' using 1:3 with lines lw 2 dt 2 title 'Reverse sorted', \
     '' using 1:4 with lines lw 2 dt 3 title 'Random', \
     '' using 1:5 with points pt 7 ps 0.7 title 'n(n-1)/2'
