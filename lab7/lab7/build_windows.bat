@echo off
setlocal
pushd "%~dp0" >nul || exit /b 1
where gcc >nul 2>&1 || (
  echo ERROR: GCC was not found. Add your MSYS2/MinGW-w64 compiler to PATH.
  popd & exit /b 1
)
for /L %%Q in (1,1,7) do (
  if not exist "Q-%%Q\output" mkdir "Q-%%Q\output"
  for %%F in ("Q-%%Q\*.c") do (
    gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror "%%~fF" -o "Q-%%Q\output\%%~nF.exe" || goto :failed
  )
)
echo Build complete. All 7 Lab 07 C programs were compiled.
popd & exit /b 0
:failed
echo ERROR: Build failed. Read the compiler message above.
popd & exit /b 1
