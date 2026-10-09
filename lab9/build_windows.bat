@echo off
setlocal
pushd "%~dp0"
py tools\build.py
if errorlevel 1 goto fail
py tools\validate.py
if errorlevel 1 goto fail
py tools\generate_evidence.py
if errorlevel 1 goto fail
popd
exit /b 0
:fail
popd
exit /b 1
