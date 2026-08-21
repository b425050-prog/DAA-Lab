@echo off
setlocal EnableExtensions
pushd "%~dp0" >nul || exit /b 1

where gcc >nul 2>&1
if errorlevel 1 (
    echo ERROR: GCC was not found. Install MSYS2 MinGW-w64 and add its bin directory to PATH.
    set "STATUS=1"
    goto :finish
)

set "CFLAGS=-std=c17 -O2 -Wall -Wextra -Wpedantic"

for %%Q in (1 2 3 4 5 6) do if not exist "Q-%%Q\output" mkdir "Q-%%Q\output"

call :build "Q-1\q1_stable_colour_sort.c" "Q-1\output\q1_stable_colour_sort.exe" || goto :failed
call :build "Q-1\q1_experimental_validation.c" "Q-1\output\q1_experimental_validation.exe" || goto :failed
call :build "Q-1\q1_plot_linear.c" "Q-1\output\q1_plot_linear.exe" || goto :failed
call :build "Q-2\q2_cross_set_pair_sum.c" "Q-2\output\q2_cross_set_pair_sum.exe" || goto :failed
call :build "Q-2\q2_experimental_validation.c" "Q-2\output\q2_experimental_validation.exe" || goto :failed
call :build "Q-2\q2_plot_complexity.c" "Q-2\output\q2_plot_complexity.exe" || goto :failed
call :build "Q-3\q3_generalized_k_sum.c" "Q-3\output\q3_generalized_k_sum.exe" || goto :failed
call :build "Q-3\q3_experimental_validation.c" "Q-3\output\q3_experimental_validation.exe" || goto :failed
call :build "Q-3\q3_plot_complexity.c" "Q-3\output\q3_plot_complexity.exe" || goto :failed
call :build "Q-4\q4_party_peak.c" "Q-4\output\q4_party_peak.exe" || goto :failed
call :build "Q-4\q4_experimental_validation.c" "Q-4\output\q4_experimental_validation.exe" || goto :failed
call :build "Q-4\q4_plot_complexity.c" "Q-4\output\q4_plot_complexity.exe" || goto :failed
call :build "Q-5\q5_merge_intervals.c" "Q-5\output\q5_merge_intervals.exe" || goto :failed
call :build "Q-5\q5_experimental_validation.c" "Q-5\output\q5_experimental_validation.exe" || goto :failed
call :build "Q-5\q5_plot_complexity.c" "Q-5\output\q5_plot_complexity.exe" || goto :failed
call :build "Q-6\q6_max_interval_overlap.c" "Q-6\output\q6_max_interval_overlap.exe" || goto :failed
call :build "Q-6\q6_experimental_validation.c" "Q-6\output\q6_experimental_validation.exe" || goto :failed
call :build "Q-6\q6_plot_complexity.c" "Q-6\output\q6_plot_complexity.exe" || goto :failed

echo.
echo Build complete: 18 native Windows executables were written to Q-1 through Q-6 output folders.
set "STATUS=0"
goto :finish

:build
gcc %CFLAGS% "%~1" -lm -o "%~2"
exit /b %errorlevel%

:failed
echo ERROR: Build failed. Read the compiler message above.
set "STATUS=1"

:finish
popd >nul
endlocal & exit /b %STATUS%
