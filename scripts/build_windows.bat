@echo off
setlocal
pushd "%~dp0.." >nul || exit /b 1

where gcc >nul 2>&1
if errorlevel 1 (
    echo ERROR: GCC was not found. Install MinGW-w64 and add its bin folder to PATH.
    set "STATUS=1"
    goto :finish
)

set "CFLAGS=-std=c11 -O2 -Wall -Wextra -pedantic"

gcc %CFLAGS% "lab1\Q-1\q1_growth_order.c" -o "lab1\Q-1\q1_growth_order.exe" -lm || goto :build_failed
gcc %CFLAGS% "lab1\Q-2\q2_coin_toss.c" -o "lab1\Q-2\q2_coin_toss.exe" -lm || goto :build_failed
gcc %CFLAGS% "lab1\Q-3\q3_bubble_sort.c" -o "lab1\Q-3\q3_bubble_sort.exe" || goto :build_failed
gcc %CFLAGS% "lab1\Q-4\q4_towers_of_hanoi.c" -o "lab1\Q-4\q4_towers_of_hanoi.exe" || goto :build_failed
gcc %CFLAGS% "lab1\Q-5\q5_partition_point.c" -o "lab1\Q-5\q5_partition_point.exe" || goto :build_failed
gcc %CFLAGS% "lab1\Q-6\q6_element_uniqueness.c" -o "lab1\Q-6\q6_element_uniqueness.exe" || goto :build_failed

echo Build complete. Native Windows executables were written into the question folders.
set "STATUS=0"
goto :finish

:build_failed
echo ERROR: Build failed. Read the compiler message above.
set "STATUS=1"

:finish
popd >nul
endlocal & exit /b %STATUS%
