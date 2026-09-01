@echo off
setlocal EnableDelayedExpansion
pushd "%~dp0" >nul || exit /b 1

where gcc >nul 2>&1 || (
  echo ERROR: GCC was not found. Install MSYS2/MinGW-w64 and add it to PATH.
  popd & exit /b 1
)

set "CFLAGS=-std=c17 -O2 -Wall -Wextra -Wpedantic"
for /L %%Q in (1,1,8) do (
  if not exist "Q-%%Q\output" mkdir "Q-%%Q\output"
  for %%F in ("Q-%%Q\*.c") do (
    set "NAME=%%~nF"
    gcc %CFLAGS% "%%~fF" -lm -o "Q-%%Q\output\!NAME!.exe" || goto :failed
  )
)
echo Build complete. All 24 Lab 06 C programs were compiled.
popd & exit /b 0

:failed
echo ERROR: Build failed. Read the compiler message above.
popd & exit /b 1
