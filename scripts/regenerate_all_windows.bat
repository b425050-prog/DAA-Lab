@echo off
setlocal
pushd "%~dp0.." >nul || exit /b 1

call "scripts\build_windows.bat"
if errorlevel 1 (
    set "STATUS=1"
    goto :finish
)

where gnuplot >nul 2>&1
if errorlevel 1 (
    echo ERROR: GNUPlot was not found. Install GNUPlot and add its bin folder to PATH.
    set "STATUS=1"
    goto :finish
)

set "DAA_NO_OPEN=1"

"lab1\Q-1\q1_growth_order.exe" || goto :run_failed
"lab1\Q-2\q2_coin_toss.exe" || goto :run_failed
"lab1\Q-3\q3_bubble_sort.exe" || goto :run_failed
"lab1\Q-4\q4_towers_of_hanoi.exe" || goto :run_failed

echo Data files and SVG graphs for Q-1 to Q-4 were regenerated.
echo Q-5 and Q-6 are interactive solutions and do not require graphs.
set "STATUS=0"
goto :finish

:run_failed
echo ERROR: One of the programs failed. Read the message above.
set "STATUS=1"

:finish
popd >nul
endlocal & exit /b %STATUS%
