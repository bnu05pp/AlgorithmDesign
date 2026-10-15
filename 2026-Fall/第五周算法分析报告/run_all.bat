@echo off
setlocal EnableDelayedExpansion

rem ============================================================
rem  Exercise 6-2 "d-ary heap" - one-click runner
rem  Flow: correctness verify -> benchmark -> plot
rem  Double-click to run (auto cds to this script's folder).
rem  To recompile, pass arg:  run_all.bat rebuild
rem ============================================================

cd /d "%~dp0"

rem ---- 0. toolchain ----
rem  g++ uses full path (the PATH g++ on this machine is broken)
set GXX="C:\Program Files\mingw64\bin\g++.exe"
if not exist %GXX% set GXX=g++

rem  python lives in the venv; try candidates, else system python
set PY=
if exist "C:\Users\jjh\.workbuddy\binaries\python\envs\default\Scripts\python.exe" (
    set PY="C:\Users\jjh\.workbuddy\binaries\python\envs\default\Scripts\python.exe"
) else if exist "C:\Users\jjh\.workbuddy\binaries\python\versions\3.13.12\python.exe" (
    set PY="C:\Users\jjh\.workbuddy\binaries\python\versions\3.13.12\python.exe"
) else (
    set PY=python
)

rem ---- 1. (optional) build ----
if /I "%~1"=="rebuild" (
    echo [build] test_6_2.exe ...
    %GXX% -O2 -std=c++17 -static -o test_6_2.exe test_6_2.cpp || goto :fail
    echo [build] bench_time.exe ...
    %GXX% -O2 -std=c++17 -static -o bench_time.exe bench_6_2.cpp || goto :fail
    echo [build] bench_count.exe ...
    %GXX% -O2 -std=c++17 -static -DDARY_HEAP_COUNT -o bench_count.exe bench_6_2.cpp || goto :fail
) else (
    if not exist test_6_2.exe (
        echo [missing] test_6_2.exe not found, building ...
        %GXX% -O2 -std=c++17 -static -o test_6_2.exe test_6_2.cpp || goto :fail
    )
    if not exist bench_time.exe (
        echo [missing] bench_time.exe not found, building ...
        %GXX% -O2 -std=c++17 -static -o bench_time.exe bench_6_2.cpp || goto :fail
    )
    if not exist bench_count.exe (
        echo [missing] bench_count.exe not found, building ...
        %GXX% -O2 -std=c++17 -static -DDARY_HEAP_COUNT -o bench_count.exe bench_6_2.cpp || goto :fail
    )
)

rem ---- 2. correctness ----
echo.
echo ========================================
echo [1/3] correctness check   test_6_2.exe
echo ========================================
test_6_2.exe
if errorlevel 1 goto :fail

rem ---- 3. benchmark ----
echo.
echo ========================================
echo [2/3] benchmark   bench_time + bench_count
echo ========================================
bench_time.exe
bench_count.exe

rem ---- 4. plot ----
echo.
echo ========================================
echo [3/3] plot figures   plot_bench.py
echo ========================================
%PY% plot_bench.py
if errorlevel 1 goto :fail

echo.
echo All done. Figures in figures\, data in data\
goto :done

:fail
echo.
echo Something failed. Check the messages above.
pause
exit /b 1

:done
echo.
echo Press any key to exit ...
pause >nul
exit /b 0
