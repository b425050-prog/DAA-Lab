@echo off
setlocal
pushd "%~dp0" >nul || exit /b 1

where gcc >nul 2>&1
if errorlevel 1 (
    echo ERROR: GCC was not found. Install MSYS2/MinGW-w64 and add its bin folder to PATH.
    set "STATUS=1"
    goto :finish
)

set "CFLAGS=-std=c17 -O2 -Wall -Wextra -Wpedantic"

for %%Q in (1 2 3 4) do if not exist "Q-%%Q\output" mkdir "Q-%%Q\output"

gcc %CFLAGS% "Q-1\q1_median_bfprt.c" -o "Q-1\output\q1_median_bfprt.exe" -lm || goto :build_failed
gcc %CFLAGS% "Q-1\q1_experimental_validation.c" -o "Q-1\output\q1_experimental_validation.exe" -lm || goto :build_failed
gcc %CFLAGS% "Q-1\q1_plot_complexity.c" -o "Q-1\output\q1_plot_complexity.exe" -lm || goto :build_failed

gcc %CFLAGS% "Q-2\q2_kth_smallest.c" -o "Q-2\output\q2_kth_smallest.exe" -lm || goto :build_failed
gcc %CFLAGS% "Q-2\q2_experimental_validation.c" -o "Q-2\output\q2_experimental_validation.exe" -lm || goto :build_failed
gcc %CFLAGS% "Q-2\q2_plot_complexity.c" -o "Q-2\output\q2_plot_complexity.exe" -lm || goto :build_failed

gcc %CFLAGS% "Q-3\q3_random_file_quicksort.c" -o "Q-3\output\q3_random_file_quicksort.exe" -lm || goto :build_failed
gcc %CFLAGS% "Q-3\q3_experimental_validation.c" -o "Q-3\output\q3_experimental_validation.exe" -lm || goto :build_failed
gcc %CFLAGS% "Q-3\q3_plot_complexity.c" -o "Q-3\output\q3_plot_complexity.exe" -lm || goto :build_failed

gcc %CFLAGS% "Q-4\q4_heap_sort_file.c" -o "Q-4\output\q4_heap_sort_file.exe" -lm || goto :build_failed
gcc %CFLAGS% "Q-4\q4_experimental_validation.c" -o "Q-4\output\q4_experimental_validation.exe" -lm || goto :build_failed
gcc %CFLAGS% "Q-4\q4_plot_complexity.c" -o "Q-4\output\q4_plot_complexity.exe" -lm || goto :build_failed

echo Build complete. Twelve native Windows executables were written into the four output folders.
set "STATUS=0"
goto :finish

:build_failed
echo ERROR: Build failed. Read the compiler message above.
set "STATUS=1"

:finish
popd >nul
endlocal & exit /b %STATUS%
